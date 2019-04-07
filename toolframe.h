#ifndef TOOLFRAME_H
#define TOOLFRAME_H

#include "stopwatch.h"
#include <QWidget>
#include <QTimer>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QSpacerItem>
#include "statusbitwidget.h"

#include <QPlainTextEdit>
#include <QTextStream>
#include <QLabel>


class SideBar;
class EventLogger;
class ToolSettings;
class ToolFrame : public QMdiSubWindow
{
    Q_OBJECT
public:
    explicit ToolFrame(QWidget *parent = 0);
    void setContent(QWidget *w);
    void setSettings(QWidget *w);
    void enable_timer(bool enable);

signals:
    void timeout();
    void start();
    void stop();

protected slots:
    void started();
    void running();
    void stopped();
    void error();
    void reading(int value);
    void resize();

private slots:
    void show_content();
    void show_settings();
    void toggle_eventlog();

protected:
    QTextStream &output() const;

private:
    QString logbuffer;
    QTextStream *eventlog;

    SideBar *m_sidebar;
    QWidget *m_content;
    QWidget *m_timer;
    QWidget *m_settings;
    QWidget *m_toolsettings;
    QWidget *m_eventlog; // EventLogger ptr instead??
    QWidget *m_wrapper;
};

class SideBar : public QWidget
{
    Q_OBJECT
public:
    explicit SideBar(QWidget *parent = 0);
    void status_started();
    void status_running();
    void status_stopped();
    void status_error();

signals:
    void main_pushed();
    void settings_pushed();
    void timer_pushed();
    void eventlog_pushed();

private:
    QPushButton *main_button;
    QPushButton *settings_button;
    QPushButton *eventlog_button;
    QSpacerItem *spacer;
    StatusBitWidget *status_led;
};

class EventLogger : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit EventLogger(QTextStream &events, QWidget *parent = 0);

private slots:
    void poll_stream();

private:
    QTimer ticker;
    QTextStream &stream;

    void print(QString line);
};

#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>

class ToolSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ToolSettings(QWidget *parent = 0);

private slots:
    void toggle_stopwatch();

private:
    QLineEdit *pressure_edit, *address_edit;
    QCheckBox *stopwatch_enable;
    QSpinBox *timeout_delay_selector;
};

#endif // TOOLFRAME_H
