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
    int frame, samples;

    QPointF rotation, mouse;
    float scale;

    bool reset, flat;

public:
    static Renderer *instance();

    explicit Renderer(QWindow *parent = 0);
    ~Renderer();

    void setSamples(int samples);
    void setRotation(const QPointF &rotation);
    void setMouse(const QPointF &mouse);
    void setScale(float scale);
    void setShader(ShaderProgram *shader);
    void setMap(const QString &mapFileName);
    void setMain(const QString &mainFileName);

    void setFlatMode();
    void setRayMode();
    void setPathMode();

    bool isFlat();

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
