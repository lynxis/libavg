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

#ifndef _RectNode_H_
#define _RectNode_H_

#include "../api.h"
#include "FilledVectorNode.h"

#include "../graphics/Pixel32.h"

namespace avg {

class AVG_API RectNode : public FilledVectorNode
{
    public:
        static NodeDefinition createDefinition();
        
        RectNode(const ArgList& Args);
        virtual ~RectNode();

        const DPoint& getPos() const;
        void setPos(const DPoint& pt);

        DPoint getSize() const;
        void setSize(const DPoint& pt);

        const std::vector<double>& getTexCoords() const;
        void setTexCoords(const std::vector<double>& coords);

        double getAngle() const;
        void setAngle(double angle);

        NodePtr getElementByPos(const DPoint & pos);

        virtual int getNumVertexes();
        virtual int getNumIndexes();
        virtual int getNumFillVertexes();
        virtual int getNumFillIndexes();
        virtual void calcVertexes(VertexArrayPtr& pVertexArray, Pixel32 color);
        virtual void calcFillVertexes(VertexArrayPtr& pVertexArray, Pixel32 color);

    private:
        DRect m_Rect;
        std::vector<double> m_TexCoords;

        double m_Angle;
};

}

#endif

