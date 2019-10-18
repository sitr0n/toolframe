#include "toolframe.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QStyle>
#define BUTTON_SIZE 45

ToolFrame::ToolFrame(QString title, QWidget *parent)
    : QMdiSubWindow(parent)
    , m_qtoolsettings(new QToolSettings(title, this))
{
    setWindowTitle(title);
    setWindowIcon(QIcon("noicon"));
    setWidget(createLayout());

    m_sidebar.setOrientation(Qt::Vertical);
    addTool(m_qtoolsettings, "Settings", style()->standardIcon(QStyle::SP_FileDialogDetailedView), Qt::AlignCenter);
    addTool(&m_terminal, "Terminal", style()->standardIcon(QStyle::SP_ComputerIcon), Qt::AlignRight);

    auto general = settings().addTab("General");
    general->addLabel("<b>Terminal</b>");
    general->addPathField("Log file directory", [=](const QString &path){
        terminal().print(QString("Log file directory: %0").arg(path));
    });
    general->addCheckBox("Enable timestamps", [=](const bool enable){
        m_terminal.timestamp(enable);
    });

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
    if (align == Qt::AlignCenter) {
        m_sidebar.addExclusiveButton(name, [this, name](){
            display(name);
        },
        icon);
    } else {
        m_sidebar.addButton(name, [this, name](){
            toggle(name);
        },
        icon);
    }
}

void ToolFrame::resize()
{
    if (!QMdiSubWindow::isMaximized()) {
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        QMdiSubWindow::resize(sizeHint().width(), size().height());
        //QMdiSubWindow::resize(sizeHint());
    }
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
    for (const auto& name : m_tools.keys()) {
        m_tools.value(name)->setVisible(false);
    }
    m_tools.value(tool)->setVisible(true);
    resize();
}

void ToolFrame::toggle(const QString &tool)
{
    m_tools.value(tool)->setVisible(!m_tools.value(tool)->isVisible());
    resize();
}

QWidget *ToolFrame::createLayout()
{
    auto mainLayout = new QHBoxLayout();
    mainLayout->addWidget(&m_sidebar);
    mainLayout->addLayout(&m_horizontalLayout);
    m_horizontalLayout.addLayout(&m_verticalLayout);
    m_verticalLayout.addLayout(&m_centerLayout);

    mainLayout->setMargin(0);
    m_horizontalLayout.setMargin(0);
    m_verticalLayout.setMargin(0);
    m_centerLayout.setMargin(0);

    auto wrapper = new QWidget(this);
    wrapper->setLayout(mainLayout);
    return wrapper;
}

void ToolFrame::resetSidebar()
{
//    QLayout *layout = m_sidebar->layout();
//    int last_item = layout->indexOf(m_status_led); // Use more robust function like length of or something
//    layout->takeAt(last_item);
//    layout->takeAt(last_item - 1);
//    layout->addItem(sidebar_separator);
//    layout->addWidget(m_status_led);
}


