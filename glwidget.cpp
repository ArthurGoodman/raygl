#include "glwidget.h"

GLWidget::GLWidget() {
    qsrand(QTime::currentTime().msec());

    resize(qApp->desktop()->size() / 2);
    setMinimumSize(qApp->desktop()->size() / 4);
    move(qApp->desktop()->rect().center() - rect().center());

    QTimer *timer = new QTimer;
    connect(timer, SIGNAL(timeout()), SLOT(update()));
    connect(this, SIGNAL(destroyed()), timer, SLOT(deleteLater()));
    timer->start(16);
}

void GLWidget::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
    case Qt::Key_F11:
        isFullScreen() ? showNormal() : showFullScreen();
        break;
    case Qt::Key_Escape:
        isFullScreen() ? showNormal() : qApp->quit();
        break;
    }
}

void GLWidget::mousePressEvent(QMouseEvent * /*e*/) {
}

void GLWidget::mouseMoveEvent(QMouseEvent * /*e*/) {
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /*e*/) {
}

void GLWidget::wheelEvent(QWheelEvent * /*e*/) {
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();

    program = new QOpenGLShaderProgram(this);

    program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader.frag");

    program->link();

    aPos = program->attributeLocation("aPos");
}

void GLWidget::resizeGL(int width, int height) {
    qreal aspect = devicePixelRatio();
    glViewport(0, 0, width * aspect, height * aspect);
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    program->bind();

    static GLfloat pos[] = {-1.f, 1.f,
                            3.f, 1.f,
                            -1.f, -3.f};

    glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, 0, pos);

    glEnableVertexAttribArray(aPos);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    glDisableVertexAttribArray(aPos);

    program->release();
}
