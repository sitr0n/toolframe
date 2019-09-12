#pragma once

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QMap>

class QSettingsForm : public QWidget
{
    Q_OBJECT
public:
    explicit QSettingsForm(QString title = QString(), QWidget *parent = nullptr);
    template<typename Proc>
    void addNumberField(const QString &name, const double lowerLimit, const double upperLimit, const double defaultValue, Proc process) {
        m_fields.insert(name, new QLineEdit(this));
        m_defaults.insert(name, defaultValue);
        m_formLayout.addRow(name, m_fields.value(name));
        loadValue(name);
        process(m_fields.value(name)->text().toDouble());

        connect(m_applyButton, &QPushButton::clicked, this,
                [=](){
            bool convertable;
            auto number = m_fields.value(name)->text().toDouble(&convertable);
            if (!convertable) {
                information(name, QString("Enter a numeric value"));
                return;
            }
            if (number < lowerLimit || upperLimit < number) {
                information(name, QString("Enter a value between %1 and %2").arg(lowerLimit).arg(upperLimit));
                return;
            }
            process(number);
            saveValue(name);
            m_applyButton->setEnabled(false);
        });

        connect(m_fields.value(name), &QLineEdit::textChanged, this,
                [=](){
            m_fields.value(name)->setStyleSheet(BLACK_TEXT);
            m_applyButton->setEnabled(true);
        });
    }

    void addSeparator();
    void setPlaceholder(const QString &name, const QString &text);


signals:
    void feedback(const QString &message);

protected:
    void loadValue(const QString &name);
    void saveValue(const QString &name);
    void information(const QString &name, const QString &message);
    void resetFields();

    QFrame *separator();

private:
    QString m_title;
    QFormLayout m_formLayout;
    QPushButton *m_resetButton;
    QPushButton *m_applyButton;
    QPushButton *m_cancelButton;

    QMap<QString, QLineEdit*> m_fields;
    QMap<QString, double> m_defaults;

    const int BUTTON_SPACING = 4;
    const QString RED_TEXT = "color: #FF0000";
    const QString BLACK_TEXT = "color: #000000";
};
