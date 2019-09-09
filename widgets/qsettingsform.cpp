#include "qsettingsform.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

QSettingsForm::QSettingsForm(QString title, QWidget *parent)
    : QWidget(parent)
    , m_title(title)
    , m_unsavedChanges(false)
    , m_resetButton(new QPushButton("Reset", this))
    , m_applyButton(new QPushButton("Apply", this))
    , m_cancelButton(new QPushButton("Cancel", this))
{
    auto wrapper = new QWidget(this);
    wrapper->setLayout(new QVBoxLayout());

    auto form = new QWidget(this);
    form->setLayout(&m_formLayout);
    wrapper->layout()->addWidget(form);

    auto buttonBar = new QWidget(this);
    buttonBar->setLayout(new QHBoxLayout());
    buttonBar->layout()->addWidget(m_resetButton);
    buttonBar->layout()->addWidget(m_applyButton);
    buttonBar->layout()->addWidget(m_cancelButton);
    wrapper->layout()->addWidget(buttonBar);

    setLayout(new QFormLayout(this));
    layout()->addWidget(form);
}

void QSettingsForm::loadOptions(const QString &name)
{
    QSettings settings(m_title, name);

}

