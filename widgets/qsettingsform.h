#pragma once

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QToolTip>
#include <QSettings>

class QSettingsForm : public QWidget
{
    Q_OBJECT
public:
    explicit QSettingsForm(QString title = QString(), QWidget *parent = nullptr);
    template<typename Proc>
    void addNumberField(const QString &info, const double lowerLimit, const double upperLimit, Proc process) {
        //auto form = static_cast<QFormLayout>(layout());
        auto textField = new QLineEdit(this);
        m_formLayout.addRow(info, textField);

        connect(m_applyButton, &QPushButton::clicked, this,
                [=](){
            bool convertable;
            auto number = textField->text().toDouble(&convertable);
            if (!convertable) {
                QToolTip::showText(textField->mapToGlobal(QPoint(0, 0)), QString("Enter a numeric value"));
                return;
            }
            if (number < lowerLimit || upperLimit < number) {
                QToolTip::showText(textField->mapToGlobal(QPoint(0, 0)), QString("Enter a value between %1 and %2").arg(lowerLimit).arg(upperLimit));
                return;
            }
            // save first or after?
            process(number);
            m_applyButton->setEnabled(false); // will this even work
            m_applyButton->setCheckable(false);
        });

        connect(textField, &QLineEdit::textChanged, this,
                [=](){
            m_unsavedChanges = true;
            m_applyButton->setEnabled(true);
            m_applyButton->setCheckable(true);
        });
    }

    void setDefault(const QString &name, const double value);

signals:
    void trigger();

protected slots:
    void loadOptions(const QString &name);

private:
    QString m_title;
    bool m_unsavedChanges;
    QFormLayout m_formLayout;
    QPushButton *m_resetButton;
    QPushButton *m_applyButton;
    QPushButton *m_cancelButton;

};
