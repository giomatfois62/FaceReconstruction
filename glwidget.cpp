#include "glwidget.h"
#include <QFileDialog>
#include <QComboBox>
//#include <QImage>

GLWidget::GLWidget(QWidget *parent): QOpenGLWidget(parent)
{
    this->setMouseTracking(true);
    m_activeUniform = "lambert";
    m_uniforms << "albedo" << "normals" << "texture" << "texture_lambert"
               << "p_error" << "shadows" << "red";
}

void GLWidget::setActiveUniform(QString uniform)
{
    //qDebug() << "swapping active uniform "<<m_activeUniform<<" with "<<uniform;
    if(m_activeUniform == uniform)
        return;

    m_uniforms.push_back(m_activeUniform);
    m_activeUniform = uniform;
    m_uniforms.removeOne(uniform);

    setUniforms();
}

void GLWidget::setUniforms()
{
    shader.program.setUniformValue("projectionMatrix",camera.projection());
    shader.program.setUniformValue("viewMatrix",camera.view());
    shader.program.setUniformValue("lightDirection",m_light);

    shader.program.setUniformValue(m_activeUniform.toStdString().c_str(),true);
    foreach (QString u, m_uniforms) {
       shader.program.setUniformValue(u.toStdString().c_str(),false);
    }
}


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.9f,0.9f,1.0f,10.f);
    shader.compile("../FaceReconstruction/shader.vert","../FaceReconstruction/shader.frag");

    m_light = QVector3D(0,0,1);

    connect(this,SIGNAL(frameSwapped()),this,SLOT(update()));
    printContext();
}

void GLWidget::resizeGL(int w, int h)
{
    //glViewport( 0, 0, w, h );
    float aspect = w/(float)h;
    camera.setAspect(aspect);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderScene();
}


void GLWidget::renderScene()
{
    shader.program.bind();
    setUniforms();

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    // draw scene
    QMap<QString,Mesh>::iterator it=meshes.begin();
    while(it!=meshes.end())
    {
        QMatrix4x4 modelViewMatrix = camera.view() * it.value().modelMatrix;
        QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
        QMatrix4x4 MVP = camera.projection() * modelViewMatrix;

        shader.program.setUniformValue("N",normalMatrix);
        shader.program.setUniformValue("MVP",MVP);

        it.value().draw(shader);
        it++;
    }

    // Draw blue outline
    if(m_drawWireframe)
    {
        glPolygonOffset(-1.0f, -1.0f);      // Shift depth values
        glEnable(GL_POLYGON_OFFSET_LINE);

        // Draw lines antialiased
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw black wireframe version of geometry
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(12.5f);
        // draw wireframe on top of mesh
        //drawShadows();

        QString currentUniform = m_activeUniform;
        setActiveUniform(QString("shadows"));

        it = meshes.begin();
        while(it!=meshes.end())
        {
            it.value().draw(shader);
            it++;
        }
        //glDisable( GL_POLYGON_OFFSET_LINE );
        //drawLambert();
        //glDepthMask(true);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_POLYGON_OFFSET_LINE);
        glLineWidth(1.0f);
        glDisable(GL_BLEND);
        glDisable(GL_LINE_SMOOTH);

        // set back previous active uniform
        setActiveUniform(currentUniform);
    }
}


void GLWidget::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_L)
        m_controlLight = !m_controlLight;
    // rotation
    if(e->key() == Qt::Key_Right )
        camera.view().rotate(5, QVector3D(0,1,0));
    if(e->key() == Qt::Key_Left )
        camera.view().rotate(-5, QVector3D(0,1,0));
    if(e->key() == Qt::Key_Up )
        camera.view().rotate(5, QVector3D(1,0,0));
    if(e->key() == Qt::Key_Down )
        camera.view().rotate(-5, QVector3D(1,0,0));

}

