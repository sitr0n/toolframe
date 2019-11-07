#include "qterminal.h"
#include "qbuttonbar.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollBar>
#include <QTime>
#include <QDebug>

QTerminal::QTerminal(QWidget *parent)
    : QWidget(parent)
    , m_timeStamp(false)
{
    setLayout(new QVBoxLayout(this));
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setMinimumWidth(TERMINAL_WIDTH);

    auto button_Bar = new QButtonBar(this);
    button_Bar->addButtonToBack("Save", [=](){

    });
    button_Bar->addButtonToBack("Copy", [this](){
        auto scrollPosition = m_terminal.verticalScrollBar()->value();
        m_terminal.selectAll();
        m_terminal.copy();
        m_terminal.moveCursor(QTextCursor::End);
        m_terminal.verticalScrollBar()->setValue(scrollPosition);
    });
    button_Bar->addButtonToBack("Clear", [this](){
        m_terminal.clear();
    });

    layout()->addWidget(button_Bar);
    layout()->addWidget(&m_terminal);

    auto bottomBar = new QWidget(this);
    bottomBar->setLayout(new QHBoxLayout());
    auto inputLine = new QLineEdit(this);
    inputLine->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    inputLine->setEnabled(false);
    layout()->addWidget(inputLine);
    bottomBar->layout()->addWidget(inputLine);
    m_autoScroll.setChecked(true);
    connect(&m_autoScroll, &QCheckBox::clicked, this,
            [=](const bool checked){
        if (checked) {
            m_autoScroll.setStyleSheet(DEFAULT_STYLE);
            scrollDown();
        } else {

        }
    });
    bottomBar->layout()->addWidget(&m_autoScroll);
    layout()->addWidget(bottomBar);

    auto p = m_terminal.palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    m_terminal.setPalette(p);
    m_terminal.setReadOnly(true);

    auto scrollBar = m_terminal.verticalScrollBar();
    connect(scrollBar, &QAbstractSlider::valueChanged, this,
            [=](){
        if (m_autoScroll.isChecked() && scrollBar->value() < scrollBar->maximum()) {
            m_autoScroll.setChecked(false);
        }
    });

    if (!setMonospaceFont()) {
        error("Operating system has no monospace fonts available!");
    }
}

void QTerminal::print(const QString &message)
{
    write(message, Qt::green);
}

void QTerminal::notify(const QString &message)
{
    write(message, Qt::yellow);
}

void QTerminal::error(const QString &message)
{
    write(message, Qt::red);
}

void QTerminal::timestamp(const bool enable)
{
    m_timeStamp = enable;
}

void QTerminal::setLineCount(const int size)
{
    m_terminal.document()->setMaximumBlockCount(size);
}

void QTerminal::write(const QString &message, QColor color)
{
    auto output = m_terminal.document()->isEmpty() ? QString("") : QString("\n");
    if (m_timeStamp) {
        output.append(QString("[%0] ").arg(QTime::currentTime().toString("hh:mm:ss")));
    }
    output.append(message);

    auto scrollPosition = m_terminal.verticalScrollBar()->value();
    m_terminal.moveCursor(QTextCursor::End);

    QTextCursor cursor(m_terminal.textCursor());
    QTextCharFormat format;
    format.setFontWeight(QFont::DemiBold);
    format.setForeground(QBrush(color));
    cursor.setCharFormat(format);
    cursor.insertText(output);

    if (m_autoScroll.isChecked()) {
        scrollDown();
    } else {
        m_terminal.verticalScrollBar()->setValue(scrollPosition);
        auto bar = m_terminal.verticalScrollBar();
        if (bar->sliderPosition() < bar->maximum()) {
            m_autoScroll.setStyleSheet(NEW_MESSAGE);
        }
    }
}

void QTerminal::scrollDown()
{
    auto bar = m_terminal.verticalScrollBar();
    bar->setValue(bar->maximum());
}

void QTerminal::updateVerticalPosition()
{

}

bool QTerminal::setMonospaceFont()
{
    QFontDatabase db;
    for (auto& name : db.families()) {
        if (db.isFixedPitch(name)) {
            m_terminal.setFont(QFont(name));
            return true;
        }
    }
    return false;
}
