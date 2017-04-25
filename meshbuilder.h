#ifndef MESHBUILDER_H
#define MESHBUILDER_H

#include <QWidget>

#include <QGroupBox>
#include <QInputDialog>
#include <QSpinBox>
#include <QLabel>

#include "mesh.h"
#include "webcamwidget.h"
#include "facedetector.h"

#include "fastm.h"
#include "shapefs.h"
#include "facedatabasewidget.h"

class MeshBuilder : public QWidget
{
    Q_OBJECT
public:
    explicit MeshBuilder(QWidget *parent = 0);
    ~MeshBuilder();

    Mesh m_mesh;

    GLWidget *meshViewer;
    WebcamWidget *imageViewer;
    Facedet *detector;

    Shapefs *sfs;
    Fastm *fastmarch;
    FaceDatabaseWidget *database;

    QGroupBox* menu();

    int subdivNum() { return m_subdivNum; }
    float edgeTol() {return m_edgeTol; }
    QString subdivType() {return m_subdivType;}

signals:

public slots:
    void buildMesh();
    void convertToNonAdaptive();
    void saveMesh();
    void loadMesh();
    void applyTexture();
    void setSubdivNum(int val) {m_subdivNum = val;}
    void setEdgeTolerance(double val) {m_edgeTol = val;}
    void setSubdivType(QString val) {m_subdivType = val;}
    void setGridType(QString val) {m_gridType = val;}
    void updateMesh();

private:
    Mesh buildMeshFromFace();
    Mesh buildMeshFromImage();

    bool findMatchingTriangle(QVector3D p, float barCoords[3], unsigned int &firstIndex);

    int m_subdivNum;
    float m_edgeTol;
    QString m_subdivType;
    QString m_gridType;
};

#endif // MESHBUILDER_H
