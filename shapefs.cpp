#include "shapefs.h"

Shapefs::Shapefs(Mesh *pmesh, QWidget *parent):
    QWidget(parent),
    mesh(pmesh)
{
    detector = new Facedet;
    db = new FaceDatabaseWidget;
    initDefault();
}

Shapefs::Shapefs(Mesh *pmesh, Facedet *det, QWidget *parent):
    QWidget(parent),
    mesh(pmesh), detector(det)

{
    db = new FaceDatabaseWidget;
    initDefault();
}

Shapefs::Shapefs(Mesh *pmesh, FaceDatabaseWidget *database, QWidget *parent):
    QWidget(parent),
    mesh(pmesh),
    db(database)
{
    detector = new Facedet;
    initDefault();
}

Shapefs::Shapefs(Mesh *pmesh, Facedet *det, FaceDatabaseWidget *database, QWidget *parent):
    QWidget(parent),
    mesh(pmesh),
    detector(det),
    db(database)
{
    initDefault();
}

void Shapefs::initDefault()
{
    m_threshold=.1;
    m_maxAzimuth=45;
    m_maxElevation=45;
    m_minImageNumber=4;
}

QGroupBox* Shapefs::menu()
{
    QGroupBox* m_menu = new QGroupBox("Photometric Stereo");

    QPushButton *psBtm = new QPushButton("execPS");
    connect(psBtm,SIGNAL(pressed()),this,SLOT(execPhotometric()));

    QSpinBox *maxAzimuthSpin = new QSpinBox;
    connect(maxAzimuthSpin,SIGNAL(valueChanged(int)),this,SLOT(setMaxAzimuth(int)));
    maxAzimuthSpin->setValue(maxAzimuth());
    QLabel *azLabel = new QLabel;
    azLabel->setText("max Azim: ");
    QFormLayout *form1 = new QFormLayout;
    form1->addRow(azLabel,maxAzimuthSpin);

    QSpinBox *maxElevationSpin = new QSpinBox;
    connect(maxElevationSpin,SIGNAL(valueChanged(int)),this,SLOT(setMaxElevation(int)));
    maxElevationSpin->setValue(maxElevation());
    QLabel *eleLabel = new QLabel;
    eleLabel->setText("max Elev: ");
    QFormLayout *form2 = new QFormLayout;
    form2->addRow(eleLabel,maxElevationSpin);

    QSpinBox *minImageNumberSpin = new QSpinBox;
    connect(minImageNumberSpin,SIGNAL(valueChanged(int)),this,SLOT(setMinImageNumber(int)));
    minImageNumberSpin->setValue(minImageNumber());
    QLabel *imgLabel = new QLabel;
    imgLabel->setText("min Img#: ");
    QFormLayout *form3 = new QFormLayout;
    form3->addRow(imgLabel,minImageNumberSpin);

    QDoubleSpinBox *thresholdSpin = new QDoubleSpinBox;
    connect(thresholdSpin,SIGNAL(valueChanged(double)),this,SLOT(setThreshold(double)));
    thresholdSpin->setValue(threshold());
    QLabel *threshLabel = new QLabel;
    threshLabel->setText("threshold: ");
    QHBoxLayout *form4 = new QHBoxLayout;
    form4->addWidget(threshLabel);
    form4->addWidget(thresholdSpin);

    QLabel *filterLabel = new QLabel;
    filterLabel->setText("filter:");
    filter1 = new QRadioButton("none");
    filter2 = new QRadioButton("intensity");
    filter3 = new QRadioButton("p-error");
    connect(filter1,SIGNAL(clicked(bool)),this,SLOT(setFilter()));
    connect(filter2,SIGNAL(clicked(bool)),this,SLOT(setFilter()));
    connect(filter3,SIGNAL(clicked(bool)),this,SLOT(setFilter()));

    filter1->setChecked(true);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(psBtm,0,0);
    layout->addItem(form1,1,0);
    layout->addItem(form2,2,0);
    layout->addItem(form3,3,0);


    layout->addWidget(filterLabel,0,1);
    layout->addWidget(filter1,1,1);
    layout->addWidget(filter2,2,1);
    layout->addWidget(filter3,3,1);
    layout->addItem(form4,4,0);

    m_menu->setLayout(layout);
    m_menu->setSizePolicy(QSizePolicy( QSizePolicy::Fixed,
                                      QSizePolicy::Preferred));

    return m_menu;
}

