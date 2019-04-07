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
    m_sidebar(new SideBar(this)),
    m_content(new QWidget(this)),
    m_timer(new Stopwatch(this)),
    m_settings(new QWidget(this)),
    m_toolsettings(new ToolSettings(this)),
    m_eventlog(new EventLogger(*eventlog, this)),
    m_wrapper(new QWidget(this))
{
    m_content->setLayout(new QVBoxLayout);
    m_content->layout()->setMargin(0);
    m_settings->setLayout(new QVBoxLayout);
    m_settings->layout()->addWidget(m_toolsettings);
    m_settings->setVisible(false);
    m_wrapper->setLayout(new QHBoxLayout);
    m_wrapper->layout()->setMargin(0);
    setWidget(m_wrapper);

    m_wrapper->layout()->addWidget(m_sidebar);
    m_wrapper->layout()->addWidget(m_content);
    m_wrapper->layout()->addWidget(m_settings);
    m_wrapper->layout()->addWidget(m_eventlog);

    m_content->layout()->addWidget(m_timer);
    m_timer->setFixedHeight(BUTTON_SIZE);
    m_timer->setMinimumWidth(4*BUTTON_SIZE);
    m_timer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    connect(m_sidebar, SIGNAL(main_pushed()), this, SLOT(show_content()));
    connect(m_sidebar, SIGNAL(settings_pushed()), this, SLOT(show_settings()));
    connect(m_sidebar, SIGNAL(eventlog_pushed()), this, SLOT(toggle_eventlog()));
    resize();
}

void ToolFrame::setContent(QWidget *w)
{
    w->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QLayout *layout = m_content->layout();
    layout->addWidget(w);
    layout->addItem(layout->takeAt(layout->indexOf(m_timer))); // Move stopwatch to the bottom of the layout
}

void ToolFrame::setSettings(QWidget *w)
{
    Q_UNUSED(w);
}

void ToolFrame::enable_timer(bool enable)
{
    if (enable) {
        if (m_timer == NULL) {
            m_timer = new Stopwatch(this);
            layout()->addWidget(m_timer);
        } else {
            m_timer->setVisible(true);
        }
    } else {
        if (m_timer != NULL) {
            layout()->takeAt(layout()->indexOf(m_timer));
            delete m_timer;
        } else {
            m_timer->setVisible(false);
        }
    }
}

void ToolFrame::started()
{
    output() << "wat";
    //wrapper->started();
}

void ToolFrame::running()
{
    m_sidebar->status_running();
}

void ToolFrame::stopped()
{
    m_sidebar->status_stopped();
}

void ToolFrame::error()
{
    m_sidebar->status_error();
}

void ToolFrame::reading(int value)
{
    Q_UNUSED(value);
    // log value in graph class
}

void ToolFrame::resize()
{
    if (!QMdiSubWindow::isMaximized()) {
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        QMdiSubWindow::resize(sizeHint());
    }
}

void ToolFrame::show_content()
{
    if (m_settings->isVisible()) {
        m_settings->setVisible(false);
    }
    m_content->setVisible(true);
    emit resize();
    emit start();
}

void ToolFrame::show_settings()
{
    if (m_content->isVisible()) {
        m_content->setVisible(false);
    }
    m_settings->setVisible(true);
    emit resize();
}

void ToolFrame::toggle_eventlog()
{
    if (m_eventlog->isVisible()) {
        m_eventlog->setVisible(false);
    } else {
        m_eventlog->setVisible(true);
    }
    emit resize();
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
    ticker.start(500);
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
    stopwatch_enable->setChecked(true);
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
