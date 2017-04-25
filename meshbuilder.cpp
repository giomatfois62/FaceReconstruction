#include <QComboBox>
#include "meshbuilder.h"


MeshBuilder::MeshBuilder(QWidget *parent) : QWidget(parent)
{
    m_subdivNum = 20;
    m_subdivType = "ADAPTIVE";
    m_edgeTol = 0.003;
    m_gridType = "IMAGE";

    detector = new Facedet;
    imageViewer = new WebcamWidget(detector);
    meshViewer = new GLWidget;

    fastmarch = new Fastm(m_mesh);
    connect(fastmarch,SIGNAL(meshUpdated()),this,SLOT(updateMesh()));

    database = new FaceDatabaseWidget;
    connect(database,SIGNAL(imageChanged(QString)),imageViewer,SLOT(setImage(QString)));

    sfs = new Shapefs(&m_mesh,detector,database);
    connect(sfs,SIGNAL(meshUpdated()),this,SLOT(updateMesh()));

    QHBoxLayout *winLayout = new QHBoxLayout;
    winLayout->addWidget(imageViewer);
    winLayout->addWidget(meshViewer);

    QHBoxLayout *menuLayout = new QHBoxLayout;
    menuLayout->addWidget(this->menu());
    menuLayout->addWidget(database->menu());
    menuLayout->addWidget(sfs->menu());
    menuLayout->addWidget(fastmarch->menu());
    menuLayout->addWidget(meshViewer->menu());

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
    if(temp.empty())
        return;

    Mesh mesh;

    if(m_gridType == "IMAGE")
        mesh = buildMeshFromImage();
    else
        mesh = buildMeshFromFace();

    m_mesh = mesh;
    m_mesh.scaleToUnity();
    m_mesh.modelMatrix.rotate(180,QVector3D(1,0,0));
    m_mesh.applyTransformation();
    m_mesh.setEdgeTolerance(edgeTol());
    m_mesh.subdivide(subdivNum(),subdivType());
    m_mesh.setGridType(m_gridType);
    updateMesh();
}

void MeshBuilder::convertToNonAdaptive()
{
    //if(m_gridType == "IMAGE")
    //    return;

    Mesh mesh;

    if(m_gridType == "IMAGE")
        mesh = buildMeshFromImage();
    else
        mesh = buildMeshFromFace();

    mesh.scaleToUnity();
    mesh.modelMatrix.rotate(180,QVector3D(1,0,0));
    mesh.applyTransformation();
    mesh.setEdgeTolerance(edgeTol());
    mesh.subdivide(qMin(subdivNum(),5),"NON-ADAPTIVE");
    mesh.setGridType(m_gridType);

    QVector<Vertex> vertices = mesh.vertices();
    QVector<unsigned int> indices = mesh.indices();
    // interpolate m_mesh on new non-adaptive mesh
    for(int i=0; i<vertices.size(); i++)
    {
        // find triangle in m_mesh containing vertices[i] (only x,y coords)
        QVector3D p = vertices[i].position;
        float barCoords[3];
        unsigned int firstIndex;
        bool found = findMatchingTriangle(p,barCoords,firstIndex);
        if(found)
        {
            //qDebug() << "Found matching triangle for vertex "<<i<<": barCoords->"<<barCoords[0]<<" "<<barCoords[1]<<" "<<barCoords[2]<<" firstIndex->"<<firstIndex;
            Vertex matching_p1 = m_mesh.vertices()[m_mesh.indices()[firstIndex]];
            Vertex matching_p2 = m_mesh.vertices()[m_mesh.indices()[firstIndex+1]];
            Vertex matching_p3 = m_mesh.vertices()[m_mesh.indices()[firstIndex+2]];
            vertices[i].position.setZ(barCoords[0]*matching_p1.position[2] + barCoords[1]*matching_p2.position[2] + barCoords[2]*matching_p3.position[2]);
            vertices[i].normal = (barCoords[0]*matching_p1.normal + barCoords[1]*matching_p2.normal + barCoords[2]*matching_p3.normal);
            vertices[i].color = (barCoords[0]*matching_p1.color + barCoords[1]*matching_p2.color + barCoords[2]*matching_p3.color);
            vertices[i].texCoords = (barCoords[0]*matching_p1.texCoords + barCoords[1]*matching_p2.texCoords + barCoords[2]*matching_p3.texCoords);
            //qDebug() << "updated vertex:"<<vertices[i].position<<" "<<vertices[i].normal<<" "<<vertices[i].color;
        }
        else
        {
            //qDebug() << "Matching Triangle not found for vertex "<<i<<" :( falling back with matched vertex "<<firstIndex;
            Vertex v = m_mesh.vertices()[firstIndex];
            vertices[i].position.setZ(v.position.z());
            vertices[i].normal = v.normal;
            vertices[i].color = v.color;
            vertices[i].texCoords = v.texCoords;

            //vertices[i].color = QVector3D(0.0f,0.0f,0.0f);
        }
        // calculate barycentric coordinates
        // interpolate vertices[i] z coord
    }

    m_mesh.setVertices(vertices);

    m_mesh.setIndices(indices);
    updateMesh();
    // set m_mesh = mesh
}

