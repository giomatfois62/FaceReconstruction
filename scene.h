#ifndef SCENE_H
#define SCENE_H

#include <QMap>
#include <QHash>
#include <QOpenGLFunctions>

#include "mesh.h"
#include "glmesh.h"
#include "shader.h"
#include "camera.h"

class Scene: public QOpenGLFunctions
{
public:
    Scene();

    void render(Shader &shader);

    void addMesh(Mesh &mesh, const QString& meshName = 0);
    void buildMesh(QVector<Vertex> &v, QVector<unsigned int> &i);
    void deleteMesh(const QString& meshName);
    void clear();

    // set mesh named "meshName" as active if it's stored in the scene
    void setActiveMesh(QString &meshName);

    // returns a reference to the currently active mesh (to interact)
    Mesh& activeMesh();
    Mesh& mesh(QString meshName);

    // returns a list with all stored meshes
    const QList<QString> meshList();

    Camera camera;
private:
    QMap<QString,Mesh> meshes;


};

#endif // SCENE_H
