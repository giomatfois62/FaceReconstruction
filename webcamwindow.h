#ifndef WEBCAMWINDOW_H
#define WEBCAMWINDOW_H

#include <QTimer>
#include "glwindow.h"
#include "mesh.h"
#include "glmesh.h"
#include "recorder.h"

class QTimer;

class WebcamWindow: public QObject
{
public:
    WebcamWindow();

    void renderFrame();
    void createCanvas();
    void init();

    GLWindow window;

private:


    QTimer *m_timer;
    cv::VideoCapture cap;    
};

#endif // WEBCAMWINDOW_H
