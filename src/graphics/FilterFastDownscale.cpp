//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2011 Ulrich von Zadow
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

#include "FilterFastDownscale.h"
#include "Pixeldefs.h"

#include "../base/Exception.h"

#include <iostream>

namespace avg {

using namespace std;
    
FilterFastDownscale::FilterFastDownscale(int factor) 
    : Filter(),
      m_Factor(factor)
{
}

FilterFastDownscale::~FilterFastDownscale()
{

}

BitmapPtr FilterFastDownscale::apply(BitmapPtr pBmpSrc) 
{
    AVG_ASSERT(pBmpSrc->getPixelFormat() == I8);
    BitmapPtr pBmpDest = BitmapPtr(new Bitmap(pBmpSrc->getSize()/m_Factor, I8,
             pBmpSrc->getName()));
    unsigned char * pSrcLine = pBmpSrc->getPixels();
    unsigned char * pDestLine = pBmpDest->getPixels();
    IntPoint size = pBmpDest->getSize();
    int srcStride = pBmpSrc->getStride();
    for (int y = 0; y < size.y; ++y) {
        unsigned char * pSrcPixel = pSrcLine;
        unsigned char * pDstPixel = pDestLine;
        switch (m_Factor) {
            case 2:
                for (int x = 0; x < size.x; ++x) {
                    int dstPixel= int(*pSrcPixel)+int(*(pSrcPixel+1))
                            +int(*(pSrcPixel+srcStride))+int(*(pSrcPixel+srcStride+1));
                    *pDstPixel = (dstPixel+2)/4;
                    pSrcPixel += 2;
                    pDstPixel++;
                }
                break;
            case 3:
                for (int x = 0; x < size.x; ++x) {
                    int dstPixel= int(*pSrcPixel)+int(*(pSrcPixel+1))+int(*(pSrcPixel+2))
                            +int(*(pSrcPixel+srcStride))+int(*(pSrcPixel+srcStride+1))
                                    +int(*(pSrcPixel+srcStride+2))
                            +int(*(pSrcPixel+srcStride*2))+int(*(pSrcPixel+srcStride*2+1))
                                    +int(*(pSrcPixel+srcStride*2+2));
                    *pDstPixel = (dstPixel+4)/9;
                    pSrcPixel += 3;
                    pDstPixel++;
                }
                break;
            default:
                for (int x = 0; x < size.x; ++x) {
                    int dstPixel=0;
                    for (int y1 = 0; y1 < m_Factor; y1++) {
                        for (int x1 = 0; x1 < m_Factor; x1++) {
                            dstPixel+= (int)(*(pSrcPixel+srcStride*y1+x1));
                        }
                    }
                    *pDstPixel = (dstPixel+(m_Factor*m_Factor)/2)/(m_Factor*m_Factor);
                    pSrcPixel += m_Factor;
                    pDstPixel++;
                }
                break;
        }
        pSrcLine = pSrcLine + pBmpSrc->getStride()*m_Factor;
        pDestLine = pDestLine + pBmpDest->getStride();
    }
    return pBmpDest;
}

} // namespace
