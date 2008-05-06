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

#include "VideoDecoderThread.h"
#include "FrameVideoMsg.h"
#include "ErrorVideoMsg.h"
#include "EOFVideoMsg.h"
#include "SeekDoneVideoMsg.h"

#include "../base/Logger.h"

using namespace std;

namespace avg {

VideoDecoderThread::VideoDecoderThread(CmdQueue& CmdQ, VideoMsgQueue& MsgQ, 
        VideoDecoderPtr pDecoder)
    : WorkerThread<VideoDecoderThread>(string("Video Decoder"), CmdQ),
      m_MsgQ(MsgQ),
      m_pDecoder(pDecoder)
{
}

VideoDecoderThread::~VideoDecoderThread()
{
}

bool VideoDecoderThread::work() 
{
    if (m_pDecoder->isEOF(SS_VIDEO)) {
        // replace this with waitForMessage()
        msleep(10);
    } else {
        vector<BitmapPtr> pBmps;
        IntPoint Size = m_pDecoder->getSize();
        PixelFormat PF = m_pDecoder->getPixelFormat();
        FrameAvailableCode FrameAvailable;
        if (PF == YCbCr420p || PF ==YCbCrJ420p) {
            BitmapPtr pBmpY = BitmapPtr(new Bitmap(Size, I8));
            IntPoint HalfSize(Size.x/2, Size.y/2);
            BitmapPtr pBmpU = BitmapPtr(new Bitmap(HalfSize, I8));
            BitmapPtr pBmpV = BitmapPtr(new Bitmap(HalfSize, I8));
            FrameAvailable = 
                    m_pDecoder->renderToYCbCr420p(pBmpY, pBmpU, pBmpV, -1);
            if (FrameAvailable == FA_NEW_FRAME) {
                pBmps.push_back(pBmpY);
                pBmps.push_back(pBmpU);
                pBmps.push_back(pBmpV);
            }
        } else {
            BitmapPtr pBmp = BitmapPtr(new Bitmap(Size, PF));
            FrameAvailable = m_pDecoder->renderToBmp(pBmp, -1);
            if (FrameAvailable == FA_NEW_FRAME) {
                pBmps.push_back(pBmp);
            }
        }
        if (m_pDecoder->isEOF(SS_VIDEO)) {
            m_MsgQ.push(VideoMsgPtr(new EOFVideoMsg()));
        } else {
            assert(FrameAvailable == FA_NEW_FRAME);
            m_MsgQ.push(VideoMsgPtr(new FrameVideoMsg(pBmps, 
                    m_pDecoder->getCurTime(SS_VIDEO))));
        }
    }
    return true;
}

void VideoDecoderThread::seek(long long DestTime)
{
    try {
        while (!m_MsgQ.empty()) {
            m_MsgQ.pop(false);
        }
    } catch (Exception&) {
    }

    bool performSeek = m_pDecoder->hasVideo();
    long long VideoFrameTime = -1;
    long long AudioFrameTime = -1;
    if (performSeek) {
        m_pDecoder->seek(DestTime);
        VideoFrameTime = m_pDecoder->getCurTime(SS_VIDEO);
        if (m_pDecoder->hasAudio()) {
            AudioFrameTime = m_pDecoder->getCurTime(SS_AUDIO);
        }
    }
    
    m_MsgQ.push(VideoMsgPtr(new SeekDoneVideoMsg(performSeek, 
                VideoFrameTime, AudioFrameTime)));
}

}
