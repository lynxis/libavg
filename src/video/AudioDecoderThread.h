//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2008 Ulrich von Zadow
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
//  Original author of this file is Nick Hebner (hebnern@gmail.com).
//

#ifndef _AudioDecoderThread_H_
#define _AudioDecoderThread_H_

#include "../avgconfigwrapper.h"
#include "IVideoDecoder.h"
#include "VideoMsg.h"

#include "../base/WorkerThread.h"
#include "../base/Command.h"
#include "../audio/AudioParams.h"

#include <boost/thread.hpp>

#include <string>

namespace avg {

class AVG_API AudioDecoderThread : public WorkerThread<AudioDecoderThread> {
    public:
        AudioDecoderThread(CQueue& CmdQ, VideoMsgQueue& MsgQ, 
                VideoDecoderPtr pDecoder, const AudioParams& AP);
        virtual ~AudioDecoderThread();
        
        bool work();
        void seek(double DestTime);
        void setVolume(double Volume);

    private:
        VideoMsgQueue& m_MsgQ;
        VideoDecoderPtr m_pDecoder;
        AudioParams m_AP;
};

}
#endif 

