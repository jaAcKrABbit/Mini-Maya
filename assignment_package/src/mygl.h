#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <mesh.h>
#include <vertexdisplay.h>
#include <halfedgedisplay.h>
#include <facedisplay.h>
#include <joint.h>
#include <skeletondisplay.h>
class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram m_progSkeleton;

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;
    Mesh m_mesh;
    VertexDisplay m_vertDisplay;
    HalfEdgeDisplay m_halfEdgeDisplay;
    FaceDisplay m_faceDisplay;
    SkeletonDisplay m_skeletonDisplay;
    Vertex* m_selectedVert;
    HalfEdge* m_selectedHE;
    Face* m_selectedFace;
    Joint* m_selectedJoint;
    Joint* m_skeleton;


public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

protected:
    void keyPressEvent(QKeyEvent *e);

public slots:

    void slot_setSelectedVertex(QListWidgetItem* vertex);
    void slot_setSelectedFace(QListWidgetItem* face);
    void slot_setSelectedHalfEdge(QListWidgetItem* halfedge);
    void slot_setSelectedJoint(QTreeWidgetItem* joint);
    void slot_changeR(double r);
    void slot_changeG(double g);
    void slot_changeB(double b);
    void slot_changeX(double x);
    void slot_changeY(double y);
    void slot_changeZ(double z);
    void slot_splitEdge();
    void slot_triangulate();
    void slot_subdivide();
    void slot_loadObj();
    void slot_extrudeFace();
    void slot_loadJson();
    void slot_meshSkin();

signals:
    void sig_sendMesh(Mesh* mesh);
    void sig_sendRootNode(QTreeWidgetItem* i);
};


#endif // MYGL_H
