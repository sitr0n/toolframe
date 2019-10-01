#include "qterminal.h"
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

    auto buttonBar = new QWidget(this);
    buttonBar->setLayout(new QHBoxLayout);
    auto saveButton = new QPushButton("Save", this);
    connect(saveButton, &QPushButton::clicked, this, [=](){});
    saveButton->setMinimumWidth(BUTTON_WIDTH);
    saveButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    saveButton->setEnabled(false);
    buttonBar->layout()->addWidget(saveButton);
    auto copyButton = new QPushButton("Copy", this);
    connect(copyButton, &QPushButton::clicked, this, [=](){
        auto scrollPosition = m_terminal.verticalScrollBar()->value();
        m_terminal.selectAll();
        m_terminal.copy();
        m_terminal.moveCursor(QTextCursor::End);
        m_terminal.verticalScrollBar()->setValue(scrollPosition);
    });
    copyButton->setMinimumWidth(BUTTON_WIDTH);
    copyButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    buttonBar->layout()->addWidget(copyButton);
    auto clearButton = new QPushButton("Clear", this);
    connect(clearButton, &QPushButton::clicked, this, [=](){
        m_terminal.clear();
    });
    clearButton->setMinimumWidth(BUTTON_WIDTH);
    clearButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    buttonBar->layout()->addWidget(clearButton);

    layout()->addWidget(buttonBar);
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
    m_terminal.insertPlainText(QString("[%1] Application opened").arg(QTime::currentTime().toString(QString("hh:mm:ss"))));

    auto scrollBar = m_terminal.verticalScrollBar();
    connect(scrollBar, &QAbstractSlider::valueChanged, this,
            [=](){
        if (m_autoScroll.isChecked() && scrollBar->value() < scrollBar->maximum()) {
            m_autoScroll.setChecked(false);
        }
    });
}

void QTerminal::print(const QString &message)
{
    auto output = m_terminal.document()->isEmpty() ? QString("") : QString("\n");
    if (m_timeStamp) {
        output.append(QString("[%0] ").arg(QTime::currentTime().toString("hh:mm:ss")));
    }
    output.append(message);

    auto scrollPosition = m_terminal.verticalScrollBar()->value();
    m_terminal.moveCursor(QTextCursor::End);
    m_terminal.insertPlainText(output);

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

void QTerminal::timestamp(const bool enable)
{
    m_timeStamp = enable;
}

void QTerminal::setRowBuffer(const int size)
{
    m_terminal.document()->setMaximumBlockCount(size);
}

void QTerminal::scrollDown()
{
    auto bar = m_terminal.verticalScrollBar();
    bar->setValue(bar->maximum());
}