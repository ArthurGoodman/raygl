#include "offscreen.h"

#include <QImage>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QString>
#include <QWidget>

#include <QDebug>

OffScreen::OffScreen(QWindow *parent)
    : QWindow(parent),
      context_(nullptr) {
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(QSurfaceFormat());
    create();
}

QImage OffScreen::render() {
    //create the context
    if (!context_) {
        context_ = new QOpenGLContext(this);
        QSurfaceFormat format;
        context_->setFormat(format);

        if (!context_->create())
            qFatal("Cannot create the requested OpenGL context!");
    }

    context_->makeCurrent(this);
    initializeOpenGLFunctions();

    //load image and create fbo
    //    QString const prefix("/Users/Qt/program/experiment_apps_and_libs/openGLTest/simpleGPGPU/");
    //    QImage img(prefix + "images/emili.jpg");
    int imgWidth = 1920 / 2, imgHeight = 1080 / 2;
    QImage img = QImage(imgWidth, imgHeight, QImage::Format_RGB888);
    if (img.isNull()) {
        qFatal("image is null");
    }
    QOpenGLFramebufferObject fbo(img.size());
    qDebug() << "bind success? : " << fbo.bind();

    //if(glCheckFramebufferStatus(fbo.handle()) != GL_FRAMEBUFFER_COMPLETE){
    //    qDebug()<<"frame buffer error";
    //}
    qDebug() << "has opengl fbo : " << QOpenGLFramebufferObject::hasOpenGLFramebufferObjects();

    //use two triangles two cover whole screen
    std::vector<GLfloat> const vertex{
        -1.0f, 1.0f,
        3.0f, 1.0f,
        -1.0f, -3.0f,
    };

    //initialize vbo
    QOpenGLBuffer buffer(QOpenGLBuffer::VertexBuffer);
    buffer.create();
    buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    buffer.bind();
    buffer.allocate(&vertex[0], vertex.size() * sizeof(GLfloat));
    buffer.release();

    //create texture
    GLuint rendered_texture;
    glGenTextures(1, &rendered_texture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, rendered_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width(), img.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, img.scanLine(0));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    //compile and link program
    QOpenGLShaderProgram program;
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader.vert")) {
        qDebug() << "QOpenGLShader::Vertex error : " + program.log();
    }

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader.frag")) {
        qDebug() << "QOpenGLShader::Fragment error : " + program.log();
    }

    // Link shader pipeline
    if (!program.link()) {
        qDebug() << "link error : " + program.log();
    }

    //render the texture as usual
    //render the texture as usual
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, img.width(), img.height());

    program.bind();
    buffer.bind();
    program.enableAttributeArray("position");
    program.setAttributeBuffer("position", GL_FLOAT, 0, 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rendered_texture);

    //bind and create fbo
    QOpenGLFramebufferObject fbo2(img.size());
    qDebug() << "bind success? : " << fbo2.bind();

    glDrawArrays(GL_TRIANGLES, 0, buffer.size());
    QImage result = fbo2.toImage();

    program.disableAttributeArray("qt_Vertex");
    program.release();
    buffer.release();

    fbo2.release();
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    context_->doneCurrent();

    return result;
}
