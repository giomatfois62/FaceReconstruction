#include "meshbuilder.h"


MeshBuilder::MeshBuilder(QWidget *parent) : QWidget(parent)
{
    m_subdivNum = 4;

    detector = new Facedet;
    imageViewer = new WebcamWidget(detector);
    meshViewer = new GLWidget;

    fastmarch = new Fastm(mesh);
    connect(fastmarch,SIGNAL(meshUpdated()),this,SLOT(updateMesh()));

    database = new FaceDatabaseWidget;
    connect(database,SIGNAL(imageChanged(QString)),imageViewer,SLOT(setImage(QString)));

    sfs = new Shapefs(&mesh,detector,database);
    connect(sfs,SIGNAL(meshUpdated()),this,SLOT(updateMesh()));

    QHBoxLayout *winLayout = new QHBoxLayout;
    winLayout->addWidget(imageViewer);
    winLayout->addWidget(meshViewer);

    QHBoxLayout *menuLayout = new QHBoxLayout;
    menuLayout->addWidget(this->menu());
    menuLayout->addWidget(database->menu());
    menuLayout->addWidget(sfs->menu());
    menuLayout->addWidget(fastmarch->menu());
    menuLayout->addWidget(meshViewer->displayMenu());

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addItem(winLayout);
    mainLayout->addItem(imageViewer->menu());
    mainLayout->addItem(menuLayout);

    this->setLayout(mainLayout);
}

MeshBuilder::~MeshBuilder()
{
    delete detector;
}

void MeshBuilder::buildMesh()
{
    cv::Mat temp = imageViewer->frame();
    mesh = detector->buildMesh(temp);
    mesh.subdivide(subdivNum());
    updateMesh();
}

void MeshBuilder::updateMesh()
{
    meshViewer->addMesh("mesh",mesh);
}

void MeshBuilder::saveMesh()
{
    bool ok;
    QString meshName = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                            tr("Mesh name:"), QLineEdit::Normal,
                                            QDir::home().dirName(), &ok);
    if(!meshName.isEmpty())
        mesh.save(meshName);
}

void MeshBuilder::loadMesh()
{
    QString fileName  = QFileDialog::getOpenFileName(0, tr("Choose"),"", tr("Mesh (*.mesh)"));
    if(!fileName.isEmpty())
        mesh.load(fileName);
    updateMesh();
}

void MeshBuilder::applyTexture()
{
    cv::Mat temp = imageViewer->frame();
    if(temp.empty())
        return;

    Mesh texmesh=detector->buildMesh(temp);
    texmesh.subdivide(mesh.numOfSubdivisions());
    texmesh.scaleTextureCoords(temp.rows,temp.cols);
    QVector<QVector2D> texCoords = texmesh.texCoords();
    mesh.setTextureCoords(texCoords);

    updateMesh();
    meshViewer->meshes["mesh"].createTexture(temp);
    meshViewer->drawTexture();
}


QGroupBox* MeshBuilder::menu()
{
    QGroupBox* m_menu = new QGroupBox("Mesh Builder");

    QPushButton *saveBtm = new QPushButton("saveMesh");
    connect(saveBtm,SIGNAL(pressed()),this,SLOT(saveMesh()));

    QPushButton *loadBtm = new QPushButton("loadMesh");
    connect(loadBtm,SIGNAL(pressed()),this,SLOT(loadMesh()));

    QPushButton *buildBtm = new QPushButton("buildMesh");
    connect(buildBtm,SIGNAL(pressed()),this,SLOT(buildMesh()));

    QPushButton *texBtm = new QPushButton("applyTexture");
    connect(texBtm,SIGNAL(pressed()),this,SLOT(applyTexture()));

    QSpinBox *subdivSpin = new QSpinBox;
    subdivSpin->setValue(subdivNum());
    connect(subdivSpin,SIGNAL(valueChanged(int)),this,SLOT(setSubdivNum(int)));
    QLabel *subLabel = new QLabel;
    subLabel->setText("subdiv#");
    QHBoxLayout *subForm = new QHBoxLayout;
    subForm->addWidget(subLabel);
    subForm->addWidget(subdivSpin);

    QGridLayout *menuLayout = new QGridLayout;
    menuLayout->addWidget(buildBtm,0,0);
    menuLayout->addWidget(texBtm,0,1);
    menuLayout->addWidget(loadBtm,1,0);
    menuLayout->addWidget(saveBtm,1,1);
    menuLayout->addItem(subForm,2,0);

    m_menu->setLayout(menuLayout);

    m_menu->setSizePolicy(QSizePolicy( QSizePolicy::Fixed,
                                      QSizePolicy::Preferred));
    return m_menu;
}
