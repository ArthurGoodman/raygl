#include "Widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent) {
    resize(qApp->desktop()->size() / 2);
    setMinimumSize(qApp->desktop()->size() / 4);
    move(qApp->desktop()->rect().center() - rect().center());

    renderer = new Renderer;

    static const double consoleOpacity = 0.8;

    console = new Console(this);
    console->setBaseColor(QColor(40, 40, 40, 255 * consoleOpacity));
    console->setTextColor(QColor(230, 230, 230, 255 * consoleOpacity));
    console->setPrompt("$ ");

    consoleParser = new ConsoleParser(console);

    consoleVisible = false;

    QTimer *timer = new QTimer;
    connect(timer, SIGNAL(timeout()), SLOT(update()));
    connect(this, SIGNAL(destroyed()), timer, SLOT(deleteLater()));
    timer->start(16);
}

Widget::~Widget() {
    delete renderer;
    delete console;
    delete consoleParser;
}

void Widget::resizeEvent(QResizeEvent *) {
    resizeConsole();

    renderer->resize(size());
}

void Widget::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
    case Qt::Key_F11:
        isFullScreen() ? showNormal() : showFullScreen();
        break;

    case Qt::Key_Escape:
        if (consoleVisible)
            toggleConsole();
        else
            isFullScreen() ? showNormal() : (void)close();
        break;

    case Qt::Key_AsciiTilde:
    case Qt::Key_QuoteLeft:
        toggleConsole();
        break;
    }
}

void Widget::mousePressEvent(QMouseEvent *) {
}

void Widget::mouseMoveEvent(QMouseEvent *) {
}

void Widget::mouseReleaseEvent(QMouseEvent *) {
}

void Widget::wheelEvent(QWheelEvent *) {
}

void Widget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.drawImage(0, 0, renderer->render());
}

void Widget::toggleConsole() {
    QPropertyAnimation *a = new QPropertyAnimation(console, "geometry");

    connect(a, SIGNAL(finished()), SLOT(resizeConsole()));

    a->setEasingCurve(QEasingCurve::InOutSine);
    a->setDuration(500);
    a->setStartValue(console->rect());

    if (consoleVisible) {
        a->setEndValue(QRect(0, 0, width(), 0));
        console->clearFocus();
    } else {
        a->setEndValue(QRect(0, 0, width(), height() / 3));
        console->setFocus();
    }

    a->start();

    consoleVisible = !consoleVisible;
}

void Widget::resizeConsole() {
    console->resize(width(), consoleVisible ? height() / 3 : 0);
}
