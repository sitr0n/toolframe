#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QCheckBox>

class QTerminal : public QWidget
{
    Q_OBJECT
public:
    explicit QTerminal(QWidget *parent = nullptr);
    void print(const QString &message);
    void notify(const QString &message);
    void error(const QString &message);

    void timestamp(const bool enable);
    void setLineCount(const int size);

protected:
    void write(const QString &message, QColor color);
    void resetCursor();
    void scrollDown();
    void updateVerticalPosition();
    bool setMonospaceFont();

private:
    QPlainTextEdit m_terminal;
    QCheckBox m_autoScroll;
    bool m_timeStamp;

    const QString DEFAULT_STYLE = QString("color: transparent");
    const QString NEW_MESSAGE = QString("color: green");

    const int BUTTON_WIDTH = 70;
    const int TERMINAL_WIDTH = 350;
};
