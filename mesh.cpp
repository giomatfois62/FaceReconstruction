#include "mesh.h"
#include <QFile>


Mesh::Mesh()
{

}

Mesh::~Mesh()
{
 //VAO.destroy();
 //VBO.destroy();
 //EBO.destroy();
 //if(!texture.id)
    //glDeleteTextures(1,&texture.id);
}

Mesh::Mesh( const Mesh& other )
{
    m_vertices=other.m_vertices;
    m_indices=other.m_indices;
    //texture=other.texture;
    modelMatrix=other.modelMatrix;
}

Mesh::Mesh(QVector<Vertex> &vertices, QVector<unsigned int> &indices)
{
    m_vertices=vertices;
    m_indices=indices;
}

Mesh::Mesh(const QVector<QVector2D> &landmarks, QString indicesFilename)
{
    QVector<Vertex> vertices;
    for(int i=0; i<landmarks.size(); i++)
    {
        Vertex v;
        v.position = QVector3D(landmarks[i]);
        v.normal = QVector3D(0,0,1);
        v.color = QVector3D(0,0,0);
        v.texCoords = landmarks[i];
        vertices.push_back(v);
    }

    QVector<unsigned int> indices;
    QVecOperator::load(indices,"../simpleIndices_original");

    m_vertices.swap(vertices);
    m_indices.swap(indices);
}

Mesh& Mesh::operator=( const Mesh& rhs )
{
   m_vertices = rhs.m_vertices;
   m_indices = rhs.m_indices;
   //texture=rhs.texture;
   modelMatrix=rhs.modelMatrix;

   return *this;
}

const QVector<Vertex> &Mesh::vertices() { return m_vertices; }

const QVector<unsigned int> &Mesh::indices() { return m_indices; }


QDataStream &operator<<(QDataStream &out, const Vertex& _vert)
{
    out << _vert.position;
    out << _vert.normal;
    out << _vert.color;
    out << _vert.texCoords;
    return out;
}

QDataStream &operator>>(QDataStream &in, Vertex& _vert)
{
    in >> _vert.position;
    in >> _vert.normal;
    in >> _vert.color;
    in >> _vert.texCoords;
    return in;
}

QVector<QVector3D> Mesh::positions()
{
    QVector<QVector3D> verts;
    for(int i=0; i<m_vertices.size(); i++)
        verts.push_back(m_vertices[i].position);
    return verts;
}

const QVector<QList<unsigned int> > &Mesh::neighbours()
{
    if(m_neighbours.size() != m_vertices.size())
        buildNeighbours();

    return m_neighbours;
}

QVector<QVector2D> Mesh::texCoords()
{
    QVector<QVector2D> texcoords;
    for(int i=0; i<m_vertices.size(); i++)
        texcoords.push_back(m_vertices[i].texCoords);

    return texcoords;
}

int Mesh::numOfTriangle()
{
    return m_indices.size()/3;
}

int Mesh::numOfSubdivisions()
{
    int triangs = numOfTriangle();
    int subdiv = 0;
    while(triangs%4 == 0 && triangs != 107)
    {
        triangs = triangs/4;
        subdiv++;
    }
    return subdiv;
}


void Mesh::setVertices(QVector<Vertex> &vertices)
{
    m_vertices=vertices;
}


void Mesh::setTextureCoords(QVector<QVector2D> &texCoords)
{
    for(int i=0; i<texCoords.size(); i++)
        m_vertices[i].texCoords = texCoords[i];
}


void Mesh::save(QString fileName)
{
    QFile file(fileName + ".mesh");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out<< m_vertices;
    out<< m_indices;
    file.close();
}

void Mesh::load(QString fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    m_vertices.clear();
    m_indices.clear();
    in >> m_vertices;
    in >> m_indices;
    file.close();
}

void Mesh::draw(Shader &shader)
{
    shader.program.bind();

    VAO.bind();
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    VAO.release();
}

