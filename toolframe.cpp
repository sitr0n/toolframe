#include "toolframe.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDebug>
#include <QApplication>
#include <QScrollBar>

#define BUTTON_SIZE 45

ToolFrame::ToolFrame(QWidget *parent) :
    QMdiSubWindow(parent),
    eventlog(new QTextStream(&logbuffer)),
    wrapper(new MainWrapper(*eventlog, this))
{
    setWidget(wrapper);

    connect(wrapper, SIGNAL(resized()), this, SLOT(resize()));
    connect(wrapper, SIGNAL(timeout()), this, SIGNAL(timeout()));
    resize();
}

void ToolFrame::setContent(QWidget *w)
{
    wrapper->setCentralWidget(w);
}

void ToolFrame::setSettings(QWidget *w)
{

}

void ToolFrame::started()
{
    wrapper->started();
}

void ToolFrame::running()
{
    wrapper->running();
}

void ToolFrame::stopped()
{
    wrapper->stopped();
}

void ToolFrame::error()
{
    wrapper->error();
}

void ToolFrame::reading(int value)
{
    // log value in graph class
}

void ToolFrame::resize()
{
    if (!QMdiSubWindow::isMaximized()) {
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        QMdiSubWindow::resize(sizeHint());
    }
}

QTextStream &ToolFrame::output() const
{
    return *eventlog;
}

