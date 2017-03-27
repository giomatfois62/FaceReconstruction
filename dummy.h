#ifndef DUMMY_H
#define DUMMY_H

#include <QWidget>
#include <QOpenGLContext>
#include "glwidget.h"
#include "scene.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

class Dummy : public QWidget
{
    Q_OBJECT
public:
    explicit Dummy(QWidget *parent = 0);

    QOpenGLContext *m_context;
    Scene *scene;
    QHBoxLayout *layout;
    QVBoxLayout *vlayout;

    void initOpenGLWindow();



signals:

public slots:
        void addMesh();
        void addWidget();
};

#endif // DUMMY_H
