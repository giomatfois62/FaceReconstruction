#include "webcamwindow.h"
#include <QDebug>

WebcamWindow::WebcamWindow()
{

}

void WebcamWindow::init()
{
    window.resize(QSize(640,480));
    window.show();
    //createCanvas();
    cap.open(0);
    m_timer = new QTimer;
    m_timer->setInterval(33);
    connect(m_timer, &QTimer::timeout, this, &WebcamWindow::renderFrame);
    m_timer->start();
}

void WebcamWindow::renderFrame()
{
    cv::Mat frame;
    cap >> frame;
}


void WebcamWindow::createCanvas()
{
    QVector<Vertex> vertices;

    Vertex v1;
    v1.position.setX(1.0); v1.position.setY(1.0); v1.position.setZ(0.0);
    v1.texCoords.setX(1.0); v1.texCoords.setY(1.0);
    Vertex v2;
    v2.position.setX(1.0); v2.position.setY(-1.0); v2.position.setZ(0.0);
    v2.texCoords.setX(1.0); v2.texCoords.setY(0.0);
    Vertex v3;
    v3.position.setX(-1.0); v3.position.setY(-1.0); v3.position.setZ(0.0);
    v3.texCoords.setX(0.0); v3.texCoords.setY(0.0);
    Vertex v4;
    v4.position.setX(-1.0); v4.position.setY(1.0); v4.position.setZ(0.0);
    v4.texCoords.setX(0.0); v4.texCoords.setY(1.0);

    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);

    QVector<unsigned int> indices = {  // Note that we start from 0!
                          0, 1, 3, // First Triangle
                          1, 2, 3  // Second Triangle
                                    };

    Mesh canvas(vertices,indices);
    //window.canvas=Mesh(canvas);
}
