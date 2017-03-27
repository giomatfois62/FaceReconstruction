#ifndef FACEDET_H
#define FACEDET_H

#include <QVector>
#include <QVector2D>

#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>

#include "mesh.h"

class Facedet
{
public:
    Facedet();

    QVector<QVector2D > getLandmarks(cv::Mat &frame); // return landmarks detected on last captured frame
    Mesh buildMesh(cv::Mat &frame);
    Mesh buildMeshWithTexture(cv::Mat &frame);

private:
    dlib::frontal_face_detector detector;
    dlib::shape_predictor pose_model;

    void getCVLandmarks(std::vector<dlib::full_object_detection> &shapes, std::vector<cv::Point2f > &imagePoints);

};

#endif // FACEDET_H
