#include "qtoolsettings.h"
#include <QFormLayout>
#include <QToolTip>

QToolSettings::QToolSettings(QWidget *parent)
    : QWidget(parent)
{
    setLayout(new QFormLayout(this));
}


bool QToolSettings::isNumber(const QString &text)
{
    QRegExp format("^[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)$");
    return format.exactMatch(text);
}


