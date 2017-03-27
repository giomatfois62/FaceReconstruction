#include "facedet.h"

using namespace dlib;

Facedet::Facedet()
{
    // init face detector
    detector=dlib::get_frontal_face_detector();
    // prepare shape predictor
    dlib::deserialize("../shape_predictor_68_face_landmarks.dat") >> pose_model;
}

QVector<QVector2D> Facedet::getLandmarks(cv::Mat &frame)
{
    if(frame.empty())
    {
        QVector<QVector2D> dummy;
        return dummy;
    }
    // get landmarks from current frame with dlib
    dlib::cv_image<bgr_pixel> cimg(frame);
    std::vector<dlib::rectangle> faces = detector(cimg);
    std::vector<full_object_detection> shapes;

    for (unsigned long i = 0; i < faces.size(); ++i)
        shapes.push_back(pose_model(cimg, faces[i]));

    // GET AND PRINT POSE
    std::vector<cv::Point3f > modelPoints;
    std::vector<cv::Point2f > imagePoints;
    cv::Mat rvec,tvec;
    rvec= cv::Mat::zeros(3, 1, CV_64F);//cv::Mat_<double>(3,1);
    tvec= cv::Mat::zeros(3, 1, CV_64F);//cv::Mat_<double>(3,1);
    //rvec << 0,0,0;
    //tvec << 0,0,0;
    cv::Mat modelPts,imgPts,camMatrix;
    int max_d = MAX(frame.rows,frame.cols);
    camMatrix = (cv::Mat_<double>(3,3) << max_d, 0, frame.cols/2.0,
                                          0,  max_d, frame.rows/2.0,
                                          0,  0,  1.0);
    double _dc[4]={0,0,0,0};

    float scale= .00714;//1; //;
    cv::Point3f modelPos;

    // custom mask pos
    modelPos=cv::Point3f(-0.143, 10.838, 6.506);;//cv::Point3f(0,0,0);//

    //reference 3d points used to recover pose
    modelPoints.push_back((cv::Point3f(2.37427,110.322,21.7776)+modelPos)*scale);	// l eye (v 314)
    modelPoints.push_back((cv::Point3f(70.0602,109.898,20.8234)+modelPos)*scale);	// r eye (v 0)
    modelPoints.push_back((cv::Point3f(36.8301,78.3185,52.0345)+modelPos)*scale);	//nose (v 1879)
    modelPoints.push_back((cv::Point3f(14.8498,51.0115,30.2378)+ modelPos)*scale);	// l mouth (v 1502)
    modelPoints.push_back((cv::Point3f(58.1825,51.0115,29.6224)+modelPos)*scale);	// r mouth (v 695)
    //modelPoints.push_back((cv::Point3f(-61.8886,127.797,-89.4523)+modelPos)*scale);	// l ear (v 2011)
    //modelPoints.push_back((cv::Point3f(127.603,126.9,-83.9129)+modelPos)*scale);		// r ear (v 1138)

    modelPts=cv::Mat(modelPoints);  //convert to Mat for opencv functions

    cv::Mat mean_;
    cv::Point2f mean;
    std::vector<cv::Point2f> points;
    point pt,high,low,left,right;
    //QDebug debug = qDebug();
    getCVLandmarks(shapes, imagePoints);

    for(unsigned long int i=0; i<shapes.size(); ++i)
    {
        std::vector<cv::Point2f> leyevec;
        for (unsigned long j = 36; j <= 41; ++j)
        {
            pt=shapes[i].part(j); //left eye
            leyevec.push_back(cv::Point2f(pt.x(),pt.y()) );
        }
        high=shapes[i].part(37);
        low=shapes[i].part(41);
        left=shapes[i].part(36);
        right=shapes[i].part(39);
        //debug <<"Left eye: "<< getEyeAperture(high,low,left,right);
        //debug << pt.x() << " "<< pt.y();

        cv::reduce(leyevec, mean_, CV_REDUCE_AVG, 1);
        // convert from Mat to Point - there may be even a simpler conversion,
        // but I do not know about it.
        mean=cv::Point2f(mean_.at<float>(0,0), mean_.at<float>(0,1));
        points.push_back(mean);


        std::vector<cv::Point2f> reyevec;
        for (unsigned long j = 42; j <= 47; ++j)
        {
            pt=shapes[i].part(j); //left eye
            reyevec.push_back(cv::Point2f(pt.x(),pt.y()) );
        }
        high=shapes[i].part(43);
        low=shapes[i].part(47);
        left=shapes[i].part(42);
        right=shapes[i].part(45);
        //fprintf(stderr, QString("Text to print").toLatin1().data());
        //debug  <<"Right eye: " << getEyeAperture(high,low,left,right);

        cv::reduce(reyevec, mean_, CV_REDUCE_AVG, 1);
        // convert from Mat to Point - there may be even a simpler conversion,
        // but I do not know about it.
        mean=cv::Point2f(mean_.at<float>(0,0), mean_.at<float>(0,1));
        points.push_back(mean);

        pt=shapes[i].part(30); // nose tip
        points.push_back(cv::Point2f(pt.x(),pt.y()));


        std::vector<cv::Point2f> mouth;
        for (unsigned long j = 60; j <= 67; ++j)
        {
            pt=shapes[i].part(j); //left eye
            mouth.push_back(cv::Point2f(pt.x(),pt.y()) );
        }

        pt=shapes[i].part(48) ;         //left mouth
        points.push_back(cv::Point2f(pt.x(),pt.y()));


        pt=shapes[i].part(54);        //right mouth
        points.push_back(cv::Point2f(pt.x(),pt.y()));



        //pt=shapes[i].part(1);        //l ear
        //points.push_back(cv::Point2f(pt.x(),pt.y()));


        //pt=shapes[i].part(15);        //r ear
        //points.push_back(cv::Point2f(pt.x(),pt.y()));

    }
    imgPts=cv::Mat(points);

    for(int i=0; i<points.size(); i++){
        qDebug()<<"imgPoint["<<i<<"]="<<points[i].x<<" "<<points[i].y;
    }

    for(int i=0; i<modelPoints.size(); i++){
        qDebug()<<"modelPoint["<<i<<"]="<<modelPoints[i].x<<" "<<modelPoints[i].y<< " "<<modelPoints[i].z;
    }

    for(int i=0; i<3; i++) {
        qDebug()<<camMatrix.at<double>(i,0)<<" "<<camMatrix.at<double>(i,1)<< " "<<camMatrix.at<double>(i,2);
    }


    //if(imgPts.rows == 7)
    qDebug() << "rvec before: "<< rvec.at<float>(0,0) <<" "<< rvec.at<float>(1,0) << " "<<rvec.at<float>(2,0);
    solvePnP(modelPts,imgPts,camMatrix,cv::Mat(1,4,CV_64FC1,_dc),rvec,tvec);//,false,CV_EPNP);
    qDebug() << "rvec after: "<< rvec.at<double>(0,0) <<" "<< rvec.at<double>(1,0) << " "<<rvec.at<double>(2,0);

    // adjust x rotation
    rvec.at<double>(0,0) = 3.141592 - rvec.at<double>(0,0);
    // AR procedure to build movelview_matrix directly
    cv::Mat Rot(3,3,CV_64FC1);
    Rodrigues(rvec, Rot); // convert rotation vector to rotation matrix
    cv::Mat direction = cv::Mat::zeros(3, 1, CV_64F);
    direction.at<double>(2,0) = 1;
    direction = Rot*direction;
    qDebug()<<"direction: "<<direction.at<double>(0,0)<<" "<<direction.at<double>(1,0)<< " "<<direction.at<double>(2,0);
    // CONTINUE AS USUAL

    // save landmarks on a QVector
    QVector<QVector2D > landmarks;
    for(unsigned long int i=0; i<shapes.size(); i++)
    {
        // clear previos face (cant store more than one for now)
        landmarks.clear();
        // store landmarks

        // I always used 68 points but I'm deleting #1 and #17 because i dont need them
        for (unsigned long int j=0; j<68; j++)
        {
            //if(j!=0 && j!=16)
            {
              point pt = shapes[i].part(j);
              landmarks.push_back( QVector2D(pt.x(),pt.y()) );
            }
        }
    }
    return landmarks;

}


