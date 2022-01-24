#include "halfedgedisplay.h"

HalfEdgeDisplay::HalfEdgeDisplay(OpenGLContext* context): Drawable(context),representedHalfEdge(nullptr)
{

}

void HalfEdgeDisplay::create() {

    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> color;
    std::vector<GLuint> idx = {0,1};


    pos.push_back(glm::vec4(representedHalfEdge->vertex->position,1));
    pos.push_back(glm::vec4(representedHalfEdge->sym->vertex->position,1));
    color.push_back(glm::vec4(COLOR_YELLOW,1));
    color.push_back(glm::vec4(COLOR_RED,1));


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

GLenum HalfEdgeDisplay::drawMode() {
    return GL_LINES;
}

void HalfEdgeDisplay::updateHalfEdge(HalfEdge* he) {
    representedHalfEdge = he;
}
