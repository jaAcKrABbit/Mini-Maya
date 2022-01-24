#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    connect(ui->mygl,
            // Signal name
            SIGNAL(sig_sendMesh(Mesh*)),
            // Widget with the slot that receives the signal
            this,
            // Slot name
            SLOT(slot_createComponentsList(Mesh*)));
    connect(ui->mygl,
              // Signal name
              SIGNAL(sig_sendRootNode(QTreeWidgetItem*)),
              // Widget with the slot that receives the signal
              this,
              // Slot name
              SLOT(slot_addRootToTreeWidget(QTreeWidgetItem*)));
    connect(ui->vertsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                ui->mygl, SLOT(slot_setSelectedVertex(QListWidgetItem*)));
    connect(ui->facesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                ui->mygl, SLOT(slot_setSelectedFace(QListWidgetItem*)));
    connect(ui->halfEdgesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                ui->mygl, SLOT(slot_setSelectedHalfEdge(QListWidgetItem*)));
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
                ui->mygl, SLOT( slot_setSelectedJoint(QTreeWidgetItem*)));

    //read vert position value
    connect(ui->vertsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
               this, SLOT(slot_setPosSpinBox(QListWidgetItem*)));
    //read joint poisition
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
               this, SLOT(slot_setJointPositionSpinBox(QTreeWidgetItem*)));
    //read rgb value
    connect(ui->facesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
               this, SLOT(slot_setColorSpinBox(QListWidgetItem*)));
    connect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeR(double)));
    connect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeG(double)));
    connect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeB(double)));

    connect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeX(double)));
    connect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeY(double)));
    connect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeZ(double)));

    connect(ui->jointPosXSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeX(double)));
    connect(ui->jointPosYSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeY(double)));
    connect(ui->jointPosZSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeZ(double)));

    connect(ui->splitEdgeButton, SIGNAL(clicked(bool)), ui->mygl, SLOT(slot_splitEdge()));
    connect(ui->triangulateButton, SIGNAL(clicked(bool)), ui->mygl, SLOT(slot_triangulate()));
    connect(ui->subdivideButton, SIGNAL(clicked(bool)), ui->mygl, SLOT(slot_subdivide()));
    connect(ui->loadObjButton, SIGNAL(clicked(bool)), ui->mygl, SLOT(slot_loadObj()));
    connect(ui->extrudeFaceButton, SIGNAL(clicked(bool)), ui->mygl, SLOT(slot_extrudeFace()));
    connect(ui->loadJsonButton, SIGNAL(clicked(bool)), ui->mygl, SLOT(slot_loadJson()));
    connect(ui->bindSkeletonButton, SIGNAL(clicked(bool)), ui->mygl, SLOT(slot_meshSkin()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}

void MainWindow::slot_createComponentsList(Mesh* mesh) {
    for(auto& vert: mesh->vertices) {
        ui->vertsListWidget->addItem(vert.get());
    }
    for(auto& he: mesh->halfEdges) {
        ui->halfEdgesListWidget->addItem(he.get());
    }
    for(auto& f: mesh->faces) {
        ui->facesListWidget->addItem(f.get());
    }
}

void MainWindow::slot_setPosSpinBox(QListWidgetItem* item) {
    if(Vertex* v = dynamic_cast<Vertex*>(item)) {
        this->ui->vertPosXSpinBox->setValue(v->position.x);
        this->ui->vertPosYSpinBox->setValue(v->position.y);
        this->ui->vertPosZSpinBox->setValue(v->position.z);
    }
}

void MainWindow::slot_setJointPositionSpinBox(QTreeWidgetItem* item) {
    if(Joint* j = dynamic_cast<Joint*>(item)) {
        this->ui->jointPosXSpinBox->setValue(j->position.x);
        this->ui->jointPosYSpinBox->setValue(j->position.y);
        this->ui->jointPosZSpinBox->setValue(j->position.z);
    }
}

void MainWindow::slot_setColorSpinBox(QListWidgetItem* item) {
    if(Face* f = dynamic_cast<Face*>(item)) {
        this->ui->faceRedSpinBox->setValue(f->color.r);
        this->ui->faceGreenSpinBox->setValue(f->color.g);
        this->ui->faceBlueSpinBox->setValue(f->color.b);
    }
}

void MainWindow::slot_addRootToTreeWidget(QTreeWidgetItem* item) {
    ui->treeWidget->addTopLevelItem(item);
}

