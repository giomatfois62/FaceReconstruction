#include "fastm.h"

Fastm::Fastm(Mesh &pmesh, QWidget *parent): QWidget(parent)
{
    mesh = &pmesh;
    m_lambda = 2;
    m_initialPoint = 30;
}

void Fastm::execFM()
{
    //ensure starting point index is in mesh (dirty!!!)
    if(mesh->vertices().size()< m_initialPoint+1)
        return;

    // we need mesh vertices and vertices neighbours for integration
    QVector<Vertex> vertices = mesh->vertices();
    QVector<QList<unsigned int>> neighbours = mesh->neighbours();

    // initialize starting points (Alive) and initial narrow band (Close)
    QVector<bool> Alive;
    QMap<long double, unsigned int> Close;
    initialize(vertices, neighbours, Alive, Close);

    // run till no more points are in the narrow band
    while(!Close.isEmpty())
    {
        // freeze value on the trivial point in the narrow band and label it Alive
        unsigned int trivial = Close.first();
        Close.erase(Close.begin());
        Alive[trivial]=true;

        // update each neighbor of the trivial point whose not Alive
        // and label it Close if not yet done
        foreach (unsigned int i, neighbours[trivial])
        {
            if(!Alive[i])
            {
                QList<Vertex> iNeighbours = getNeighbours(vertices,neighbours[i]);
                update(vertices[i], vertices[m_initialPoint], iNeighbours);

                if(!Close.values().contains(i))
                    Close.insertMulti(vertices[i].position.z(), i);
            }
        }

    }

    Vertex v0 = vertices[m_initialPoint];
    for(int i=0; i<vertices.size(); i++)
    {
        Vertex v = vertices[i];
        long double dx = v.position.x() - v0.position.x();
        long double dy = v.position.y() - v0.position.y();
        vertices[i].position.setZ( -(vertices[i].position.z() - m_lambda*(dx*dx+dy*dy)));
    }

    // set new depths in mesh and exit
    mesh->setVertices(vertices);
    mesh->scaleToUnity();

    // notify outside world of the changes to the mesh
    emit meshUpdated();
}

void Fastm::initialize(QVector<Vertex> &vertices, QVector<QList<unsigned int>> &neighbours, QVector<bool> &Alive, QMap<long double, unsigned int> &Close)
{
    // initialize all points depth very high (penalization)
    for(int i=0; i<vertices.size(); i++)
    {
        Alive.push_back(false);
        vertices[i].position.setZ(2);
    }

    // set starting point depth to zero
    vertices[m_initialPoint].position.setZ(0);
    Alive[m_initialPoint] = true;

    // update the values of all initial point's neighbours and label them Alive
    QList<unsigned int> initialFront;
    Vertex v0 = vertices[m_initialPoint];
    foreach (unsigned int j, neighbours[m_initialPoint])
    {
        Vertex v = vertices[j];

        long double h = deltaX(v,v0);
        long double z = v0.position.z() + h*F(v,v0);

        vertices[j].position.setZ(z);
        Alive[j] = true;
        initialFront.push_back(j);
    }

    // init Narrow Band
    foreach (unsigned int j, initialFront)
    {

        QList<unsigned int> jNeighbours = neighbours[j];
        foreach (unsigned int k, jNeighbours)
        {

            if(!Alive[k])
            {
                QList<Vertex> kNeighbours = getNeighbours(vertices,neighbours[k]);
                update(vertices[k],v0,kNeighbours);

                if(!Close.values().contains(j))
                    Close.insertMulti(vertices[k].position.z(), k);
            }
        }
    }

}

// update vertex v usign the semi lagrangian scheme
void Fastm::update(Vertex &v, Vertex &v0, QList<Vertex> &neighbours)
{
    // Interpolation
    /*
    long double min_dist = std::numeric_limits<long double>::max();
    foreach(Vertex vn, neighbours)
    {
        long double dist = deltaX(vn,v);
        if(dist < min_dist)
            min_dist = dist;
    }

    long double min_z = std::numeric_limits<long double>::max();
    foreach(Vertex vn, neighbours)
    {
        long double z = interpolate(v,vn,min_dist);
        if(z < min_z)
            min_z = z;
    }

    long double z = min_z + min_dist*F(v,v0);
    v.position.setZ(z);
    */


    // look for minimum neighbour
    Vertex minNeighbour = getMinNeighbour(neighbours);
    // compute distance from neighbour
    long double h = deltaX(v,minNeighbour);
    // semilagrangian fixed point scheme
    long double z = minNeighbour.position.z() + h*F(v,v0);

    v.position.setZ(z);

}