void Shapefs::setFilter()
{
    if(filter1->isChecked())
        setFilter("none");

    if(filter2->isChecked())
        setFilter("intensity");

    if(filter3->isChecked())
        setFilter("p-error");

    qDebug() << "filter selected: "<<m_filterType;
}

void Shapefs::execPhotometric()
{
    if(!db->imageList().size())
        return;
    // load image intensities & light directions from database
    qDebug() << "loading data for photometric stereo...";
    QVector<QVector<float> > intensities;
    QVector<QVector3D> lightDirections;
    loadSfsData(intensities,lightDirections);
    qDebug() << "images data loaded! starting photometric stereo...";
    // execute photometric stereo
    execPS(intensities,lightDirections);
    emit meshUpdated();
}

void Shapefs::estimateLightDirection(cv::Mat &img, QVector4D &light)
{
    QVector<Vertex> verts = mesh->vertices();

    float sommaAlb = 0;   float sommaAlbNx = 0; float sommaAlbNy =0; float sommaAlbNz =0;
                          float sommaNxNx = 0;  float sommaNxNy= 0;  float sommaNxNz= 0;
                                                float sommaNyNy= 0;  float sommaNyNz= 0;
                                                                     float sommaNzNz= 0;
    float b0 = 0;
    float b1 = 0;
    float b2 = 0;
    float b3 = 0;

    for(int i=0; i< verts.size(); i++)
    {
        int k = verts[i].texCoords.y();
        int l = verts[i].texCoords.x();
        float I0 = img.at<uchar>(k,l);

        float smoothAlbedo = 0;
        smoothAlbedo = verts[i].color.x();
        //for(int j=0; j<neighbours[i].size(); j++)
        //      smoothAlbedo += verts[neighbours[i].at(j)].color.x();
        //smoothAlbedo /= (float)neighbours[i].size();

        float albsqrd = smoothAlbedo*smoothAlbedo;

        sommaAlb += albsqrd;
        float albnx = albsqrd*verts[i].normal.x();
        float albny = albsqrd*verts[i].normal.y();
        float albnz = albsqrd*verts[i].normal.z();
        sommaAlbNx += albnx;  sommaAlbNy += albny; sommaAlbNz += albnz;

        sommaNxNx += albnx*verts[i].normal.x();  sommaNxNy += albnx*verts[i].normal.y();  sommaNxNz += albnx*verts[i].normal.z();
                                          sommaNyNy += albny*verts[i].normal.y();  sommaNyNz += albny*verts[i].normal.z();
                                                                            sommaNzNz += albnz*verts[i].normal.z();

        //float I0 =  (frame.at<uchar>(k+1,l) + frame.at<uchar>(k-1,l) + frame.at<uchar>(k,l+1) + frame.at<uchar>(k,l-1) )/4;
        b0 += smoothAlbedo*I0;
        b1 += smoothAlbedo*I0*verts[i].normal.x();
        b2 += smoothAlbedo*I0*verts[i].normal.y();
        b3 += smoothAlbedo*I0*verts[i].normal.z();
    }

    QMatrix4x4 moore;
    moore(0,0) = sommaAlb;  moore(0,1) = sommaAlbNx; moore(0,2) = sommaAlbNy;  moore(0,3) = sommaAlbNz;
                            moore(1,1) = sommaNxNx;  moore(1,2) = sommaNxNy;   moore(1,3) = sommaNxNz;
                                                     moore(2,2) = sommaNyNy;   moore(2,3) = sommaNyNz;
                                                                               moore(3,3) = sommaNzNz;
   for(int i =0; i<4; i++)
       for(int j=i; j<4; j++)
           moore(j,i) = moore(i,j);

   moore = moore.inverted();
   qDebug() << moore;

   QVector4D data(b0,b1,b2,b3);
   qDebug() << "data"<<data;

   light = moore*data;
   qDebug() << "Estimated 4D light: "<< light;
   qDebug() << "whose norm is: "<<light.length();
}


