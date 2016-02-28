#pragma once

#include <QtWidgets>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_Core>

class Renderer : public QWindow, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLContext *context;
    QOpenGLShaderProgram *program;
    QOpenGLBuffer *buffer;
    GLuint texture;

    QImage image;

public:
    explicit Renderer(QWindow *parent = 0);
    ~Renderer();

public slots:
    void start();
    void resizeViewport(const QSize &size);

private slots:
    void render();

signals:
    void updatePixmap(const QImage &image);

private:
    void initialize();
    void createVertexBuffer();
    void loadShaders();
};
