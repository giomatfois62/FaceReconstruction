#ifndef FACEDATABASEWIDGET_H
#define FACEDATABASEWIDGET_H

#include <QWidget>

#include <QGroupBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>

#include "qvecoperator.h"

class FaceDatabaseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FaceDatabaseWidget(QWidget *parent = 0);

public:
    QString name() { return m_name; }
    QList<QString> imageList() { return m_imageList; }
    QVector<QVector3D> lightDirections() { return m_lightDirections; }

    QGroupBox* menu();

signals:
    void imageChanged(QString imageName);

public slots:
    void loadPrevious();
    void loadNext();
    void loadDatabase(int dbNumber);
    void loadArtificial(int dbNumber);

private:
    QString m_name;
    QList<QString> m_imageList;
    QVector<QVector3D> m_lightDirections;
    int m_currentFrame;
};

#endif // FACEDATABASEWIDGET_H
