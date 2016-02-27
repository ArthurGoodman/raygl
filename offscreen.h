#pragma once

#include <QOpenGLFunctions>
#include <QWindow>

class QImage;
class QOpenGLContext;

class OffScreen : public QWindow, protected QOpenGLFunctions {
public:
    explicit OffScreen(QWindow *parent = 0);

    QImage render();

private:
    QOpenGLContext *context_;
};
