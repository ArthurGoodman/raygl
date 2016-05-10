#pragma once

#include <QtOpenGL>
#include <QObject>

class ShaderProgram : public QObject {
    Q_OBJECT

    QString preFileName, mapFileName, mainFileName, postFileName;
    QOpenGLShaderProgram *mainProgram, *postProgram;
    bool compiled;

public:
    explicit ShaderProgram(const QString &preFileName, const QString &mapFileName, const QString &mainFileName, const QString &postFileName);
    ~ShaderProgram();

    bool isCompiled();
    bool usePostProcessing();

    void compile();

    QOpenGLShaderProgram *getMainProgram();
    QOpenGLShaderProgram *getPostProgram();

    void setMapFileName(const QString &mapFileName);
    void setMainFileName(const QString &mainFileName);

private:
    void loadShaderFromFile(QOpenGLShaderProgram *program, const QString &name);
    void loadShaderFromCode(QOpenGLShaderProgram *program, const QString &code);

private:
    QString readFile(const QString &fileName);
};
