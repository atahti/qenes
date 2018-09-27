#ifndef DIALOGDBVIEW2_H
#define DIALOGDBVIEW2_H

#include <QDialog>
#include "ui_databaseview2.h"
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include "data/date.h"

class PersonTableModel2;
class MyProxyModel2;

class DbView2 : public QDialog, public Ui::dbView2
{
    Q_OBJECT

    private:
        int getId();
    public:
        DbView2(QWidget* = 0);
        DbView2(Date dateLimits, bool male, bool female, QWidget * = 0);
        //void setup(Date dateLimits, bool maleOnly, bool femaleOnly);
        void setup(Date dateLimits);
        PersonTableModel2* personModel;
        MyProxyModel2* proxyModel;
        QList<int>  idList;
        bool acceptRow(int row);
        bool showMales, showFemales;
        QDate dateMin, dateMax;

    private slots:
        void slotSearch(QString);
        void slotSelection();
        void slotGoTo();
        void slotUpdate();
        void slotClose();        
        void updateBegin();
        void slotDateChanged();

    signals:
        void sigUpdate(quint16);

};

class MyProxyModel2 : public QSortFilterProxyModel
{
    Q_OBJECT

    private:
        PersonTableModel2* personModel;
        DbView2* p;
    public:
        MyProxyModel2(PersonTableModel2* pm, DbView2 *parent=0) : QSortFilterProxyModel(parent) { p = parent ; personModel = pm; }
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

class PersonTableModel2 : public QAbstractTableModel
{
    Q_OBJECT

public:
    PersonTableModel2(QObject *parent=0) : QAbstractTableModel(parent) { koe=0; }
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;    
    QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
private:
    int koe;    
public slots:    
    void slotUpdate();
    void updateBegin();    
};

#endif // DIALOGDBVIEW2_H
