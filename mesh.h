#ifndef MESH_H
#define MESH_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QMap>
#include <QDataStream>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>

#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>

#include "shader.h"
#include "qvecoperator.h"
#include "opencv2/opencv.hpp"


struct Vertex
{
    QVector3D position;
    QVector3D normal;
    QVector3D color;
    QVector2D texCoords;

    friend QDataStream &operator<<(QDataStream &out,
                                   const Vertex& _vert);
    friend QDataStream &operator>>(QDataStream &in,
                                   Vertex& _vert);
};

struct Texture
{
    GLuint id;
};

class Mesh: protected QOpenGLFunctions
{
public:
    Mesh();
    ~Mesh();
    Mesh( const Mesh& other );
    Mesh(QVector<Vertex> &vertices,
         QVector<unsigned int> &indices);
    Mesh(const QVector<QVector2D> &landmarks,
         QString indicesFilename);

    // overloaded operator to store meshes in QVector
    Mesh& operator=( const Mesh& rhs );

    // getter
    const QVector<Vertex>& vertices();
    const QVector<unsigned int>& indices();
    const QVector<QList<unsigned int>>& neighbours();
    QVector<QVector3D> positions();
    QVector<QVector2D> texCoords();
    int numOfTriangle();
    int numOfSubdivisions();

    // setter
    void setVertices(QVector<Vertex> &vertices);
    void setIndices(QVector<unsigned int> &indices);
    void setTextureCoords(QVector<QVector2D> &texCoords);

    void load(QString fileName);
    void save(QString fileName);

    // transformations
    void subdivide(int sub);
    void subdivideNonAdaptive(int subdivNumber);
    void scaleToUnity();
    void scaleTextureCoords(int w,int h);
    void applyTransformation();

    float edgeLength(QVector3D v1, QVector3D v2);
    QPair<int,int> maxEdge(unsigned int idx0,unsigned int idx1,unsigned int idx2);
    void bisectTriangleQuad(QPair<int,int> edge1,
                            QPair<int,int> edge2,
                            unsigned int commonVert,
                            QMap<QPair<int,int>, int> &table,
                            QVector<unsigned int> &newTriangles);
    void bisectTriangle(QPair<int,int> edge,
                        unsigned int vertex,
                        QMap<QPair<int,int>, int> &table,
                        QVector<unsigned int> &newTriangles);

    // opengl
    void setup();
    void clean();
    void draw(Shader &shader);
    void createTexture(cv::Mat &img);

    QMatrix4x4 modelMatrix;

private:
    void buildNeighbours();

    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO, EBO;

    QVector<Vertex> m_vertices;
    QVector<unsigned int> m_indices;
    QVector<QList<unsigned int>> m_neighbours;

    Texture texture;
};

#endif // MESH_H
