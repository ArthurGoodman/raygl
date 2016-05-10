#include "consoleparser.h"
#include <QtWidgets>

#include "console.h"
#include "renderer.h"
#include "shaderprogram.h"

ConsoleParser::ConsoleParser(Console *console)
    : console(console) {
    connect(console, SIGNAL(command(QString)), SLOT(command(QString)));
}

bool ConsoleParser::execute(const QString &cmd) {
    this->cmd = cmd;
    pos = 0;

    if (getToken().isEmpty()) {
    } else if (token == "cls" || token == "clear") {
        console->clear();
        console->insertPrompt();

        return false;
    } else if (token == "exit" || token == "quit") {
        qApp->quit();
    } else if (token == "help" || token == "?") {
        *console << "This is help."
                 << "";
    } else if (token == "blank") {
        Renderer::instance()->setShader(new ShaderProgram("blank", ""));
    } else {
        *console << "error: unknown command '" + token + "'\n";
    }

    return true;
}

QString ConsoleParser::getToken() {
    token = "";

    while (cmd[pos].isSpace() || cmd[pos] == '\n')
        pos++;

    if (cmd[pos] == '"')
        while (cmd[++pos] != '"' && !cmd[pos].isNull())
            token += cmd[pos];
    else
        while (!cmd[pos].isSpace() && !cmd[pos].isNull())
            token += cmd[pos++];

    return token;
}

void ConsoleParser::command(const QString &cmd) {
    console->lock();

    if (execute(cmd)) {
        console->insertBlock();
        console->insertPrompt();
    }
}
