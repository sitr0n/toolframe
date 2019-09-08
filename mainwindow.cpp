#include "mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : ToolFrame(parent)
{
    useEventlog();
    auto cont = new QWidget(this);
    cont->setMinimumSize(QSize(20, 20));
    cont->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    putContent(cont);

    auto sett = new QWidget(this);
    sett->setMinimumSize(QSize(20, 20));
    sett->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    putSettings(sett);

    addField("teSt", [=](){
        qDebug() << "yo";
    });
    addNumberField("Number:", [=](int value){
        qDebug() << QString::number(value);
    });
}
