#ifndef QVECOPERATOR_H
#define QVECOPERATOR_H

#include<QVector>
#include<QVector2D>
#include<QVector3D>

#include<QFile>
#include <QTextStream>
#include <QDataStream>
#include <QDebug>

class QVecOperator
{

public:
    static void save(QVector<QVector2D> &vec, QString filename);
    static void save(QVector<QVector3D> &vec, QString filename);
    static void save(QVector<unsigned int> &vec, QString filename);

    static bool load(QVector<QVector3D> &vec, QString filename);
    static bool load(QVector<QVector2D> &vec, QString filename);
    static bool load(QVector<unsigned int> &vec, QString filename);
    static bool load(QList<QString> &vec, QString filename);
};

#endif // QVECOPERATOR_H
