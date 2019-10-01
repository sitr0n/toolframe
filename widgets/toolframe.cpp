#include "toolframe.h"
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QDebug>
#include <QtWidgets/QApplication>

#include <QStyle>
#define BUTTON_SIZE 45

ToolFrame::ToolFrame(QString title, QWidget *parent)
    : QMdiSubWindow(parent)
    , m_qtoolsettings(new QToolSettings(title, this))
    , m_usingTimer(false)
    , m_usingPlot(false)
    , m_start_button(new QPushButton(this))
    , m_settings_button(new QPushButton(this))
    , sidebar_separator(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding))
    , m_status_led(new StatusBitWidget(this))
    , m_sidebar(new QWidget(this))
    , m_content(new QWidget(this))
    , m_settings(new QWidget(this))
    , m_wrapper(new QWidget(this))
{
    setWindowTitle(title);
    setWindowIcon(QIcon("noicon"));

    m_start_button->setMinimumSize(QSize(BUTTON_SIZE, BUTTON_SIZE));
    m_start_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_start_button->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    m_start_button->setIconSize(QSize(BUTTON_SIZE*2/3, BUTTON_SIZE*2/3));
    m_start_button->setFocusPolicy(Qt::TabFocus);

    m_settings_button->setMinimumSize(QSize(BUTTON_SIZE, BUTTON_SIZE));
    m_settings_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_settings_button->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    m_settings_button->setIconSize(QSize(BUTTON_SIZE/2, BUTTON_SIZE/2));
    m_settings_button->setFocusPolicy(Qt::TabFocus);

    m_status_led->setFixedSize(QSize(BUTTON_SIZE, BUTTON_SIZE));
    m_status_led->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_sidebar->setLayout(new QVBoxLayout);
    m_content->setLayout(new QVBoxLayout);
    m_settings->setLayout(new QVBoxLayout);
    m_wrapper->setLayout(new QHBoxLayout);

    m_sidebar->layout()->addWidget(m_start_button);
    m_sidebar->layout()->addWidget(m_settings_button);
    m_sidebar->layout()->addItem(sidebar_separator);
    m_sidebar->layout()->addWidget(m_status_led);

    m_settings->layout()->addWidget(m_qtoolsettings);
    m_settings->setVisible(false);

    m_wrapper->layout()->addWidget(m_sidebar);
    m_wrapper->layout()->addWidget(m_content);
    m_wrapper->layout()->addWidget(m_settings);

    this->layout()->setMargin(6);
    m_sidebar->layout()->setMargin(0);
    m_content->layout()->setMargin(0);
    m_wrapper->layout()->setMargin(0);
    setWidget(m_wrapper);

    connect(m_start_button, SIGNAL(clicked(bool)), this, SLOT(show_content()));
    connect(m_settings_button, SIGNAL(clicked(bool)), this, SLOT(show_settings()));
    //connect(m_toolsettings, SIGNAL(first_time_setup()), this, SLOT(request_settings()));
    //connect(m_toolsettings, SIGNAL(update_frame()), this, SLOT(load_settings()));

    resize();
}

