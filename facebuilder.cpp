#include "facebuilder.h"

Facebuilder::Facebuilder(QWidget *parent) : QWidget(parent)
{
    _subdivNum = 4;

    detector = new Facedet;

    // pass face detector to image viewer to enable draw landmarks fucntions
    imageViewer = new WebcamWidget(detector);

    meshViewer = new GLWidget;

    //
    fastmarch = new Fastm(mesh);
    connect(fastmarch,SIGNAL(meshUpdated()),this,SLOT(updateMesh()));

    database = new FaceDatabaseWidget;
    connect(database,SIGNAL(imageChanged(QString)),imageViewer,SLOT(setImage(QString)));

    sfs = new Shapefs(&mesh,detector,database);
    connect(sfs,SIGNAL(meshUpdated()),this,SLOT(updateMesh()));

    initMenu();
}


void Facebuilder::initMenu()
{
    QHBoxLayout *winLayout = new QHBoxLayout;

    winLayout->addWidget(imageViewer);
    winLayout->addWidget(meshViewer);
    qDebug() << "window added";

    QGridLayout *menuLayout = new QGridLayout;
    //menuLayout->addWidget(pstereo->menu,0,1);
    menuLayout->addWidget(sfs->menu(),0,1);
    QVBoxLayout *vla = new QVBoxLayout;
    vla->addWidget(database->menu());
    vla->addWidget(fastmarch->menu());
    menuLayout->addItem(vla,0,0);
    //menuLayout->addWidget(fastmarch->menu(),0,2);
    //menuLayout->addWidget(database->menu(),0,0);
    menuLayout->addWidget(meshViewer->displayMenu(),0,3);
    qDebug() << "menu added";

    QPushButton *saveBtm = new QPushButton("save");
    connect(saveBtm,SIGNAL(pressed()),this,SLOT(saveMesh()));

    QPushButton *buildBtm = new QPushButton("buildMesh");
    connect(buildBtm,SIGNAL(pressed()),this,SLOT(buildMesh()));

    QPushButton *texBtm = new QPushButton("applyTexture");
    connect(texBtm,SIGNAL(pressed()),this,SLOT(applyTexture()));

    QPushButton *estimLightBtm = new QPushButton("estimLight");
    connect(estimLightBtm,SIGNAL(pressed()),this,SLOT(estimateLight()));

    QSpinBox *subdivSpin = new QSpinBox;
    subdivSpin->setValue(subdivNum());
    connect(subdivSpin,SIGNAL(valueChanged(int)),this,SLOT(setSubdivNum(int)));
    QLabel *subLabel = new QLabel;
    subLabel->setText("subdiv#");
    QFormLayout *subForm = new QFormLayout;
    subForm->addRow(subLabel,subdivSpin);

    //QVBoxLayout *vla2 = new QVBoxLayout;
    //vla2->addItem(subForm);
    //vla2->addWidget(meshViewer->displayMenu());
    //menuLayout->addItem(vla2,0,3);
    //imageViewer->menu->addWidget(saveBtm);
    //imageViewer->menu->addWidget(buildBtm);
    //imageViewer->menu->addWidget(texBtm);
    //imageViewer->menu->addWidget(estimLightBtm);
    menuLayout->addItem(subForm,0,3);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addItem(winLayout);
    //mainLayout->addItem(imageViewer->menu);
    mainLayout->addItem(menuLayout);

    this->setLayout(mainLayout);
}


void Facebuilder::buildMesh()
{
    cv::Mat temp = imageViewer->frame();
    mesh = detector->buildMesh(temp);
    mesh.subdivide(subdivNum());
    meshViewer->addMesh("mesh",mesh);
}

void Facebuilder::saveMesh()
{
    mesh.save("mysuperMesh");
    QVector<QVector3D> verts = mesh.positions();
    QVecOperator::save(verts,"simpleVertices.txt");
}

void Facebuilder::updateMesh()
{
    meshViewer->addMesh("mesh",mesh);
}

void Facebuilder::estimateLight()
{
    QVector4D lightDir;
    sfs->estimateLightDirection(imageViewer->frame(), lightDir);
    QVector4D normal;
    normal.setX(1);
    normal.setY( mesh.vertices()[28].normal.x());
    normal.setZ( mesh.vertices()[28].normal.y());
    normal.setW( mesh.vertices()[28].normal.z());

    qDebug() <<" the normal"<<normal;
    normal = normal*mesh.vertices()[28].color.x();
    qDebug() << "the color"<<mesh.vertices()[28].color.x();
    qDebug() << QVector4D::dotProduct(lightDir,normal);
    qDebug() << QVector4D::dotProduct(QVector3D(0,0,1),mesh.vertices()[28].normal*mesh.vertices()[28].color.x());
}

void Facebuilder::applyTexture()
{
    cv::Mat temp = imageViewer->frame();
    if(temp.empty())
        return;

    Mesh texmesh=detector->buildMesh(temp);
    texmesh.subdivide(mesh.numOfSubdivisions());
    texmesh.scaleTextureCoords(temp.rows,temp.cols);
    QVector<QVector2D> texCoords = texmesh.texCoords();
    mesh.setTextureCoords(texCoords);

    meshViewer->addMesh("mesh",mesh);
    meshViewer->meshes["mesh"].createTexture(temp);
}
