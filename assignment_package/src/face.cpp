#include "face.h"

int Face::last = 0;

Face::Face(): QListWidgetItem(), halfEdge(nullptr), color(), id(last), name("Face")
{
    this->setText(name + "_" + QString::number(last));
    last++;
}
