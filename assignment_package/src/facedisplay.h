#ifndef FACEDISPLAY_H
#define FACEDISPLAY_H

#include "drawable.h"
#include "face.h"
class FaceDisplay : public Drawable
{
protected:
    Face* representedFace;
public:
    FaceDisplay(OpenGLContext* context);
    void create() override;
    GLenum drawMode() override;
    void updateFace(Face* face);
};

#endif // FACEDISPLAY_H
