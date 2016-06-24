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
    QString mapFileName, mainFileName;

    QSize size, newSize;

    QTime time;
    int frame;

    QPoint rotation, mouse;
    float scale;

    bool reset;

public:
    static Renderer *instance();

    explicit Renderer(QWindow *parent = 0);
    ~Renderer();

    void setRotation(const QPoint &rotation);
    void setMouse(const QPoint &mouse);
    void setScale(float scale);
    void setShader(ShaderProgram *shader);
    void setMap(const QString &mapFileName);
    void setMain(const QString &mainFileName);

    void setFlatMode();
    void setRayMode();
    void setPathMode();

    void invalidate();

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
