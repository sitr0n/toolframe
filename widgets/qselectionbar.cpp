#include "qselectionbar.h"

QSelectionBar::QSelectionBar(QWidget *parent)
    : QButtonBar(parent)
{
}

void QSelectionBar::addButtonToFront(const QString &name, std::function<void ()> process, QIcon icon)
{
    auto button = createButton(name, icon);
    m_items.push_front(button);
    resetLayout();
    m_buttons.insert(name, button);
    highlight(name);

    connect(button, &QPushButton::clicked, this, [this, name, process](){
        if (QString::compare(name, m_highlighted) != 0) {
            highlight(name);
            process();
        }
    });
}

void QSelectionBar::addButtonToBack(const QString &name, std::function<void ()> process, QIcon icon)
{
    auto button = createButton(name, icon);
    m_items.push_back(button);
    resetLayout();
    m_buttons.insert(name, button);
    if (m_buttons.size() == 1) {
        highlight(name);
    }

    connect(button, &QPushButton::clicked, this, [this, name, process](){
        if (QString::compare(name, m_highlighted) != 0) {
            highlight(name);
            process();
        }
    });
}

void QSelectionBar::highlight(const QString &button)
{
    for (const auto& name : m_buttons.keys()) {
        m_buttons.value(name)->setStyleSheet(INACTIVE_BUTTONSTYLE);
    }
    m_buttons.value(button)->setStyleSheet(ACTIVE_BUTTONSTYLE);
    m_highlighted = button;
}
