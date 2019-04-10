#include "toolframe.h"
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDebug>
#include <QApplication>
#include <QScrollBar>
#include <QRegExp>
#include <QToolTip>

ToolFrame::ToolFrame(QWidget *parent) :
    QMdiSubWindow(parent),
    m_usingTimer(false),
    m_usingPlot(false),
    m_usingEventlog(false),
    eventlog(new QTextStream(&logbuffer)),
    m_sidebar(new SideBar(this)),
    m_content(new QWidget(this)),
    m_settings(new QWidget(this)),
    m_settings_header(new QLabel(QString("<b> Settings </b>"), this)),
    m_toolsettings(new ToolSettings(this)),
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

    connect(m_sidebar, SIGNAL(main_pushed()), this, SLOT(show_content()));
    connect(m_sidebar, SIGNAL(settings_pushed()), this, SLOT(show_settings()));
    connect(m_toolsettings, SIGNAL(first_time_setup()), this, SLOT(request_settings()));
    connect(m_toolsettings, SIGNAL(update_frame()), this, SLOT(load_settings()));

    resize();
}

void ToolFrame::useTimer()
{
    if (m_usingTimer) {
        return;
    }
    m_usingTimer = true;

    m_timer = new Stopwatch(this);
    m_content->layout()->addWidget(m_timer);
    m_timer->setFixedHeight(BUTTON_SIZE);
    m_timer->setMinimumWidth(4*BUTTON_SIZE);
    m_timer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    m_toolsettings->addTimer();

    connect(m_timer, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

void ToolFrame::usePlot()
{
    if (m_usingPlot) {
        return;
    }
    m_usingPlot = true;
    // ----------------------------
}

void ToolFrame::useEventlog()
{
    if (m_usingEventlog) {
        return;
    }
    m_usingEventlog = true;

    m_eventlog = new EventLogger(*eventlog, this);
    m_wrapper->layout()->addWidget(m_eventlog);
    m_toolsettings->addEventlog();

    connect(m_sidebar, SIGNAL(eventlog_pushed()), this, SLOT(toggle_eventlog()));
}

void ToolFrame::putContent(QWidget *w)
{
    w->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QLayout *layout = m_content->layout();
    layout->addWidget(w);
    layout->addItem(layout->takeAt(layout->indexOf(m_timer))); // Move stopwatch to the bottom of the layout
    resize();
}

void ToolFrame::putSettings(QWidget *w)
{
    w->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QLayout *layout = m_settings->layout();
    layout->addWidget(w);
    layout->addItem(layout->takeAt(layout->indexOf(m_toolsettings))); // Move general settings to the bottom of the layout

    QString application_name = windowTitle(); // If it won't save settings, strip the string for white spaces
    m_settings_header->setText(QString("<b>%1 settings</b>").arg(application_name));

    m_toolsettings->setContext(context());
    resize();
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
    if (m_usingTimer) {
        if (!m_timer->isRunning() && store().value(QString("toolSettings/timeoutEnabled")).toBool()) {
            m_timer->start(); // EVT: with set first.
        }
    }
}

void ToolFrame::stopped()
{
    m_sidebar->status_stopped();
    if (m_usingTimer) {
        if (m_timer->isRunning()) {
            m_timer->stop();
        }
    }
}

void ToolFrame::error()
{
    m_sidebar->status_error();
    if (m_usingTimer) {
        if (m_timer->isRunning()) {
            m_timer->stop(); // pause?
        }
    }
}

void ToolFrame::plot(int value)
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
    resize();
    emit start();
}

void ToolFrame::show_settings()
{
    if (m_content->isVisible()) {
        m_content->setVisible(false);
    }
    m_content->setVisible(false);
    m_settings->setVisible(true);
    resize();
}

void ToolFrame::toggle_eventlog()
{
    if (m_eventlog->isVisible()) {
        m_eventlog->setVisible(false);
    } else {
        m_eventlog->setVisible(true);
    }
    resize();
}

void ToolFrame::load_settings()
{
    if (m_usingTimer) {
        QVariant timeout_enabled = store().value(QString("toolSettings/timeoutEnabled"));
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
            QVariant timeout = store().value(QString("toolSettings/timeout"));
            if (!timeout.isNull()) {
                m_timer->set(timeout.toString());
            } else {
                output() << QString("No settings for timeout found");
            }
        }
    }

    if (m_usingPlot) {
        QVariant graph_enabled = store().value(QString("toolSettings/graphEnabled"));
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
    }


    if (m_usingEventlog) {
        QVariant eventlog_enabled = store().value(QString("toolSettings/eventlogEnabled"));
        if (!eventlog_enabled.isNull()) {
            if (eventlog_enabled.toBool()) {
                // m_sidebar->eventlog_button->setVisible(true);
            } else {
                //m_eventlog->stop(); AND
                //m_eventlog_button->setVisible(false);
                m_eventlog->setVisible(false);
            }
        } else {
            output() << QString("No settings for eventlog found");
        }
        if (eventlog_enabled.toBool()) {
            QVariant sampling = store().value(QString("toolSettings/eventlogSampling"));
            if (!sampling.isNull()) {
                m_eventlog->setSampleInterval(sampling.toInt());
                output() << QString("Found sampling: %1").arg(sampling.toInt());
            } else {
                output() << QString("No settings for elog sampling found");
            }
        }
    }

    resize(); // Remove this when settings are fixed
}

