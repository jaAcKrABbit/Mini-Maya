#include "joint.h"

std::vector<uPtr<Joint>> Joint::skeleton;
std::vector<glm::mat4> Joint::transforms;
std::vector<glm::mat4> Joint::bindMats;

int Joint::last = 0;

Joint::Joint():id(last++), name(), parent(nullptr), children{},
    position(), rotation(), bindMatrix()
{

}


Joint* Joint::loadSkeleton(const QString filePath) {
    Joint::clearJointData();
    QFile jsonFile(filePath);
    if(!jsonFile.open(QIODevice::ReadOnly)){
       qWarning("Could not open the JSON file.");
       return nullptr;
    }
    QByteArray file_data = jsonFile.readAll();
    QJsonDocument jdoc(QJsonDocument::fromJson(file_data));
    QJsonObject root = jdoc.object()["root"].toObject();
    return createSkeleton(root);

}

Joint* createSkeleton(QJsonObject obj) {
    uPtr<Joint> joint =mkU<Joint>();
    QString jointName = obj["name"].toString();
    QJsonArray pos = obj["pos"].toArray();
    QJsonArray rot = obj["rot"].toArray();
    QJsonArray children = obj["children"].toArray();
    joint->name = jointName;
    joint->QTreeWidgetItem::setText(0, joint->name);
    joint->parent = nullptr;
    joint->position = glm::vec3(pos[0].toDouble(), pos[1].toDouble(), pos[2].toDouble());
    float angle = (float)rot[0].toDouble();
    glm::vec3 axis = glm::vec3(rot[1].toDouble(), rot[2].toDouble(), rot[3].toDouble());
    joint->rotation = glm::angleAxis(angle,axis);
    Joint* current = joint.get();
    Joint::skeleton.push_back(std::move(joint));
    for(auto child: children) {
        auto childObj = child.toObject();
        Joint* childPtr = createSkeleton(childObj);
        current->addChild(childPtr);
    }
    return current;
}

void Joint::addChild(Joint* child) {
    child->parent = this;
    child->QTreeWidgetItem::setText(0, child->name);
    QTreeWidgetItem::addChild(child);
    this->children.push_back(child);
}

void Joint::resetMats() {
    transforms.clear();
    for(auto& joint: skeleton) {
        if(this->id == joint->id) {
            joint->rotation = this->rotation;
        }
        transforms.push_back(joint->getOverallTransformation());
    }
}


glm::mat4 Joint::getLocalTransformation() {
    glm::mat4 model = glm::mat4(1.0f);
    return glm::translate(model, position) * glm::mat4_cast(rotation);
}

glm::mat4 Joint::getOverallTransformation() {
    if(!parent) {
        return getLocalTransformation();
    }else{
        return parent->getOverallTransformation() * getLocalTransformation();
    }
}

void Joint::clearJointData() {
    skeleton.clear();
    transforms.clear();
    bindMats.clear();
    last = 0;
}
