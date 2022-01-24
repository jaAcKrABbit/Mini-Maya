#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_geomSquare(this),
      m_progLambert(this), m_progFlat(this), m_progSkeleton(this),
      m_glCamera(), m_mesh(this), m_vertDisplay(this),
      m_halfEdgeDisplay(this),m_faceDisplay(this),
      m_skeletonDisplay(this), m_selectedVert(nullptr),
      m_selectedHE(nullptr), m_selectedFace(nullptr),
      m_selectedJoint(nullptr), m_skeleton(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
    m_mesh.destroy();
    m_vertDisplay.destroy();
    m_faceDisplay.destroy();
    m_halfEdgeDisplay.destroy();
    m_skeletonDisplay.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    // m_geomSquare.create();
    m_mesh.createCube();
    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    m_progSkeleton.create(":/glsl/skeleton.vert.glsl", ":/glsl/skeleton.frag.glsl");


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
    emit sig_sendMesh(&m_mesh);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);
    m_progSkeleton.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{

    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    m_progSkeleton.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setCamPos(m_glCamera.eye);


    //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
    // glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::rotate(glm::mat4(), 0.25f * 3.14159f, glm::vec3(0,1,0));
    glm::mat4 model = glm::mat4(1.0f);
    //Send the geometry's transformation matrix to the shader
    m_progLambert.setModelMatrix(model);
    m_progFlat.setModelMatrix(model);
    m_progSkeleton.setModelMatrix(model);


    //if skeleton binded, use skeleton shader
    if (m_mesh.binded) {

        m_progSkeleton.setBindMatrices(Joint::bindMats);
        m_progSkeleton.setOverallTransformationMatrices(Joint::transforms);
        m_progSkeleton.draw(m_mesh);
    } else {
        m_progLambert.draw(m_mesh);
    }

    glDisable(GL_DEPTH_TEST);
    if(m_selectedVert){
        m_vertDisplay.create();
        m_progFlat.draw(m_vertDisplay);
    }
    if(m_selectedHE){
        m_halfEdgeDisplay.create();
        m_progFlat.draw(m_halfEdgeDisplay);
    }
    if(m_selectedFace){
        m_faceDisplay.create();
        m_progFlat.draw(m_faceDisplay);
    }
    if(m_skeleton){
        m_skeletonDisplay.create();
        m_progFlat.draw(m_skeletonDisplay);
    }
    glEnable(GL_DEPTH_TEST);
    //Now do the same to render the cylinder
    //We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
   // model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));
//    m_progLambert.setModelMatrix(model);
  //  m_progLambert.draw(m_geomSquare);
}


void MyGL::keyPressEvent(QKeyEvent *e)
{
    const glm::vec3 x_axis = glm::vec3(1, 0, 0);
    const glm::vec3 y_axis = glm::vec3(0, 1, 0);
    const glm::vec3 z_axis = glm::vec3(0, 0, 1);
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());
    } else if (e->key() == Qt::Key_N) {
        if(m_selectedHE) {
            m_selectedVert = nullptr;
            m_selectedFace = nullptr;
            m_selectedHE = m_selectedHE->next;
            m_halfEdgeDisplay.updateHalfEdge(m_selectedHE);
            QListWidgetItem* i = dynamic_cast<QListWidgetItem*>(m_selectedHE);
            i->setSelected(true);
        }
    } else if (e->key() == Qt::Key_M) {
        if(m_selectedHE) {
            m_selectedVert = nullptr;
            m_selectedFace = nullptr;
            m_selectedHE = m_selectedHE->sym;
            m_halfEdgeDisplay.updateHalfEdge(m_selectedHE);
            QListWidgetItem* i = dynamic_cast<QListWidgetItem*>(m_selectedHE);
            i->setSelected(true);
        }
    } else if (e->key() == Qt::Key_F) {
        if (m_selectedHE) {
            m_selectedVert = nullptr;
            m_selectedFace = nullptr;
            m_selectedFace = m_selectedHE->face;
            m_faceDisplay.updateFace(m_selectedFace);
            QListWidgetItem* i = dynamic_cast<QListWidgetItem*>(m_selectedFace);
            i->setSelected(true);
        }
    } else if (e->key() == Qt::Key_V) {
        if (m_selectedHE) {
            m_selectedVert = nullptr;
            m_selectedFace = nullptr;
            m_selectedVert = m_selectedHE->vertex;
            m_vertDisplay.updateVertex(m_selectedVert);
            QListWidgetItem* i = dynamic_cast<QListWidgetItem*>(m_selectedVert);
            i->setSelected(true);
        }
    } else if (e->key() == Qt::Key_H && (e->modifiers() & Qt::ShiftModifier)) {
        if (m_selectedFace) {
            m_selectedVert = nullptr;
            m_selectedHE = nullptr;
            m_selectedHE = m_selectedFace->halfEdge;
            m_halfEdgeDisplay.updateHalfEdge(m_selectedHE);
            QListWidgetItem* i = dynamic_cast<QListWidgetItem*>(m_selectedHE);
            i->setSelected(true);
        }
    } else if (e->key() == Qt::Key_H) {
        if (m_selectedVert) {
            m_selectedHE = nullptr;
            m_selectedFace = nullptr;
            m_selectedHE = m_selectedVert->halfEdge;
            m_halfEdgeDisplay.updateHalfEdge(m_selectedHE);
            QListWidgetItem* i = dynamic_cast<QListWidgetItem*>(m_selectedHE);
            i->setSelected(true);
        }
    } else if (e->key() == Qt::Key_I) {
        if(m_selectedJoint) {
            m_selectedJoint->rotation *= glm::angleAxis(glm::radians(5.f), x_axis);
            m_selectedJoint->resetMats();

        }
    } else if (e->key() == Qt::Key_K) {
        if(m_selectedJoint) {
            m_selectedJoint->rotation *= glm::angleAxis(glm::radians(-5.f), x_axis);
            m_selectedJoint->resetMats();
        }
    } else if (e->key() == Qt::Key_J) {
        if(m_selectedJoint) {
            m_selectedJoint->rotation *= glm::angleAxis(glm::radians(5.f), y_axis);
            m_selectedJoint->resetMats();
        }
    } else if (e->key() == Qt::Key_L) {
        if(m_selectedJoint) {
            m_selectedJoint->rotation *= glm::angleAxis(glm::radians(-5.f), y_axis);
            m_selectedJoint->resetMats();
        }
    } else if (e->key() == Qt::Key_U) {
        if(m_selectedJoint) {
            m_selectedJoint->rotation *= glm::angleAxis(glm::radians(5.f), z_axis);
            m_selectedJoint->resetMats();
        }
    } else if (e->key() == Qt::Key_O) {
        if(m_selectedJoint) {
            m_selectedJoint->rotation *= glm::angleAxis(glm::radians(-5.f), z_axis);
            m_selectedJoint->resetMats();
        }
    }
    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::slot_setSelectedVertex(QListWidgetItem *vertex) {
    if((m_selectedVert = dynamic_cast<Vertex*>(vertex))) {
        m_vertDisplay.updateVertex(m_selectedVert);
        update();
    }

}

