#ifndef VERTEX_H
#define VERTEX_H
#include <la.h>
#include <halfedge.h>
#include <joint.h>
#include <QListWidget>
class HalfEdge;

class Vertex: public QListWidgetItem
{
public:
    static int last;
    glm::vec3 position;
    HalfEdge* halfEdge;
    int id;
    QString name;
    std::vector<std::pair<Joint*, float>> influenceJoints;
    Vertex();
    Vertex(float x, float y, float z);
};

#endif // VERTEX_H