void Shapefs::loadSfsData(QVector<QVector<float> > &intensities, QVector<QVector3D> &lightDirections)
{
    // build a dummy mesh with the first frame of the database to get textureCoords
    QString framesPath = "../Database/"+db->name()+"/";
    QString firstFramePath = framesPath+db->imageList()[0];
    cv::Mat firstFrame = cv::imread(firstFramePath.toUtf8().constData(),true);

    if(!mesh->vertices().size())
    {
        Mesh texmesh = detector->buildMesh(firstFrame);
        texmesh.subdivide(4);
        mesh = new Mesh(texmesh);
        //qDebug() << mesh->vertices().size()<<" "<<mesh->vertices()[0].position;
    }
    else
    {
        //texmesh.subdivide(mesh->numOfSubdivisions());
    }
    intensities.resize(mesh->vertices().size());

    // define limits on light angles
    float minz = cos(m_maxAzimuth*3.1415/180.0);
    float maxy = sin(m_maxElevation*3.1415/180.0);

    // load vertex intensities and light directions
    for(int i=0; i<db->imageList().size(); i++)
    {
        if(db->lightDirections()[i].z() >= minz && fabs(db->lightDirections()[i].y())<= maxy)
        {
            QString toLoad = framesPath + db->imageList()[i];
            cv::Mat frame = cv::imread(toLoad.toUtf8().constData(),true);
            cv::cvtColor( frame, frame, CV_BGR2GRAY );
            QVector3D adjustedLights = db->lightDirections()[i];
            adjustedLights.setX(-adjustedLights.x());
            adjustedLights.setY(adjustedLights.y());
            adjustedLights.setZ(adjustedLights.z());
            //lightDirections.push_back(db->lightDirections()[i]);
            lightDirections.push_back(adjustedLights);

            for(int j=0; j<mesh->vertices().size(); j++)
            {
                int k = mesh->vertices()[j].texCoords.y();
                int l = mesh->vertices()[j].texCoords.x();
                float I0 = frame.at<uchar>(k,l) ;
                //float I0 = ( frame.at<uchar>(k+1,l) + frame.at<uchar>(k-1,l) + frame.at<uchar>(k,l+1) + frame.at<uchar>(k,l-1)  )/4.0;
                intensities[j].push_back(I0);
            }
        }
    }
    //done!
}

void Shapefs::execPS(QVector<QVector<float> > &intensities, QVector<QVector3D> &lightDirections)
{
    if(!intensities.size())
        return;

    //qDebug() << "started";
    // build and invert linear system matrix only once
    QMatrix3x3 matrix = buildSystemMatrix(lightDirections);
    invertQMatrix3x3_QR(matrix);

    //qDebug() << "systemBuilded";

    // solve linear system for each vertex and store computed normal and color
    QVector<Vertex> vertices = mesh->vertices();
    for(int i=0; i< vertices.size(); i++)
    {

        // solve system once with all images available
        QVector3D data = buildSystemData(intensities[i],lightDirections);

        QVector3D normal=solveSystem(matrix,data);


        float albedo = normal.length();
        if(albedo>255)
            albedo = 255;
        normal.normalize();

        vertices[i].color=QVector3D(albedo,albedo,albedo);
        vertices[i].normal=normal;

        if(m_filterType == "none")
        {
            continue;
        }

        if(m_filterType == "intensity")
        {

        }

        if(m_filterType == "p-error")
        {
            // compute p-error map and discardx all images for wich p-error is higher than threshold
            QVector3D norm = albedo*normal;
            QMap<float,unsigned int> errorMap = computeErrorMap(norm,lightDirections,intensities[i]);
            QMap<float,unsigned int>::iterator it = errorMap.end();
            while(it!=errorMap.begin() && errorMap.size() > m_minImageNumber)
            {
                it--;

                if(it.key() > m_threshold)
                {
                    it=errorMap.erase(it);
                }
            }

            QVector<float> newIntensities;
            QVector<QVector3D> newLights;
            for(int j=0; j<errorMap.values().size(); j++)
            {
                newIntensities.push_back(intensities[i][errorMap.values().at(j)]);
                newLights.push_back(lightDirections[errorMap.values().at(j)]);
            }
            QMatrix3x3 newMatrix = buildSystemMatrix(newLights);
            invertQMatrix3x3_QR(newMatrix);

            // solve system once with all images available
            QVector3D newData = buildSystemData(newIntensities,newLights);
            QVector3D newNormal=solveSystem(newMatrix,newData);

            normal = newNormal;
            albedo = normal.length();

            if(albedo>255)
                albedo = 255;
            normal.normalize();

            float perror=0;
            for(int j=0; j<errorMap.keys().size(); j++)
            {
                perror += errorMap.keys().at(j);
            }
            perror /= errorMap.keys().size();

            vertices[i].color=QVector3D(albedo, newIntensities.size()/float(intensities[i].size()), perror/.2);
            vertices[i].normal=normal;
        }


    }

    qDebug() << "processed all "<<vertices.size()<<" vertices";

    mesh->setVertices(vertices);
}

