#ifndef SKELETONDISPLAY_H
#define SKELETONDISPLAY_H

#include "drawable.h"
#include "joint.h"
#include "glm/gtx/rotate_vector.hpp"

class SkeletonDisplay : public Drawable
{
protected:
    Joint* representedJoint;
public:
    SkeletonDisplay(OpenGLContext* context);
    void create() override;
    GLenum drawMode() override;
    void updateJoint(Joint* joint);
};

#endif // SKELETONDISPLAY_H
