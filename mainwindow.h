#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTimer>
#include <QHBoxLayout>

#include "glwidget.h"
#include "pstereowidget.h"
#include "fastmarchingwidget.h"
#include "webcamwidget.h"
#include "shapefs.h"
#include "fastm.h"
#include "mesh.h"
#include "facebuilder.h"
#include "facedatabase.h"
#include "facedetector.h"
#include "mainmenu.h"

class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    void initOpenGLWindows();
    void initMenu();

    MainMenu menu;

    // Opengl Windows
    WebcamWidget *imageWidget;
    GLWidget *meshWidget;

    PStereoWidget pstereo;
    FastMarchingWidget fastm;

    // Mesh construnction
    Mesh mesh;
    FaceDatabase database;
    Facebuilder builder;
    Facedet detector;

    // Opencv image & webcam
    cv::Mat frame;
    cv::VideoCapture cap;
    QTimer *m_timer;

public slots:
    void updateMesh();
    // image loading slots
    void loadImage();
    void drawLandmarks();
    void loadYale(int db) {}
    void loadArtificial(int db) {}
    void loadDatabase(QString dbName) {}
    void loadPrevious() {}
    void loadNext() {}

    // webcam stream slots
    void renderFrame();
    void startStream();
    void stopStream();

    // Mesh building slots
    void buildMesh(){}
    void createTexture();
    void saveMesh(){}
    void buildArtificial(){}
    void buildMeshOnly();
    void subdivide(){}
    void execPS(){}
    void execFM(){}
};

#endif // MAINWINDOW_H
