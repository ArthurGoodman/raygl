#pragma once

#include <QtWidgets>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_Core>

class ShaderProgram;

class Renderer : public QWindow, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLContext *context;
    QOpenGLBuffer *vertexBuffer;
    QOpenGLFramebufferObject *backBuffer, *frameBuffer;

    ShaderProgram *shader;

    QSize size, newSize;

    QTime time;
    int frame;

    QPoint rotation;
    float scale;

    bool reset;

public:
    static Renderer *instance();

    explicit Renderer(QWindow *parent = 0);
    ~Renderer();

    void setRotation(const QPoint &rotation);
    void setScale(float scale);
    void setShader(ShaderProgram *shader);

public slots:
    void start();
    void resizeViewport(const QSize &size);

private slots:
    void render();

signals:
    void updatePixmap(const QImage &image);

private:
    void draw(QOpenGLShaderProgram *program, QOpenGLFramebufferObject *buffer, GLfloat elapsed);
    void initialize();
    void createVertexBuffer();
};
