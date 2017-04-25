#include "webcamwidget.h"

WebcamWidget::WebcamWidget()
{
    detector = new Facedet;

    m_timer = new QTimer;
    m_timer->setInterval(33);
    connect(m_timer,&QTimer::timeout,this,&WebcamWidget::renderWebcam);
}


WebcamWidget::WebcamWidget(Facedet *det): detector(det)
{
    m_timer = new QTimer;
    m_timer->setInterval(33);
    connect(m_timer,&QTimer::timeout,this,&WebcamWidget::renderWebcam);
}

cv::Mat &WebcamWidget::frame() {return m_frame;}

void WebcamWidget::createCanvas(int imgW, int imgH)
{
    QVector<Vertex> vertices;

    float ratio = imgW/float(imgH);
    float xdim,ydim;
    if(ratio >=1 )
    {
        xdim = 1;
        ydim = 1/ratio;
    }
    else
    {
        ydim = 1;
        xdim = ratio;
    }

    Vertex v1;
    v1.position.setX(xdim); v1.position.setY(ydim); v1.position.setZ(0.0);
    v1.normal.setZ(1.0);
    v1.texCoords.setX(1.0); v1.texCoords.setY(0.0);
    Vertex v2;
    v2.position.setX(xdim); v2.position.setY(-ydim); v2.position.setZ(0.0);
    v2.normal.setZ(1.0);
    v2.texCoords.setX(1.0); v2.texCoords.setY(1.0);
    Vertex v3;
    v3.position.setX(-xdim); v3.position.setY(-ydim); v3.position.setZ(0.0);
    v3.normal.setZ(1.0);
    v3.texCoords.setX(0.0); v3.texCoords.setY(1.0);
    Vertex v4;
    v4.position.setX(-xdim); v4.position.setY(ydim); v4.position.setZ(0.0);
    v4.normal.setZ(1.0);
    v4.texCoords.setX(0.0); v4.texCoords.setY(0.0);

    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);

    QVector<unsigned int> indices = {  // Note that we start from 0!
                          0, 1, 3, // First Triangle
                          1, 2, 3  // Second Triangle
                                    };

    Mesh canvas(vertices,indices);
    this->addMesh("canvas",canvas);
    this->setActiveUniform("texture");
}


void WebcamWidget::setImage(cv::Mat &img)
{
    //if(meshes.isEmpty())
    createCanvas(img.cols,img.rows);

    this->meshes["canvas"].createTexture(img);
    emit frameUpdated();
}

void WebcamWidget::setImage(QString imageName)
{
    std::string fn = imageName.toUtf8().constData();
    m_frame = cv::imread(fn);

    //if(meshes.isEmpty())
    createCanvas(m_frame.cols,m_frame.rows);

    this->meshes["canvas"].createTexture(m_frame);
    emit frameUpdated();
}

void WebcamWidget::drawLandmarks()
{
    if(m_frame.empty())
        return;

    QVector<QVector2D> landmarks = detector->getLandmarks(m_frame);
    //saveDelaunay(m_frame,landmarks);
    //saveTexCoords(m_frame,landmarks);

    cv::Mat temp;
    m_frame.copyTo(temp);
    cv::Scalar color = {0,255,0};
    for(int i=0; i<landmarks.size(); i++)
    {
        cv::Point p = cv::Point(landmarks[i].x(),landmarks[i].y());
        cv::circle(temp,p,2,color,2);
    }
    setImage(temp);
}

void WebcamWidget::loadImage()
{
    QString fileName  = QFileDialog::getOpenFileName(0, tr("Choose"),"", tr("Images (*.png *.jpg *.jpeg *.bmp *.gif *.pgm)"));
    setImage(fileName);
}


void WebcamWidget::startStream()
{
    if(!cap.open(0))
        return;
    m_timer->start();
}


void WebcamWidget::stopStream()
{
    m_timer->stop();
    cap.release();
}

void WebcamWidget::renderWebcam()
{
    cap >> m_frame;
    drawLandmarks();
    //setImage(m_frame);
}

void WebcamWidget::saveTexCoords(cv::Mat &frame, QVector<QVector2D> &imagePoints)
{
    using namespace cv;
    // Rectangle to be used with Subdiv2D
    Size size = frame.size();

    int first = 0; int last = 16;
    float A = (imagePoints[last].y() - imagePoints[first].y()); // A = y2-y1
    float B = (float) -(imagePoints[last].x() - imagePoints[first].x());      // B= -(x2-x1)
    float C = -A*imagePoints[first].x() - B*imagePoints[first].y();
    float M = std::sqrt(A*A+B*B);
    A = A/M; B = B/M; C = C/M;

    for(int i=first+1; i<last; i++) {
            float D = A* imagePoints[i].x()  + B*imagePoints[i].y()  + C;
            QVector2D p(imagePoints[i].x() - 1.95*A*D, imagePoints[i].y() -1.95*B*D);
            imagePoints.push_back(p);
    }
    qDebug() << "there are "<<imagePoints.size()<<" points" << endl;

    QString filename2 = "texCoords.txt";
    QFile file2(filename2);
    if (file2.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QTextStream stream(&file2);
        for(int i=0; i<imagePoints.size(); i++)
            stream << imagePoints[i].x()/size.width << " " << imagePoints[i].y()/size.height  << endl;
    }
    file2.close();

}

