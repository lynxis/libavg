//
// $Id$
// 

#ifndef _AVGDFBDisplayEngine_H_
#define _AVGDFBDisplayEngine_H_

#include "AVGFontManager.h"

#include <directfb/directfb.h>
#include <paintlib/plrect.h>

#include <string>

class PLBmp;
class PLDirectFBBmp;
class PLPoint;


class AVGDFBDisplayEngine
{
    public:
        AVGDFBDisplayEngine();
        virtual ~AVGDFBDisplayEngine();
        virtual void init(int width, int height, bool isFullscreen);
        virtual void teardown();

        virtual void setClipRect();
        virtual void setClipRect(const PLRect& rc);
        virtual void setDirtyRect(const PLRect& rc);
        virtual void clear();
        virtual void render(PLBmp * pBmp, const PLPoint& pos, double opacity);
        virtual void render(IDirectFBSurface * pSrc, const PLPoint& pos, 
                double opacity, bool bAlpha);
        virtual void swapBuffers();

	    virtual PLBmp * createSurface();
        void dumpSurface(IDirectFBSurface * pSurf, const std::string & name);

        IDirectFB * getDFB();
        IDirectFBSurface * getPrimary();
        IDirectFBEventBuffer * getEventBuffer();
        AVGFontManager * getFontManager();
        int getWidth();
        int getHeight();
        void DFBErrorCheck(int avgcode, DFBResult dfbcode); 

    private:
        void initInput();

        int m_Width;
        int m_Height;
        bool m_IsFullscreen;
        PLRect m_ClipRect;
        PLRect m_DirtyRect;

        IDirectFB * m_pDirectFB;
        IDirectFBWindow * m_pDFBWindow;
        IDirectFBDisplayLayer * m_pDFBLayer;
        IDirectFBSurface * m_pPrimary;
        
        IDirectFBEventBuffer * m_pEventBuffer;

        AVGFontManager *m_pFontManager;
};

#endif //_AVGDFBDisplayEngine_H_
