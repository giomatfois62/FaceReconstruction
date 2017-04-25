#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "mesh.h"
#include "shader.h"
#include "camera.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GLWidget(QWidget *parent=0);

    void initializeGL();
    void resizeGL(int w,int h);
    void paintGL();

    void keyPressEvent(QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *m);
    void wheelEvent(QWheelEvent *w);

    void addMesh(QString meshName, Mesh mesh);
    void renderScene();

    QString activeUniform() {return m_activeUniform;}
    QList<QString> uniforms() {return m_uniforms;}


    QGroupBox *menu();

    Shader shader;
    Camera camera;
    QMap<QString,Mesh> meshes;

public slots:
    void setActiveUniform(QString uniform);
    void setUniforms();
    void controlLight();
    void drawWireframe();
    void savePic();

private:
    void printContext();

    QString m_activeUniform;
    QList<QString> m_uniforms;

    bool m_drawWireframe = false;
    bool m_controlLight = false;
    bool m_mouseLook = false;
    QPoint m_lastPos = QPoint(0,0);
    float m_lastFov = 90;
    QVector3D m_light;
};

#endif // GLWIDGET_H
