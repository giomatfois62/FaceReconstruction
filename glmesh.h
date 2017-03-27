#ifndef GLMESH_H
#define GLMESH_H

#include "mesh.h"
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>

#include "shader.h"
#include "opencv2/opencv.hpp"


class GLMesh: public QOpenGLFunctions
{
public:
    GLMesh();
    GLMesh(Mesh& mesh);
    GLMesh(const GLMesh& mesh);
    GLMesh& operator=( const GLMesh& rhs );

    const QVector<Vertex>& vertices() { return _vertices; }
    const QVector<unsigned int>& indices() { return _indices; }

    void draw(Shader &shader);
    void createTexture(cv::Mat &img);

    QMatrix4x4 modelMatrix;


private:
    void setupMesh();

    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO, EBO;

    QVector<Vertex> _vertices;
    QVector<unsigned int> _indices;

    Texture texture;

};

#endif // GLMESH_H
