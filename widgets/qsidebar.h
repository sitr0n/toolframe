#pragma once
#include "qbuttonbar.h"
#include "qselectionbar.h"
class QSideBar : public QWidget
{
public:
    explicit QSideBar(QWidget *parent = nullptr);
    void addApplication(const QString &name, std::function<void()> process, QIcon icon = QIcon());
    void addTool(const QString &name, std::function<void()> process, QIcon icon = QIcon());

protected:
    QFrame *createSeparator();

private:
    QSelectionBar m_applicationBar;
    QButtonBar m_toolBar;

    const int BUTTON_SIZE = 45;
};
