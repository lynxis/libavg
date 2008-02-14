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

#include "AVGNode.h"

#include "NodeDefinition.h"

#include "../base/XMLHelper.h"

#include <iostream>

using namespace std;

namespace avg {

NodeDefinition AVGNode::getNodeDefinition()
{
    return NodeDefinition("avg", Node::buildNode<AVGNode>)
        .extendDefinition(DivNode::getNodeDefinition())
        .addChild(NodeDefinition("%anyNode;"))
        .addArg("enablecrop", "true")
        .addArg("onkeyup", "")
        .addArg("onkeydown", "");
}

AVGNode::AVGNode (const ArgList& Args, Player * pPlayer)
    : DivNode(Args, pPlayer)
{
    m_bEnableCrop = Args.getBoolArg ("enablecrop");
    addEventHandler(Event::KEYUP, Event::NONE, 
            Args.getStringArg ("onkeyup"));
    addEventHandler(Event::KEYDOWN, Event::NONE, 
            Args.getStringArg ("onkeydown"));
    Node::setAngle(0.0);
}

AVGNode::~AVGNode()
{
}

string AVGNode::getTypeStr ()
{
    return "AVGNode";
}

bool AVGNode::getCropSetting() {
    return m_bEnableCrop;
}

void AVGNode::setAngle(double angle)
{
    
}

}
