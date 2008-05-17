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
#include "Player.h"

#include "NodeDefinition.h"

#include "../base/XMLHelper.h"

#include <iostream>

using namespace std;

namespace avg {

NodeDefinition AVGNode::getNodeDefinition()
{
    return NodeDefinition("avg", Node::buildNode<AVGNode>)
        .extendDefinition(DivNode::getNodeDefinition())
        .setGroupNode()
        .addArg(Arg<bool>("enablecrop", true, false, offsetof(AVGNode, m_bEnableCrop)))
        .addArg(Arg<string>("onkeyup", ""))
        .addArg(Arg<string>("onkeydown", ""));
}

AVGNode::AVGNode (const ArgList& Args, Player * pPlayer)
    : DivNode(Args, pPlayer)
{
    Args.setMembers(this);
    addEventHandler(Event::KEYUP, Event::NONE, Args.getArgVal<string>("onkeyup"));
    addEventHandler(Event::KEYDOWN, Event::NONE, Args.getArgVal<string>("onkeydown"));
    Node::setAngle(0.0);
}

AVGNode::~AVGNode()
{
}

string AVGNode::getTypeStr ()
{
    return "AVGNode";
}

string AVGNode::getEffectiveMediaDir()
{
    string sMediaDir;
    sMediaDir = getPlayer()->getCurDirName()+getMediaDir();
    if (sMediaDir[sMediaDir.length()-1] != '/') {
        sMediaDir += '/';
    }
    return sMediaDir;
}

bool AVGNode::getCropSetting() {
    return m_bEnableCrop;
}

void AVGNode::setAngle(double angle)
{
    
}

}
