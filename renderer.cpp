#include "renderer.h"

Renderer::Renderer(QWindow *parent)
    : QWindow(parent), context(0), fbo(0) {
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(QSurfaceFormat());
    create();
}

Renderer::~Renderer() {
    delete context;
    delete program;
    delete buffer;
    delete fbo;
}

void Renderer::setRotation(const QPoint &rotation) {
    this->rotation = rotation;
}

void Renderer::setScale(float scale) {
    this->scale = scale;
}

void Renderer::start() {
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Renderer::render);
    connect(this, SIGNAL(destroyed()), timer, SLOT(deleteLater()));
    timer->start(16);

    time.start();
}

void Renderer::resizeViewport(const QSize &size) {
    newSize = size;
}

void Renderer::render() {
    if (context == 0)
        initialize();

    if (newSize != size) {
        size = newSize;

        context->makeCurrent(this);

        glViewport(0, 0, size.width(), size.height());

        if (fbo)
            delete fbo;

        fbo = new QOpenGLFramebufferObject(size);

        context->doneCurrent();
    }

    if (fbo == 0)
        return;

    context->makeCurrent(this);

    program->bind();
    buffer->bind();

    static const char *vertexBufferName = "position";

    program->enableAttributeArray(vertexBufferName);
    program->setAttributeBuffer(vertexBufferName, GL_FLOAT, 0, 2);

    fbo->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo->texture());

    program->setUniformValue("uBuffer", 0);
    program->setUniformValue("uResolution", size);
    program->setUniformValue("uRotation", rotation);
    program->setUniformValue("uScale", scale);
    program->setUniformValue("uTime", (GLfloat)time.elapsed() / 1000.f);
    program->setUniformValue("uSamples", 1);

    for (int i = 0; i < buffer->size(); i += 12)
        glDrawArrays(GL_TRIANGLES, i, 12);

    emit updatePixmap(fbo->toImage());

    program->disableAttributeArray("vertexBufferName");

    program->release();
    buffer->release();

    fbo->release();

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
    const GLfloat step = 2;

    QVector<GLfloat> vertices;

    for (GLfloat x = -1; x < 1; x += step)
        for (GLfloat y = -1; y < 1; y += step)
            vertices << x << y << x << y + step << x + step << y + step
                     << x << y << x + step << y << x + step << y + step;

    buffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    buffer->create();

    buffer->setUsagePattern(QOpenGLBuffer::StaticDraw);

    buffer->bind();
    buffer->allocate(vertices.data(), vertices.size() * sizeof(GLfloat));
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