void Mesh::createTexture(cv::Mat &img)
{
    glDeleteTextures(1,&texture.id);

    glEnable(GL_TEXTURE_2D);
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

void Mesh::clean()
{
    glDeleteTextures(1,&texture.id);
    VAO.destroy();
    EBO.destroy();
    VBO.destroy();
}

void Mesh::setup()
{
    //qDebug() << "i'm setupping a mesh with this context"<<QOpenGLContext::currentContext();
    initializeOpenGLFunctions();

    EBO=QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

    VAO.create();

    VAO.bind();

    VBO.create();
    VBO.setUsagePattern( QOpenGLBuffer::StaticDraw );
    VBO.bind();
    VBO.allocate( &m_vertices[0], m_vertices.size()*sizeof(Vertex) );

    EBO.create();
    EBO.setUsagePattern( QOpenGLBuffer::StaticDraw );
    EBO.bind();
    EBO.allocate( &m_indices[0], m_indices.size()*sizeof(unsigned int) );

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

void Mesh::buildNeighbours()
{
    m_neighbours.clear();

    QVector<QList<unsigned int> > localNeighbours;
    for(int i = 0; i<m_vertices.size(); i++)
        localNeighbours.append(QList<unsigned int>());

    for(int jj=0; jj<m_indices.size()/3; jj++)
    {
        unsigned int i = m_indices[3*jj];
        unsigned int j = m_indices[3*jj+1];
        unsigned int k = m_indices[3*jj+2];

        if( !localNeighbours[i].contains(j) )
            localNeighbours[i].append(j);

        if( !localNeighbours[i].contains(k) )
            localNeighbours[i].append(k);

        if( !localNeighbours[j].contains(i) )
            localNeighbours[j].append(i);

        if( !localNeighbours[j].contains(k) )
            localNeighbours[j].append(k);

        if( !localNeighbours[k].contains(i) )
            localNeighbours[k].append(i);

        if( !localNeighbours[k].contains(j) )
            localNeighbours[k].append(j);

    }

    m_neighbours.swap(localNeighbours);

}

void Mesh::subdivide(int sub)
{
    for(int i=0; i< sub; i++)
    {
        // lookup table to keep track of subdivided edges
        QMap<QPair<int,int>, int> table;

        // new indices list
        QVector<unsigned int> newTriangles;

        float tol = 0.005;

        // visit each triangle and subdivide its longest edge
        for(int i=0; i< m_indices.size()/3;  i++)
        {
            // gather triangle vertices
            unsigned int idx0 = m_indices[3*i];
            unsigned int idx1 = m_indices[3*i+1];
            unsigned int idx2 = m_indices[3*i+2];

            // get edge with maximum length in the triangle
            QPair<int,int> maxE = maxEdge(idx0,idx1,idx2);

            // subdivide it if not yet done
            if(!table.contains(maxE) && edgeLength(m_vertices[maxE.first].position,m_vertices[maxE.second].position)>tol)
            {
                Vertex v;
                v.position = (m_vertices[maxE.first].position + m_vertices[maxE.second].position)/2.0;
                v.texCoords = (m_vertices[maxE.first].texCoords + m_vertices[maxE.second].texCoords)/2.0;
                // register edge subdivision
                table[QPair<int,int>(maxE.first,maxE.second)] = m_vertices.size();
                table[QPair<int,int>(maxE.second,maxE.first)] = m_vertices.size();
                m_vertices.push_back(v);
            }
        }

        // visit all triangles again to create new subtriangles
        for(int i=0; i< m_indices.size()/3;  i++)
        {
            // gather triangle vertices
            unsigned int idx0 = m_indices[3*i];
            unsigned int idx1 = m_indices[3*i+1];
            unsigned int idx2 = m_indices[3*i+2];

            QPair<int,int> e0(idx0,idx1);
            QPair<int,int> e1(idx0,idx2);
            QPair<int,int> e2(idx1,idx2);

            // all edges are subdivided, need to create 4 child triangles
            if(table.contains(e0) && table.contains(e1) && table.contains(e2))
            {
                //qDebug() << "Found a Red Triangle!";
                // add newly formed triangles indices to vector
                int mid01 = table[e0];
                int mid02 = table[e1];
                int mid12 = table[e2];

                newTriangles.push_back(idx0); newTriangles.push_back(mid01); newTriangles.push_back(mid02);
                newTriangles.push_back(mid01); newTriangles.push_back(idx1); newTriangles.push_back(mid12);
                newTriangles.push_back(mid02); newTriangles.push_back(mid12); newTriangles.push_back(idx2);
                newTriangles.push_back(mid01); newTriangles.push_back(mid12); newTriangles.push_back(mid02);
            }
            // the two longest edges are subdivided, need to create a tringle and a quadri, then split the quadri
            else if(table.contains(e0) && table.contains(e1)){bisectTriangleQuad(e0,e1,idx0,table,newTriangles);}
            else if(table.contains(e0) && table.contains(e2)){bisectTriangleQuad(e0,e2,idx1,table,newTriangles);}
            else if(table.contains(e1) && table.contains(e2)){bisectTriangleQuad(e1,e2,idx2,table,newTriangles);}

            // only longest edge is subdivided, need to bisect the triangle
            else if(table.contains(e0)) {bisectTriangle(e0,idx2,table,newTriangles);}
            else if(table.contains(e1)) {bisectTriangle(e1,idx1,table,newTriangles);}
            else if(table.contains(e2)) {bisectTriangle(e2,idx0,table,newTriangles);}

            // dont touch this triangle!
            else
            {
                newTriangles.push_back(idx0); newTriangles.push_back(idx1); newTriangles.push_back(idx2);
            }
        }

        // swap new triangles with older ones
        m_indices.swap(newTriangles);
    }
}

void Mesh::bisectTriangle(QPair<int,int> edge,
                    unsigned int vertex,
                    QMap<QPair<int,int>, int> &table,
                    QVector<unsigned int> &newTriangles)
{
    int midp = table[edge];
    newTriangles.push_back(midp); newTriangles.push_back(edge.first); newTriangles.push_back(vertex);
    newTriangles.push_back(midp); newTriangles.push_back(edge.second); newTriangles.push_back(vertex);
}

void Mesh::bisectTriangleQuad(QPair<int,int> edge1,
                        QPair<int,int> edge2,
                        unsigned int commonVert,
                        QMap<QPair<int,int>, int> &table,
                        QVector<unsigned int> &newTriangles)
{
    int mid1 = table[edge1];
    int mid2 = table[edge2];
    newTriangles.push_back(mid1); newTriangles.push_back(mid2); newTriangles.push_back(commonVert);

    unsigned int opposite_mid1 = (edge2.first == commonVert) ? edge2.second : edge2.first;
    unsigned int opposite_mid2 = (edge1.first == commonVert) ? edge1.second : edge1.first;

    // cut quadri along minimum diagonal
    float l1 = edgeLength(m_vertices[mid1].position, m_vertices[opposite_mid1].position);
    float l2 = edgeLength(m_vertices[mid2].position, m_vertices[opposite_mid2].position);
    if(l1 > l2)
    {
        newTriangles.push_back(mid1); newTriangles.push_back(mid2); newTriangles.push_back(opposite_mid2);
        newTriangles.push_back(opposite_mid1); newTriangles.push_back(mid2); newTriangles.push_back(opposite_mid2);
    }
    else
    {
        newTriangles.push_back(mid1); newTriangles.push_back(mid2); newTriangles.push_back(opposite_mid1);
        newTriangles.push_back(opposite_mid1); newTriangles.push_back(mid1); newTriangles.push_back(opposite_mid2);
    }

}

float Mesh::edgeLength(QVector3D v1, QVector3D v2)
{
    float length = (v1.x() - v2.x())*(v1.x() - v2.x()) + (v1.y() - v2.y())*(v1.y() - v2.y());
    return sqrt(length);
}

QPair<int,int> Mesh::maxEdge(unsigned int idx0,unsigned int idx1,unsigned int idx2)
{
    Vertex v0 = m_vertices[idx0];
    Vertex v1 = m_vertices[idx1];
    Vertex v2 = m_vertices[idx2];

    float l0 = edgeLength(v0.position,v1.position);
    int max0 = idx0;
    int max1 = idx1;
    float max = l0;
    float l1 = edgeLength(v0.position,v2.position);
    if(l1 > max)
    {
        max = l1;
        max0 = idx0;
        max1 = idx2;
    }
    float l2 = edgeLength(v1.position,v2.position);
    if(l2 > max)
    {
        max = l2;
        max0 = idx1;
        max1 = idx2;
    }
    return QPair<int,int>(max0,max1);
}

// simple diadic points subdivision algorithm: divides each triangle into 4 new triangles
// also update the texture coordinates if any is present
void Mesh::subdivideNonAdaptive(int subdivNumber)
{

    for(int i=0; i<subdivNumber; i++)
    {
        // lookup table to keep track of subdivided edges
        QMap<QPair<int,int>, int> table;

        // new indices list
        QVector<unsigned int> newTriangles;

        // visit each triangle and subdivide it
        for(int i=0; i< m_indices.size()/3;  i++)
        {
            // gather triangle vertices
            unsigned int idx0 = m_indices[3*i];
            unsigned int idx1 = m_indices[3*i+1];
            unsigned int idx2 = m_indices[3*i+2];

            // add midpoints on each edge if not already done (i.e. if its not in the table)
            if(!table.contains(QPair<int,int>(idx0,idx1)))
            {
                Vertex v;
                v.position = (m_vertices[idx0].position + m_vertices[idx1].position)/2.0;
                v.texCoords = (m_vertices[idx0].texCoords + m_vertices[idx1].texCoords)/2.0;
                table[QPair<int,int>(idx0,idx1)] = m_vertices.size();
                table[QPair<int,int>(idx1,idx0)] = m_vertices.size();
                m_vertices.push_back(v);
            }
            if(!table.contains(QPair<int,int>(idx0,idx2)))
            {
                Vertex v;
                v.position = (m_vertices[idx0].position + m_vertices[idx2].position)/2.0;
                v.texCoords = (m_vertices[idx0].texCoords + m_vertices[idx2].texCoords)/2.0;
                table[QPair<int,int>(idx0,idx2)] = m_vertices.size();
                table[QPair<int,int>(idx2,idx0)] = m_vertices.size();
                m_vertices.push_back(v);
            }
            if(!table.contains(QPair<int,int>(idx1,idx2)))
            {
                Vertex v;
                v.position = (m_vertices[idx1].position + m_vertices[idx2].position)/2.0;
                v.texCoords = (m_vertices[idx1].texCoords + m_vertices[idx2].texCoords)/2.0;
                table[QPair<int,int>(idx1,idx2)] = m_vertices.size();
                table[QPair<int,int>(idx2,idx1)] = m_vertices.size();
                m_vertices.push_back(v);
            }

            // add newly formed triangles indices to vector
            int mid01 = table[QPair<int,int>(idx0,idx1)];
            int mid02 = table[QPair<int,int>(idx0,idx2)];
            int mid12 = table[QPair<int,int>(idx1,idx2)];

            newTriangles.push_back(idx0); newTriangles.push_back(mid01); newTriangles.push_back(mid02);
            newTriangles.push_back(mid01); newTriangles.push_back(idx1); newTriangles.push_back(mid12);
            newTriangles.push_back(mid02); newTriangles.push_back(mid12); newTriangles.push_back(idx2);
            newTriangles.push_back(mid01); newTriangles.push_back(mid12); newTriangles.push_back(mid02);
        }

        // swap new triangles with older ones
        m_indices.swap(newTriangles);
    }

}



void Mesh::scaleToUnity()
{
    double amin = std::numeric_limits<double>::max();
    double amax = std::numeric_limits<double>::min();
    QVector3D minDimension(amin,amin,amin);
    QVector3D maxDimension(amax,amax,amax);

    for(int i=0; i<m_vertices.size(); i++)
    {
        QVector3D vec = m_vertices[i].position;
        if(vec.x() < minDimension.x())
            minDimension.setX(vec.x());
        if(vec.y() < minDimension.y())
            minDimension.setY(vec.y());
        if(vec.z() < minDimension.z())
            minDimension.setZ(vec.z());
        if(vec.x() > maxDimension.x())
            maxDimension.setX(vec.x());
        if(vec.y() > maxDimension.y())
            maxDimension.setY(vec.y());
        if(vec.z() > maxDimension.z())
            maxDimension.setZ(vec.z());
    }

    // Calculate scale and translation needed to center and fit on screen
    float dist = qMax(maxDimension.x() - minDimension.x(), qMax(maxDimension.y()-minDimension.y(), maxDimension.z() - minDimension.z()));
    float scale = 1.0/dist;
    QVector3D center = (maxDimension - minDimension)/2;
    QVector3D translation = -(maxDimension - center);
    //qDebug() << "translation: "<<translation<<" scale: "<<scale;

    for(int i=0; i<m_vertices.size(); i++)
    {
        m_vertices[i].position += translation;
         m_vertices[i].position *= scale;
    }
    // Apply the scale and translation to a matrix
    /*
    modelMatrix.translate(translation);
    modelMatrix.scale(scale);

    qDebug() << modelMatrix;

    applyTransformation();
    */
}

void Mesh::scaleTextureCoords(int w, int h)
{
    for(int i=0; i<m_vertices.size(); i++)
    {
        m_vertices[i].texCoords.setX(m_vertices[i].texCoords.x()/h);
        m_vertices[i].texCoords.setY(m_vertices[i].texCoords.y()/w);
    }
}

void Mesh::applyTransformation()
{
    for(int i=0; i<m_vertices.size(); i++)
        m_vertices[i].position = modelMatrix*m_vertices[i].position;
    modelMatrix.setToIdentity();
}