QMatrix3x3 Shapefs::buildSystemMatrix(QVector<QVector3D> &lightsVec)
{
    QMatrix3x3 LS;
    LS.fill(0);

    for(int j=0; j< lightsVec.size(); j++)
    {
        LS(0,0) += lightsVec[j].x()*lightsVec[j].x();
        LS(0,1) += lightsVec[j].x()*lightsVec[j].y();
        LS(0,2) += lightsVec[j].x()*lightsVec[j].z();
        LS(1,1) += lightsVec[j].y()*lightsVec[j].y();
        LS(1,2) += lightsVec[j].y()*lightsVec[j].z();
        LS(2,2) += lightsVec[j].z()*lightsVec[j].z();
    }

    LS(1,0) = LS(0,1);
    LS(2,0) = LS(0,2);
    LS(2,1) = LS(1,2);

    return LS;
}

QVector3D Shapefs::buildSystemData(QVector<float> &intensityMap, QVector<QVector3D> &lightsVec)
{
    QVector3D data;

    for(int j=0; j< intensityMap.size(); j++)
    {
        float I0 = intensityMap[j];
        data[0] += lightsVec[j].x()*I0;
        data[1] += lightsVec[j].y()*I0;
        data[2] += lightsVec[j].z()*I0;
    }

    return data;
}

QVector3D Shapefs::solveSystem(QMatrix3x3 &inverse, QVector3D &data)
{
    QVector3D result;

    for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
            result[i] += inverse(i,j) * data[j];

    return result;
}


void Shapefs::invertQMatrix3x3(QMatrix3x3 &moore)
{
    double determinant =    +moore(0,0)*(moore(1,1)*moore(2,2)-moore(2,1)*moore(1,2))
                            -moore(0,1)*(moore(1,0)*moore(2,2)-moore(1,2)*moore(2,0))
                            +moore(0,2)*(moore(1,0)*moore(2,1)-moore(1,1)*moore(2,0));
    if(!determinant)
        return;

    double invdet = 1/determinant;

    QMatrix3x3 result;

    result(0,0) =  (moore(1,1)*moore(2,2)-moore(2,1)*moore(1,2))*invdet;
    result(1,0) = -(moore(0,1)*moore(2,2)-moore(0,2)*moore(2,1))*invdet;
    result(2,0) =  (moore(0,1)*moore(1,2)-moore(0,2)*moore(1,1))*invdet;
    result(0,1) = -(moore(1,0)*moore(2,2)-moore(1,2)*moore(2,0))*invdet;
    result(1,1) =  (moore(0,0)*moore(2,2)-moore(0,2)*moore(2,0))*invdet;
    result(2,1) = -(moore(0,0)*moore(1,2)-moore(1,0)*moore(0,2))*invdet;
    result(0,2) =  (moore(1,0)*moore(2,1)-moore(2,0)*moore(1,1))*invdet;
    result(1,2) = -(moore(0,0)*moore(2,1)-moore(2,0)*moore(0,1))*invdet;
    result(2,2) =  (moore(0,0)*moore(1,1)-moore(1,0)*moore(0,1))*invdet;

    moore = result;
}

