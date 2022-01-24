#ifndef HALFEDGE_H
#define HALFEDGE_H
#include <vertex.h>
#include <face.h>
#include <la.h>
#include <QListWidget>

class Face;
class Vertex;

class HalfEdge: public QListWidgetItem
{
public:
    static int last;
    HalfEdge* next;
    HalfEdge* sym;
    Face* face;
    Vertex* vertex;
    int id;
    QString name;
    HalfEdge();
    HalfEdge* getPrev();
};

#endif // HALFEDGE_H
