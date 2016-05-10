#include "renderer.h"

#include "shaderprogram.h"

Renderer *Renderer::instance() {
    static Renderer *renderer = new Renderer;
    return renderer;
}

Renderer::Renderer(QWindow *parent)
    : QWindow(parent), context(0), frameBuffer(0), shader(0), reset(false) {
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(QSurfaceFormat());
    create();

    mapFileName = "mandelbulb";
    mainFileName = "blank";
}

Renderer::~Renderer() {
    delete context;
    delete vertexBuffer;
    delete backBuffer;
    delete frameBuffer;
}

void Renderer::setRotation(const QPoint &rotation) {
    this->rotation = rotation;
    reset = true;
}

void Renderer::setScale(float scale) {
    this->scale = scale;
    reset = true;
}

void Renderer::setShader(ShaderProgram *shader) {
    if (this->shader)
        delete this->shader;

    this->shader = shader;

    reset = true;
}

void Renderer::setMap(const QString &mapFileName) {
    shader->setMapFileName(this->mapFileName = mapFileName);
    reset = true;
}

void Renderer::setMain(const QString &mainFileName) {
    shader->setMainFileName(this->mainFileName = mainFileName);
    reset = true;
}

void Renderer::setFlatMode() {
    setShader(new ShaderProgram("pre", "", mainFileName, ""));
}

void Renderer::setRayMode() {
    setShader(new ShaderProgram("pre", mapFileName, "rayMain", ""));
}

void Renderer::setPathMode() {
    setShader(new ShaderProgram("pre", mapFileName, "pathMain", "post"));
}

void Renderer::invalidate() {
    reset = true;
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

    if (newSize != size || reset) {
        frame = 0;
        reset = false;

        size = newSize;

        context->makeCurrent(this);

        glViewport(0, 0, size.width(), size.height());

        QOpenGLFramebufferObjectFormat fboFormat;
        fboFormat.setInternalTextureFormat(GL_RGB32F);

        if (backBuffer)
            delete backBuffer;

        backBuffer = new QOpenGLFramebufferObject(size, fboFormat);

        if (frameBuffer)
            delete frameBuffer;

        frameBuffer = new QOpenGLFramebufferObject(size, fboFormat);

        context->doneCurrent();
    }

    if (backBuffer == 0 || frameBuffer == 0 || shader == 0)
        return;

    context->makeCurrent(this);

    if (!shader->isCompiled())
        shader->compile();

    GLfloat elapsed = (GLfloat)time.elapsed() / 1000.f;

    draw(shader->getMainProgram(), backBuffer, elapsed);

    if (shader->usePostProcessing())
        draw(shader->getPostProgram(), frameBuffer, elapsed);

    emit updatePixmap((shader->usePostProcessing() ? frameBuffer : backBuffer)->toImage());

    frame++;

    context->doneCurrent();
}

void Renderer::draw(QOpenGLShaderProgram *program, QOpenGLFramebufferObject *buffer, GLfloat elapsed) {
    static const char *vertexBufferName = "position";

    program->bind();
    vertexBuffer->bind();

    program->enableAttributeArray(vertexBufferName);
    program->setAttributeBuffer(vertexBufferName, GL_FLOAT, 0, 2);

    buffer->bind();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, backBuffer->texture());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffer->texture());

    program->setUniformValue("uBuffer", 1);
    program->setUniformValue("uResolution", size);
    program->setUniformValue("uRotation", rotation);
    program->setUniformValue("uScale", scale);
    program->setUniformValue("uTime", elapsed);
    program->setUniformValue("uFrame", frame);
    program->setUniformValue("uSamples", 1);

    for (int i = 0; i < vertexBuffer->size(); i += 12)
        glDrawArrays(GL_TRIANGLES, i, 12);

    program->disableAttributeArray(vertexBufferName);

    program->release();
    vertexBuffer->release();

    buffer->release();

    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::initialize() {
    context = new QOpenGLContext(this);

    QSurfaceFormat format;
    context->setFormat(format);

    context->create();

    context->makeCurrent(this);

    initializeOpenGLFunctions();

    createVertexBuffer();

    context->doneCurrent();
}

void Renderer::createVertexBuffer() {
    const GLfloat step = 2;

    QVector<GLfloat> vertices;

    for (GLfloat x = -1; x < 1; x += step)
        for (GLfloat y = -1; y < 1; y += step)
            vertices << x << y << x << y + step << x + step << y + step
                     << x << y << x + step << y << x + step << y + step;

    vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vertexBuffer->create();

    vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);

    vertexBuffer->bind();
    vertexBuffer->allocate(vertices.data(), vertices.size() * sizeof(GLfloat));
    vertexBuffer->release();
}
