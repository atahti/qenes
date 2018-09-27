#ifndef SEARCHANDHISTORY_H
#define SEARCHANDHISTORY_H

#include <QDialog>
#include "ui_SearchAndHistory.h"

class BrowseHistoryModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        BrowseHistoryModel(QObject *parent=0) : QAbstractTableModel(parent) {  }
        int             rowCount(const QModelIndex &parent) const;
        int             columnCount(const QModelIndex &parent) const;
        QVariant        data(const QModelIndex &index, int role) const;
        Qt::ItemFlags   flags(const QModelIndex &index) const;
        void            updt();
        QList<quint16>      * personList;
};

class SearchAndHistory : public QFrame, Ui::SearchAndHistory
{
    Q_OBJECT

private:
    QPalette        p;

public:
    SearchAndHistory(QFrame *parent = 0);
    void            updt();
    BrowseHistoryModel * browseModel;

private slots:
    void            slot1st();
    void            slotSearch();
    void            slotClear();
    void            slotMotherinlaw();
    void            browseHistoryClicked(QModelIndex);

signals:
    void update(quint16);
};

#endif // SEARCHANDHISTORY_H
