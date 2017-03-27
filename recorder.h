#ifndef RECORDER_H
#define RECORDER_H

#include <QTimer>
#include "opencv2/opencv.hpp"

class Recorder: public QObject
{

public:
    Recorder();
    cv::Mat frame() { return _frame;}

    void startStream();
    void endStream();

public slots:
    void grabFrame();

private:

    cv::VideoCapture cap;
    cv::Mat _frame;
};

#endif // RECORDER_H
