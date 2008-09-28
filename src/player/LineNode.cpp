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

#include "LineNode.h"

#include "NodeDefinition.h"

#include "../graphics/VertexArray.h"
#include "../base/Exception.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace avg {

NodeDefinition LineNode::createDefinition()
{
    return NodeDefinition("line", Node::buildNode<LineNode>)
        .extendDefinition(VectorNode::createDefinition())
        .addArg(Arg<double>("x1", 0, true, offsetof(LineNode, m_P1.x)))
        .addArg(Arg<double>("y1", 0, true, offsetof(LineNode, m_P1.y)))
        .addArg(Arg<double>("x2", 0, true, offsetof(LineNode, m_P2.x)))
        .addArg(Arg<double>("y2", 0, true, offsetof(LineNode, m_P2.y)));
}

LineNode::LineNode(const ArgList& Args, bool bFromXML)
    : VectorNode(Args)
{
    Args.setMembers(this);
}

LineNode::~LineNode()
{
}

double LineNode::getX1() const 
{
    return m_P1.x;
}

void LineNode::setX1(double x) 
{
    m_P1.x = x;
    setDrawNeeded(true);
}

double LineNode::getY1() const 
{
    return m_P1.y;
}

void LineNode::setY1(double y) 
{
    m_P1.y = y;
    setDrawNeeded(true);
}

const DPoint& LineNode::getPos1() const 
{
    return m_P1;
}

void LineNode::setPos1(const DPoint& pt) 
{
    m_P1 = pt;
    setDrawNeeded(true);
}

double LineNode::getX2() const 
{
    return m_P2.x;
}

void LineNode::setX2(double x) 
{
    m_P2.x = x;
    setDrawNeeded(true);
}

double LineNode::getY2() const 
{
    return m_P2.y;
}

void LineNode::setY2(double y) 
{
    m_P2.y = y;
    setDrawNeeded(true);
}

const DPoint& LineNode::getPos2() const 
{
    return m_P2;
}

void LineNode::setPos2(const DPoint& pt) 
{
    m_P2 = pt;
    setDrawNeeded(true);
}

int LineNode::getNumTriangles()
{
    return 2;
}

void LineNode::updateData(VertexArrayPtr pVertexArray, int triIndex, double opacity, 
        bool bParentDrawNeeded)
{
    if (isDrawNeeded() || bParentDrawNeeded) {
        double curOpacity = opacity*getOpacity();
        Pixel32 color = getColorVal();
        color.setA(curOpacity*255);

        DPoint m = (m_P2-m_P1);
        m.normalize();
        DPoint w = DPoint(m.y, -m.x)*getStrokeWidth()/2;
        pVertexArray->setPos(triIndex, 0, m_P1-w, DPoint(0,0), color);
        pVertexArray->setPos(triIndex, 1, m_P1+w, DPoint(0,0), color);
        pVertexArray->setPos(triIndex, 2, m_P2+w, DPoint(0,0), color);

        pVertexArray->setPos(triIndex+1, 0, m_P1-w, DPoint(0,0), color);
        pVertexArray->setPos(triIndex+1, 1, m_P2+w, DPoint(0,0), color);
        pVertexArray->setPos(triIndex+1, 2, m_P2-w, DPoint(0,0), color);
    }
    setDrawNeeded(false);
}

}
