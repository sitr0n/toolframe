#include "qtestingpanel.h"
#include <QFormLayout>

QTestingPanel::QTestingPanel(QWidget *parent)
    : QWidget(parent)
{
    m_powerOnButton.setText("Power on");
    m_powerOffButton.setText("Power off");
    m_autoPairOnButton.setText("Autopair on");
    m_autoPairOffButton.setText("Autopair off");
    m_pairButton.setText("Pair");

    auto form = new QFormLayout(this);
    form->addRow(&m_powerOnButton);
    form->addRow(&m_powerOffButton);
    form->addRow(&m_autoPairOnButton);
    form->addRow(&m_autoPairOffButton);
    form->addRow(&m_pairButton);
    form->addRow("&WingCom interval:", &m_wingComIntervalEdit);
}
