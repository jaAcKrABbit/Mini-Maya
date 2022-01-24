#include "facedisplay.h"
#include "mesh.h"
#include <iostream>
FaceDisplay::FaceDisplay(OpenGLContext* context): Drawable(context),representedFace(nullptr)
{

}

void FaceDisplay::create() {

    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> color;
    std::vector<GLuint> idx;
    HalfEdge* e = representedFace->halfEdge;
    GLuint edgeCount = 0;
    do{
        pos.push_back(glm::vec4(e->vertex->position,1));
        color.push_back(glm::vec4(1) - glm::vec4(representedFace->color,1));
        idx.push_back(edgeCount);
        idx.push_back(++edgeCount);
        e = e->next;
    }while(e != representedFace->halfEdge);
    //last element is 0
    idx.back() = 0;

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

GLenum FaceDisplay::drawMode() {
    return GL_LINES;
}

void FaceDisplay::updateFace(Face* face) {
    representedFace = face;
}
