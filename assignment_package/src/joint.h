#ifndef JOINT_H
#define JOINT_H

#include "drawable.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QTextStream>
#include <iostream>
#include <smartpointerhelp.h>

class Joint :public QTreeWidgetItem
{
public:
    static int last;
    int id;
    QString name;
    Joint* parent;
    std::vector<Joint*> children;
    glm::vec3 position;
    glm::quat rotation;
    glm::mat4 bindMatrix;

    static std::vector<uPtr<Joint>> skeleton;
    static std::vector<glm::mat4> transforms;
    static std::vector<glm::mat4> bindMats;

    glm::mat4 getLocalTransformation();
    glm::mat4 getOverallTransformation();

    Joint();
    static Joint* loadSkeleton(const QString filePath);
    static void clearJointData();
    void addChild(Joint* child);
    void resetMats();

};
Joint* createSkeleton(QJsonObject obj);
#endif // JOINT_H
