#include "widget.h"

#include "shaderprogram.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent) {
    resize(qApp->desktop()->size() / 2);
    setMinimumSize(qApp->desktop()->size() / 4);
    move(qApp->desktop()->rect().center() - rect().center());

    renderer = Renderer::instance();
    connect(this, &Widget::resizeViewport, renderer, &Renderer::resizeViewport);
    connect(renderer, &Renderer::updatePixmap, this, &Widget::updatePixmap);

    renderer->setFlatMode();

    defaults();

    static const double consoleOpacity = 0.8;

    console = new Console(this);
    console->setBaseColor(QColor(40, 40, 40, 255 * consoleOpacity));
    console->setTextColor(QColor(230, 230, 230, 255 * consoleOpacity));
    console->setPrompt("$ ");

    consoleParser = new ConsoleParser(console);

    consoleVisible = false;

    setMouseTracking(true);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(update()));
    connect(this, SIGNAL(destroyed()), timer, SLOT(deleteLater()));
    timer->start(16);

    QThread *thread = new QThread;
    connect(thread, SIGNAL(finished()), SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), renderer, SLOT(deleteLater()));
    connect(thread, SIGNAL(started()), renderer, SLOT(start()));
    renderer->moveToThread(thread);
    thread->start();
}

Widget::~Widget() {
    delete console;
    delete consoleParser;
}

void Widget::resizeEvent(QResizeEvent *) {
    resizeConsole();

    emit resizeViewport(size());
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

void Widget::keyReleaseEvent(QKeyEvent *e) {
    switch (e->key()) {
    case Qt::Key_Return:
        if (consoleVisible)
            break;

        const static char *imagesDirName = "images";

        QDir dir;
        if (!dir.exists(imagesDirName))
            dir.mkdir(imagesDirName);

        pixmap.save(QString(imagesDirName) + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh.mm.ss.zzz") + ".png");
        break;
    }
}

void Widget::mousePressEvent(QMouseEvent *e) {
    lastMousePosition = e->pos();
}

void Widget::mouseMoveEvent(QMouseEvent *e) {
    if (e->buttons() & Qt::LeftButton) {
        rotation += e->pos() - lastMousePosition;
        renderer->setRotation(rotation);

        lastMousePosition = e->pos();
    } else if (e->buttons() & Qt::RightButton)
        renderer->setMouse(e->pos() - rotation);
}

void Widget::mouseReleaseEvent(QMouseEvent *) {
}

void Widget::wheelEvent(QWheelEvent *e) {
    if (e->delta() > 1)
        scale *= 1.05f;
    else
        scale /= 1.05f;

    renderer->setScale(scale);
}

void Widget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.drawPixmap(0, 0, pixmap);
}

void Widget::defaults() {
    renderer->setRotation(rotation = QPoint());
    renderer->setScale(scale = 1);
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

void Widget::updatePixmap(const QImage &image) {
    pixmap = QPixmap::fromImage(image);
}
