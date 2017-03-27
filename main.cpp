#include <QApplication>
#include "mainwindow.h"
#include "facebuilder.h"
#include "meshbuilder.h"
#include "webcamwidget.h"
#include "glwidget.h"
#include "mesh.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3,3);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);


    // FACEBUILDER WIDGET


    MeshBuilder builder;
    builder.resize(1024,768);
    builder.show();

    // WEBCAM WIDGET
    /*
    WebcamWidget *web = new WebcamWidget();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(web);
    layout->addItem(web->menu());

    QWidget *widget = new QWidget;

    widget->setLayout(layout);
    widget->resize(640,480);
    widget->show();
    */

    // WEBCAM WIDGET + DATABASE

    /*
    FaceDatabaseWidget *db = new FaceDatabaseWidget;
    WebcamWidget *web = new WebcamWidget();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(web);
    layout->addItem(web->menu());

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(db->menu());
    mainLayout->addItem(layout);
    QObject::connect(db,SIGNAL(imageChanged(QString)),web,SLOT(setImage(QString)));

    QWidget *widget = new QWidget;

    widget->setLayout(mainLayout);
    widget->resize(640,480);
    widget->show();
    */




    // MESH VIEWER WIDGET
    /*
    Mesh mesh;
    GLWidget *meshViewer = new GLWidget;
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(meshViewer);
    layout->addWidget(meshViewer->displayMenu());

    QWidget *widget = new QWidget;

    widget->setLayout(layout);
    widget->resize(640,480);
    widget->show();

    mesh.load("yaleB3.mesh");
    meshViewer->addMesh("mesh",mesh);
    */



    return app.exec();
}
