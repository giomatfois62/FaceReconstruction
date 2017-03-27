#include "dummy.h"

Dummy::Dummy(QWidget *parent) : QWidget(parent)
{
    m_context = new QOpenGLContext;
    m_context->create();
}

void Dummy::initOpenGLWindow()
{


}

void Dummy::addMesh()
{
    QVector<Vertex> vertices;

    Vertex v1;
    v1.position.setX(1.0); v1.position.setY(1.0); v1.position.setZ(0.0);
    v1.texCoords.setX(1.0); v1.texCoords.setY(1.0);
    Vertex v2;
    v2.position.setX(1.0); v2.position.setY(-1.0); v2.position.setZ(0.0);
    v2.texCoords.setX(1.0); v2.texCoords.setY(0.0);
    Vertex v3;
    v3.position.setX(-1.0); v3.position.setY(-1.0); v3.position.setZ(0.0);
    v3.texCoords.setX(0.0); v3.texCoords.setY(0.0);
    Vertex v4;
    v4.position.setX(-1.0); v4.position.setY(1.0); v4.position.setZ(0.0);
    v4.texCoords.setX(0.0); v4.texCoords.setY(1.0);

    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);

    QVector<unsigned int> indices = {  // Note that we start from 0!
                          0, 1, 3, // First Triangle
                          1, 2, 3  // Second Triangle
                                    };

    Mesh canvas(vertices,indices);
    //Mesh canvas2(canvas);
    scene->addMesh(canvas,"canvas");
}

void Dummy::addWidget()
{}
