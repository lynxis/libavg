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

#ifndef _Node_H_
#define _Node_H_

#include "Region.h"
#include "Event.h"
#include "ISurface.h"

#include "../base/Point.h"
#include "../base/Rect.h"

#include <libxml/parser.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <Python.h>
#undef HAVE_STAT
#undef HAVE_TEMPNAM

#include <vector>
#include <string>
#include <map>

namespace avg {

class Node;
class DivNode;
class AVGNode;
class Region;
class DisplayEngine;
class Player;
class OGLSurface;

typedef boost::shared_ptr<Node> NodePtr;
typedef boost::weak_ptr<Node> NodeWeakPtr;
typedef boost::shared_ptr<DivNode> DivNodePtr;
typedef boost::weak_ptr<DivNode> DivNodeWeakPtr;
typedef boost::shared_ptr<AVGNode> AVGNodePtr;
typedef boost::weak_ptr<AVGNode> AVGNodeWeakPtr;

class Node
{
    public:
        enum NodeState {NS_UNCONNECTED, NS_CONNECTED};
        
        virtual ~Node () = 0;
        virtual void setThis(NodeWeakPtr This);
        void setParent(DivNodeWeakPtr pParent);
        virtual void setDisplayEngine(DisplayEngine * pEngine);
        virtual void disconnect();
        
        virtual const std::string& getID () const;
        void setID(const std::string& ID);

        double getX() const;
        void setX(double x);
        
        double getY() const;
        void setY(double Y);
        
        virtual double getWidth();
        void setWidth(double width);
        
        virtual double getHeight();
        void setHeight(double height);
       
        DPoint getRelSize() const;

        double getAngle() const;
        void setAngle(double Angle);
        
        double getPivotX() const;
        void setPivotX(double Pivotx);
        
        double getPivotY() const;
        void setPivotY(double Pivoty);
        
        double getOpacity() const;
        void setOpacity(double opacity);
        
        bool getActive() const;
        void setActive(bool bActive);
        
        bool getSensitive() const;
        void setSensitive(bool bSensitive);

        virtual DivNodePtr getParent() const;
        DPoint getRelPos(const DPoint& AbsPos) const;

        void setMouseEventCapture();
        void releaseMouseEventCapture();
        void setEventCapture(int cursorID);
        void releaseEventCapture(int cursorID);
        void setEventHandler(Event::Type Type, Event::Source Source, PyObject * pFunc);

        bool isActive();
        bool reactsToMouseEvents();
        virtual NodePtr getElementByPos (const DPoint & pos);
        virtual void maybeRender (const DRect& Rect);
        virtual void render (const DRect& Rect);
        virtual void setViewport (double x, double y, double width, 
                double height);
        virtual const DRect& getRelViewport () const;
        virtual double getEffectiveOpacity();

        virtual std::string dump (int indent = 0);
        virtual std::string getTypeStr () const;
        
        virtual void handleEvent (Event* pEvent); 
        NodeState getState() const;
        bool isDisplayAvailable() const;
        
        // TODO: Do we still need this? Isn't rtti good enough?
        enum {NT_UNKNOWN, NT_IMAGE, NT_AVG, NT_VIDEO, NT_TEXT, NT_EXCL, 
                NT_CAMERA, NT_DIV, NT_PANOIMAGE};

    protected:
        Node (const xmlNodePtr xmlNode, Player * pPlayer);
        virtual DPoint getPreferredMediaSize() 
            { return DPoint(0,0); };
        DPoint getPivot() const;
        Player * getPlayer() const;
        DisplayEngine * getEngine() const;
        NodePtr getThis() const;

        void callPython (PyObject * pFunc, avg::Event* pEvent);
        void addEventHandlers(Event::Type EventType, const std::string& Code);
        void addEventHandler(Event::Type EventType, Event::Source Source, 
                const std::string& Code);
            
        void initFilename (Player * pPlayer, std::string& sFilename);
        void setState(NodeState State);
        DPoint toLocal(const DPoint& pos) const;
        DPoint toGlobal(const DPoint& pos) const;
 
    private:
        PyObject * findPythonFunc(const std::string& Code);

        DivNodeWeakPtr m_pParent;
        NodeWeakPtr m_This;
        DisplayEngine * m_pEngine;
        Player * m_pPlayer;

        std::string m_ID;

        struct EventHandlerID {
            EventHandlerID(Event::Type EventType, Event::Source Source);

            bool operator < (const EventHandlerID& other) const;

            Event::Type m_Type;
            Event::Source m_Source;
        };
        typedef std::map<EventHandlerID, PyObject *> EventHandlerMap;
        EventHandlerMap m_EventHandlerMap;

        DRect m_RelViewport;      // In coordinates relative to the parent.
        double m_Opacity;
        bool m_bActive;
        bool m_bSensitive;
        double m_Angle;
        DPoint m_Pivot;
        bool m_bHasCustomPivot;
        
        // Size specified by user.
        DPoint m_WantedSize;

        NodeState m_State;
};

}

#endif //_Node_H_
