#pragma once

#include <QTextEdit>

class Console : public QTextEdit {
    Q_OBJECT

    QStringList *history;
    QString prompt;

    int historyPos;
    bool locked;

public:
    explicit Console(QWidget *parent = 0);

    void setPrompt(QString prompt);

    void lock();
    void unlock();

    void output(QString);
    Console &operator<<(const QString &str);

    void insertBlock();
    void insertPrompt();

    void setBaseColor(QColor c);
    void setTextColor(QColor c);

protected:
    void keyPressEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent *);

private:
    void onReturn();
    void historyAdd(QString);
    void historyBack();
    void historyForward();
    void scrollDown();

signals:
    void command(const QString &);
};
