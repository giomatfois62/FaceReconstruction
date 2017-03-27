#include "shader.h"


void Shader::compile(QString vert, QString frag)
{
    if(!program.addShaderFromSourceFile(QOpenGLShader::Vertex, vert.toUtf8()))
        qDebug() << "Error in vertex shader:" << program.log();

    if(!program.addShaderFromSourceFile(QOpenGLShader::Fragment, frag.toUtf8()))
       qDebug() << "Error in fragment shader:" << program.log();

    // Link the shaders together into a program
    if ( !program.link() )
        qDebug() << "Error linking shader program:" << program.log();
}
