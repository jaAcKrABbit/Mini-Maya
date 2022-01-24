#ifndef FACE_H
#define FACE_H
#include <halfedge.h>
#include <la.h>
#include <QListWidget>

class HalfEdge;

class Face: public QListWidgetItem
{
public:
    static int last;
    HalfEdge* halfEdge;
    glm::vec3 color;
    int id;
    QString name;
    Face();
};

#endif // FACE_H
