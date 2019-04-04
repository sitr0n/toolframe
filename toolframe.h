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

class MainWrapper;
class CentralWrapper;
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

protected:
    QTextStream &output() const;

private:
    QString logbuffer;
    QTextStream *eventlog;
    MainWrapper *wrapper;
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

class CentralWrapper : public QWidget
{
    Q_OBJECT
public:
    explicit CentralWrapper(QWidget *parent = 0);
    void setCentralWidget(QWidget *w);
    void enable_stopwatch(bool enable);

signals:
    void timeout();

private:
    QWidget *content;
    Stopwatch *stopwatch;
};

class MainWrapper : public QWidget
{
    Q_OBJECT
public:
    explicit MainWrapper(QTextStream &stream, QWidget *parent = 0);
    void setCentralWidget(QWidget *w);
    void enable_timer(bool enable);

    /* WORK AROUND THESE FUNCTIONS
     * "MainWrapper" class should
     * contain every QWidget of the
     * ToolFrame to reduce overhead
     * functions. */
    void started();
    void running();
    void stopped();
    void error();

signals:
    void resized();
    void start();
    void timeout(); //is this right?

private slots:
    void show_content();
    void show_settings();
    void toggle_eventlog();

private:
    SideBar *sidebar;
    CentralWrapper *content;
    QWidget *settings;
    ToolSettings *tool_settings;
    EventLogger *eventlog;
    //help
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
