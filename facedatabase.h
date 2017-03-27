#ifndef FACEDATABASE_H
#define FACEDATABASE_H

#include <opencv2/opencv.hpp>

#include <QList>
#include <QVector>
#include <QVector3D>
#include "qvecoperator.h"

class FaceDatabase
{
public:
    FaceDatabase();
    ~FaceDatabase() {}

    bool load(QString database);

    QList<QString> imageList;
    QVector<QVector3D> lightDirections;
    QString name;

    int currentFrame = 0;
};

#endif // FACEDATABASE_H
