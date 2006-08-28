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

#ifndef _SDLDisplayEngine_H_
#define _SDLDisplayEngine_H_

#include "OGLHelper.h"
#include "OGLShader.h"
#include "IEventSource.h"
#include "DisplayEngine.h"
#include "../graphics/Bitmap.h"
#include "../graphics/Pixel32.h"

#include <SDL/SDL.h>

#include <string>
#include <vector>

namespace avg {

class OGLSurface;

class SDLDisplayEngine: public DisplayEngine, public IEventSource
{
    public:
        SDLDisplayEngine();
        virtual ~SDLDisplayEngine();

        // From DisplayEngine
        virtual void init(const DisplayParams& DP);
        virtual void teardown();
        virtual double getRefreshRate();
        virtual void setGamma(double Red, double Green, double Blue);

        virtual void render(AVGNodePtr pRootNode, bool bRenderEverything);
        
        virtual void setClipRect();
        virtual bool pushClipRect(const DRect& rc, bool bClip);
        virtual void popClipRect();
        virtual const DRect& getClipRect();
        virtual void blt32(ISurface * pSurface, const DRect* pDestRect, 
                double opacity, double angle, const DPoint& pivot,
                BlendMode Mode);
        virtual void blta8(ISurface * pSurface, const DRect* pDestRect,
                double opacity, const Pixel32& color, double angle, 
                const DPoint& pivot, BlendMode Mode);

        virtual ISurface * createSurface();
        virtual void surfaceChanged(ISurface * pSurface);

        virtual int getWidth();
        virtual int getHeight();
        virtual int getBPP();

        virtual bool supportsBpp(int bpp);
        virtual bool hasRGBOrdering();
        
        virtual YCbCrMode getYCbCrMode();
        OGLShaderPtr getYCbCr420pShader();
        
        virtual void showCursor (bool bShow);
        virtual BitmapPtr screenshot ();

        // From IEventSource
        virtual std::vector<Event *> pollEvents();

        // Texture config.
        int getTextureMode();
        int getMaxTexSize();
        
        int getOGLDestMode(PixelFormat pf);
        int getOGLSrcMode(PixelFormat pf);
        int getOGLPixelType(PixelFormat pf);
        OGLMemoryMode getMemoryModeSupported();

        void setOGLOptions(bool bUsePOW2Textures, YCbCrMode ycbcrMode, bool bUseRGBOrder,
                bool bUsePixelBuffers, int MultiSampleSamples);

    private:
        void initSDL(int width, int height, bool isFullscreen, int bpp);
        void initInput();
        void initTranslationTable();
        void initJoysticks();
        void logConfig(); 
        void setDirtyRect(const DRect& rc);
        virtual void swapBuffers();
        void clip();
        void setClipPlane(double Eqn[4], int WhichPlane);
        void safeSetAttribute( SDL_GLattr attr, int value);

        Event * createMouseMotionEvent 
                (Event::Type Type, const SDL_Event & SDLEvent);
        Event * createMouseButtonEvent
                (Event::Type Type, const SDL_Event & SDLEvent);
        Event * createKeyEvent
                (Event::Type Type, const SDL_Event & SDLEvent);
        
        int m_Width;
        int m_Height;
        bool m_IsFullscreen;
        int m_bpp;
        int m_WindowWidth;
        int m_WindowHeight;
        std::vector<DRect> m_ClipRects;
        DRect m_DirtyRect;
        bool m_bEnableCrop;

        SDL_Surface * m_pScreen;

        void checkYCbCrSupport();
        YCbCrMode m_YCbCrMode;
        OGLShaderPtr m_pYCbCrShader;

        // Vertical blank stuff.
        virtual bool initVBlank(int rate);
        bool vbWait(int rate);
        typedef enum VBMethod {VB_SGI, VB_APPLE, VB_DRI, VB_NONE};
        VBMethod m_VBMethod;
        int m_VBMod;
        int m_LastVBCount;
        bool m_bFirstVBFrame;
        int m_dri_fd;

        static void calcRefreshRate();
        static double s_RefreshRate;

        static std::vector<long> KeyCodeTranslationTable;

        // Texture config.
        int m_TextureMode;
        int m_MaxTexSize;

        bool m_bShouldUsePOW2Textures;
        YCbCrMode m_DesiredYCbCrMode;
        bool m_bShouldUseRGBOrder;
        bool m_bShouldUsePixelBuffers;
        int m_MultiSampleSamples;

        bool m_bCheckedMemoryMode;
        OGLMemoryMode m_MemoryMode;
};

}

#endif 