void MeshBuilder::updateMesh()
{
    meshViewer->addMesh("mesh",m_mesh);
}

Mesh MeshBuilder::buildMeshFromFace()
{
    cv::Mat temp = imageViewer->frame();

    QVector<QVector2D> landmarks = detector->getLandmarks(temp);
    QVector<unsigned int> indices;
    QVector<Vertex> vertices;
    for(int i=0; i<landmarks.size(); i++)
    {
        Vertex v;
        v.position = QVector3D(landmarks[i]);
        v.normal = QVector3D(0,0,1);
        v.color = QVector3D(1.0f,0,0);
        v.texCoords = landmarks[i];
        vertices.push_back(v);
    }
    QVecOperator::load(indices,"../simpleIndices_original");
    Mesh mesh(vertices,indices);
    return mesh;
}

Mesh MeshBuilder::buildMeshFromImage()
{
    cv::Mat temp = imageViewer->frame();

    QVector<QVector2D> landmarks = detector->getLandmarks(temp);
    QVector<unsigned int> indices;
    QVector<Vertex> vertices;

    QVector<QVector2D> resized_landmarks = landmarks;
    double amin = std::numeric_limits<double>::max();
    double amax = std::numeric_limits<double>::min();
    QVector2D minDimension(amin,amin);
    QVector2D maxDimension(amax,amax);

    for(int i=0; i<landmarks.size(); i++)
    {
        QVector2D vec = landmarks[i];
        if(vec.x() < minDimension.x())
            minDimension.setX(vec.x());
        if(vec.y() < minDimension.y())
            minDimension.setY(vec.y());
        if(vec.x() > maxDimension.x())
            maxDimension.setX(vec.x());
        if(vec.y() > maxDimension.y())
            maxDimension.setY(vec.y());
    }

    // Calculate scale and translation needed to center and fit on screen
    //float dist = qMax(maxDimension.x() - minDimension.x(), maxDimension.y()-minDimension.y(), maxDimension.z() - minDimension.z());

    // Calculate scale and translation needed to center and fit on screen
    float scale1 = (480.0-landmarks[30].y())/(maxDimension.y()-landmarks[30].y());
    float scale2 = (landmarks[30].y())/(landmarks[30].y()-minDimension.y());
    float scale = qMin(scale1,scale2);
    QVector2D center(landmarks[30]);
    for(int i=0; i<landmarks.size();i++)
        resized_landmarks[i] = (landmarks[i] - center)*scale + center;

    for(int i=0; i<resized_landmarks.size(); i++)
    {
        QVector2D vec = resized_landmarks[i];
        if(vec.x() < minDimension.x())
            minDimension.setX(vec.x());
        if(vec.y() < minDimension.y())
            minDimension.setY(vec.y());
        if(vec.x() > maxDimension.x())
            maxDimension.setX(vec.x());
        if(vec.y() > maxDimension.y())
            maxDimension.setY(vec.y());
    }

    minDimension.setX(0);
    minDimension.setY(0);
    maxDimension.setX(640);
    maxDimension.setY(480);
    for(int i=0; i<5; i++)
        resized_landmarks[i].setX( minDimension.x() );
    resized_landmarks[5].setX( minDimension.x() );
    resized_landmarks[5].setY( maxDimension.y() );
    for(int i=6; i<11; i++)
        resized_landmarks[i].setY( maxDimension.y() );
    resized_landmarks[11].setX( maxDimension.x() );
    resized_landmarks[11].setY( maxDimension.y() );
    for(int i=12; i<17; i++)
        resized_landmarks[i].setX( maxDimension.x() );
    resized_landmarks[17].setX( minDimension.x() );
    resized_landmarks[17].setY( minDimension.y() );
    for(int i=18; i<26; i++)
        resized_landmarks[i].setY( minDimension.y() );
    resized_landmarks[26].setX( maxDimension.x() );
    resized_landmarks[26].setY( minDimension.y() );



    for(int i=0; i<landmarks.size(); i++)
    {
        if(i==5 || i==11 ||i==17 ||i==26 || i == 30)
        {
            Vertex v;
            //if(i<28)
            //    v.position = QVector3D(landmarks[i].x(),landmarks[i].y(),2.0*dist);
            //else
            v.position = QVector3D(resized_landmarks[i]);
            v.normal = QVector3D(0,0,1);
            v.color = QVector3D(1.0f,0,0);
            v.texCoords = resized_landmarks[i];
            vertices.push_back(v);
        }
    }

    indices = {0,1,4,1,3,4,2,3,4,2,4,0};
    Mesh mesh(vertices,indices);
    return mesh;
}

