#ifndef DIALOGSOURCEVIEW_H
#define DIALOGSOURCEVIEW_H

#include <QDialog>
#include "ui_sourceview.h"
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QCheckBox>
#include <QSignalMapper>

class SourceTableModel;
class SourceProxyModel;

class Sourceview : public QDialog, public Ui::sourceview
{

    Q_OBJECT

    private:
        int         getId();        
        QCheckBox       * uiDelete;
        QSignalMapper   * deleteMapper;

    public:
        Sourceview(QWidget* = 0);
        SourceTableModel* personModel;
        SourceProxyModel* proxyModel;
        bool acceptRow(int row);
        bool showMales, showFemales;

    private slots:
        void slotSelection();
        void slotUpdate();
        void slotClose();        
        void updateBegin();
        void slotAddNew();
        void slotDelete(int id);

    signals:
        void sigUpdate(quint16);

};


class SourceProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    private:
        SourceTableModel* personModel;
        Sourceview* p;
    public:
        SourceProxyModel(SourceTableModel* pm, Sourceview *parent=0) : QSortFilterProxyModel(parent) { p = parent ; personModel = pm; }
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};


class SourceTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    SourceTableModel(QObject *parent=0) : QAbstractTableModel(parent) { koe=0; }
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

#endif // DIALOGSOURCEVIEW_H
