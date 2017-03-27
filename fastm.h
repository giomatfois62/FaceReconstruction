#ifndef FASTM_H
#define FASTM_H

#include <QWidget>

// menu objects
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>

#include "mesh.h"

class Fastm: public QWidget
{
    Q_OBJECT
public:
    explicit Fastm(Mesh &pmesh, QWidget *parent =0);

    // getter funcs for integration parameters
    long double lambda() {return m_lambda;}
    int initialPoint() {return m_initialPoint;}

    // optional menu to interact with widget
    QGroupBox *menu();


public slots:
    // setter funcs for integration parameters
    void setLambda(double val) {m_lambda=val;}
    void setInitPoint(int val) {m_initialPoint=val;}
    // integrate mesh normals using a fast-marching semi-lagrangian scheme
    void execFM();

signals:
    void meshUpdated();

private:
    Mesh *mesh;

    // functions useful for the semilagrangian scheme
    void initialize(QVector<Vertex> &vertices,
                    QVector<QList<unsigned int> > &neighbours,
                    QVector<bool> &Alive,
                    QMap<long double,unsigned int> &Close);
    void update(Vertex &v, Vertex &v0, QList<Vertex> &neighbours);
    long double deltaX(Vertex &v1, Vertex &v2);
    long double F(Vertex &v, Vertex &v0);
    QList<Vertex> getNeighbours(QVector<Vertex> &vertices,
                                QList<unsigned int> &neighboursInd);
    Vertex getMinNeighbour(QList<Vertex> &neighbours);
    long double interpolate(Vertex v0, Vertex v1, long double dist);

    // penalization parameter (as suggested in ECCV-Kriegman)
    long double m_lambda;

    // "boundary" conditions (index of the vertex with fixed value)
    unsigned int m_initialPoint;
};

#endif // FASTM_H
