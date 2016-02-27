#include <QApplication>

//#include "glwidget.h"

#include "offscreen.h"
#include <QLabel>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

//    GLWidget w;
//    w.show();

    OffScreen off;

    QLabel label;
    label.setPixmap(QPixmap::fromImage(off.render()));
    label.show();

    return a.exec();
}