bool MeshBuilder::findMatchingTriangle(QVector3D p, float barCoords[3], unsigned int &firstIndex)
{
    QVector2D p1, p2, p3;
    float alpha, beta, gamma;
    //float max = -1000;

    QVector<unsigned int> triangles = m_mesh.indices();
    QVector<Vertex> vertices = m_mesh.vertices();

    for(int i=0; i<triangles.size(); i=i+3)
    {
        p1 = QVector2D(vertices[triangles[i]].position[0], vertices[triangles[i]].position[1]);
        p2 = QVector2D(vertices[triangles[i+1]].position[0], vertices[triangles[i+1]].position[1]);
        p3 = QVector2D(vertices[triangles[i+2]].position[0], vertices[triangles[i+2]].position[1]);

        alpha = ((p2.y() - p3.y())*(p.x() - p3.x()) + (p3.x() - p2.x())*(p.y() - p3.y())) /
                ((p2.y() - p3.y())*(p1.x() - p3.x()) + (p3.x() - p2.x())*(p1.y() - p3.y()));
        beta = ((p3.y() - p1.y())*(p.x() - p3.x()) + (p1.x() - p3.x())*(p.y() - p3.y())) /
                ((p2.y() - p3.y())*(p1.x() - p3.x()) + (p3.x() - p2.x())*(p1.y() - p3.y()));
        gamma = 1.0f - alpha - beta;

        if( alpha >=0 && beta >=0  && gamma >=0 )
        {
            barCoords[0] = alpha;
            barCoords[1] = beta;
            barCoords[2] = gamma;
            firstIndex = i;
            //float interp = alpha*verts[triangles[i]].z() + beta*verts[triangles[i+1]].z() + gamma*verts[triangles[i+2]].z();
            //if(interp > max)
            //    max = interp;
            return true;
        }
    }

    float minDist = std::numeric_limits<float>::max();
    // workaround to triangle not found: find closest vertex
    for(int i=0; i<vertices.size(); i++)
    {
        QVector3D v = vertices[i].position;
        float dist = (v.x()-p.x())*(v.x()-p.x()) + (v.y()-p.y())*(v.y()-p.y());
        dist = sqrt(dist);
        if(dist<minDist){
            minDist = dist;
            firstIndex = i;
        }
    }
    return false;

}