QTextStream &ToolFrame::output() const
{
    return *eventlog;
}

QSettings &ToolFrame::store() const
{
    return *m_toolsettings->store();
}

QString ToolFrame::context()
{
    QRegExp space("\\s");
    return QString(windowTitle().simplified().remove(space));
}

SideBar::SideBar(QWidget *parent) :
    QWidget(parent),
    main_button(new QPushButton(this)),
    settings_button(new QPushButton(this)),
    eventlog_button(new QPushButton(this)),
    status_led(new StatusBitWidget(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(main_button);
    layout->addWidget(settings_button);
    layout->addWidget(eventlog_button);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
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
    setSampleInterval(1000);
    ticker.start();
}

void EventLogger::setSampleInterval(int interval)
{
    ticker.setInterval(interval);
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
    m_usingTimer(false),
    m_usingPlot(false),
    m_usingEventlog(false)
{
    setLayout(new QFormLayout(this));
}

void ToolSettings::setContext(QString context)
{
    m_store = new QSettings("Lemon", context);
    connectWidgets();
    loadSettings();
}

void ToolSettings::addTimer()
{
    if (m_usingTimer) {
        return;
    }
    m_usingTimer = true;

    timeout_checker = new QCheckBox(this);
    timeout_edit = new QLineEdit(this);
    timeout_edit->setPlaceholderText("[hh:mm:ss]");

    QFormLayout *form = qobject_cast<QFormLayout*>(layout());
    form->addRow(separator());
    form->addRow(tr("&Enable timer:"), timeout_checker);
    form->addRow(tr("&Set timeout:"), timeout_edit);

    connect(timeout_checker, SIGNAL(clicked()), this, SLOT(update_form()));
    connect(timeout_edit, SIGNAL(returnPressed()), this, SLOT(set_timeout()));
}

void ToolSettings::addPlot()
{
    if (m_usingPlot) {
        return;
    }
    m_usingPlot = true;

    graph_checker = new QCheckBox(this);
    graph_sample_edit = new QLineEdit(this);
    graph_sample_edit->setPlaceholderText("[ms]");

    QFormLayout *form = qobject_cast<QFormLayout*>(layout());
    form->addRow(separator());
    form->addRow(tr("&Enable graph:"), graph_checker);
    form->addRow(tr("&Sample interval"), graph_sample_edit);

    connect(graph_checker, SIGNAL(clicked()), this, SLOT(update_form()));
}

void ToolSettings::addEventlog()
{
    if (m_usingEventlog) {
        return;
    }
    m_usingEventlog = true;

    eventlog_checker = new QCheckBox(this);
    eventlog_edit = new QLineEdit(this);
    eventlog_edit->setPlaceholderText("[ms]");

    QFormLayout *form = qobject_cast<QFormLayout*>(layout());
    form->addRow(separator());
    form->addRow(tr("&Event log"), eventlog_checker);
    form->addRow(tr("&Sample interval"), eventlog_edit);

    connect(eventlog_checker, SIGNAL(clicked()), this, SLOT(update_form()));
    connect(eventlog_edit, SIGNAL(returnPressed()), this, SLOT(set_eventlog_sampling()));
}

QSettings *ToolSettings::store() const
{
    return m_store;
}

void ToolSettings::update_form()
{
    if (m_usingTimer) {
        if (timeout_checker->isChecked()) {
            timeout_edit->setEnabled(true);
        } else {
            timeout_edit->setEnabled(false);
        }
    }

    if (m_usingPlot) {
        if (graph_checker->isChecked()) {
            graph_sample_edit->setEnabled(true);
        } else {
            graph_sample_edit->setEnabled(false);
        }
    }

    if (m_usingEventlog) {
        if (eventlog_checker->isChecked()) {
            eventlog_edit->setEnabled(true);
        } else {
            eventlog_edit->setEnabled(false);
        }
    }

    saveSettings();
    emit update_frame();
}

void ToolSettings::set_timeout()
{
    QRegExp format("\\d\\d:\\d\\d:\\d\\d");
    QString timeout = timeout_edit->text();
    if (format.exactMatch(timeout)) {
        store()->setValue(QString("toolSettings/timeout"), timeout);
        emit update_frame();
    } else {
        QToolTip::showText(timeout_edit->mapToGlobal(QPoint(0, 0)), QString("Use hh:mm:ss format!"));
    }
}

void ToolSettings::set_eventlog_sampling()
{
    QRegExp format("\\d*");
    QString interval = eventlog_edit->text();
    if (format.exactMatch(interval)) {
        store()->setValue(QString("toolSettings/eventlogSampling"), interval);
        emit update_frame();
    } else {
        QToolTip::showText(eventlog_edit->mapToGlobal(QPoint(0, 0)), QString("Only digits!"));
    }
}

QFrame *ToolSettings::separator()
{
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}

void ToolSettings::loadSettings()
{
    bool settings_found = true;
    if (m_usingTimer) {
        QVariant timeout_enabled = store()->value(QString("toolSettings/timeoutEnabled"));
        if (!timeout_enabled.isNull()) {
            timeout_checker->setChecked(timeout_enabled.toBool());
        } else {
            settings_found = false;
        }
        QVariant timeout = store()->value(QString("toolSettings/timeout"));
        if (!timeout.isNull()) {
            timeout_edit->setText(timeout.toString());
        } else {
            settings_found = false;
        }
    }

    if (m_usingPlot) {
        QVariant graph_enabled = store()->value(QString("toolSettings/graphEnabled"));
        if (!graph_enabled.isNull()) {
            graph_checker->setChecked(graph_enabled.toBool());
        } else {
            settings_found = false;
        }
        QVariant graph_sampling = store()->value(QString("toolSettings/graphSampling"));
        if (!graph_sampling.isNull()) {
            graph_sample_edit->setText(graph_sampling.toString());
        } else {
            settings_found = false;
        }
    }

    if (m_usingEventlog) {
        QVariant eventlog_enabled = store()->value(QString("toolSettings/eventlogEnabled"));
        if (!eventlog_enabled.isNull()) {
            eventlog_checker->setChecked(eventlog_enabled.toBool());
        } else {
            settings_found = false;
        }
        QVariant eventlog_sampling = store()->value(QString("toolSettings/eventlogSampling"));
        if (!eventlog_sampling.isNull()) {
            eventlog_edit->setText(eventlog_sampling.toString());
        } else {
            settings_found = false;
        }
    }

    if (!settings_found) {
        emit first_time_setup();
    }
    update_form();
}

void ToolSettings::saveSettings()
{
    if (m_usingTimer) {
        store()->setValue(QString("toolSettings/timeoutEnabled"), timeout_checker->isChecked());
        store()->setValue(QString("toolSettings/timeout"), timeout_edit->text());
    }


//    store()->setValue(QString("toolSettings/graphEnabled"), graph_checker->isChecked());
//    store()->setValue(QString("toolSettings/graphSampling"), graph_sample_edit->text());

    if (m_usingEventlog) {
        store()->setValue(QString("toolSettings/eventlogEnabled"), eventlog_checker->isChecked());
        store()->setValue(QString("toolSettings/eventlogSampling"), eventlog_edit->text());
    }

}

void ToolSettings::connectWidgets()
{

    //connect graph samples

}
