#include "toolframe.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDebug>
#include <QApplication>
#include <QScrollBar>
#include <QRegExp>
#include <QToolTip>

#define BUTTON_SIZE 45

ToolFrame::ToolFrame(QWidget *parent) :
    QMdiSubWindow(parent),
    eventlog(new QTextStream(&logbuffer)),
    m_sidebar(new SideBar(this)),
    m_content(new QWidget(this)),
    m_timer(new Stopwatch(this)),
    m_settings(new QWidget(this)),
    m_settings_header(new QLabel(QString("<b> Settings </b>"), this)),
    m_toolsettings(new ToolSettings(this)),
    m_eventlog(new EventLogger(*eventlog, this)),
    m_wrapper(new QWidget(this))
{
    layout()->setMargin(6);
    m_content->setLayout(new QVBoxLayout);
    m_content->layout()->setMargin(0);
    m_settings->setLayout(new QVBoxLayout);
    m_settings->layout()->addWidget(m_settings_header);
    m_settings->layout()->addWidget(m_toolsettings);
    m_settings->setVisible(false);
    m_wrapper->setLayout(new QHBoxLayout);
    m_wrapper->layout()->setMargin(0);
    setWidget(m_wrapper);

    m_wrapper->layout()->addWidget(m_sidebar);
    m_wrapper->layout()->addWidget(m_content);
    m_wrapper->layout()->addWidget(m_settings);
    m_wrapper->layout()->addWidget(m_eventlog);
    m_eventlog->setVisible(false);

    m_content->layout()->addWidget(m_timer);
    m_timer->setFixedHeight(BUTTON_SIZE);
    m_timer->setMinimumWidth(4*BUTTON_SIZE);
    m_timer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    connect(m_sidebar, SIGNAL(main_pushed()), this, SLOT(show_content()));
    connect(m_sidebar, SIGNAL(settings_pushed()), this, SLOT(show_settings()));
    connect(m_sidebar, SIGNAL(eventlog_pushed()), this, SLOT(toggle_eventlog()));
    connect(m_toolsettings, SIGNAL(first_time_setup()), this, SLOT(request_settings()));
    connect(m_toolsettings, SIGNAL(update_frame()), this, SLOT(load_settings()));
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
    w->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QLayout *layout = m_settings->layout();
    layout->addWidget(w);
    layout->addItem(layout->takeAt(layout->indexOf(m_toolsettings))); // Move generic settings to the bottom of the layout

    QString application_name = windowTitle(); // If it won't save settings, strip the string for white spaces
    m_settings_header->setText(QString("<b>%1 settings</b>").arg(application_name));
    QRegExp space("\\s");
    m_toolsettings->setContext(application_name.simplified().remove(space));
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
            //m_timer->setVisible(false);
        }
    }
}