void ToolFrame::addTool(QWidget *tool, const QString &name, QIcon icon, Qt::Alignment align)
{
    switch (align) {
    case Qt::AlignCenter:
        m_centerLayout.addWidget(tool);
        break;
    case Qt::AlignLeft:
        m_horizontalLayout.insertWidget(0, tool);
        break;
    case Qt::AlignTop:
        m_verticalLayout.insertWidget(0, tool);
        break;
    case Qt::AlignRight:
        m_horizontalLayout.addWidget(tool);
        break;
    case Qt::AlignBottom:
        m_verticalLayout.addWidget(tool);
        break;
    default:
        m_terminal.print(QString("ToolFrame::addTool(%0) received unexpected alignment value: %1").arg(name).arg(align));
        return;
    }

    m_tools.insert(name, tool);

//     m_sidebar->addButton(name, [&](){
//       if (align == Qt::AlignCenter) {
//           display(tools.value(name));
//       } else {
//           toggle(tools.value(name));
//       }

//     }
//     , icon);
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

    //m_toolsettings->addTimer();

    connect(m_timer, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

void ToolFrame::usePlot()
{
    if (m_usingPlot) {
        return;
    }
    m_usingPlot = true;

    //new PlotCreator

    m_plot_button = new QPushButton(this);
    m_plot_button->setMinimumSize(QSize(BUTTON_SIZE, BUTTON_SIZE));
    m_plot_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_plot_button->setIcon(QIcon(":/icons/plot.png"));
    m_plot_button->setIconSize(QSize(BUTTON_SIZE, BUTTON_SIZE));
    m_sidebar->layout()->addWidget(m_plot_button);
    resetSidebar();

    //m_toolsettings->addPlot();

    // Connect something
}

void ToolFrame::useEventlog()
{
    m_wrapper->layout()->addWidget(&m_terminal);

    m_eventlog_button = new QPushButton(this);
    m_eventlog_button->setMinimumSize(QSize(BUTTON_SIZE, BUTTON_SIZE));
    m_eventlog_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_eventlog_button->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    m_eventlog_button->setIconSize(QSize(BUTTON_SIZE/2, BUTTON_SIZE/2));
    m_sidebar->layout()->addWidget(m_eventlog_button);
    resetSidebar();

    auto general = settings().addTab("General");
    general->addLabel("<b>Terminal</b>");
    general->addPathField("Log file directory", [=](const QString &path){
        terminal().print(QString("Log file directory: %0").arg(path));
    });
    general->addCheckBox("Enable timestamps", [=](const bool enable){m_terminal.timestamp(enable);});

    connect(m_eventlog_button, SIGNAL(clicked(bool)), this, SLOT(toggle_eventlog()));
}

void ToolFrame::putContent(QWidget *w)
{
    w->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QLayout *layout = m_content->layout();
    layout->addWidget(w);
    if (m_usingTimer) {
        layout->addItem(layout->takeAt(layout->indexOf(m_timer))); // Move stopwatch to the bottom of the layout
    }
    resize();
}

void ToolFrame::started()
{
    m_status_led->setBlue();
}

void ToolFrame::running()
{
    m_status_led->setGreen();
//    if (m_usingTimer) {
//        if (!m_timer->isRunning() && store().value(QString("toolSettings/timeoutEnabled")).toBool()) {
//            m_timer->start(); // EVT: with set first.
//        }
//    }
}

void ToolFrame::stopped()
{
    m_status_led->setInactive();
    if (m_usingTimer) {
        if (m_timer->isRunning()) {
            m_timer->stop();
        }
    }
}

void ToolFrame::error()
{
    m_status_led->setYellow();
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
        QMdiSubWindow::resize(sizeHint().width(), size().height());
        //QMdiSubWindow::resize(sizeHint());
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
    if (m_terminal.isVisible()) {
        m_terminal.setVisible(false);
    } else {
        m_terminal.setVisible(true);
    }
    resize();
}

QToolSettings &ToolFrame::settings() const
{
    return *m_qtoolsettings;
}

QTerminal &ToolFrame::terminal()
{
    return m_terminal;
}

void ToolFrame::display(const QString &tool)
{

}

void ToolFrame::toggle(const QString &tool)
{
    m_tools.value(tool)->setVisible(!m_tools.value(tool)->isVisible());
}

void ToolFrame::resetSidebar()
{
    QLayout *layout = m_sidebar->layout();
    int last_item = layout->indexOf(m_status_led); // Use more robust function like length of or something
    layout->takeAt(last_item);
    layout->takeAt(last_item - 1);
    layout->addItem(sidebar_separator);
    layout->addWidget(m_status_led);
}

//void ToolSettings::set_timeout()
//{
//    QRegExp format("\\d\\d:\\d\\d:\\d\\d");
//    QString timeout = timeout_edit->text();
//    if (format.exactMatch(timeout)) {
//        store()->setValue(QString("toolSettings/timeout"), timeout);
//        emit update_frame();
//    } else {
//        QToolTip::showText(timeout_edit->mapToGlobal(QPoint(0, 0)), QString("Use hh:mm:ss format!"));
//    }
//}

