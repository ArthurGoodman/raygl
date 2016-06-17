#include "shaderprogram.h"

ShaderProgram::ShaderProgram(const QString &preFileName, const QString &mapFileName, const QString &mainFileName, const QString &postFileName)
    : preFileName(preFileName), mapFileName(mapFileName), mainFileName(mainFileName), postFileName(postFileName), mainProgram(0), postProgram(0), compiled(false) {
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
    delete mainProgram;
    delete postProgram;

    QString mainCode;

    if (!preFileName.isEmpty())
        mainCode += readFile(QString("%0.frag").arg(preFileName));

    if (!mapFileName.isEmpty())
        mainCode += readFile(QString("%0.frag").arg(mapFileName));

    mainCode += readFile(QString("%0.frag").arg(mainFileName));

    loadShaderFromCode(mainProgram = new QOpenGLShaderProgram, mainCode);

    if (!postFileName.isEmpty())
        loadShaderFromFile(postProgram = new QOpenGLShaderProgram, postFileName);

    compiled = true;

    QFile file("dump.frag");
    file.open(QFile::WriteOnly | QFile::Text);

    QTextStream out(&file);

    out << mainCode;
}

QOpenGLShaderProgram *ShaderProgram::getMainProgram() {
    return mainProgram;
}

QOpenGLShaderProgram *ShaderProgram::getPostProgram() {
    return postProgram;
}

void ShaderProgram::setMapFileName(const QString &mapFileName) {
    this->mapFileName = mapFileName;
    compiled = false;
}

void ShaderProgram::setMainFileName(const QString &mainFileName) {
    this->mainFileName = mainFileName;
    compiled = false;
}

void ShaderProgram::loadShaderFromFile(QOpenGLShaderProgram *program, const QString &name) {
    loadShaderFromCode(program, readFile(QString("%0.frag").arg(name)));
}

void ShaderProgram::loadShaderFromCode(QOpenGLShaderProgram *program, const QString &code) {
    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, "shader.vert"))
        qDebug() << program->log();

    if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment, code))
        qDebug() << program->log();

    if (!program->link())
        qDebug() << program->log();
}

QString ShaderProgram::readFile(const QString &fileName) {
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly | QFile::Text))
        return "";

    QTextStream stream(&file);
    return stream.readAll();
}
