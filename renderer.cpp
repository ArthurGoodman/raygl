#include "renderer.h"

Renderer::Renderer(QWindow *parent)
    : QWindow(parent), context(0) {
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(QSurfaceFormat());
    create();
}

Renderer::~Renderer() {
    delete context;
    delete program;
    delete buffer;
}

void Renderer::start() {
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Renderer::render);
    connect(this, SIGNAL(destroyed()), timer, SLOT(deleteLater()));
    timer->start(0);
}

void Renderer::resizeViewport(const QSize &size) {
    image = QImage(size, QImage::Format_RGB888);

    context->makeCurrent(this);

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    glViewport(0, 0, image.width(), image.height());

    context->doneCurrent();
}

void Renderer::render() {
    if (context == 0)
        initialize();

    if (image.isNull())
        return;

    context->makeCurrent(this);

    static const char *vertexBufferName = "position";

    program->bind();
    buffer->bind();
    program->enableAttributeArray(vertexBufferName);
    program->setAttributeBuffer(vertexBufferName, GL_FLOAT, 0, 2);

    static float time = 0;
    time += 0.05;

    program->setUniformValue("resolution", image.size());
    program->setUniformValue("time", time);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    QOpenGLFramebufferObject fbo(image.size());
    fbo.bind();

    glDrawArrays(GL_TRIANGLES, 0, buffer->size());

    emit updatePixmap(fbo.toImage());

    program->disableAttributeArray("vertexBufferName");
    program->release();
    buffer->release();

    fbo.release();

    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    context->doneCurrent();
}

void Renderer::initialize() {
    context = new QOpenGLContext(this);

    QSurfaceFormat format;
    context->setFormat(format);

    context->create();

    context->makeCurrent(this);

    initializeOpenGLFunctions();

    createVertexBuffer();
    loadShaders();

    context->doneCurrent();
}

void Renderer::createVertexBuffer() {
    static const GLfloat vertices[] = {
        -1.0f, 1.0f,
        3.0f, 1.0f,
        -1.0f, -3.0f,
    };

    buffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    buffer->create();

    buffer->setUsagePattern(QOpenGLBuffer::StaticDraw);

    buffer->bind();
    buffer->allocate(vertices, sizeof(vertices) * sizeof(GLfloat));
    buffer->release();
}

void Renderer::loadShaders() {
    program = new QOpenGLShaderProgram(this);

    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader.vert"))
        qDebug() << program->log();

    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader.frag"))
        qDebug() << program->log();

    if (!program->link())
        qDebug() << program->log();
}