// save delaunay triangles
void WebcamWidget::saveDelaunay(cv::Mat &frame, QVector<QVector2D> imagePoints)
{
    using namespace cv;
    // Rectangle to be used with Subdiv2D
    Size size = frame.size();
    Rect rect(0, 0, size.width, size.height);
    Subdiv2D subdiv(rect);

    int first = 0; int last = 16;
    float A = (imagePoints[last].y() - imagePoints[first].y()); // A = y2-y1
    float B = (float) -(imagePoints[last].x() - imagePoints[first].x());      // B= -(x2-x1)
    float C = -A*imagePoints[first].x() - B*imagePoints[first].y();
    float M = std::sqrt(A*A+B*B);
    A = A/M; B = B/M; C = C/M;

    for(int i=first+1; i<last; i++) {
            float D = A* imagePoints[i].x()  + B*imagePoints[i].y()  + C;
            QVector2D p(imagePoints[i].x() - 2*A*D, imagePoints[i].y() -2*B*D);
            imagePoints.push_back(p);
    }
     qDebug() << "there are "<<imagePoints.size()<<" qvector2D to triangulate" << endl;

    std::vector<cv::Point2f> cvPts;
    for(int i=0; i<imagePoints.size(); i++)
    {
        cv::Point2f p(imagePoints[i].x(),imagePoints[i].y());
        cvPts.push_back(p);
    }

    qDebug() << "there are "<<cvPts.size()<<" points to triangulate" << endl;
    qDebug() << imagePoints;

    // Insert points into subdiv
    for( std::vector<Point2f>::iterator it = cvPts.begin(); it != cvPts.end(); it++)
    {
          subdiv.insert(*it);
    }

    std::vector<Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);
    std::vector<Point> pt(3);

    QString filename = "delaunay_idx.txt";
    QFile file(filename);

    QList<unsigned int> externalMouth;
    for(int i=61; i<68; i++)
        externalMouth.append(i);

    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
          QTextStream stream(&file);

          for( size_t i = 0; i < triangleList.size(); i++ )
          {
               Vec6f t = triangleList[i];
               pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
               pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
               pt[2] = Point(cvRound(t[4]), cvRound(t[5]));

               // Draw triangles completely inside the image.
               if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
               {

                   int idx0=getPointIdx(pt[0],cvPts);
                   int idx1=getPointIdx(pt[1],cvPts);
                   int idx2=getPointIdx(pt[2],cvPts);
                   /*
                     stream << pt[0].x <<" "<< pt[0].y << endl;
                     stream << pt[1].x <<" "<< pt[1].y << endl;
                     stream << pt[2].x <<" "<< pt[2].y << endl;
                     stream << endl;
                   */
                   //if(!externalMouth.contains(idx0) && !externalMouth.contains(idx1) && !externalMouth.contains(idx2))
                   stream << idx0 << " " << idx1 << " " << idx2 << endl;
               }
          }
    }
    file.close();

    QString filename2 = "texCoords.txt";
    QFile file2(filename2);
    if (file2.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QTextStream stream(&file2);
        for(int i=0; i<imagePoints.size(); i++)
            stream << imagePoints[i].x()/size.width << " " << imagePoints[i].y()/size.height  << endl;
    }
    file2.close();
}

int WebcamWidget::getPointIdx(cv::Point2f pt, std::vector<cv::Point2f> &imagePoints)
{
    double min=1000;
    int idx=-1;
    for(uint i=0; i<imagePoints.size(); i++)
    {
        double dist= (pt.x-imagePoints[i].x)*(pt.x-imagePoints[i].x) + (pt.y-imagePoints[i].y)*(pt.y-imagePoints[i].y);
        if(dist<min)
        {
            min=dist;
            idx=i;
        }
    }

    return idx;
}


QHBoxLayout* WebcamWidget::menu()
{
    QPushButton *loadBtm = new QPushButton("loadImage");
    connect(loadBtm,SIGNAL(pressed()),this,SLOT(loadImage()));

    QPushButton *landBtm = new QPushButton("landmarks");
    connect(landBtm,SIGNAL(pressed()),this,SLOT(drawLandmarks()));

    QPushButton *startBtm = new QPushButton("startStream");
    connect(startBtm,SIGNAL(pressed()),this,SLOT(startStream()));

    QPushButton *stopBtm = new QPushButton("stopStream");
    connect(stopBtm,SIGNAL(pressed()),this,SLOT(stopStream()));

    QHBoxLayout *m_menu = new QHBoxLayout;
    m_menu->addWidget(loadBtm);
    m_menu->addWidget(landBtm);
    m_menu->addWidget(startBtm);
    m_menu->addWidget(stopBtm);

    return m_menu;
}
