#ifndef SHADER_H
#define SHADER_H
#include <QOpenGLShaderProgram>


class Shader
{
public:

    void compile(QString vert, QString frag);

    QOpenGLShaderProgram program;
};

#endif // SHADER_H
