#include "qbuttonbar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>


QButtonBar::QButtonBar(QWidget *parent)
    : QWidget(parent)
    , m_orientation(Qt::Orientation())
    , m_buttonSize(QSize())
    , m_dependentLayout(new QHBoxLayout())
    , m_independentLayout(new QHBoxLayout())
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
        delete m_dependentLayout;
        m_dependentLayout = new QHBoxLayout();
        delete m_independentLayout;
        m_independentLayout = new QHBoxLayout();
        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
        break;
    case Qt::Vertical:
        delete layout();
        setLayout(new QVBoxLayout());
        delete m_dependentLayout;
        m_dependentLayout = new QHBoxLayout();
        delete m_independentLayout;
        m_independentLayout = new QHBoxLayout();
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        break;
    default:
        emit error("QButtonBar received an unknown orientation!");
        return;
    }
    fillLayout();
    m_orientation = orientation;
}

void QButtonBar::setButtonSize(QSize size)
{
    m_buttonSize = size;
}

QPushButton *QButtonBar::createButton(const QString &name, QIcon icon)
{
    auto button = new QPushButton(this);
    button->setFocusPolicy(Qt::NoFocus);
    //button->setFocus();
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
    switch (m_orientation) {
    case Qt::Horizontal:
        //q_objectcast<QHBoxLayout*>
        break;
    case Qt::Vertical:

        break;
    default:
        emit error("QButtonBar has an unknown orientation!");
        return;
    }
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

