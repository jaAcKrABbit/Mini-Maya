#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <mesh.h>



namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();
    void slot_createComponentsList(Mesh* mesh);
    void slot_setPosSpinBox(QListWidgetItem* item);
    void slot_setColorSpinBox(QListWidgetItem* item);
    void slot_addRootToTreeWidget(QTreeWidgetItem* item);
    void slot_setJointPositionSpinBox(QTreeWidgetItem* item);

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
