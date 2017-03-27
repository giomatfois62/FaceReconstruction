#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "mesh.h"
#include "glmesh.h"
#include "shader.h"
#include "scene.h"
#include "camera.h"
#include "recorder.h"

class GLWindow : public QOpenGLWindow,
                 protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWindow();
    ~GLWindow();

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void cleanupGL();

    Shader shader;
    Scene scene;

private:
    void printContext();
    bool m_controlLight;
};

#endif // GLWINDOW_H
