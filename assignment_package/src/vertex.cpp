#include "vertex.h"
#include <iostream>
using namespace std;
int Vertex::last = 0;

Vertex::Vertex():QListWidgetItem(), position(), halfEdge(nullptr),id(last),name("Vertex"){
    this->setText(name + "_" + QString::number(last));
    last++;
}
Vertex::Vertex(float x, float y, float z):QListWidgetItem(), position(glm::vec3(x,y,z)), halfEdge(nullptr),id(last),name("Vertex")
{  
    this->setText(name + "_" + QString::number(last));
    last++;
}
