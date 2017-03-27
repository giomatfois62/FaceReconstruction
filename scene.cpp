#include "scene.h"

Scene::Scene()
{
    //initializeOpenGLFunctions();

    //shader.compile("../Renderer/shader.vert","../Renderer/shader.frag");
}

void Scene::render(Shader &shader)
{
    shader.program.bind();
    shader.program.setUniformValue("projectionMatrix",camera.projection());
    shader.program.setUniformValue("viewMatrix",camera.view());


    //foreach(Mesh mesh, meshes)
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

}

void Scene::addMesh(Mesh &mesh, const QString &meshName)
{
    meshes.insert(meshName,mesh);
    //meshes[meshName].setup();
}

void Scene::buildMesh(QVector<Vertex> &v, QVector<unsigned int> &i)
{
    Mesh mesh(v,i);
    meshes.insert("",mesh);
    //meshes[""].setup();
}

void Scene::deleteMesh(const QString &meshName)
{
    meshes.remove(meshName);
}

void Scene::clear()
{
    meshes.clear();
}

Mesh& Scene::mesh(QString meshName)
{
    QMap<QString,Mesh>::iterator i=meshes.find(meshName);
    return i.value();
}

Mesh& Scene::activeMesh()
{
    return meshes.first();
}

const QList<QString> Scene::meshList()
{
    return meshes.keys();
}
