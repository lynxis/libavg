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

#include "BlurFXNode.h"
#include "SDLDisplayEngine.h"

#include "../base/ObjectCounter.h"
#include "../graphics/ShaderRegistry.h"

#include <string>

using namespace std;

namespace avg {

BlurFXNode::BlurFXNode() 
    : FXNode(),
      m_StdDev(1)
{
    ObjectCounter::get()->incRef(&typeid(*this));
}

BlurFXNode::~BlurFXNode()
{
    ObjectCounter::get()->decRef(&typeid(*this));
}

void BlurFXNode::disconnect()
{
    m_pFilter = GPUBlurFilterPtr();
    FXNode::disconnect();
}

void BlurFXNode::setParam(double stdDev)
{
    m_StdDev = stdDev;
    if (m_pFilter) {
        m_pFilter->setParam(stdDev);
    }
}

GPUFilterPtr BlurFXNode::createFilter(const IntPoint& size)
{
    m_pFilter = GPUBlurFilterPtr(new GPUBlurFilter(size, B8G8R8A8, B8G8R8A8, m_StdDev, 
            false));
    return m_pFilter;
}

}

