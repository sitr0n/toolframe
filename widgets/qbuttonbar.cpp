#include "qbuttonbar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>


QButtonBar::QButtonBar(QWidget *parent)
    : QWidget(parent)
    , m_orientation(Qt::Orientation())
{
    setOrientation(Qt::Horizontal);
}

void QButtonBar::setOrientation(Qt::Orientation orientation)
{
    if (orientation == m_orientation) {
        return;
    }
    switch (orientation) {
    case Qt::Horizontal:
        delete layout();
        setLayout(new QHBoxLayout());
        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
        break;
    case Qt::Vertical:
        delete layout();
        setLayout(new QVBoxLayout());
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        break;
    default:
        emit error("QButtonBar received an unknown orientation!");
        return;
    }
    fillLayout();
    m_orientation = orientation;
}

QPushButton *QButtonBar::createButton(const QString &name, QIcon icon)
{
    auto button = new QPushButton(this);
    button->setFocusPolicy(Qt::TabFocus);
    button->setMinimumSize(QSize(BTN_SIZE, BTN_SIZE));
    button->setToolTip(name);
    if (icon.isNull()) {
        button->setText(name);
    } else {
        button->setIcon(icon);
    }
    m_items.push_back(button);
    return button;
}

void QButtonBar::fillLayout()
{
    for (const auto& item : m_items) {
        layout()->addWidget(item);
    }
    layout()->setMargin(0);
}

void QButtonBar::highlight(const QString &button)
{
    for (const auto& name : m_dependentButtons.keys()) {
        m_dependentButtons.value(name)->setStyleSheet(INACTIVE_BUTTONSTYLE);
    }
    m_dependentButtons.value(button)->setStyleSheet(ACTIVE_BUTTONSTYLE);
    m_highlighted = button;
}

void QButtonBar::toggle(const QString &button)
{
    Q_UNUSED(button)
}

