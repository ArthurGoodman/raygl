#pragma once

#include <QtWidgets>

#include "renderer.h"
#include "console.h"
#include "consoleparser.h"

class Widget : public QWidget {
    Q_OBJECT

    Renderer *renderer;
    Console *console;
    ConsoleParser *consoleParser;

    bool consoleVisible;

    QPixmap pixmap;

    QPointF lastMousePosition;

    QPointF rotation;
    float scale, param;
    int samples;

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void resizeEvent(QResizeEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    void defaults();
    void toggleConsole();

private slots:
    void resizeConsole();
    void updatePixmap(const QImage &image);

signals:
    void resizeViewport(const QSize &size);
};
