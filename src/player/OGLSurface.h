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

#ifndef _OGLSurface_H_
#define _OGLSurface_H_

#include "../api.h"
#include "OGLTexture.h"

#include "../base/Point.h"

#include "../graphics/Bitmap.h"
#include "../graphics/OGLHelper.h"

#include <vector>
#include <string>

namespace avg {

class SDLDisplayEngine;

class AVG_API OGLSurface {
    public:
        OGLSurface(const MaterialInfo& material);
        virtual ~OGLSurface();

        virtual void create(SDLDisplayEngine * pEngine, const IntPoint& Size, 
                PixelFormat PF, bool bFastDownload);
        virtual void destroy();
        virtual BitmapPtr lockBmp(int index=0);
        virtual void unlockBmps();
        const MaterialInfo& getMaterial() const;
        void setMaterial(const MaterialInfo& material);

        void bindPBO(int index=0);
        void unbindPBO();
        void downloadTexture();
        OGLTexturePtr getTexture();

        PixelFormat getPixelFormat();
        IntPoint getSize();
        OGLMemoryMode getMemMode() const;
        BitmapPtr getBmp(int i=0);

    private:
        void createBitmap(const IntPoint& Size, PixelFormat pf, int index);
        void deleteBuffers();
        void unlockBmp(int i);

        bool m_bCreated;

        BitmapPtr m_pBmps[3];
        OGLTexturePtr m_pTexture;
        IntPoint m_Size;
        PixelFormat m_pf;
        MaterialInfo m_Material;

        OGLMemoryMode m_MemoryMode;

        // PBO memory mode
        GLuint m_hPixelBuffers[3];
};

}

#endif

