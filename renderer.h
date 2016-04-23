#pragma once

#include <QtWidgets>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_Core>

class Renderer : public QWindow, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLContext *context;
    QOpenGLShaderProgram *mainProgram, *postProgram;
    QOpenGLBuffer *vertexBuffer;
    QOpenGLFramebufferObject *backBuffer, *frameBuffer;

    QSize size, newSize;

    QTime time;
    int frame;

    QPoint rotation;
    float scale;

public:
    explicit Renderer(QWindow *parent = 0);
    ~Renderer();

    void setRotation(const QPoint &rotation);
    void setScale(float scale);

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
    void loadShader(QOpenGLShaderProgram *program, const QString &name);
};
