#ifndef FACEBUILDER_H
#define FACEBUILDER_H

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>

#include "glwidget.h"
#include "webcamwidget.h"
//#include "fastmarchingwidget.h"
#include "fastm.h"
#include "pstereowidget.h"
#include "facedet.h"
#include "facedatabasewidget.h"
#include "mesh.h"
#include "shapefs.h"

class Facebuilder: public QWidget
{
    Q_OBJECT
public:
    explicit Facebuilder(QWidget *parent = 0);

    Mesh mesh;
    GLWidget *meshViewer;
    WebcamWidget *imageViewer;

    Facedet *detector;


    FaceDatabaseWidget *database;
    //PStereoWidget *pstereo;
    Fastm *fastmarch;
    Shapefs *sfs;

    int subdivNum() {return _subdivNum;}

public slots:
    void buildMesh();
    void applyTexture();
    void saveMesh();
    void setSubdivNum(int val) {_subdivNum = val;}
    void updateMesh();
    void estimateLight();

private:
    int _subdivNum;
    void initMenu();
};

#endif // FACEBUILDER_H
