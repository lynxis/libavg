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

#include "TestHelper.h"
#include "Player.h"
#include "MouseEvent.h"
#include "TouchEvent.h"
#include "KeyEvent.h"

#include "../base/Exception.h"
#include "../base/ObjectCounter.h"

#include <iostream>
#include <limits>

using namespace std;

namespace avg {
    
TestHelper::TestHelper(Player * pPlayer)
    : m_pPlayer(pPlayer)
{
}

TestHelper::~TestHelper() 
{
}

void TestHelper::useFakeCamera(bool bFake)
{
    m_pPlayer->useFakeCamera(bFake);
}

void TestHelper::fakeMouseEvent(Event::Type eventType,
        bool leftButtonState, bool middleButtonState, 
        bool rightButtonState,
        int xPosition, int yPosition, int button)
{
    checkEventType(eventType);
    MouseEventPtr pEvent(new MouseEvent(eventType, leftButtonState, 
            middleButtonState, rightButtonState, IntPoint(xPosition, yPosition), button,
            DPoint(0,0)));
    m_Events.push_back(pEvent);
}

void TestHelper::fakeTouchEvent(int id, Event::Type eventType,
        Event::Source source, const DPoint& pos, const DPoint& lastDownPos,
        const DPoint& speed)
{
    checkEventType(eventType);
    BlobPtr pBlob(new Blob(Run(int(pos.y), int(pos.x), int(pos.x)+1)));
    pBlob->calcStats();
    // The id is modified to avoid collisions with real touch events.
    TouchEventPtr pEvent(new TouchEvent(id+numeric_limits<int>::max()/2, eventType, 
            pBlob, IntPoint(pos), source, speed, IntPoint(lastDownPos)));

    m_Events.push_back(pEvent);
}


void TestHelper::fakeKeyEvent(Event::Type eventType,
        unsigned char scanCode, int keyCode, 
        const string& keyString, int unicode, int modifiers)
{
    KeyEventPtr pEvent(new KeyEvent(eventType, scanCode, keyCode, 
        keyString, unicode, modifiers));
    m_Events.push_back(pEvent);
}

void TestHelper::dumpObjects()
{
    cerr << ObjectCounter::get()->dump();
}

// From IEventSource
std::vector<EventPtr> TestHelper::pollEvents()
{
    vector<EventPtr> TempEvents = m_Events;
    m_Events.clear();
    return TempEvents;
}

void TestHelper::checkEventType(Event::Type eventType)
{
    if (eventType == Event::CURSOROVER || eventType == Event::CURSOROUT) {
        throw Exception(AVG_ERR_UNSUPPORTED, "TestHelper::fakeXxxEvent: Can't send "
                "CURSOROVER and CURSOROUT events directly. They are generated "
                "internally.");
    }
}
    
}
    
