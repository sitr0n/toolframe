#ifndef TOOLFRAME_H
#define TOOLFRAME_H

#include <QtWidgets/QMdiSubWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMap>

#include "qbuttonbar.h"
#include "qtoolsettings.h"
#include "qterminal.h"

class ToolFrame : public QMdiSubWindow
{
    Q_OBJECT
public:
    explicit ToolFrame(QString title, QWidget *parent = 0);

protected:
    void addTool(QWidget *tool, const QString &name, QIcon icon = QIcon(), Qt::Alignment align = Qt::AlignCenter);
    QToolSettings &settings() const;
    QTerminal &terminal();

private:
    QWidget* createLayout();
    void display(const QString &tool);
    void toggle(const QString &tool);
    void resize();

    QMap<QString, QWidget*> m_tools;
    QHBoxLayout m_centerLayout;
    QHBoxLayout m_horizontalLayout;
    QVBoxLayout m_verticalLayout;

    QToolSettings *m_qtoolsettings;
    QTerminal m_terminal;
    QButtonBar m_sidebar;

    void resetSidebar();
};


#endif // TOOLFRAME_H
