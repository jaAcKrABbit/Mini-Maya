#include "vertexdisplay.h"
#include <iostream>
VertexDisplay::VertexDisplay(OpenGLContext* context): Drawable(context), representedVertex(nullptr)
{

}

void VertexDisplay::create() {

    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> color;
    std::vector<GLuint> idx;


    pos.push_back(glm::vec4(representedVertex->position,1));
    color.push_back(glm::vec4(1,1,1,1));
    idx.push_back(0);

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

GLenum VertexDisplay::drawMode() {
    return GL_POINTS;
}

void VertexDisplay::updateVertex(Vertex* vert) {
    representedVertex = vert;
}
