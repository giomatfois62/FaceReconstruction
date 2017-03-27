#ifndef MESHBUILDER_H
#define MESHBUILDER_H

#include <QWidget>

#include <QGroupBox>
#include <QInputDialog>
#include <QSpinBox>
#include <QLabel>

#include "mesh.h"
#include "webcamwidget.h"
#include "facedet.h"

#include "fastm.h"
#include "shapefs.h"
#include "facedatabasewidget.h"

class MeshBuilder : public QWidget
{
    Q_OBJECT
public:
    explicit MeshBuilder(QWidget *parent = 0);
    ~MeshBuilder();


    Mesh mesh;

    GLWidget *meshViewer;
    WebcamWidget *imageViewer;
    Facedet *detector;

    Shapefs *sfs;
    Fastm *fastmarch;
    FaceDatabaseWidget *database;

    QGroupBox* menu();

    int subdivNum() { return m_subdivNum; }

signals:

public slots:
    void buildMesh();
    void saveMesh();
    void loadMesh();
    void applyTexture();
    void setSubdivNum(int val) {m_subdivNum = val;}
    void updateMesh();

private:
    int m_subdivNum;

};

#endif // MESHBUILDER_H
