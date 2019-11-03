#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

class QTestingPanel : public QWidget
{
    Q_OBJECT
public:
    explicit QTestingPanel(QWidget *parent = nullptr);

private:
    QPushButton m_powerOnButton;
    QPushButton m_powerOffButton;
    QPushButton m_autoPairOnButton;
    QPushButton m_autoPairOffButton;
    QPushButton m_pairButton;
    QLineEdit m_wingComIntervalEdit;

};