void Shapefs::invertQMatrix3x3_QR(QMatrix3x3 &toInvert)
{
    float A[3][3];
    for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
            A[i][j] = toInvert(i,j);

    float Q[3][3];
    float D[3][3];
    diagonalize(A,Q,D);

    QMatrix3x3 QQ;
    QMatrix3x3 DD;
    for(int i=0; i<3; i++)
    {
        for(int j=0; j<3; j++)
        {
            QQ(i,j)=Q[i][j];
            DD(i,j)=D[i][j];
        }
        // we should be sure that no eigenvalue is zero
        DD(i,i) = 1.0/DD(i,i);
    }

    toInvert = QQ*DD*QQ.transposed();
}

// Slightly modified version of  Stan Melax's code for 3x3 matrix diagonalization (Thanks Stan!)
// source: http://www.melax.com/diag.html?attredirects=0
void Shapefs::diagonalize(const float (&A)[3][3], float (&Q)[3][3], float (&D)[3][3])
{
    // A must be a symmetric matrix.
    // returns Q and D such that
    // Diagonal matrix D = QT * A * Q;  and  A = Q*D*QT
    const int maxsteps=24;  // certainly wont need that many.
    int k0, k1, k2;
    float o[3], m[3];
    float q [4] = {0.0,0.0,0.0,1.0};
    float jr[4];
    float sqw, sqx, sqy, sqz;
    float tmp1, tmp2, mq;
    float AQ[3][3];
    float thet, sgn, t, c;
    for(int i=0;i < maxsteps;++i)
    {
        // quat to matrix
        sqx      = q[0]*q[0];
        sqy      = q[1]*q[1];
        sqz      = q[2]*q[2];
        sqw      = q[3]*q[3];
        Q[0][0]  = ( sqx - sqy - sqz + sqw);
        Q[1][1]  = (-sqx + sqy - sqz + sqw);
        Q[2][2]  = (-sqx - sqy + sqz + sqw);
        tmp1     = q[0]*q[1];
        tmp2     = q[2]*q[3];
        Q[1][0]  = 2.0 * (tmp1 + tmp2);
        Q[0][1]  = 2.0 * (tmp1 - tmp2);
        tmp1     = q[0]*q[2];
        tmp2     = q[1]*q[3];
        Q[2][0]  = 2.0 * (tmp1 - tmp2);
        Q[0][2]  = 2.0 * (tmp1 + tmp2);
        tmp1     = q[1]*q[2];
        tmp2     = q[0]*q[3];
        Q[2][1]  = 2.0 * (tmp1 + tmp2);
        Q[1][2]  = 2.0 * (tmp1 - tmp2);

        // AQ = A * Q
        AQ[0][0] = Q[0][0]*A[0][0]+Q[1][0]*A[0][1]+Q[2][0]*A[0][2];
        AQ[0][1] = Q[0][1]*A[0][0]+Q[1][1]*A[0][1]+Q[2][1]*A[0][2];
        AQ[0][2] = Q[0][2]*A[0][0]+Q[1][2]*A[0][1]+Q[2][2]*A[0][2];
        AQ[1][0] = Q[0][0]*A[0][1]+Q[1][0]*A[1][1]+Q[2][0]*A[1][2];
        AQ[1][1] = Q[0][1]*A[0][1]+Q[1][1]*A[1][1]+Q[2][1]*A[1][2];
        AQ[1][2] = Q[0][2]*A[0][1]+Q[1][2]*A[1][1]+Q[2][2]*A[1][2];
        AQ[2][0] = Q[0][0]*A[0][2]+Q[1][0]*A[1][2]+Q[2][0]*A[2][2];
        AQ[2][1] = Q[0][1]*A[0][2]+Q[1][1]*A[1][2]+Q[2][1]*A[2][2];
        AQ[2][2] = Q[0][2]*A[0][2]+Q[1][2]*A[1][2]+Q[2][2]*A[2][2];
        // D = Qt * AQ
        D[0][0] = AQ[0][0]*Q[0][0]+AQ[1][0]*Q[1][0]+AQ[2][0]*Q[2][0];
        D[0][1] = AQ[0][0]*Q[0][1]+AQ[1][0]*Q[1][1]+AQ[2][0]*Q[2][1];
        D[0][2] = AQ[0][0]*Q[0][2]+AQ[1][0]*Q[1][2]+AQ[2][0]*Q[2][2];
        D[1][0] = AQ[0][1]*Q[0][0]+AQ[1][1]*Q[1][0]+AQ[2][1]*Q[2][0];
        D[1][1] = AQ[0][1]*Q[0][1]+AQ[1][1]*Q[1][1]+AQ[2][1]*Q[2][1];
        D[1][2] = AQ[0][1]*Q[0][2]+AQ[1][1]*Q[1][2]+AQ[2][1]*Q[2][2];
        D[2][0] = AQ[0][2]*Q[0][0]+AQ[1][2]*Q[1][0]+AQ[2][2]*Q[2][0];
        D[2][1] = AQ[0][2]*Q[0][1]+AQ[1][2]*Q[1][1]+AQ[2][2]*Q[2][1];
        D[2][2] = AQ[0][2]*Q[0][2]+AQ[1][2]*Q[1][2]+AQ[2][2]*Q[2][2];
        o[0]    = D[1][2];
        o[1]    = D[0][2];
        o[2]    = D[0][1];
        m[0]    = fabs(o[0]);
        m[1]    = fabs(o[1]);
        m[2]    = fabs(o[2]);

        k0      = (m[0] > m[1] && m[0] > m[2])?0: (m[1] > m[2])? 1 : 2; // index of largest element of offdiag
        k1      = (k0+1)%3;
        k2      = (k0+2)%3;
        if (o[k0]==0.0)
        {
            break;  // diagonal already
        }
        thet    = (D[k2][k2]-D[k1][k1])/(2.0*o[k0]);
        sgn     = (thet > 0.0)?1.0:-1.0;
        thet   *= sgn; // make it positive
        t       = sgn /(thet +((thet < 1.E6)?sqrt(thet*thet+1.0):thet)) ; // sign(T)/(|T|+sqrt(T^2+1))
        c       = 1.0/sqrt(t*t+1.0); //  c= 1/(t^2+1) , t=s/c
        if(c==1.0)
        {
            break;  // no room for improvement - reached machine precision.
        }
        jr[0 ]  = jr[1] = jr[2] = jr[3] = 0.0;
        jr[k0]  = sgn*sqrt((1.0-c)/2.0);  // using 1/2 angle identity sin(a/2) = sqrt((1-cos(a))/2)
        jr[k0] *= -1.0; // since our quat-to-matrix convention was for v*M instead of M*v
        jr[3 ]  = sqrt(1.0f - jr[k0] * jr[k0]);
        if(jr[3]==1.0)
        {
            break; // reached limits of floating point precision
        }
        q[0]    = (q[3]*jr[0] + q[0]*jr[3] + q[1]*jr[2] - q[2]*jr[1]);
        q[1]    = (q[3]*jr[1] - q[0]*jr[2] + q[1]*jr[3] + q[2]*jr[0]);
        q[2]    = (q[3]*jr[2] + q[0]*jr[1] - q[1]*jr[0] + q[2]*jr[3]);
        q[3]    = (q[3]*jr[3] - q[0]*jr[0] - q[1]*jr[1] - q[2]*jr[2]);
        mq      = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
        q[0]   /= mq;
        q[1]   /= mq;
        q[2]   /= mq;
        q[3]   /= mq;
    }
}

QMap<float, unsigned int> Shapefs::computeErrorMap(QVector3D &normal, QVector<QVector3D> &lightDirections, QVector<float> &intensities)
{
    QMap<float, unsigned int> errorMap;
    for(int i=0; i< intensities.size(); i++)
    {
        float I = intensities[i];
        float Ivirtual = qMax(QVector3D::dotProduct(normal,lightDirections[i]),0.0f);
        float error = fabs(I-Ivirtual)/255.0;
        errorMap.insertMulti(error,i);
    }
    return errorMap;
}

