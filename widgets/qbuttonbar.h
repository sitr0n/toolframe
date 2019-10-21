#ifndef QBUTTONBAR_H
#define QBUTTONBAR_H

#include <QWidget>
#include <QPushButton>
#include <QSpacerItem>
#include <vector>
#include <QVector>
#include <QMap>
#include <QDebug>
#include <QFrame>
#include <QLayout>
#include <functional>

class QButtonBar : public QWidget
{
    Q_OBJECT
public:
    explicit QButtonBar(QWidget *parent = nullptr);
    void setOrientation(Qt::Orientation orientation);
    void setButtonSize(QSize size);
    void addButtonToFront(const QString &name, std::function<void()> process, QIcon icon = QIcon());
    void addButtonToBack(const QString &name, std::function<void()> process, QIcon icon = QIcon());

protected:
    QPushButton* createButton(const QString &name, QIcon icon);
    void addToFront(QWidget *item);
    void addToBack(QWidget *item);
    void resetLayout();
    void newLayout();
    void fillLayout();

    QVector<QWidget*> m_items;

signals:
    void error(const QString &message);

private:
    Qt::Orientation m_orientation;
    QSize m_size;

    const int LAYOUT_MARGIN = 0;
};

#endif // QBUTTONBAR_H
