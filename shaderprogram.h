#pragma once

#include <QtOpenGL>
#include <QObject>

class ShaderProgram : public QObject {
    Q_OBJECT

    QString mainFileName, postFileName;
    QOpenGLShaderProgram *mainProgram, *postProgram;
    bool compiled;

public:
    explicit ShaderProgram(const QString &mainFileName, const QString &postFileName);
    ~ShaderProgram();

    bool isCompiled();
    bool usePostProcessing();

    void compile();

    QOpenGLShaderProgram *getMainProgram();
    QOpenGLShaderProgram *getPostProgram();

private:
    void loadShader(QOpenGLShaderProgram *program, const QString &name);
};
