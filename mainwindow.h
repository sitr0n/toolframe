#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "widgets/toolframe.h"

class MainWindow : public ToolFrame
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
};

#endif // MAINWINDOW_H
