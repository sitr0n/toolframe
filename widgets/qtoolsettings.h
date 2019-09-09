#ifndef QTOOLSETTINGS_H
#define QTOOLSETTINGS_H

#include <QWidget>
#include <QFrame>
#include <QCheckBox>
#include <QLineEdit>
#include <QSettings>
#include <QToolTip>
#include <QFormLayout>
#include <QPushButton>
#include <QMap>
#include <QSharedPointer>

class QToolSettings : public QWidget
{
    Q_OBJECT
public:
    explicit QToolSettings(QWidget *parent = 0);
    void addTab(const QString &name);
    QWidget& tab(const QString &name);
    template<typename Proc>
    void addNumberField(const QString &description, Proc actuate) {
//        auto textField = new QLineEdit(this);
//        auto wrapper = new QWidget(this);
//        auto form = new QFormLayout(wrapper);
//        form->addRow(description, textField);

//        QLayout *l = layout();
//        l->addWidget(wrapper);

        auto textField = new QLineEdit(this);
        m_contentLayout.addRow(description, textField);

        connect(textField, &QLineEdit::returnPressed, this,
                [=](){
            auto value = textField->text();
            if (!isNumber(value)) {
                QToolTip::showText(textField->mapToGlobal(QPoint(0, 0)), QString("Only digits!"));
                return;
            }
            auto number = value.toDouble();
            const double MIN_NUMBER = 10000.0;
            const double MAX_NUMBER = 60000.0;
            if (number < MIN_NUMBER || MAX_NUMBER < number) {
                QToolTip::showText(textField->mapToGlobal(QPoint(0, 0)), QString("This field expects a value between %1 and %2").arg(MIN_NUMBER).arg(MAX_NUMBER));
                return;
            }
            actuate(value.toInt());
        });
    }


private:
    QHBoxLayout m_tabLayout;
    QFormLayout m_contentLayout;
    QPushButton *m_resetButton;
    QPushButton *m_applyButton;
    QPushButton *m_cancelButton;

    QMap<QString, QWidget*> m_tabs;

    bool isNumber(const QString &text);
};

#endif // QTOOLSETTINGS_H
