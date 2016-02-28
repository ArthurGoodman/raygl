#pragma once

#include <QtWidgets>
#include <QtOpenGL>

class Renderer : public QWindow, protected QOpenGLFunctions {
    QOpenGLContext *context;
    QOpenGLShaderProgram *program;
    QOpenGLBuffer *buffer;
    GLuint texture;

    QImage image;

public:
    explicit Renderer(QWindow *parent = 0);
    ~Renderer();

    void resize(const QSize &size);
    const QImage &render();

private:
    void initialize();
    void createVertexBuffer();
    void loadShaders();
};
