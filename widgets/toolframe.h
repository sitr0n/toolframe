#ifndef TOOLFRAME_H
#define TOOLFRAME_H

#include <QtWidgets/QMdiSubWindow>
#include "stopwatch.h"
#include <QtWidgets/QPushButton>
#include "statusbitwidget.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>

#include "qtoolsettings.h"
#include "qterminal.h"

#include <QDebug>
#include <vector>
#include <QMap>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ToolFrame : public QMdiSubWindow
{
    Q_OBJECT
public:
    explicit ToolFrame(QString title, QWidget *parent = 0);
    void addTool(QWidget *tool, const QString &name, QIcon icon = QIcon(), Qt::Alignment align = Qt::AlignCenter); // TODO: Implement
    void useTimer(); // startTimer ?
    void usePlot(); // currently not working
    void useEventlog();
    void putContent(QWidget *w);

signals:
    void timeout();
    void start();
    void stop();

protected slots:
    void started();
    void running();
    void stopped();
    void error();
    void plot(int value);

    void resize();
    void request_settings();

private slots:
    void show_content();
    void show_settings();
    void toggle_eventlog();

protected:
    QToolSettings &settings() const;
    QTerminal &terminal();


private:
    void display(const QString &tool);
    void toggle(const QString &tool);

    QMap<QString, QWidget*> m_tools;
    QHBoxLayout m_centerLayout;
    QHBoxLayout m_horizontalLayout;
    QVBoxLayout m_verticalLayout;


    QToolSettings *m_qtoolsettings;
    QTerminal m_terminal;
    bool m_usingTimer;
    bool m_usingPlot;

    QPushButton *m_start_button;
    QPushButton *m_settings_button;
    QPushButton *m_plot_button;
    QPushButton *m_eventlog_button;
    QSpacerItem *sidebar_separator;
    StatusBitWidget *m_status_led;


    QWidget *m_sidebar;
    QWidget *m_content;
    Stopwatch *m_timer;
    QWidget *m_settings;
    QLabel *m_settings_header;
    QWidget *m_wrapper;

    void resetSidebar();
};


#endif // TOOLFRAME_H
