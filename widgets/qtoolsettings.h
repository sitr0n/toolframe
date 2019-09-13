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
#include "qsettingsform.h"

class QToolSettings : public QWidget
{
    Q_OBJECT
public:
    explicit QToolSettings(QString title = QString(), QWidget *parent = nullptr);
    QSettingsForm *addTab(const QString &name);
    QSettingsForm &tab(const QString &name);

signals:
    void feedback(const QString &message);

protected:
    void display(const QString &name);

private:
    QString m_title;
    QHBoxLayout *m_tabLayout;
    QHBoxLayout *m_contentLayout;

    QMap<QString, QPushButton*> m_buttons;
    QMap<QString, QSettingsForm*> m_forms;
};

#endif // QTOOLSETTINGS_H
