#include "qtoolsettings.h"
#include <QFormLayout>
#include <QToolTip>
#include <QLabel>

QToolSettings::QToolSettings(QWidget *parent)
    : QWidget(parent)
{
    setLayout(new QVBoxLayout(this));
    layout()->addWidget(new QLabel("Settings"));

    auto tabs = new QWidget(this);
    tabs->setLayout(&m_tabLayout);
    layout()->addWidget(tabs);

    auto content = new QWidget(this);
    content->setLayout(&m_contentLayout);
    layout()->addWidget(new QWidget(content));

    auto buttons = new QWidget(this);
    buttons->setLayout(new QHBoxLayout());
    buttons->layout()->addWidget(m_resetButton);
    buttons->layout()->addWidget(m_applyButton);
    buttons->layout()->addWidget(m_cancelButton);
    layout()->addWidget(buttons);
}

void QToolSettings::addTab(const QString &name)
{
    if (m_tabs.contains(name)) {
        return;
    }
    auto content = new QWidget(this);
    content->setLayout(new QFormLayout());
    m_tabs.insert(name, content);

    auto tab = new QPushButton(name, this);
    m_tabLayout.addWidget(tab);

    connect(tab, &QPushButton::clicked, this,
            [=](){
        QMapIterator<QString, QWidget*> i(m_tabs);
        while (i.hasNext()) {
            i.next();
            i.value()->setVisible(false);
        }
        m_tabs.value(name)->setVisible(true);
    });
}

QWidget &QToolSettings::tab(const QString &name)
{
    return *m_tabs.value(name);
}


bool QToolSettings::isNumber(const QString &text)
{
    QRegExp format("^[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)$");
    return format.exactMatch(text);
}


