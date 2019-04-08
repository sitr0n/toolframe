#ifndef TOOLFRAME_H
#define TOOLFRAME_H

#include <QMdiSubWindow>
#include <QWidget>
#include <QPlainTextEdit>
#include <QTextStream>
#include "stopwatch.h"
#include <QTimer>
#include <QPushButton>
#include <QSpacerItem>
#include "statusbitwidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QSettings>

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
    void request_settings();

private slots:
    void show_content();
    void show_settings();
    void toggle_eventlog();
    void load_settings();

protected:
    //void setTitle(QString title);
    QTextStream &output() const;

private:
    QString logbuffer;
    QTextStream *eventlog;

    SideBar *m_sidebar;
    QWidget *m_content;
    Stopwatch *m_timer;
    QWidget *m_settings;
    QLabel *m_settings_header;
    ToolSettings *m_toolsettings;
    EventLogger *m_eventlog; // EventLogger ptr instead??
    QWidget *m_wrapper;


    QString context();

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

class ToolSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ToolSettings(QWidget *parent = 0);
    void setContext(QString context);
    QSettings *store() const;

signals:
    void first_time_setup();
    void update_frame();

private slots:
    void update_form();
    void set_timeout();

private:
    QString m_context;

    QFrame *timeout_line;
    QCheckBox *timeout_checker;
    QLineEdit *timeout_edit;
    QFrame *graph_line;
    QCheckBox *graph_checker;
    QLineEdit *graph_sample_edit;
    QFrame *eventlog_line;
    QCheckBox *eventlog_checker;
    QLineEdit *eventlog_edit;

    QSettings *m_store;

    void loadSettings();
    void saveSettings();
    void connectWidgets();
};

#endif // TOOLFRAME_H
