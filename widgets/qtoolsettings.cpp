#include "qtoolsettings.h"
#include <QFormLayout>
#include <QToolTip>
#include <QDebug>

QToolSettings::QToolSettings(QString title, QWidget *parent)
    : QWidget(parent)
    , m_title(title)
    , m_tabLayout(new QHBoxLayout())
    , m_contentLayout(new QHBoxLayout())
{
    setLayout(new QVBoxLayout(this));
    m_header.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    m_header.setMinimumHeight(28);
    m_header.setAlignment(Qt::AlignCenter);
    layout()->addWidget(&m_header);
    layout()->addWidget(&m_tabs);

    auto content = new QWidget(this);
    content->setLayout(m_contentLayout);
    layout()->addWidget(content);
}

QSettingsForm *QToolSettings::addTab(const QString &name)
{
    if (m_forms.contains(name)) {
        return &tab(name);
    }

    m_tabs.addButtonToFront(name, [=]{ display(name); });
    m_forms.insert(name, new QSettingsForm(QString("%0/%1").arg(m_title).arg(name), this));
    m_contentLayout->addWidget(m_forms.value(name));
    display(name);

    connect(m_forms.value(name), &QSettingsForm::feedback, this, &QToolSettings::feedback);
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

    setHeader(name);
}

void QToolSettings::setHeader(const QString &title)
{
    m_header.setText(QString("<b>%0 settings</b>").arg(title));
}




