#pragma once

#include <QWidget>
#include <QPlainTextEdit>
#include <QCheckBox>

class QTerminal : public QWidget
{
    Q_OBJECT
public:
    explicit QTerminal(QWidget *parent = nullptr);
    void print(const QString &message);
    void notify(const QString &message);
    void error(const QString &message);

    void timestamp(const bool enable);
    void setRowBuffer(const int size);

protected:
    void resetCursor();
    void scrollDown();

private:
    QPlainTextEdit m_terminal;
    QCheckBox m_autoScroll;
    bool m_timeStamp;

    const int BUTTON_WIDTH = 70;

    const QString DEFAULT_STYLE = QString("color: transparent");
    const QString NEW_MESSAGE = QString("color: green");
};
