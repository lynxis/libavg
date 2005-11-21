//
// $Id$
// 

#ifndef _RasterNode_H_
#define _RasterNode_H_

#include "Node.h"
#include "DisplayEngine.h"
#include "../graphics/Point.h"

#include <string>

#include "../avgconfig.h"

namespace avg {

class RasterNode: public Node
{
    public:
        virtual ~RasterNode ();
        void initVisible();
        
        // Warping support.
        int getNumVerticesX();
        int getNumVerticesY();
        DPoint getOrigVertexCoord(int x, int y);
        DPoint getWarpedVertexCoord(int x, int y);
        void setWarpedVertexCoord(int x, int y, const DPoint& Vertex);

        double getAngle() const;
        void setAngle(double Angle);
        double getPivotX() const;
        void setPivotX(double Pivotx);
        double getPivotY() const;
        void setPivotY(double Pivoty);

        int getMaxTileWidth() const
        {
            return m_MaxTileSize.x;
        }

        int getMaxTileHeight() const
        {
            return m_MaxTileSize.y;
        }
        
        const std::string& getBlendModeStr() const;
        void setBlendModeStr(const std::string& sBlendMode);
#ifdef AVG_ENABLE_GL
        OGLSurface * RasterNode::getOGLSurface();
#endif
        DisplayEngine::BlendMode getBlendMode() const;
        virtual std::string getTypeStr ();
        Node * getElementByPos (const DPoint & pos);
        
    protected:
        RasterNode ();
        RasterNode (const xmlNodePtr xmlNode, Container * pParent);
        DPoint getPivot();
        ISurface * getSurface();
 
    private:
        ISurface * m_pSurface;

        double m_Angle;
        bool m_bHasCustomPivot;
        DPoint m_Pivot;
        
        IntPoint m_MaxTileSize;
        std::string m_sBlendMode;
        DisplayEngine::BlendMode m_BlendMode;
};

}

#endif 