void MyGL::slot_setSelectedFace(QListWidgetItem* face) {
    if((m_selectedFace = dynamic_cast<Face*>(face))) {
        m_faceDisplay.updateFace(m_selectedFace);
        update();
    }
}

void MyGL::slot_setSelectedHalfEdge(QListWidgetItem* he) {
    if((m_selectedHE = dynamic_cast<HalfEdge*>(he))) {
        m_halfEdgeDisplay.updateHalfEdge(m_selectedHE);
        update();
    }
}

void MyGL::slot_setSelectedJoint(QTreeWidgetItem* joint) {
    if((m_selectedJoint = dynamic_cast<Joint*>(joint))) {
        m_skeletonDisplay.updateJoint(m_selectedJoint);
        update();
    }
}

void MyGL::slot_changeR(double r) {
    if(m_selectedFace) {
        m_selectedFace->color.r = r;
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
}
void MyGL::slot_changeG(double g) {
    if(m_selectedFace) {
        m_selectedFace->color.g = g;
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
}
void MyGL::slot_changeB(double b) {
    if(m_selectedFace) {
        m_selectedFace->color.b = b;
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
}

void MyGL::slot_changeX(double x) {
    if(m_selectedVert) {
        m_selectedVert->position.x = x;
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
    if(m_selectedJoint) {
        m_selectedJoint->position.x = x;
        m_selectedJoint->resetMats();
        update();
    }
}
void MyGL::slot_changeY(double y) {
    if(m_selectedVert) {
        m_selectedVert->position.y = y;
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
    if(m_selectedJoint) {
        m_selectedJoint->position.y = y;
        m_selectedJoint->resetMats();
        update();
    }
}
void MyGL::slot_changeZ(double z) {
    if(m_selectedVert) {
        m_selectedVert->position.z = z;
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
    if(m_selectedJoint) {
        m_selectedJoint->position.z = z;
        m_selectedJoint->resetMats();
        update();
    }
}

void MyGL::slot_splitEdge() {
    if(m_selectedHE) {
        m_mesh.splitEdge(m_selectedHE);
        emit sig_sendMesh(&m_mesh);
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
}

void MyGL::slot_triangulate() {
    if(m_selectedFace) {
        m_mesh.triangulate(m_selectedFace);
        emit sig_sendMesh(&m_mesh);
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
}

void MyGL::slot_subdivide() {
    m_mesh.subdivide();
    emit sig_sendMesh(&m_mesh);
    m_mesh.destroy();
    m_mesh.create();
    update();
}

void MyGL::slot_loadObj() {
    m_selectedVert = nullptr;
    m_selectedFace = nullptr;
    m_selectedHE = nullptr;
    QString filePath = QFileDialog::getOpenFileName(0, QString("Load OBJ File"), QString(), QString("*.obj"));
    m_mesh.createObj(filePath);
    emit sig_sendMesh(&m_mesh);
    m_mesh.destroy();
    m_mesh.create();
    update();
}

void MyGL::slot_extrudeFace() {
    if(m_selectedFace) {
        m_mesh.extrude(m_selectedFace);
        emit sig_sendMesh(&m_mesh);
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
    m_selectedVert = nullptr;
    m_selectedFace = nullptr;
    m_selectedHE = nullptr;
}

void MyGL::slot_loadJson() {
    m_skeleton = nullptr;
    m_skeletonDisplay.destroy();
    QString filename = QFileDialog::getOpenFileName(0, QString("Load JSON File"), QDir::currentPath().append(QString("../..")), QString("*.json"));
    m_skeleton = Joint::loadSkeleton(filename);
    for(auto& joint: Joint::skeleton) {
        Joint::transforms.push_back(joint->getOverallTransformation());
        Joint::bindMats.push_back(glm::inverse(joint->getOverallTransformation()));
        joint->bindMatrix = glm::inverse(joint->getOverallTransformation());
    }
    emit sig_sendRootNode(m_skeleton);
    update();
}

void MyGL::slot_meshSkin() {
    m_mesh.binded = false;
    if(m_skeleton){
       m_mesh.bindSkeleton();
       m_mesh.destroy();
       m_mesh.create();
       update();
    }

}
