#include "qsidebar.h"
#include <QVBoxLayout>
QSideBar::QSideBar(QWidget *parent)
    : QWidget(parent)
{
    m_applicationBar.setOrientation(Qt::Vertical);
    m_applicationBar.setButtonSize(QSize(BUTTON_SIZE, BUTTON_SIZE));
    m_toolBar.setOrientation(Qt::Vertical);
    m_toolBar.setButtonSize(QSize(BUTTON_SIZE, BUTTON_SIZE));

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(&m_applicationBar);
    layout()->addWidget(createSeparator());
    layout()->addWidget(&m_toolBar);
    layout()->addItem(new QSpacerItem(0,0, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
}

void QSideBar::addApplication(const QString &name, std::function<void ()> process, QIcon icon)
{
    m_applicationBar.addButtonToFront(name, process, icon);
}

void QSideBar::addTool(const QString &name, std::function<void ()> process, QIcon icon)
{
    m_toolBar.addButtonToBack(name, process, icon);
}

QFrame *QSideBar::createSeparator()
{
    auto line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}
