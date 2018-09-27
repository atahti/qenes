#ifndef DIALOGDBVIEW_H
#define DIALOGDBVIEW_H

#include <QDialog>
#include "ui_databaseview.h"
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include "data/date.h"

class PersonTableModel;
class FamilyTableModel;
class MyProxyModel;
class FamilyProxyModel;


class DbView : public QDialog, public Ui::dbView
{
    Q_OBJECT

    private:
        int getId();
    public:
        DbView(QWidget* = 0);
        DbView(Date dateLimits, bool maleOnly, bool femaleOnly, QWidget * = 0);
        void setup(Date dateLimits, bool maleOnly, bool femaleOnly);
        PersonTableModel* personModel;
        FamilyTableModel* familyModel;
        MyProxyModel* proxyModel;
        FamilyProxyModel*   familyProxyModel;
        bool searchFirstName, searchFamilyName;
        QList<int>  idList;
        bool acceptRow(int row);        

    private slots:
        void slotSearch(QString);
        void slotUiSearchIsCaseSensitive(bool);
        void slotUiSearchFamilyName();
        void slotUiSearchFirstName();
        void slotSelection();
        void slotGoTo();
        void slotUpdate();
        void slotToBrowseHistory();
        void slotClose();
        void updateBegin();

    signals:
        void sigUpdate(quint16);

};

class PersonTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    PersonTableModel(QObject *parent=0) : QAbstractTableModel(parent) { koe=0; }    
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

class FamilyTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    FamilyTableModel(QObject *parent=0) : QAbstractTableModel(parent) { }
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;        
public slots:
    void slotUpdate();
    void updateBegin();
};

class MyProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    private:
        PersonTableModel* personModel;
        DbView* p;
    public:        
        MyProxyModel(PersonTableModel* pm, DbView *parent=0) : QSortFilterProxyModel(parent) { p = parent ; personModel = pm; }
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;        
};

class FamilyProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    private:
        FamilyTableModel* familyModel;
        DbView* p;
    public:
        FamilyProxyModel(FamilyTableModel* pm, DbView *parent=0) : QSortFilterProxyModel(parent) { p = parent ; familyModel = pm; }
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;        
};

#endif // DIALOGDBVIEW_H
