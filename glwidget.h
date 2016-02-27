#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtWidgets>
#include <QtOpenGL>

class QOpenGLShaderProgram;

class GLWidget : public QGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

    GLuint aPos;

    QOpenGLShaderProgram *program;

public:
    GLWidget();

protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
};

#endif // GLWIDGET_H
