//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2006 Ulrich von Zadow
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Current versions can be found at www.libavg.de
//

#include "AsyncVideoDecoder.h"
#include "EOFVideoMsg.h"
#include "ErrorVideoMsg.h"
#include "SeekDoneVideoMsg.h"

#include "../base/ObjectCounter.h"

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <math.h>
#include <iostream>

using namespace boost;
using namespace std;

namespace avg {

AsyncVideoDecoder::AsyncVideoDecoder(VideoDecoderPtr pSyncDecoder)
    : m_pSyncDecoder(pSyncDecoder),
      m_pVDecoderThread(0),
      m_pADecoderThread(0),
      m_Size(0,0),
      m_NumFrames(0),
      m_FPS(0),
      m_PF(NO_PIXELFORMAT),
      m_Duration(0),
      m_bAudioEOF(false),
      m_bVideoEOF(false),
      m_bVideoSeekPending(false),
      m_bAudioSeekPending(false),
      m_Volume(1.0),
      m_LastVideoFrameTime(-1000),
      m_LastAudioFrameTime(-1000)
{
    ObjectCounter::get()->incRef(&typeid(*this));
}

AsyncVideoDecoder::~AsyncVideoDecoder()
{
    if (m_pVDecoderThread || m_pADecoderThread) {
        close();
    }
    ObjectCounter::get()->decRef(&typeid(*this));
}

void AsyncVideoDecoder::open(const std::string& sFilename, const AudioParams* pAP,
        YCbCrMode ycbcrMode, bool bThreadedDemuxer)
{
    m_bAudioEOF = false;
    m_bVideoEOF = false;
    m_bVideoSeekPending = false;
    m_bAudioSeekPending = false;
    m_sFilename = sFilename;

    m_pSyncDecoder->open(m_sFilename, pAP, ycbcrMode, bThreadedDemuxer);
    m_bHasVideo = m_pSyncDecoder->hasVideo();
    m_bHasAudio = m_pSyncDecoder->hasAudio();
    m_Duration = m_pSyncDecoder->getDuration();

    if (m_bHasVideo) {
        m_LastVideoFrameTime = -1000;
        m_Size = m_pSyncDecoder->getSize();
        m_NumFrames = m_pSyncDecoder->getNumFrames();
        m_FPS = m_pSyncDecoder->getFPS();
        m_PF = m_pSyncDecoder->getPixelFormat();
        m_StreamFPS = m_pSyncDecoder->getNominalFPS();
        m_pVCmdQ = VideoDecoderThread::CmdQueuePtr(new VideoDecoderThread::CmdQueue);
        m_pVMsgQ = VideoMsgQueuePtr(new VideoMsgQueue(8));
        m_pVDecoderThread = new boost::thread(
                 VideoDecoderThread(*m_pVCmdQ, *m_pVMsgQ, m_pSyncDecoder));
    }
    
    if (m_bHasAudio) {
        m_pACmdQ = AudioDecoderThread::CmdQueuePtr(new AudioDecoderThread::CmdQueue);
        m_pAMsgQ = VideoMsgQueuePtr(new VideoMsgQueue(8));
        m_pADecoderThread = new boost::thread(
                 AudioDecoderThread(*m_pACmdQ, *m_pAMsgQ, m_pSyncDecoder, *pAP));
        m_AudioMsgData = 0;
        m_AudioMsgSize = 0;
        m_LastAudioFrameTime = 0;
    }
}

void AsyncVideoDecoder::close()
{
    if (m_pVDecoderThread) {
        m_pVCmdQ->push(Command<VideoDecoderThread>(boost::bind(
                &VideoDecoderThread::stop, _1)));
        getNextBmps(false); // If the Queue is full, this breaks the lock in the thread.
        m_pVDecoderThread->join();
        delete m_pVDecoderThread;
        m_pVDecoderThread = 0;
    }
    {
        scoped_lock Lock1(m_AudioMutex);
        if (m_pADecoderThread) {
            m_pACmdQ->push(Command<AudioDecoderThread>(boost::bind(
                    &AudioDecoderThread::stop, _1)));
            try {
                m_pAMsgQ->pop(false);
            } catch(Exception&) {}
            m_pADecoderThread->join();
            delete m_pADecoderThread;
            m_pADecoderThread = 0;
        }
        m_pSyncDecoder->close();
    }        
}

void AsyncVideoDecoder::seek(long long DestTime)
{
    waitForSeekDone();
    scoped_lock Lock1(m_AudioMutex);
    scoped_lock Lock2(m_SeekMutex);
    m_bAudioEOF = false;
    m_bVideoEOF = false;
    m_bVideoSeekPending = false;
    m_bAudioSeekPending = false;
    m_LastVideoFrameTime = -1000;
    if (m_pVCmdQ) {
        m_bVideoSeekPending = true;
        m_pVCmdQ->push(Command<VideoDecoderThread>(boost::bind(
                    &VideoDecoderThread::seek, _1, DestTime)));
    }
    if (m_pACmdQ) {
        m_bAudioSeekPending = true;
        m_pACmdQ->push(Command<AudioDecoderThread>(boost::bind(
                    &AudioDecoderThread::seek, _1, DestTime)));
    }
    try {
        do {
            bool& bSeekPending = m_bVideoSeekPending ? 
                    m_bVideoSeekPending : m_bAudioSeekPending;
            VideoMsgQueuePtr pMsgQ = m_bVideoSeekPending ? m_pVMsgQ : m_pAMsgQ;
            VideoMsgPtr pMsg = pMsgQ->pop(false);
            SeekDoneVideoMsgPtr pSeekDoneMsg = dynamic_pointer_cast<SeekDoneVideoMsg>(pMsg);
            if (pSeekDoneMsg) {
                bSeekPending = false;
                if (pSeekDoneMsg->performedSeek()) {
                    m_LastVideoFrameTime = pSeekDoneMsg->getVideoFrameTime();
                    m_LastAudioFrameTime = pSeekDoneMsg->getAudioFrameTime();
                }
            }

        } while (m_bVideoSeekPending || m_bAudioSeekPending);
    } catch (Exception&) {
    }
}

StreamSelect AsyncVideoDecoder::getMasterStream()
{
    if (m_bHasAudio) {
        return SS_AUDIO;
    } else {
        return SS_VIDEO;
    }
}

bool AsyncVideoDecoder::hasVideo()
{
    return m_bHasVideo;
}

bool AsyncVideoDecoder::hasAudio()
{
    return m_bHasAudio;
}

IntPoint AsyncVideoDecoder::getSize()
{
    return m_Size;
}

int AsyncVideoDecoder::getCurFrame()
{
    return int(getCurTime(SS_VIDEO)*m_StreamFPS/1000.0+0.5);
}

int AsyncVideoDecoder::getNumFrames()
{
    return m_NumFrames;
}

int AsyncVideoDecoder::getNumFramesQueued()
{
    return m_pVMsgQ->size();
}

long long AsyncVideoDecoder::getCurTime(StreamSelect Stream)
{
    switch(Stream) {
        case SS_VIDEO:
            assert(m_bHasVideo);
            return m_LastVideoFrameTime;
            break;
        case SS_AUDIO:
            assert(m_bHasAudio);
            return m_LastAudioFrameTime;
            break;
        case SS_DEFAULT:
            return getCurTime(getMasterStream());
            break;
        default:
            assert(false);
    }
    return -1;
}

long long AsyncVideoDecoder::getDuration()
{
    return m_Duration;
}

double AsyncVideoDecoder::getNominalFPS()
{
    return m_StreamFPS;
}

double AsyncVideoDecoder::getFPS()
{
    assert(m_pVDecoderThread);
    return m_FPS;
}

void AsyncVideoDecoder::setFPS(double FPS)
{
    assert(!m_pADecoderThread);
    m_pVCmdQ->push(Command<VideoDecoderThread>(boost::bind(
            &VideoDecoderThread::setFPS, _1, FPS)));
    if (FPS != 0) {
        m_FPS = FPS;
    }
}

double AsyncVideoDecoder::getVolume()
{
    return m_Volume;
}

void AsyncVideoDecoder::setVolume(double Volume)
{
    m_Volume = Volume;
    if (m_bHasAudio) {
        m_pACmdQ->push(Command<AudioDecoderThread>(boost::bind(
                &AudioDecoderThread::setVolume, _1, Volume)));
    }
}

PixelFormat AsyncVideoDecoder::getPixelFormat()
{
    assert(m_pVDecoderThread);
    return m_PF;
}

FrameAvailableCode AsyncVideoDecoder::renderToBmp(BitmapPtr pBmp, long long TimeWanted)
{
    FrameAvailableCode FrameAvailable;
    FrameVideoMsgPtr pFrameMsg = getBmpsForTime(TimeWanted, FrameAvailable);
    if (FrameAvailable == FA_NEW_FRAME) {
        pBmp->copyPixels(*(pFrameMsg->getBitmap(0)));
    }
    return FrameAvailable;
}

FrameAvailableCode AsyncVideoDecoder::renderToYCbCr420p(BitmapPtr pBmpY, BitmapPtr pBmpCb, 
       BitmapPtr pBmpCr, long long TimeWanted)
{
    FrameAvailableCode FrameAvailable;
    FrameVideoMsgPtr pFrameMsg = getBmpsForTime(TimeWanted, FrameAvailable);
    if (FrameAvailable == FA_NEW_FRAME) {
        pBmpY->copyPixels(*(pFrameMsg->getBitmap(0)));
        pBmpCb->copyPixels(*(pFrameMsg->getBitmap(1)));
        pBmpCr->copyPixels(*(pFrameMsg->getBitmap(2)));
    }
    return FrameAvailable;
}

bool AsyncVideoDecoder::isEOF(StreamSelect Stream)
{
    switch(Stream) {
        case SS_AUDIO:
            return (!m_bHasAudio || m_bAudioEOF);
        case SS_VIDEO:
            return (!m_bHasVideo || m_bVideoEOF);
        case SS_ALL:
            return isEOF(SS_VIDEO) && isEOF(SS_AUDIO);
        default:
            return false;
    }
}

int AsyncVideoDecoder::fillAudioBuffer(AudioBufferPtr pBuffer)
{
    assert (m_pADecoderThread);
    if (m_bAudioEOF) {
        return 0;
    }
    scoped_lock Lock(m_AudioMutex);
    waitForSeekDone();

    unsigned char* audioBuffer = (unsigned char *)(pBuffer->getData());
    int audioBufferSize = pBuffer->getNumBytes();

    int bufferLeftToFill = audioBufferSize;
    while (bufferLeftToFill > 0) {
        while (m_AudioMsgSize > 0 && bufferLeftToFill > 0) {
            int copyBytes = min(bufferLeftToFill, m_AudioMsgSize);
            memcpy(audioBuffer, m_AudioMsgData, copyBytes);
            m_AudioMsgSize -= copyBytes;
            m_AudioMsgData += copyBytes;
            bufferLeftToFill -= copyBytes;
            audioBuffer += copyBytes;

            m_LastAudioFrameTime += (long long)(1000.0 * copyBytes / 
                    (pBuffer->getFrameSize() * pBuffer->getRate()));
        }
        if (bufferLeftToFill != 0) {
            try {
                VideoMsgPtr pMsg = m_pAMsgQ->pop(false);

                EOFVideoMsgPtr pEOFMsg(dynamic_pointer_cast<EOFVideoMsg>(pMsg));
                if (pEOFMsg) {
                    m_bAudioEOF = true;
                    return pBuffer->getNumFrames()-bufferLeftToFill/pBuffer->getFrameSize();
                }

                m_pAudioMsg = dynamic_pointer_cast<AudioVideoMsg>(pMsg);
                assert(m_pAudioMsg);

                m_AudioMsgSize = m_pAudioMsg->getBuffer()->getNumFrames()
                        *pBuffer->getFrameSize();
                m_AudioMsgData = (unsigned char *)(m_pAudioMsg->getBuffer()->getData());
                m_LastAudioFrameTime = m_pAudioMsg->getTime();
            } catch (Exception &) {
                return pBuffer->getNumFrames()-bufferLeftToFill/pBuffer->getFrameSize();
            }
        }
    }
    return pBuffer->getNumFrames();
}
        
FrameVideoMsgPtr AsyncVideoDecoder::getBmpsForTime(long long TimeWanted, 
        FrameAvailableCode& FrameAvailable)
{
    // XXX: This code is sort-of duplicated in FFMpegDecoder::readFrameForTime()
    long long FrameTime = -1000;
    FrameVideoMsgPtr pFrameMsg;
    if (TimeWanted == -1) {
        pFrameMsg = getNextBmps(true);
        FrameAvailable = FA_NEW_FRAME;
    } else {
        if (getMasterStream() == SS_AUDIO) {
            TimeWanted = m_LastAudioFrameTime;
        }

//        cerr << "getBmpsForTime " << TimeWanted << ", LastFrameTime= " << m_LastVideoFrameTime 
//                << ", diff= " << TimeWanted-m_LastVideoFrameTime <<  endl;
        double TimePerFrame = 1000.0/getFPS();
        if (fabs(double(TimeWanted-m_LastVideoFrameTime)) < 0.5*TimePerFrame || 
                m_LastVideoFrameTime > TimeWanted+TimePerFrame) {
//            cerr << "   LastFrameTime = " << m_LastVideoFrameTime << ", display again." <<  endl;
            // The last frame is still current. Display it again.
            FrameAvailable = FA_USE_LAST_FRAME;
            return FrameVideoMsgPtr();
        } else {
            if (m_bVideoEOF) {
//                cerr << "  EOF." << endl;
                FrameAvailable = FA_USE_LAST_FRAME;
                return FrameVideoMsgPtr();
            }
            while (FrameTime-TimeWanted < -0.5*TimePerFrame && !m_bVideoEOF) {
                pFrameMsg = getNextBmps(false);
                if (pFrameMsg) {
                    FrameTime = pFrameMsg->getFrameTime();
//                    cerr << "   readFrame returned time " << FrameTime << "." <<  endl;
                } else {
//                    cerr << "   no frame available." <<  endl;
                    FrameAvailable = FA_STILL_DECODING;
                    return FrameVideoMsgPtr();
                }
            }
            FrameAvailable = FA_NEW_FRAME;
//            cerr << "  frame ok." << endl;
        }
    }
    if (pFrameMsg) {
        m_LastVideoFrameTime = pFrameMsg->getFrameTime();
    }
    return pFrameMsg;
}

FrameVideoMsgPtr AsyncVideoDecoder::getNextBmps(bool bWait)
{
    try {
        waitForSeekDone();
        VideoMsgPtr pMsg = m_pVMsgQ->pop(bWait);
        FrameVideoMsgPtr pFrameMsg = dynamic_pointer_cast<FrameVideoMsg>(pMsg);
        while (!pFrameMsg) {
            EOFVideoMsgPtr pEOFMsg(dynamic_pointer_cast<EOFVideoMsg>(pMsg));
            ErrorVideoMsgPtr pErrorMsg(dynamic_pointer_cast<ErrorVideoMsg>(pMsg));
            if (pEOFMsg) {
                m_bVideoEOF = true;
                return FrameVideoMsgPtr();
            } else if (pErrorMsg) {
                m_bVideoEOF = true;
                return FrameVideoMsgPtr();
            } else {
                // Unhandled message type.
                assert(false);
            }
            pMsg = m_pVMsgQ->pop(bWait);
            pFrameMsg = dynamic_pointer_cast<FrameVideoMsg>(pMsg);
        }
        return pFrameMsg;
    } catch (Exception&) {
        return FrameVideoMsgPtr();
    }
}

void AsyncVideoDecoder::waitForSeekDone()
{
    scoped_lock Lock(m_SeekMutex);
    if (m_bVideoSeekPending || m_bAudioSeekPending) {
        do {
            bool& bSeekPending = m_bVideoSeekPending ? 
                    m_bVideoSeekPending : m_bAudioSeekPending;
            VideoMsgQueuePtr pMsgQ = m_bVideoSeekPending ? m_pVMsgQ : m_pAMsgQ;
            VideoMsgPtr pMsg = pMsgQ->pop(true);
            SeekDoneVideoMsgPtr pSeekDoneMsg = dynamic_pointer_cast<SeekDoneVideoMsg>(pMsg);
            if (pSeekDoneMsg) {
                bSeekPending = false;
                if (pSeekDoneMsg->performedSeek()) {
                    m_LastVideoFrameTime = pSeekDoneMsg->getVideoFrameTime();
                    m_LastAudioFrameTime = pSeekDoneMsg->getAudioFrameTime();
                }
            }

        } while (m_bVideoSeekPending || m_bAudioSeekPending);
    }
}

}
