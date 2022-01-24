#include "skeletondisplay.h"

SkeletonDisplay::SkeletonDisplay(OpenGLContext* context): Drawable(context), representedJoint(nullptr)
{

}

void SkeletonDisplay::create() {
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> color;
    std::vector<GLuint> idx;
    const glm::vec3 x_axis = glm::vec3(1, 0, 0);
    const glm::vec3 y_axis = glm::vec3(0, 1, 0);
    const glm::vec3 z_axis = glm::vec3(0, 0, 1);
    const float radius = 0.5f;
    glm::mat4 jointTransform;
    //circle drawer w.r.t local position
    glm::vec4 p;
    //vert counter
    int j = 0;
    //draw sphere
    for(auto& joint: Joint::skeleton) {
        glm::vec4 overallPos;
        jointTransform = joint->getOverallTransformation();
        //start point
        p = glm::vec4(radius, 0, 0, 1);
        //about z, the blue one
        for(int i = 0; i < 12; i++) {
            overallPos = jointTransform * p;
            p = glm::rotate(p, glm::radians(30.f), z_axis);
            pos.push_back(overallPos);
            if(representedJoint){
                if(joint->name == representedJoint->name) {
                    color.push_back(glm::vec4(1, 1, 1, 1));
                }else {
                    color.push_back(glm::vec4(0, 0, 1, 1));
                }
            }else{
                color.push_back(glm::vec4(0, 0, 1, 1));
            }


            if(i == 11) {
                idx.push_back(j * 12 + i);
                idx.push_back(j * 12);
            }else{
                idx.push_back(j * 12 + i);
                idx.push_back(j * 12 + i + 1);
            }
        }
        j++;
        //about x, red
        p = glm::vec4(0, radius, 0, 1);
        for(int i = 0; i < 12; i++) {
            overallPos = jointTransform * p;
            p = glm::rotate(p, glm::radians(30.f), x_axis);
            pos.push_back(overallPos);
            if(representedJoint){
                if(joint->name == representedJoint->name) {
                    color.push_back(glm::vec4(1, 1, 1, 1));
                }else {
                    color.push_back(glm::vec4(1, 0, 0, 1));
                }
            }else{
                color.push_back(glm::vec4(1, 0, 0, 1));
            }

            if(i == 11) {
                idx.push_back(j * 12 + i);
                idx.push_back(j * 12);
            }else{
                idx.push_back(j * 12 + i);
                idx.push_back(j * 12 + i + 1);
            }
        }
        j++;
        //about y, green
        p = glm::vec4(radius,0,0,1);
        for(int i = 0; i < 12; i++) {
            overallPos = jointTransform * p;
            p = glm::rotate(p, glm::radians(30.f), y_axis);
            pos.push_back(overallPos);
            if(representedJoint){
                if(joint->name == representedJoint->name) {
                    color.push_back(glm::vec4(1, 1, 1, 1));
                }else {
                    color.push_back(glm::vec4(0, 1, 0, 1));
                }
            }else{
                color.push_back(glm::vec4(0, 1, 0, 1));
            }

            if(i == 11) {
                idx.push_back(j * 12 + i);
                idx.push_back(j * 12);
            }else{
                idx.push_back(j * 12 + i);
                idx.push_back(j * 12 + i + 1);
            }
        }
        j++;
    }
    // index， hardcoded
    int m = 1;
    int n = idx.back() + 11;
    //draw connections
    for(auto& joint: Joint::skeleton) {
        jointTransform = joint->getOverallTransformation();
        glm::vec4 jointPos = jointTransform * glm::vec4(0, 0, 0, 1);
        glm::vec4 parentPos;
        //draw connections
        if(joint->parent) {
            parentPos = joint->parent->getOverallTransformation() * glm::vec4(0, 0, 0, 1);
            pos.push_back(parentPos);
            pos.push_back(jointPos);
            color.push_back(glm::vec4(1, 0, 1, 1));
            color.push_back(glm::vec4(1, 1, 0, 1));
            idx.push_back(n + m);
            idx.push_back(n + m + 1);
            m += 2;
        }

    }
    count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), color.data(), GL_STATIC_DRAW);
}

GLenum SkeletonDisplay::drawMode() {
    return GL_LINES;
}

void SkeletonDisplay::updateJoint(Joint* joint) {
    representedJoint = joint;
}


