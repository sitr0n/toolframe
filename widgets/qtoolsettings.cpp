#include "qtoolsettings.h"
#include <QFormLayout>
#include <QToolTip>
#include <QLabel>
#include <QDebug>

QToolSettings::QToolSettings(QString title, QWidget *parent)
    : QWidget(parent)
    , m_title(title)
    , m_tabLayout(new QHBoxLayout())
    , m_contentLayout(new QHBoxLayout())
{
    setLayout(new QVBoxLayout(this));
    layout()->addWidget(new QLabel("Settings"));

    auto tabs = new QWidget(this);
    tabs->setLayout(m_tabLayout);
    layout()->addWidget(tabs);

    auto content = new QWidget(this);
    content->setLayout(m_contentLayout);
    layout()->addWidget(content);
}

QSettingsForm *QToolSettings::addTab(const QString &name)
{
    if (m_forms.contains(name)) {
        return &tab(name);
    }
    m_buttons.insert(name, new QPushButton(name, this));
    m_tabLayout->addWidget(m_buttons.value(name));

    m_forms.insert(name, new QSettingsForm(QString("%0/%1").arg(m_title).arg(name), this));
    m_contentLayout->addWidget(m_forms.value(name));
    connect(m_forms.value(name), &QSettingsForm::feedback, this, &QToolSettings::feedback);

    display(name);
    connect(m_buttons.value(name), &QPushButton::clicked, this,
            [=](){
        display(name);
    });

    return m_forms.value(name);
}

QSettingsForm &QToolSettings::tab(const QString &name)
{
    return *m_forms.value(name);
}

void QToolSettings::display(const QString &name)
{
    if (!m_forms.contains(name)) {
        return;
    }
    QMapIterator<QString, QSettingsForm*> i(m_forms);
    while (i.hasNext()) {
        i.next();
        i.value()->setVisible(false);
    }
    m_forms.value(name)->setVisible(true);
}




