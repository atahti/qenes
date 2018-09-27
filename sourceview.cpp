/*
 *
 *  qenes genealogy software
 *
 *  Copyright (C) 2009-2010 Ari Tähti
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License veasion 2 as
 *  published by the Free Software Foundation.
 */


#include "sourceview.h"
#include <QDebug>
#include "mainwindow.h"
#include <QSettings>
#include "macro.h"

extern GeneData        * geneData;
extern QSettings        sets;

void Sourceview::slotDelete(int id)
{
    QCheckBox * qcb;
    qcb = (QCheckBox *)(uiTableView2->indexWidget(uiTableView2->model()->index(id, 11)));
    //qDebug() << "works" << id << qcb->isChecked();

    bool toBeDeleted = !qcb->isChecked();

    int x;
    bool indi;

    if (toBeDeleted) {
        if ( GENE.sour[id+1].usedBy(&x, &indi) ) {
            QString msg = "This source is in use by ";
            if (indi) msg += "person "; else msg += "family ";
            msg += QString::number(x);

            QMessageBox msgBox;
            msgBox.setText(msg);
            msgBox.setInformativeText("Delete that source data.");
            msgBox.setStandardButtons(QMessageBox::Ok );

            msgBox.exec();
        } else {
            GENE.sour[id+1].aboutToBeDeleted = toBeDeleted;
        }
    } else GENE.sour[id+1].aboutToBeDeleted = toBeDeleted;



    this->personModel->updateBegin();   // emit(this->layoutAboutToBeChanged());
    this->personModel->slotUpdate();    // this->layoutChanged();
}

void Sourceview::slotAddNew()
{
    int sourId = GENE.sourLastUsed + 1;
    GENE.sourSpace(sourId);
    SOUR(sourId).title = "new";
    SOUR(sourId).id = sourId;

    this->personModel->updateBegin();   // emit(this->layoutAboutToBeChanged());
    this->personModel->slotUpdate();    // this->layoutChanged();

    uiDelete = new QCheckBox();
    uiTableView2->setIndexWidget(uiTableView2->model()->index(sourId-1, 11), uiDelete);
}

void Sourceview::slotUpdate()
{
    personModel->slotUpdate();
}

void Sourceview::updateBegin()
{
    personModel->updateBegin();
}

Sourceview::Sourceview(QWidget *parent) : QDialog(parent)
{
    setupUi(this);

    connect(uiExit2, SIGNAL(clicked()), this, SLOT(slotClose()));

 // create a person view model and install it
    personModel = new SourceTableModel(this);
    proxyModel = new SourceProxyModel(personModel, this);
    proxyModel->setSourceModel(personModel);
    uiTableView2->setModel(proxyModel);

    /*
    uiTableView2->setColumnWidth(0, 80); // familyname
    uiTableView2->setColumnWidth(1, 100); // first name
    uiTableView2->setColumnWidth(2, 200); // birthday
    */

    connect(uiAdd, SIGNAL(pressed()), this, SLOT(slotAddNew()));

    slotUpdate(); //tämä jossain tapauksessa kaataa softan eikä näköjään edes tarvita, miksi tää oli tässä?

    deleteMapper = new QSignalMapper(this);

    for (quint16 row = 0 ; row<GENE.sourLastUsed ; row++) {
        uiDelete = new QCheckBox();
        connect(uiDelete, SIGNAL(pressed()), deleteMapper, SLOT(map()));
        deleteMapper->setMapping(uiDelete, row);
        uiTableView2->setIndexWidget(uiTableView2->model()->index(row, 11), uiDelete );
    }

    connect(deleteMapper, SIGNAL(mapped(int)), this, SLOT(slotDelete(int)));
}

bool Sourceview::acceptRow(int sourceRow)
{
    Q_UNUSED(sourceRow);

    if (SOUR(sourceRow).used() && sourceRow && !SOUR(sourceRow).deleted ) return true; else return false;
}

void Sourceview::slotClose()
{
    for (quint16 i=0 ; i<GENE.sourLastUsed ; i++) SOUR(i).deleted = SOUR(i).aboutToBeDeleted;
    reject();
}

