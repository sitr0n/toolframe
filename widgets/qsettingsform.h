#pragma once

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QSettings>
#include <QMap>
#include <QFileInfo>

class QSettingsForm : public QWidget
{
    Q_OBJECT
public:
    explicit QSettingsForm(QString title = QString(), QWidget *parent = nullptr);
    void addLabel(const QString &text);
    void addSeparator();

    template<typename Proc>
    void addNumberField(const QString &name, const double lowerLimit, const double upperLimit, const double defaultValue, Proc process) {
        m_fields.insert(name, new QLineEdit(this));
        setPlaceholder(name, QString("[%0 - %1]").arg(lowerLimit).arg(upperLimit));
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
                information(name, QString("Enter a value between %0 and %1").arg(lowerLimit).arg(upperLimit));
                return;
            }
            process(number);
            saveValue(name);
        });

        connect(m_fields.value(name), &QLineEdit::returnPressed, this,
                [=](){
            m_applyButton->click();
        });

        connect(m_fields.value(name), &QLineEdit::textChanged, this,
                [=](){
            m_fields.value(name)->setStyleSheet(BLACK_TEXT);
            if (!m_applyButton->isEnabled()) {
                m_applyButton->setEnabled(true);
            }
        });
    }

    template<typename Proc>
    void addPathField(const QString &name, Proc process, const QString &defaultPath = QString("~")) {
        m_fields.insert(name, new QLineEdit(this));
        setPlaceholder(name, QString("Path"));
        m_defaults.insert(name, defaultPath);
        m_formLayout.addRow(name, m_fields.value(name));
        loadValue(name);

        process(formatPath(m_fields.value(name)->text()));
        connect(m_applyButton, &QPushButton::clicked, this,
                [=](){
            auto path = formatPath(m_fields.value(name)->text());
            if (!QFileInfo(path).isDir()) {
                information(name, QString("Cannot locate this directory"));
                return;
            }
            if (!QFileInfo(path).isWritable()) {
                information(name, QString("No write privileges in this directory"));
                return;
            }

            process(path);
            saveValue(name);
        });

        connect(m_fields.value(name), &QLineEdit::returnPressed, this,
                [=](){
            m_applyButton->click();
        });

        connect(m_fields.value(name), &QLineEdit::textChanged, this,
                [=](){
            m_fields.value(name)->setStyleSheet(BLACK_TEXT);
            if (!m_applyButton->isEnabled()) {
                m_applyButton->setEnabled(true);
            }
        });
    }

    template<typename Proc>
    void addCheckBox(const QString &name, Proc process) {
        auto box = new QCheckBox(this);
        //box->setChecked(true);
        m_formLayout.addRow(name, box);
        process(box->isChecked());
        connect(box, &QCheckBox::clicked, this,
                [=](const bool checked){
            process(checked);
        });
    }


signals:
    void feedback(const QString &message);

protected:
    void loadValue(const QString &name);
    void saveValue(const QString &name);
    QString formatPath(const QString &path);
    void information(const QString &name, const QString &message);
    void setPlaceholder(const QString &name, const QString &text);
    void resetFields();

    QFrame *separator();

private:
    QString m_title;
    QFormLayout m_formLayout;
    QPushButton *m_resetButton;
    QPushButton *m_applyButton;
    QPushButton *m_cancelButton;

    QMap<QString, QLineEdit*> m_fields;
    QMap<QString, QVariant> m_defaults;

    const int BUTTON_SPACING = 4;
    const int TOOLTIP_OFFSET = 0; //-14
    const QString RED_TEXT = "color: #FF0000";
    const QString BLACK_TEXT = "color: #000000";
};
