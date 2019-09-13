#include "qsettingsform.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QToolTip>

QSettingsForm::QSettingsForm(QString title, QWidget *parent)
    : QWidget(parent)
    , m_title(title)
    , m_resetButton(new QPushButton("Reset", this))
    , m_applyButton(new QPushButton("Apply", this))
    , m_cancelButton(new QPushButton("Cancel", this))
{
    setLayout(new QVBoxLayout(this));
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    auto form = new QWidget(this);
    form->setLayout(&m_formLayout);
    layout()->addWidget(form);

    auto buttonBar = new QWidget(this);
    buttonBar->setLayout(new QHBoxLayout());
    buttonBar->layout()->addWidget(m_resetButton);
    buttonBar->layout()->addItem(new QSpacerItem(BUTTON_SPACING, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    buttonBar->layout()->addWidget(m_applyButton);
    m_applyButton->setEnabled(false);
    buttonBar->layout()->addWidget(m_cancelButton);
    layout()->addWidget(buttonBar);

    connect(m_resetButton, &QPushButton::clicked, this,
            [=](){
        QMessageBox popup;
        popup.setText("<b>Settings will be reset to default values.</b>");
        popup.setInformativeText("Are you sure you want to continue?");
        popup.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        popup.setDefaultButton(QMessageBox::Cancel);
        auto userDecision = popup.exec();
        switch (userDecision) {
        case QMessageBox::Yes:
            resetFields();
            m_applyButton->setEnabled(false);
            emit feedback(QString("%0 has been reset to default.").arg(m_title));
            break;

        case QMessageBox::Cancel:
            emit feedback(QString("%0 reset was cancelled by user.").arg(m_title));
            break;
        }
    });

    connect(m_cancelButton, &QPushButton::clicked, this,
            [=](){
        for (const auto& name: m_fields.keys()) {
            loadValue(name);
            m_fields.value(name)->setStyleSheet(BLACK_TEXT);
        }
        m_applyButton->setEnabled(false);
    });
}

void QSettingsForm::addLabel(const QString &text)
{
    m_formLayout.addRow(new QLabel(text, this));
}

void QSettingsForm::addSeparator()
{
    m_formLayout.addRow(separator());
}

void QSettingsForm::setPlaceholder(const QString &name, const QString &text)
{
    m_fields.value(name)->setPlaceholderText(text);
}

void QSettingsForm::loadValue(const QString &name)
{
    QSettings settings(QString("ToolFrame"), m_title);
    auto value = settings.value(name, m_defaults.value(name));
    m_fields.value(name)->setText(value.toString());
}

void QSettingsForm::saveValue(const QString &name)
{
    QSettings settings(QString("ToolFrame"), m_title);
    auto value = m_fields.value(name)->text();
    settings.setValue(name, value);
}

void QSettingsForm::information(const QString &name, const QString &message)
{
    m_fields.value(name)->setStyleSheet(RED_TEXT);
    QToolTip::showText(m_fields.value(name)->mapToGlobal(QPoint(0, -12)), message);
}

void QSettingsForm::resetFields()
{
    for (const auto& name: m_fields.keys()) {
        auto value = m_defaults.value(name);
        m_fields.value(name)->setText(value.toString());
        saveValue(name);
        m_fields.value(name)->setStyleSheet(BLACK_TEXT);
    }
}

QFrame *QSettingsForm::separator()
{
    auto line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}