void Sourceview::slotSelection()
{
    INDI(GENE.currentId).selectNone();
}


// -----------------------------------------------------------------------------------------------------------

int SourceTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    const int tmp = GENE.sourLastUsed + 1;

    return tmp;
}

int SourceTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 12;
}

QVariant SourceTableModel::data(const QModelIndex &index, int role) const
{    
    int row = index.row();

    QVariant result;

    result = QVariant();
    //qDebug() << "data " << index.row();
    if (index.isValid()) {
        if ( role == Qt::BackgroundColorRole ) {

            if (row & 1) result = ( QColor(210, 210, 210)); else result = ( QColor(235, 235, 235) );
            if ( SOUR(row).aboutToBeDeleted ) result = QColor(255, 50, 50);
        }        

        if ( role == Qt::TextAlignmentRole ) { result =  ( Qt::AlignLeft ) ; }

        if ( role == Qt::DisplayRole ) {

            if ( row > 0 ) {                
                if (index.column() == 0) result = SOUR(row).title;
                if (index.column() == 1) result = SOUR(row).author;
                if (index.column() == 2) result = SOUR(row).quay;
                if (index.column() == 3) result = SOUR(row).publication;
                if (index.column() == 4) result = SOUR(row).changed;
                if (index.column() == 5) result = SOUR(row).note.text;
                if (index.column() == 6) result = SOUR(row).page;
                if (index.column() == 7) result = SOUR(row).name;
                if (index.column() == 8) result = SOUR(row).copyright;
                if (index.column() == 9) result = SOUR(row).text;
                if (index.column() == 10) result = SOUR(row).phone;
            }
        }
    }
/*
        Address         address;
        QString         version;
        QString         corporation;
        QString         productName;
        QString         phone;
        QDateTime       changed;
        */
    return result;
}

bool SourceTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
     if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();
        int column = index.column();
        if ( column == 0 ) SOUR(row).title     = value.toString();
        if ( column == 1 ) SOUR(row).author    = value.toString();
        //if ( column == 2 ) SOUR(row).data.quay      = value.toString();
        if ( column == 3 ) SOUR(row).publication= value.toString();
        //if ( column == 4 ) SOUR(row).data.changed   = value.toString();
        if ( column == 5 ) SOUR(row).note.text = value.toString();
        if ( column == 6 ) SOUR(row).page      = value.toString();
        if ( column == 7 ) SOUR(row).name      = value.toString();
        if ( column == 8 ) SOUR(row).copyright = value.toString();
        if ( column == 9 ) SOUR(row).text      = value.toString();
        if ( column == 10 ) SOUR(row).phone    = value.toString();


        emit(dataChanged(index, index));
        return true;
    }

    return false;
}

void SourceTableModel::updateBegin()
{
    emit(this->layoutAboutToBeChanged());
}


void SourceTableModel::slotUpdate()
{    
    this->layoutChanged();
}


QModelIndex SourceTableModel::index( int row, int column, const QModelIndex & parent ) const
{
    Q_UNUSED(parent);

    return this->createIndex(row, column, row);
}

Qt::ItemFlags SourceTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
 }

QVariant SourceTableModel::headerData(int section, Qt::Orientation orientation, int role) const
 {
     if (role != Qt::DisplayRole) return QVariant();

     if (orientation == Qt::Horizontal) {
         switch (section) {
            case 0: return "Title";
            case 1: return "Author";
            case 2: return "Quality";
            case 3: return "Publication";
            case 4: return "Changed";
            case 5: return "Note";
            case 6: return "Page";
            case 7: return "Name";
            case 8: return "Copyright";
            case 9: return "Text";
            case 10: return "Phone";
            case 11: return "Delete";

            default:
                 return QVariant();
         }
     }
     if (orientation == Qt::Vertical) {
         return section;
     }
     return QVariant();
 }

// --------------------------------------------------------------------------------------------------------


bool SourceProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    // sourcerow equals to id-number of the individual

    return p->acceptRow(sourceRow);
}


