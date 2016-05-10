#include "shaderprogram.h"

ShaderProgram::ShaderProgram(const QString &mainFileName, const QString &postFileName)
    : mainFileName(mainFileName), postFileName(postFileName), mainProgram(0), postProgram(0), compiled(false) {
}

ShaderProgram::~ShaderProgram() {
    delete mainProgram;
    delete postProgram;
}

bool ShaderProgram::isCompiled() {
    return compiled;
}

bool ShaderProgram::usePostProcessing() {
    return postProgram != 0;
}

void ShaderProgram::compile() {
    loadShader(mainProgram = new QOpenGLShaderProgram, mainFileName);

    if (!postFileName.isEmpty())
        loadShader(postProgram = new QOpenGLShaderProgram, postFileName);

    compiled = true;
}

QOpenGLShaderProgram *ShaderProgram::getMainProgram() {
    return mainProgram;
}

QOpenGLShaderProgram *ShaderProgram::getPostProgram() {
    return postProgram;
}

void ShaderProgram::loadShader(QOpenGLShaderProgram *program, const QString &name) {
    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader.vert"))
        qDebug() << program->log();

    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(":/%0.frag").arg(name)))
        qDebug() << program->log();

    if (!program->link())
        qDebug() << program->log();
}