void ToolFrame::started()
{
    m_sidebar->status_started();
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

void ToolFrame::request_settings()
{
    show_settings();
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
    m_content->setVisible(false);
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

void ToolFrame::load_settings()
{
    QRegExp space("\\s");
    QString context = QString("%1/").arg(windowTitle().simplified().remove(space));
    QSettings settings;
    QVariant timeout_enabled = settings.value(QString("%1toolSettings/timeoutEnabled").arg(context));
    if (!timeout_enabled.isNull()) {
        if (timeout_enabled.toBool()) {
            m_timer->setVisible(true);
        } else {
            m_timer->stop();
            m_timer->setVisible(false);
        }
    } else {
        output() << QString("No settings for timer found");
    }
    if (timeout_enabled.toBool()) {
        QVariant timeout = settings.value(QString("%1toolSettings/timeout").arg(context));
        if (!timeout.isNull()) {
            output() << QString("Found timeout: %1").arg(timeout.toString());
            int timeout_seconds = 0;
            //timeout_seconds -------------------------------------------------------------------------------------------------------------------------------------|||||||
            //m_timer->set();
        } else {
            output() << QString("No settings for timeout found");
        }
    }

    QVariant graph_enabled = settings.value(QString("%1toolSettings/graphEnabled").arg(context));
    if (!graph_enabled.isNull()) {
        if (graph_enabled.toBool()) {
//            m_graph->setVisible(true);
            output() << QString("Give me graph");
        } else {
            output() << QString("Remove graph");
//            m_graph->stop();
//            m_graph->setVisible(false);
        }
    } else {
        //output() << QString("No settings for graph found");
    }

    QVariant eventlog_enabled = settings.value(QString("%1toolSettings/eventlogEnabled").arg(context));
    if (!eventlog_enabled.isNull()) {
        if (eventlog_enabled.toBool()) {
            m_eventlog->setVisible(true);
        } else {
            //m_eventlog->stop(); AND
            //m_eventlog_button->setVisible(false);
            m_eventlog->setVisible(false);
        }
    } else {
        //output() << QString("No settings for eventlog found");
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
    m_context(QString::null),
    timeout_line(new QFrame(this)),
    timeout_checker(new QCheckBox(this)),
    timeout_edit(new QLineEdit(this)),
    graph_line(new QFrame(this)),
    graph_checker(new QCheckBox(this)),
    graph_sample_edit(new QLineEdit(this)),
    eventlog_line(new QFrame(this)),
    eventlog_checker(new QCheckBox(this)),
    eventlog_edit(new QLineEdit(this))
{
    timeout_line->setFrameShape(QFrame::HLine);
    timeout_line->setFrameShadow(QFrame::Sunken);
    timeout_edit->setPlaceholderText("[hh:mm:ss]");
    graph_line->setFrameShape(QFrame::HLine);
    graph_line->setFrameShadow(QFrame::Sunken);
    graph_sample_edit->setPlaceholderText("[ms]");
    eventlog_line->setFrameShape(QFrame::HLine);
    eventlog_line->setFrameShadow(QFrame::Sunken);
    eventlog_edit->setPlaceholderText("[ms]");



    QFormLayout *layout = new QFormLayout(this);
    layout->addRow(timeout_line);
    layout->addRow(tr("&Enable timer:"), timeout_checker);
    timeout_checker->setChecked(true);
    layout->addRow(tr("&Set timeout:"), timeout_edit);
    layout->addRow(graph_line);
    layout->addRow(tr("&Enable graph:"), graph_checker);
    layout->addRow(tr("&Sample frequency"), graph_sample_edit);
    layout->addRow(eventlog_line);
    layout->addRow(tr("&Event log"), eventlog_checker);
    layout->addRow(tr("&Sample frequency"), eventlog_edit);

    connect(timeout_checker, SIGNAL(clicked()), this, SLOT(update_form()));
    connect(graph_checker, SIGNAL(clicked()), this, SLOT(update_form()));
    connect(eventlog_checker, SIGNAL(clicked()), this, SLOT(update_form()));
    connect(timeout_edit, SIGNAL(returnPressed()), this, SLOT(set_timeout()));
    connect(timeout_edit, SIGNAL(editingFinished()), this, SLOT(set_timeout()));

}

void ToolSettings::setContext(QString context)
{
    m_context = QString("%1/").arg(context);
    qDebug() << QString("Using context: %1").arg(m_context);
    loadSettings();
}

void ToolSettings::update_form()
{
    if (timeout_checker->isChecked()) {
        timeout_edit->setEnabled(true);
    } else {
        timeout_edit->setEnabled(false);
    }
    if (graph_checker->isChecked()) {
        graph_sample_edit->setEnabled(true);
    } else {
        graph_sample_edit->setEnabled(false);
    }
    if (eventlog_checker->isChecked()) {
        eventlog_edit->setEnabled(true);
    } else {
        eventlog_edit->setEnabled(false);
    }
    saveSettings();
    emit update_frame();
}

void ToolSettings::set_timeout()
{
    QRegExp regex("\\d\\d:\\d\\d:\\d\\d");
    QString timeout = timeout_edit->text();
    if (regex.exactMatch(timeout)) {
        QSettings settings;
        settings.setValue(QString("%1toolSettings/timeout").arg(m_context), timeout);
        emit update_frame();
    } else {
        QToolTip::showText(timeout_edit->mapToGlobal(QPoint(0, 0)), QString("Use hh:mm:ss format!"));
    }
}

void ToolSettings::loadSettings()
{
    bool settings_found = true;
    QSettings settings;
    QVariant timeout_enabled = settings.value(QString("%1toolSettings/timeoutEnabled").arg(m_context));
    if (!timeout_enabled.isNull()) {
        timeout_checker->setChecked(timeout_enabled.toBool());
    } else {
        settings_found = false;
    }
    QVariant timeout = settings.value(QString("%1toolSettings/timeout").arg(m_context));
    if (!timeout.isNull()) {
        timeout_edit->setText(timeout.toString());
    } else {
        settings_found = false;
    }

    QVariant graph_enabled = settings.value(QString("%1toolSettings/graphEnabled").arg(m_context));
    if (!graph_enabled.isNull()) {
        graph_checker->setChecked(graph_enabled.toBool());
    } else {
        settings_found = false;
    }
    QVariant graph_sampling = settings.value(QString("%1toolSettings/graphSampling").arg(m_context));
    if (!graph_sampling.isNull()) {
        graph_sample_edit->setText(graph_sampling.toString());
    } else {
        settings_found = false;
    }

    QVariant eventlog_enabled = settings.value(QString("%1toolSettings/eventlogEnabled").arg(m_context));
    if (!eventlog_enabled.isNull()) {
        eventlog_checker->setChecked(eventlog_enabled.toBool());
    } else {
        settings_found = false;
    }
    QVariant eventlog_sampling = settings.value(QString("%1toolSettings/eventlogSampling").arg(m_context));
    if (!eventlog_sampling.isNull()) {
        eventlog_edit->setText(eventlog_sampling.toString());
    } else {
        settings_found = false;
    }

    if (!settings_found) {
        emit first_time_setup();
    }
    update_form();
}

void ToolSettings::saveSettings()
{
    QSettings settings;
    settings.setValue(QString("%1toolSettings/timeoutEnabled").arg(m_context), timeout_checker->isChecked());
    settings.setValue(QString("%1toolSettings/timeout").arg(m_context), timeout_edit->text());

//    settings.setValue(QString("%1toolSettings/graphEnabled").arg(m_context), graph_checker->isChecked());
//    settings.setValue(QString("%1toolSettings/graphSampling").arg(m_context), graph_sample_edit->text());

//    settings.setValue(QString("%1toolSettings/eventlogEnabled").arg(m_context), eventlog_checker->isChecked());
//    settings.setValue(QString("%1toolSettings/eventlogSampling").arg(m_context), eventlog_edit->text());
}