void MeshBuilder::saveMesh()
{
    bool ok;
    QString meshName = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                            tr("Mesh name:"), QLineEdit::Normal,
                                            QDir::home().dirName(), &ok);
    if(!meshName.isEmpty())
        m_mesh.save(meshName);
}

void MeshBuilder::loadMesh()
{
    QString fileName  = QFileDialog::getOpenFileName(0, tr("Choose"),"", tr("Mesh (*.mesh)"));
    if(!fileName.isEmpty())
        m_mesh.load(fileName);
    updateMesh();
}

void MeshBuilder::applyTexture()
{
    cv::Mat temp = imageViewer->frame();
    if(temp.empty())
        return;

    /*
    Mesh texmesh;
    if(m_gridType == "IMAGE")
        texmesh = buildMeshFromImage();
    else
        texmesh = buildMeshFromFace();

    texmesh.setEdgeTolerance(edgeTol());
    texmesh.subdivide(m_mesh.numOfSubdivisions(),m_mesh.subdivType()); // commented
    texmesh.scaleTextureCoords(temp.rows,temp.cols);
    QVector<QVector2D> texCoords = texmesh.texCoords();
    m_mesh.setTextureCoords(texCoords);
    m_mesh.setGridType(m_gridType);
    */
    m_mesh.scaleTextureCoords(temp.rows,temp.cols);

    updateMesh();
    meshViewer->meshes["mesh"].createTexture(temp);
    meshViewer->setActiveUniform("texture");
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

    QDoubleSpinBox *edgeSpin = new QDoubleSpinBox;
    edgeSpin->setDecimals(4);
    edgeSpin->setValue(edgeTol());
    connect(edgeSpin,SIGNAL(valueChanged(double)),this,SLOT(setEdgeTolerance(double)));
    QLabel *edgeLabel = new QLabel;
    edgeLabel->setText("edgeTol");
    QHBoxLayout *edgeForm = new QHBoxLayout;
    edgeForm->addWidget(edgeLabel);
    edgeForm->addWidget(edgeSpin);

    QComboBox *comboBox = new QComboBox;
    connect(comboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(setSubdivType(QString)));
    comboBox->addItem(tr("ADAPTIVE"));
    comboBox->addItem(tr("NON-ADAPTIVE"));

    QComboBox *comboBox2 = new QComboBox;
    connect(comboBox2,SIGNAL(currentTextChanged(QString)),this,SLOT(setGridType(QString)));
    comboBox2->addItem(tr("IMAGE"));
    comboBox2->addItem(tr("FACE"));

    QPushButton *convertBtm = new QPushButton("convertMesh");
    connect(convertBtm,SIGNAL(pressed()),this,SLOT(convertToNonAdaptive()));

    QGridLayout *menuLayout = new QGridLayout;
    menuLayout->addWidget(buildBtm,0,0);
    menuLayout->addWidget(texBtm,0,1);
    menuLayout->addWidget(loadBtm,1,0);
    menuLayout->addWidget(saveBtm,1,1);
    menuLayout->addItem(subForm,2,0);
    menuLayout->addItem(edgeForm,2,1);
    menuLayout->addWidget(comboBox,3,0);
    menuLayout->addWidget(comboBox2,3,1);
    menuLayout->addWidget(convertBtm,4,1);

    m_menu->setLayout(menuLayout);

    m_menu->setSizePolicy(QSizePolicy( QSizePolicy::Fixed,
                                      QSizePolicy::Preferred));
    return m_menu;
}