Mesh Facedet::buildMesh(cv::Mat &frame)
{
    QVector<QVector2D> landmarks = getLandmarks(frame);
    Mesh mesh(landmarks,"");
    mesh.scaleToUnity();
    mesh.modelMatrix.rotate(180,QVector3D(1,0,0));
    mesh.applyTransformation();
    return mesh;
}

Mesh Facedet::buildMeshWithTexture(cv::Mat &frame)
{
    QVector<QVector2D> landmarks = getLandmarks(frame);
    Mesh mesh(landmarks,"");
    mesh.scaleToUnity();
    mesh.scaleTextureCoords(frame.rows,frame.cols);
    mesh.modelMatrix.rotate(180,QVector3D(1,0,0));
    return mesh;
}

void Facedet::getCVLandmarks(std::vector<full_object_detection> &shapes, std::vector<cv::Point2f > &imagePoints)
{
    imagePoints.clear();
    point pt;
    for(unsigned long int i=0; i<shapes.size(); ++i)
    {
        for (unsigned long j = 0; j <= 67; ++j)
        {
             pt=shapes[i].part(j);
             imagePoints.push_back(cv::Point2f(pt.x(),pt.y()));
        }

        cv::Mat mean_;
        cv::Point2f mean;
        point pt;

    std::vector<cv::Point2f> leyevec;
    for (unsigned long j = 36; j <= 41; ++j)
    {
         pt=shapes[i].part(j); //left eye
         leyevec.push_back(cv::Point2f(pt.x(),pt.y()) );
    }
    cv::reduce(leyevec, mean_, CV_REDUCE_AVG, 1);
    // convert from Mat to Point - there may be even a simpler conversion,
    // but I do not know about it.
    mean=cv::Point2f(mean_.at<float>(0,0), mean_.at<float>(0,1));
    imagePoints.push_back(mean);


    std::vector<cv::Point2f> reyevec;
    for (unsigned long j = 42; j <= 47; ++j)
    {
         pt=shapes[i].part(j); //left eye
         reyevec.push_back(cv::Point2f(pt.x(),pt.y()) );
    }
    cv::reduce(reyevec, mean_, CV_REDUCE_AVG, 1);
    // convert from Mat to Point - there may be even a simpler conversion,
    // but I do not know about it.
    mean=cv::Point2f(mean_.at<float>(0,0), mean_.at<float>(0,1));
    imagePoints.push_back(mean);

    }

}
