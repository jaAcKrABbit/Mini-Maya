#include "halfedge.h"

int HalfEdge::last = 0;

HalfEdge::HalfEdge(): QListWidgetItem(),next(nullptr), sym(nullptr), face(nullptr), vertex(nullptr), id(last), name("HalfEdge")
{
    this->setText(name + "_" + QString::number(last));
    last++;
}

HalfEdge* HalfEdge::getPrev() {
    HalfEdge* current = this;
    while (current->next != this){
        current = current->next;
    }
    return current;
}
