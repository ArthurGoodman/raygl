#pragma once

#include <QObject>

class Console;

class ConsoleParser : public QObject {
    Q_OBJECT

    Console *console;
    QString cmd, token;
    int pos;

public:
    explicit ConsoleParser(Console *console);

    bool execute(const QString &cmd);

private:
    QString getToken();

public slots:
    void command(const QString &cmd);
};