long double Fastm::interpolate(Vertex v0, Vertex v1, long double dist)
{
    QVector3D direction = v1.position - v0.position;
    //direction.normalize();

    QVector3D otherDir = direction; otherDir.setZ(0);

    direction.normalize();
    otherDir.normalize();
    long double dotp = QVector3D::dotProduct(direction,otherDir);
    long double interp_dist;
    if(dotp > 0.00001)
        interp_dist = dist/dotp;
    else
        interp_dist = dist;


    //double dist2 = deltaX(v0,v1);
    //double ratio = dist/dist2;

    QVector3D interp = v0.position + (interp_dist)*direction;
    //QVector3D interp = v0.position*(1-ratio) + v1.position*(ratio);
    return interp.z();
}

// compute 2D distance between v1 and v2 (done on X and Y coords)
long double Fastm::deltaX(Vertex &v1, Vertex &v2)
{
    long double dx = v2.position.x() - v1.position.x();
    long double dy = v2.position.y() - v1.position.y();

    return sqrt( dx*dx + dy*dy );
}

// compute Source term of the Eikhonal Equation
long double Fastm::F(Vertex &v, Vertex &v0)
{
    long double dx = v.position.x() - v0.position.x();
    long double dy = v.position.y() - v0.position.y();

    long double P = v.normal.x() / v.normal.z();
    long double Q = v.normal.y() / v.normal.z();

    long double Wx = P + 2*m_lambda*dx;
    long double Wy = Q + 2*m_lambda*dy;

    return sqrt( Wx*Wx+Wy*Wy );
}

// get the minimum neighbour in the list for the semi lagrangian scheme
Vertex Fastm::getMinNeighbour(QList<Vertex> &neighbours)
{
    Vertex minNeighbour;
    long double min = std::numeric_limits<long double>::max();

    for(int i=0; i<neighbours.size(); i++)
    {
       if(neighbours.at(i).position.z() < min)
       {
           min = neighbours.at(i).position.z();
           minNeighbour = neighbours.at(i);
       }
    }

    return minNeighbour;
}

QList<Vertex> Fastm::getNeighbours(QVector<Vertex> &vertices, QList<unsigned int> &neighboursInd)
{
    QList<Vertex> neighbours;
    for(int i=0; i<neighboursInd.size(); i++)
        neighbours.push_back(vertices[neighboursInd.at(i)]);

    return neighbours;
}

QGroupBox* Fastm::menu()
{
    QGroupBox *m_menu = new QGroupBox("Fast Marching");

    // exec fast marching pushbutton
    QPushButton *fmBtm = new QPushButton("exec FM");
    connect(fmBtm,SIGNAL(pressed()),this,SLOT(execFM()));

    //double spinbox to set lambda value
    QDoubleSpinBox *lambdaSpin = new QDoubleSpinBox;
    lambdaSpin->setValue(lambda());
    connect(lambdaSpin,SIGNAL(valueChanged(double)),this,SLOT(setLambda(double)));
    QLabel *lambdaLabel = new QLabel;
    lambdaLabel->setText("lambda:");
    QHBoxLayout *form1 = new QHBoxLayout;
    form1->addWidget(lambdaLabel);
    form1->addWidget(lambdaSpin);

    //int spinbox to set starting point value
    QSpinBox *startSpin = new QSpinBox;
    startSpin->setValue(initialPoint());
    connect(startSpin,SIGNAL(valueChanged(int)),this,SLOT(setInitPoint(int)));
    QLabel *initLabel = new QLabel;
    initLabel->setText("start:");
    QHBoxLayout *form2 = new QHBoxLayout;
    form2->addWidget(initLabel);
    form2->addWidget(startSpin);

    // menu layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(fmBtm,0,0);
    layout->addItem(form1,1,0);
    layout->addItem(form2,2,0);
    //QSpacerItem *spacer = new QSpacerItem(1,120,QSizePolicy::Expanding, QSizePolicy::Minimum);
    //layout->addItem(spacer);

    m_menu->setLayout(layout);
    m_menu->setSizePolicy(QSizePolicy( QSizePolicy::Fixed,QSizePolicy::Preferred));
    return m_menu;
}
