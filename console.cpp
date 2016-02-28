#include "console.h"
#include <QtWidgets>

Console::Console(QWidget *parent)
    : QTextEdit(parent) {
    setFrameShape(QFrame::NoFrame);
    viewport()->setCursor(Qt::ArrowCursor);

    setFont(QFont("Consolas", 12));

    setWordWrapMode(QTextOption::WrapAnywhere);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    history = new QStringList;
    historyPos = 0;

    locked = false;
}

void Console::setPrompt(QString prompt) {
    this->prompt = prompt;
    insertPrompt();
}

void Console::lock() {
    locked = true;
}

void Console::unlock() {
    locked = false;
}

void Console::output(QString str) {
    textCursor().insertBlock();

    textCursor().insertText(str);

    scrollDown();
}

Console &Console::operator<<(const QString &str) {
    output(str);
    return *this;
}

void Console::insertBlock() {
    textCursor().insertBlock();
}

void Console::insertPrompt() {
    textCursor().insertText(prompt);

    scrollDown();

    locked = false;
}

void Console::setBaseColor(QColor c) {
    QPalette p = palette();
    p.setColor(QPalette::Base, c);
    setPalette(p);
}

void Console::setTextColor(QColor c) {
    QPalette p = palette();
    p.setColor(QPalette::Text, c);
    setPalette(p);
}

void Console::keyPressEvent(QKeyEvent *e) {
    if (locked)
        return;

    if ((e->key() == Qt::Key_V && e->modifiers() == Qt::ControlModifier))
        QTextEdit::keyPressEvent(e);
    else if (e->key() >= Qt::Key_Space && e->key() <= Qt::Key_BraceRight && e->key() != Qt::Key_QuoteLeft) {
        if (e->modifiers() == Qt::NoModifier || e->modifiers() == Qt::ShiftModifier)
            QTextEdit::keyPressEvent(e);
    }

    else
        switch (e->key()) {
        case Qt::Key_Return:
            onReturn();
            break;

        case Qt::Key_Up:
            historyBack();
            break;
        case Qt::Key_Down:
            historyForward();
            break;

        case Qt::Key_Backspace:
            if (textCursor().positionInBlock() > prompt.length())
                QTextEdit::keyPressEvent(e);
            break;

        case Qt::Key_End:
        case Qt::Key_Delete:
            QTextEdit::keyPressEvent(e);
            break;

        case Qt::Key_Left:
            if (e->modifiers() == Qt::NoModifier) {
                QTextEdit::keyPressEvent(e);

                QTextCursor cursor = textCursor();

                if (cursor.positionInBlock() < prompt.length())
                    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);

                setTextCursor(cursor);
            } else if (e->modifiers() == Qt::ControlModifier) {
                QTextEdit::keyPressEvent(e);

                QTextCursor cursor = textCursor();

                if (cursor.positionInBlock() < prompt.length())
                    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 2);

                setTextCursor(cursor);
            }
            break;

        case Qt::Key_Right:
            if (e->modifiers() == Qt::NoModifier || e->modifiers() == Qt::ControlModifier)
                QTextEdit::keyPressEvent(e);
            break;

        case Qt::Key_Home: {
            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, prompt.length());
            setTextCursor(cursor);
        } break;

        //    case Qt::Key_PageUp:
        //    case Qt::Key_PageDown:
        //    {
        //        QTextEdit::keyPressEvent(e);

        //        QTextCursor cursor = textCursor();
        //        cursor.movePosition(QTextCursor::End);
        //        setTextCursor(cursor);
        //    } break;

        case Qt::Key_PageUp: {
            QScrollBar *vbar = verticalScrollBar();
            vbar->setValue(vbar->value() - 15);
        } break;

        case Qt::Key_PageDown: {
            QScrollBar *vbar = verticalScrollBar();
            vbar->setValue(vbar->value() + 15);
        } break;

        //    case Qt::Key_Escape:
        //    {
        //        QTextCursor cursor = textCursor();
        //        cursor.movePosition(QTextCursor::StartOfBlock);
        //        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        //        cursor.removeSelectedText();

        //        insertPrompt();
        //    } break;

        default:
            QWidget::keyPressEvent(e);
        }
}

void Console::mousePressEvent(QMouseEvent *) {
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *) {
}

void Console::contextMenuEvent(QContextMenuEvent *) {
}

void Console::onReturn() {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::EndOfBlock);
    setTextCursor(cursor);

    QString cmd = cursor.block().text().mid(prompt.length());
    historyAdd(cmd);

    emit command(cmd);
}

void Console::scrollDown() {
    QScrollBar *vbar = verticalScrollBar();
    vbar->setValue(vbar->maximum());
}

void Console::historyAdd(QString cmd) {
    history->append(cmd);
    historyPos = history->length();
}

void Console::historyBack() {
    if (!historyPos)
        return;

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    cursor.insertText(prompt + history->at(historyPos - 1));

    setTextCursor(cursor);

    historyPos--;
}

void Console::historyForward() {
    if (historyPos == history->length())
        return;

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    if (historyPos == history->length() - 1)
        cursor.insertText(prompt);
    else
        cursor.insertText(prompt + history->at(historyPos + 1));

    setTextCursor(cursor);

    historyPos++;
}