SideBar::SideBar(QWidget *parent) :
    QWidget(parent),
    main_button(new QPushButton(this)),
    settings_button(new QPushButton(this)),
    eventlog_button(new QPushButton(this)),
    spacer(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding)),
    status_led(new StatusBitWidget(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(main_button);
    layout->addWidget(settings_button);
    layout->addWidget(eventlog_button);
    layout->addItem(spacer);
    layout->addWidget(status_led);
    layout->setMargin(0);

    main_button->setMinimumSize(QSize(BUTTON_SIZE, BUTTON_SIZE));
    main_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    main_button->setIcon(QIcon(":/icons/start.png"));
    main_button->setIconSize(QSize(BUTTON_SIZE/2, BUTTON_SIZE/2));

    settings_button->setMinimumSize(QSize(BUTTON_SIZE, BUTTON_SIZE));
    settings_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    settings_button->setIcon(QIcon(":/icons/settings.png"));
    settings_button->setIconSize(QSize(BUTTON_SIZE/2, BUTTON_SIZE/2));

    eventlog_button->setMinimumSize(QSize(BUTTON_SIZE, BUTTON_SIZE));
    eventlog_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    eventlog_button->setIcon(QIcon(":/icons/eventlog.png"));
    eventlog_button->setIconSize(QSize(BUTTON_SIZE/2, BUTTON_SIZE/2));

    status_led->setFixedSize(QSize(BUTTON_SIZE, BUTTON_SIZE));
    status_led->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(main_button, SIGNAL(clicked()), this, SIGNAL(main_pushed()));
    connect(settings_button, SIGNAL(clicked()), this, SIGNAL(settings_pushed()));
    connect(eventlog_button, SIGNAL(clicked()), this, SIGNAL(eventlog_pushed()));
}

void SideBar::status_started()
{
    status_led->setBlue();
}

void SideBar::status_running()
{
    status_led->setGreen();
}

void SideBar::status_stopped()
{
    status_led->setInactive();
}

void SideBar::status_error()
{
    status_led->setYellow();
}


CentralWrapper::CentralWrapper(QWidget *parent) :
    QWidget(parent),
    stopwatch(new Stopwatch(this))
{
    QLayout *layout = new QVBoxLayout(this);
    layout->addWidget(stopwatch);
    layout->setMargin(0);

    stopwatch->setFixedHeight(BUTTON_SIZE);
    stopwatch->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    connect(stopwatch, SIGNAL(timeout()), this, SIGNAL(timeout()));

    stopwatch->start(0, 0, 7);
}

void CentralWrapper::setCentralWidget(QWidget *w)
{
    w->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QLayout *layout = this->layout();
    layout->addWidget(w);
    layout->addItem(layout->takeAt(layout->indexOf(stopwatch))); // Move stopwatch to the bottom of the layout
}

void CentralWrapper::enable_stopwatch(bool enable)
{
    if (enable) {
        if (stopwatch == NULL) {
            stopwatch = new Stopwatch(this);
            layout()->addWidget(stopwatch);
        } else {
            stopwatch->setVisible(true);
        }
    } else {
        if (stopwatch != NULL) {
            layout()->takeAt(layout()->indexOf(stopwatch));
            delete stopwatch;
        } else {
            stopwatch->setVisible(false);
        }
    }
}

MainWrapper::MainWrapper(QTextStream &stream, QWidget *parent) :
    QWidget(parent),
    sidebar(new SideBar(this)),
    content(new CentralWrapper(this)),
    settings(new QWidget(this)),
    tool_settings(new ToolSettings(this)),
    eventlog(new EventLogger(stream, this))
{
    QLayout *layout = new QHBoxLayout(this);
    layout->addWidget(sidebar);
    layout->addWidget(content);
    layout->addWidget(settings);
    settings->setVisible(false);
    settings->setLayout(new QFormLayout);
    settings->layout()->addWidget(tool_settings);
    layout->addWidget(eventlog);
    eventlog->setVisible(false);
    layout->setMargin(3);

    sidebar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    content->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    connect(sidebar, SIGNAL(main_pushed()), this, SLOT(show_content()));
    connect(sidebar, SIGNAL(settings_pushed()), this, SLOT(show_settings()));
    connect(sidebar, SIGNAL(eventlog_pushed()), this, SLOT(toggle_eventlog()));
    connect(content, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

void MainWrapper::setCentralWidget(QWidget *w)
{
    content->setCentralWidget(w);
}

void MainWrapper::enable_timer(bool enable)
{
    content->enable_stopwatch(enable);
}

void MainWrapper::toggle_eventlog()
{
    if (eventlog->isVisible()) {
        eventlog->setVisible(false);
    } else {
        eventlog->setVisible(true);
    }
    emit resized();
}

void MainWrapper::started()
{
    sidebar->status_running();
}

void MainWrapper::running()
{
    sidebar->status_running();
}

void MainWrapper::stopped()
{
    sidebar->status_stopped();
}

void MainWrapper::error()
{
    sidebar->status_error();
}

void MainWrapper::show_content()
{
    if (settings->isVisible()) {
        settings->setVisible(false);
    }
    content->setVisible(true);
    emit resized();
    emit start();
}

void MainWrapper::show_settings()
{
    if (content->isVisible()) {
        content->setVisible(false);
    }
    settings->setVisible(true);
    emit resized();
}


EventLogger::EventLogger(QTextStream &events, QWidget *parent) :
    QPlainTextEdit(parent),
    stream(events)
{
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->setMinimumHeight(BUTTON_SIZE);
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);
    setReadOnly(true);
    insertPlainText(QString("[%1] Application opened").arg(QTime::currentTime().toString(QString("hh:mm:ss"))));

    connect(&ticker, SIGNAL(timeout()), this, SLOT(poll_stream()));
    ticker.start(1000);
}

void EventLogger::print(QString line)
{
    QTextCursor cursor = textCursor();
    cursor.setPosition(this->document()->characterCount() - 1);
    setTextCursor(cursor);
    insertPlainText(QString("\n[%1] %2").arg(QTime::currentTime().toString(QString("hh:mm:ss"))).arg(line));

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void EventLogger::poll_stream()
{
    QString line = stream.readLine();
    if (!line.isNull()) {
        print(line);
    }
}


ToolSettings::ToolSettings(QWidget *parent) :
    QWidget(parent),
    pressure_edit(new QLineEdit(this)),
    address_edit(new QLineEdit(this)),
    stopwatch_enable(new QCheckBox(this)),
    timeout_delay_selector(new QSpinBox(this))
{
    QFormLayout *layout = new QFormLayout(this);
    QLabel *title = new QLabel(this);
    title->setText("<b>Settings</b>");
    layout->addRow(title);
    layout->addRow(tr("&Enable timer:"), stopwatch_enable);
    layout->addRow(tr("&Set timeout:"), timeout_delay_selector);
    layout->addRow(tr("&Set manual pressure:"), pressure_edit);
    layout->addRow(tr("&Device address:"), address_edit);
    QSpinBox *spinbox = new QSpinBox(this);
    layout->addRow(tr("&Something:"), spinbox);

    connect(stopwatch_enable, SIGNAL(clicked()), this, SLOT(toggle_stopwatch()));
}

void ToolSettings::toggle_stopwatch()
{
    if (stopwatch_enable->isChecked()) {
        timeout_delay_selector->setEnabled(true);
    } else {
        timeout_delay_selector->setEnabled(false);
    }
}
