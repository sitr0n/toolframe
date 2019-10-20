#pragma once

#include "qbuttonbar.h"
#include <QVector>

class QSelectionBar : public QButtonBar
{
    Q_OBJECT
public:
    explicit QSelectionBar(QWidget *parent = nullptr);
    virtual void addButtonToFront(const QString &name, std::function<void()> process, QIcon icon = QIcon());
    virtual void addButtonToBack(const QString &name, std::function<void()> process, QIcon icon = QIcon());

protected:
    void highlight(const QString &button);

private:
    QString m_highlighted;
    QMap<QString, QWidget*> m_buttons;

    const QString ACTIVE_BUTTONSTYLE = "background-color: #4EDCFF;";
    const QString INACTIVE_BUTTONSTYLE = "";
};
