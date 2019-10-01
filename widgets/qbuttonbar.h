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

class QButtonBar : public QWidget
{
    Q_OBJECT
public:
    explicit QButtonBar(QWidget *parent = nullptr);
    void setOrientation(Qt::Orientation orientation);
    template <typename Proc>
    void addExclusiveButton(const QString &name, Proc process, QIcon icon = QIcon()) {
        auto button = createButton(name, icon);
        layout()->addWidget(button);
        m_dependentButtons.insert(name, button);
        if (m_dependentButtons.size() == 1) {
            highlight(name);
        }

        connect(button, &QPushButton::clicked, this, [this, name, process](){
            if (QString::compare(name, m_highlighted) != 0) {
                highlight(name);
                process();
            }
        });
    }
    template <typename Proc>
    void addButton(const QString &name, Proc process, QIcon icon = QIcon()) {
        auto button = createButton(name, icon);
        layout()->addWidget(button);

        connect(button, &QPushButton::clicked, this, [process](){
            process();
        });
    }

    void addSpacer() {
        auto spacer = new QSpacerItem(0,0, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        layout()->addItem(spacer);
        //m_items.push_back(spacer);
    }

protected:
    QPushButton* createButton(const QString &name, QIcon icon);
    void fillLayout();
    void highlight(const QString &button);
    void toggle(const QString &button);

signals:
    void error(const QString &message);

private:
    Qt::Orientation m_orientation;
    QString m_highlighted;
    QFrame m_frame;
    QMap<QString, QWidget*> m_dependentButtons;
    QVector<QWidget*> m_items;

    const int BTN_SIZE = 45;
    const QString ACTIVE_BUTTONSTYLE = "background-color: #4EDCFF;";
    const QString INACTIVE_BUTTONSTYLE = "";

};

#endif // QBUTTONBAR_H
