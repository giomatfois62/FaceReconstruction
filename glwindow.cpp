#include "glwindow.h"
#include <QDebug>
#include <QString>


GLWindow::GLWindow()
{

}

GLWindow::~GLWindow()
{
    makeCurrent();
    cleanupGL();
}


void GLWindow::initializeGL()
{
    initializeOpenGLFunctions();
    printContext();
    glClearColor(0.0f,0.9f,1.0f,10.f);
    shader.compile("../Renderer/shader.vert","../Renderer/shader.frag");
    connect(this,SIGNAL(frameSwapped()),this,SLOT(update()));
}

void GLWindow::resizeGL(int width, int height)
{
    glViewport( 0, 0, width, height );
    float aspect = (float)width/(float)height;
    scene.camera.setAspect(aspect);
}

void GLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    scene.render(shader);
}

void GLWindow::cleanupGL()
{

}


// Private
void GLWindow::printContext()
{
    QString glType;
    QString glVersion;
    QString glProfile;

    // Get Version Information
    glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

    // Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
    switch (format().profile())
    {
    CASE(NoProfile);
    CASE(CoreProfile);
    CASE(CompatibilityProfile);
    }
#undef CASE

    // qPrintable() will print our QString w/o quotes around it.
    qDebug() << glType +" "+ glVersion + "(" + glProfile + ")";
}
