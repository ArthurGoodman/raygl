#include "renderer.h"

Renderer::Renderer(QWindow *parent)
    : QWindow(parent), context(0) {
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(QSurfaceFormat());
    create();

    initialize();
}

Renderer::~Renderer() {
    delete context;
    delete program;
    delete buffer;
}

void Renderer::resize(const QSize &size) {
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

const QImage &Renderer::render() {
    context->makeCurrent(this);

    static const char *vertexBufferName = "position";

    program->bind();
    buffer->bind();
    program->enableAttributeArray(vertexBufferName);
    program->setAttributeBuffer(vertexBufferName, GL_FLOAT, 0, 2);

    program->setUniformValue("resolution", image.size());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    QOpenGLFramebufferObject fbo(image.size());
    fbo.bind();

    glDrawArrays(GL_TRIANGLES, 0, buffer->size());

    image = fbo.toImage();

    program->disableAttributeArray("vertexBufferName");
    program->release();
    buffer->release();

    fbo.release();

    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    context->doneCurrent();

    return image;
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
