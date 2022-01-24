#ifndef MESH_H
#define MESH_H

#include "drawable.h"
#include <smartpointerhelp.h>
#include <halfedge.h>
#include <face.h>
#include <vertex.h>
#include <iostream>
#include <utils.h>
#include <map>
#include <unordered_map>
#include <QFileDialog>

#define COLOR_RED  glm::vec3(1, 0, 0)
#define COLOR_GREEN glm::vec3(0, 1, 0)
#define COLOR_BLUE glm::vec3(0, 0, 1)
#define COLOR_YELLOW glm::vec3(1, 1, 0)
#define COLOR_PURPLE glm::vec3(1, 0, 1)
#define COLOR_CYAN glm::vec3(0, 1, 1)

class Face;
class HalfEdge;
class Vertex;

class Mesh : public Drawable
{
public:
    std::vector<uPtr<Face>> faces;
    std::vector<uPtr<HalfEdge>> halfEdges;
    std::vector<uPtr<Vertex>> vertices;

    bool binded;

    void create() override;
    GLenum drawMode() override;
    void createCube();
    void setVertexForEdge();
    void setSymForEdge();
    void splitEdge(HalfEdge* he1);
    void triangulate(Face* face);
    void subdivide();
    void quadrangulate(Face* face, Vertex* centroid,
                       std::vector<HalfEdge*> &edges);
    void createObj(const QString filePath);
    void extrude(Face* face);
    void clearMeshData();
    void bindSkeleton();

    Mesh(OpenGLContext* context);
};




#endif // MESH_H
