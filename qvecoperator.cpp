#include "qvecoperator.h"


void QVecOperator::save(QVector<QVector2D> &vec, QString filename)
{
    filename += ".txt";
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QTextStream stream(&file);
        for(int i=0; i< vec.size(); i++)
            stream << vec[i].x() << " " << vec[i].y() << endl;
    }
    file.close();
}

void QVecOperator::save(QVector<QVector3D> &vec, QString filename)
{
    filename += ".txt";
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QTextStream stream(&file);
        for(int i=0; i< vec.size(); i++)
            stream << vec[i].x() << " " << vec[i].y() << " " << vec[i].z() << endl;
    }
    file.close();

}

void QVecOperator::save(QVector<unsigned int> &vec, QString filename)
{
    filename += ".txt";
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QTextStream stream(&file);
        for(int i=0; i< vec.size(); i++)
            stream << vec[i] << endl;
    }
    file.close();
}


bool QVecOperator::load(QVector<QVector3D> &vec, QString filename)
{
    vec.clear();

    filename += ".txt";
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);    // read the data serialized from the file
        while (!in.atEnd())
        {
            QString line = in.readLine(); //read one line at a time
            QStringList linelst = line.split(" ");

            float x = linelst[0].toFloat();
            float y = linelst[1].toFloat();
            float z = linelst[2].toFloat();
            vec.push_back(QVector3D(x,y,z));
        }
        file.close();
        return true;
    }
    else
        return false;
}

bool QVecOperator::load(QVector<QVector2D> &vec, QString filename)
{
    vec.clear();

    filename += ".txt";
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);    // read the data serialized from the file
        while (!in.atEnd())
        {
            QString line = in.readLine(); //read one line at a time
            QStringList linelst = line.split(" ");

            float x = linelst[0].toFloat();
            float y = linelst[1].toFloat();
            vec.push_back(QVector2D(x,y));
        }
        file.close();
        return true;
    }
    else
        return false;
}

bool QVecOperator::load(QVector<unsigned int> &vec, QString filename)
{
    vec.clear();

    filename += ".txt";
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);    // read the data serialized from the file
        while (!in.atEnd())
        {
            QString line = in.readLine(); //read one line at a time
            QStringList linelst = line.split(" ");

            int x = linelst[0].toInt();
            vec.push_back(x);
        }
        file.close();
        return true;
    }
    else
        return false;
}


bool QVecOperator::load(QList<QString> &vec, QString filename)
{
    vec.clear();

    filename += ".info";
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);    // read the data serialized from the file
        while (!in.atEnd())
        {
            QString line = in.readLine(); //read one line at a time
            QStringList linelst = line.split(" ");

            QString str = linelst[0];
            vec.push_back(str);
        }
        file.close();
        return true;
    }
    else
        return false;
}
