#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{

}

void MainWindow::initOpenGLWindows()
{
    m_timer = new QTimer;
    m_timer->setInterval(33);
    connect(m_timer,&QTimer::timeout,this,&MainWindow::renderFrame);

    QHBoxLayout *windowsLayout = new QHBoxLayout;
    imageWidget = new WebcamWidget;
    windowsLayout->addWidget(imageWidget);
    meshWidget = new GLWidget;
    windowsLayout->addWidget(meshWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addItem(windowsLayout);
    mainLayout->addItem(menu.bar);

    connect(menu.loadImg,SIGNAL(pressed()),this,SLOT(loadImage()));
    connect(menu.drawLandmarks,SIGNAL(pressed()),this,SLOT(drawLandmarks()));
    connect(menu.startStream,SIGNAL(pressed()),this,SLOT(startStream()));
    connect(menu.stopStream,SIGNAL(pressed()),this,SLOT(stopStream()));
    connect(menu.buildMeshOnly,SIGNAL(pressed()),this,SLOT(buildMeshOnly()));
    connect(menu.buildTexture,SIGNAL(pressed()),this,SLOT(createTexture()));

    QHBoxLayout *menuLayout = new QHBoxLayout;
    //connect(pstereo.psBtm,SIGNAL(pressed()),this,SLOT(execPS()));
    menuLayout->addWidget(pstereo.menu);
    menuLayout->addWidget(fastm.menu);


    mainLayout->addItem(menuLayout);

    QWidget *widget=new QWidget;
    widget->setLayout(mainLayout);
    this->setCentralWidget(widget);
}

void MainWindow::initMenu()
{

}

void MainWindow::updateMesh()
{
    meshWidget->addMesh("mesh",mesh);
}


void MainWindow::loadImage()
{
    QString fileName  = QFileDialog::getOpenFileName(0, tr("Choose"),"", tr("Images (*.png *.jpg *.jpeg *.bmp *.gif *.pgm)"));
    std::string utf8_fn = fileName.toUtf8().constData();
    if(utf8_fn != "")
    {
        frame = cv::imread(utf8_fn);
        imageWidget->setImage(frame);
    }
}

void MainWindow::startStream()
{
    if(!cap.open(0))
        return;
    m_timer->start();
}

void MainWindow::stopStream()
{
    m_timer->stop();
    cap.release();
}


void MainWindow::renderFrame()
{
    cap >> frame;
    //imageWidget->setImage(frame);
    drawLandmarks();
    //imageWidget->setImage(frame);
}

void MainWindow::drawLandmarks()
{
    if(frame.empty())
        return;

    QVector<QVector2D> landmarks = detector.getLandmarks(frame);

    cv::Mat temp;
    frame.copyTo(temp);
    cv::Scalar color = {0,255,0};
    for(int i=0; i<landmarks.size(); i++)
    {
        cv::Point p = cv::Point(landmarks[i].x(),landmarks[i].y());
        cv::circle(temp,p,2,color,2);
    }
    imageWidget->setImage(temp);

    mesh=Mesh(landmarks,"");
    mesh.scaleToUnity();
    mesh.scaleTextureCoords(frame.rows,frame.cols);
    mesh.modelMatrix.rotate(180,QVector3D(1,0,0));

    meshWidget->addMesh("mesh",mesh);

    meshWidget->meshes["mesh"].createTexture(frame);
}

void MainWindow::buildMeshOnly()
{
    if(frame.empty())
        return;
    qDebug() << "building";

    QVector<QVector2D> landmarks = detector.getLandmarks(frame);
    mesh=Mesh(landmarks,"");
    mesh.scaleToUnity();
    mesh.scaleTextureCoords(frame.rows,frame.cols);
    mesh.modelMatrix.rotate(180,QVector3D(1,0,0));
    meshWidget->addMesh("mesh",mesh);
    qDebug() << "done";
}

void MainWindow::createTexture()
{
    if(frame.empty())
        return;

    QVector<QVector2D> landmarks = detector.getLandmarks(frame);
    if(!landmarks.size())
        return;

    Mesh texmesh=Mesh(landmarks,"");

    texmesh.subdivide(mesh.numOfSubdivisions());
    texmesh.scaleTextureCoords(frame.rows,frame.cols);
    QVector<QVector2D> texCoords = texmesh.texCoords();
    mesh.setTextureCoords(texCoords);

    meshWidget->addMesh("mesh",mesh);
    meshWidget->meshes["mesh"].createTexture(frame);
}