void GLWidget::mouseMoveEvent(QMouseEvent *m)
{
    float width = this->width();
    float height = this->height();
    if(m->buttons() && m_controlLight)
    {
        QPointF pos = QPointF( m->pos().x() /width*2 -1, 1- m->pos().y()/height*2  );
        m_light = QVector3D (pos.x(),pos.y(),sqrt( qMax(1-pos.x()*pos.x()-pos.y()*pos.y(),0.0) ) );
    }

    float dx = (m->x() )/width*2-1;
    float dy = 1-(m->y() )/height*2;

    if (m->buttons() & Qt::LeftButton && !m_controlLight)
    {
        camera = Camera();
        camera.setZoom(m_lastFov);
        camera.view().translate(dx,dy,0);
    }
    else if (m->buttons() & Qt::RightButton && !m_controlLight)
    {
        if(!m_mouseLook)
        {
            m_lastPos = m->pos();
            m_mouseLook = true;
        }
        dx = (m->x() -m_lastPos.x());
        dy = (m->y() -m_lastPos.y());
        camera.view().rotate(dx/10.0,QVector3D(0,1,0));
        camera.view().rotate(dy/10.0,QVector3D(1,0,0));
        //camera.setZoom(lastFov);
        m_lastPos = m->pos();
    }
    else
    {
        m_mouseLook = false;
    }

}

void GLWidget::wheelEvent(QWheelEvent *w)
{
    float step = 2;
    float fov;
    if(w->delta() < 0)
        fov = qMin( m_lastFov + step, (float)120.0);
    else
        fov = qMax( m_lastFov - step, (float)10.0);
    camera.setZoom(fov);
    m_lastFov = fov;
}

void GLWidget::addMesh(QString meshName, Mesh mesh)
{
    makeCurrent();
    //Mesh newMesh=mesh;
    QMap<QString,Mesh>::iterator it=meshes.begin();
    while(it!=meshes.end())
    {
        it.value().clean();
        it++;
    }
    meshes.insert(meshName,mesh);
    meshes[meshName].setup();
}

// Private
void GLWidget::printContext()
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

void GLWidget::controlLight()
{
    m_controlLight = !m_controlLight;
}

void GLWidget::drawWireframe()
{
    m_drawWireframe = !m_drawWireframe;
}

void GLWidget::savePic()
{
    QString file = QFileDialog::getSaveFileName(this, "Save as...", "name", "PNG (*.png);; BMP (*.bmp);;TIFF (*.tiff *.tif);; JPEG (*.jpg *.jpeg)");
    //this->grabFramebuffer().save(file);

    QImage img = this->grabFramebuffer();
    qDebug() << "image grabbed! size "<<img.size();
    bool success = img.save(file);
    if(success)
        qDebug() << "image saved!";
    else
        qDebug() << "image NOT saved :(";
    //QPainter painter(&img);
    //this->render(&painter);
    //img.save("/home/mat/file.jpg");
}


QGroupBox* GLWidget::menu()
{
   QGroupBox *m_menu = new QGroupBox("Display Options");

   QPushButton *wireBtm = new QPushButton("wireframe");
   connect(wireBtm,SIGNAL(pressed()),this,SLOT(drawWireframe()));

   QComboBox *comboBox = new QComboBox;
   connect(comboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(setActiveUniform(QString)));
   comboBox->addItem(tr(m_activeUniform.toStdString().c_str()));
   foreach (QString u, m_uniforms) {
      comboBox->addItem(tr(u.toStdString().c_str()));
   }

   QPushButton *lightBtm = new QPushButton("moveLight");
   connect(lightBtm,SIGNAL(pressed()),this,SLOT(controlLight()));

   QPushButton *savePicBtm = new QPushButton("savePic");
   connect(savePicBtm,SIGNAL(pressed()),this,SLOT(savePic()));

   QGridLayout *menuLayout = new QGridLayout;
   menuLayout->addWidget(wireBtm,0,0);
   menuLayout->addWidget(lightBtm,0,1);
   menuLayout->addWidget(comboBox,1,0);
   menuLayout->addWidget(savePicBtm,2,0);

   m_menu->setLayout(menuLayout);
   m_menu->setSizePolicy(QSizePolicy( QSizePolicy::Fixed,QSizePolicy::Fixed));
   return m_menu;
}

