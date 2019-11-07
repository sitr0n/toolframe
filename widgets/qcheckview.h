#pragma once

#include <QtWidgets/QListView>
#include <QtWidgets/QCheckBox>
#include <QtGui/QStandardItem>
#include <QMap>
#include <vector>
#include <queue>

class QCheckView : public QListView
{
    Q_OBJECT
public:
    explicit QCheckView(QWidget *parent = nullptr);
    ~QCheckView();
    void addRow(int id, const QString &name);
    QStandardItem* getRow(int id);
    void toggleRows();
    std::vector<int> checkedItems();
    bool isEmpty() const;
    void clear();

signals:
    void selected(const int id);

protected:
    QStandardItem* newRow(const QString &name);

private:
    std::map<int, QStandardItem*> m_checkBoxes;
    QMap<int, QStandardItem*> m_rows;
    std::vector<int> m_identifiers;

};
