#ifndef HALFEDGEDISPLAY_H
#define HALFEDGEDISPLAY_H

#include "drawable.h"
#include "halfedge.h"
#include "mesh.h"

class HalfEdgeDisplay : public Drawable
{
protected:
    HalfEdge* representedHalfEdge;
public:
    HalfEdgeDisplay(OpenGLContext* context);
    void create() override;
    GLenum drawMode() override;
    void updateHalfEdge(HalfEdge* he);
};

#endif // HALFEDGEDISPLAY_H
