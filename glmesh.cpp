#include "glmesh.h"

GLMesh::GLMesh()
{

}

GLMesh::GLMesh(Mesh &mesh)
{
    _vertices=mesh.vertices();
    _indices=mesh.indices();
    setupMesh();
}

GLMesh::GLMesh(const GLMesh& mesh)
{
    _vertices=mesh._vertices;
    _indices=mesh._indices;
    texture=mesh.texture;
    modelMatrix=mesh.modelMatrix;
    setupMesh();
}

GLMesh &GLMesh::operator=(const GLMesh &rhs)
{
    _vertices=rhs._vertices;
    _indices=rhs._indices;
    texture=rhs.texture;
    modelMatrix=rhs.modelMatrix;
    return *this;
}

void GLMesh::draw(Shader &shader)
{
    shader.program.bind();

    VAO.bind();
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    VAO.release();
}

void GLMesh::createTexture(cv::Mat &img)
{
    glDeleteTextures(1,&texture.id);

    glEnable(GL_TEXTURE_2D);
    cv::flip(img, img, 0);
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    // set min-mag filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // fill texture with webcam frame
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.ptr());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLMesh::setupMesh()
{
    initializeOpenGLFunctions();

    EBO=QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

    VAO.create();
    VAO.bind();

    VBO.create();
    VBO.setUsagePattern( QOpenGLBuffer::StaticDraw );
    VBO.bind();
    VBO.allocate( &_vertices[0], _vertices.size()*sizeof(Vertex) );

    EBO.create();
    EBO.setUsagePattern( QOpenGLBuffer::StaticDraw );
    EBO.bind();
    EBO.allocate( &_indices[0], _indices.size()*sizeof(unsigned int) );

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(GLvoid*)0);
    // Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    // Vertex Colors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
    // Vertex Texture Coords
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));

    VAO.release();
}

