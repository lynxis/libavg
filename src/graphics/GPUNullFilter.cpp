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

#include "GPUNullFilter.h"
#include "Bitmap.h"
#include "ShaderRegistry.h"
#include "OGLShader.h"
#include "BitmapLoader.h"

#include "../base/ObjectCounter.h"
#include "../base/Exception.h"

#include <iostream>

#define SHADERID "null"

using namespace std;

namespace avg {

GPUNullFilter::GPUNullFilter(const IntPoint& size, bool bStandalone)
    : GPUFilter(SHADERID, true, bStandalone)
{
    ObjectCounter::get()->incRef(&typeid(*this));

    setDimensions(size);
    m_pTextureParam = getShader()->getParam<int>("u_Texture");
}

GPUNullFilter::~GPUNullFilter()
{
    ObjectCounter::get()->decRef(&typeid(*this));
}

void GPUNullFilter::applyOnGPU(GLTexturePtr pSrcTex)
{
    getShader()->activate();
    m_pTextureParam->set(0);
    draw(pSrcTex);

}

}
