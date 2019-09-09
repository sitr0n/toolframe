#pragma once

#include <QWidget>

class QSettingsForm : public QWidget
{
    Q_OBJECT
public:
    explicit QSettingsForm(QWidget *parent = nullptr);
    void addNumberField(const QString &info, int lowerLimit, int upperLimit, )
};
