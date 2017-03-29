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

    QGroupBox *displayMenu();

    Shader shader;
    Camera camera;
    QMap<QString,Mesh> meshes;

    QVector3D light;

public slots:
    void controlLight();
    void drawTexture();
    void drawTextureLambert();
    void drawNormals();
    void drawColors();
    void drawLambert();
    void drawShadows();
    void drawError();
    void drawWireframe();
    void drawCustom();
    void savePic();

private:
    void printContext();

    bool m_controlLight = false;
    bool m_mouseLook = false;
    QPoint lastPos =QPoint(0,0);
    float lastFov = 90;

    bool m_drawTexture = true;
    bool m_drawTextureLambert = false;
    bool m_drawNormals = false;
    bool m_drawColors = false;
    bool m_drawLambert = true;
    bool m_drawShadows = false;
    bool m_drawError = false;
    bool m_drawWireframe = false;
    bool m_drawCustom = false;
};

#endif // GLWIDGET_H
