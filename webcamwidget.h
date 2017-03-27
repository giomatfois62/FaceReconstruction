#ifndef WEBCAMWIDGET_H
#define WEBCAMWIDGET_H

#include <QFileDialog>
#include <QTimer>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "opencv2/opencv.hpp"
#include "glwidget.h"
#include "facedet.h"

class QTimer;

class WebcamWidget : public GLWidget
{
    Q_OBJECT
public:
    WebcamWidget();
    WebcamWidget(Facedet *det);

    cv::Mat &frame();
    void setImage(cv::Mat &img);
    void saveDelaunay(cv::Mat &frame, QVector<QVector2D> imagePoints);
    void saveTexCoords(cv::Mat &frame, QVector<QVector2D> &imagePoints);
    int getPointIdx(cv::Point2f pt, std::vector<cv::Point2f> &imagePoints);
    QHBoxLayout *menu();

public slots:
    void startStream();
    void stopStream();
    void drawLandmarks();
    void loadImage();
    void setImage(QString imageName);

signals:
    void frameUpdated();

private:
    void createCanvas(int imgW, int imgH);
    void renderWebcam();

    cv::Mat m_frame;
    Facedet *detector;
    QTimer *m_timer;
    cv::VideoCapture cap;
};

#endif // WEBCAMWIDGET_H
