#include "qbuttonbar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>


QButtonBar::QButtonBar(QWidget *parent)
    : QWidget(parent)
    , m_orientation(Qt::Orientation())
    , m_size(QSize())
{
    setOrientation(Qt::Horizontal);
}

void QButtonBar::setOrientation(Qt::Orientation orientation)
{
    if (orientation == m_orientation) {
        return;
    }
    m_orientation = orientation;
    resetLayout();
}

void QButtonBar::setButtonSize(QSize size)
{
    if (size == m_size) {
        return;
    }
    m_size = size;
    for (auto& button: m_items) {
        button->setMinimumSize(m_size);
    }
}

void QButtonBar::addButtonToFront(const QString &name, std::function<void ()> process, QIcon icon)
{
    auto button = createButton(name, icon);
    m_items.push_front(button);
    resetLayout();

    connect(button, &QPushButton::clicked, this, [process](){
        process();
    });
}

void QButtonBar::addButtonToBack(const QString &name, std::function<void ()> process, QIcon icon)
{
    auto button = createButton(name, icon);
    m_items.push_back(button);
    resetLayout();

    connect(button, &QPushButton::clicked, this, [process](){
        process();
    });
}

QPushButton *QButtonBar::createButton(const QString &name, QIcon icon)
{
    auto button = new QPushButton(this);
    button->setFocusPolicy(Qt::NoFocus);
    button->setToolTip(name);
    if (!m_size.isEmpty()) {
        button->setMinimumSize(m_size);
    }
    if (icon.isNull()) {
        button->setText(name);
    } else {
        button->setIcon(icon);
    }
    return button;
}

void QButtonBar::resetLayout()
{
    newLayout();
    fillLayout();
}

void QButtonBar::newLayout()
{
    switch (m_orientation) {
    case Qt::Horizontal:
        delete layout();
        setLayout(new QHBoxLayout());
        break;
    case Qt::Vertical:
        delete layout();
        setLayout(new QVBoxLayout());
        break;
    default:
        emit error("QButtonBar received an unknown orientation!");
        return;
    }
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    layout()->setMargin(LAYOUT_MARGIN);
}

void QButtonBar::fillLayout()
{
    for (const auto& item : m_items) {
        layout()->addWidget(item);
    }
}
