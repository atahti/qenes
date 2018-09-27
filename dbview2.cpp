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


#include "dbview2.h"
#include <QDebug>
#include "mainwindow.h"
#include <QSettings>
#include "macro.h"

extern GeneData        * geneData;
extern QSettings        sets;

int DbView2::getId()
{
    int id;

    if ( !this->uiTableView2->selectionModel()->selection().isEmpty() ) {
        QModelIndex qmi = proxyModel->mapToSource(this->uiTableView2->selectionModel()->selection().indexes().at(0));
        id = qmi.row();
    } else id = 0;

    return id;
}

void DbView2::slotGoTo()
{    
    int id = getId();
    this->done(id);
}

void DbView2::slotSearch(QString search)
{
    proxyModel->setFilterRegExp(QRegExp(QRegExp(search, Qt::CaseInsensitive, QRegExp::FixedString)));
    this->slotUpdate();
}

void DbView2::slotUpdate()
{    
    personModel->slotUpdate();
}

DbView2::DbView2(Date dateLimits, bool male, bool female, QWidget *parent) : QDialog(parent)
{
    showFemales = female;
    showMales = male;
    setFont(GeneData::panelFont);
    setup(dateLimits);
    //setup(dateLimits, male, female);
}

DbView2::DbView2(QWidget *parent) : QDialog(parent)
{
    Date date;
    date.setDate1(GET_ALLMINDAY);
    date.setDate2(GET_ALLMAXDAY);
    this->setup(date);
    //this->setup(date, true, true);
}

bool DbView2::acceptRow(int sourceRow)
{
    QString nameSearch  = this->uiSearch2->text();

    if (INDI(sourceRow).deleted) return false;

    if (!showFemales && INDI(sourceRow).sex == FEMALE ) return false;
    if (!showMales && INDI(sourceRow).sex == MALE ) return false;

    if ( !( ( INDI(sourceRow).estimatedBirth.day1 >= dateMin && INDI(sourceRow).estimatedBirth.day1 <= dateMax )
       || ( INDI(sourceRow).estimatedBirth.day2 >= dateMin && INDI(sourceRow).estimatedBirth.day2 <= dateMax ) ) )
        return false;

    if (nameSearch == "" ) return true;

    if (     INDI(sourceRow).nameFirst.contains(nameSearch)
         || (INDI(sourceRow).name2nd.contains(nameSearch))
         || (INDI(sourceRow).name3rd.contains(nameSearch))
         || (INDI(sourceRow).nameFamily.contains(nameSearch)) ) return true;

    return false;
}

void DbView2::slotClose()
{
    INDI(GENE.currentId).selectNone();
    close();
}

void DbView2::updateBegin()
{
    this->personModel->updateBegin();    
}

void DbView2::slotSelection()
{
    INDI(GENE.currentId).selectNone();
}

void DbView2::slotDateChanged()
{
    dateMin = uiDateMin2->date();
    dateMax = uiDateMax2->date();

    slotUpdate();
}

void DbView2::setup(Date dateLimits)
{
    setupUi(this);

    connect(uiExit2, SIGNAL(clicked()), this, SLOT(slotClose()));

 // create a person view model and install it
    personModel = new PersonTableModel2(this);
    proxyModel = new MyProxyModel2(personModel, this);
    proxyModel->setSourceModel(personModel);
    uiTableView2->setModel(proxyModel);

    uiTableView2->setColumnWidth(0, 80); // familyname
    uiTableView2->setColumnWidth(1, 100); // first name
    uiTableView2->setColumnWidth(2, 200); // birthday

    for (quint16 i=1 ; i<=GENE.indiLastUsed+1 ; i++) idList.append(i);

    proxyModel->setFilterKeyColumn(0);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

// connect signals & slots
    connect(uiSearch2, SIGNAL(textChanged(QString)), this, SLOT(updateBegin()));
    connect(uiSearch2, SIGNAL(textChanged(QString)), this, SLOT(slotSearch(QString)));

    connect(uiGoTo2, SIGNAL(clicked()), this, SLOT(slotGoTo()));

    connect(uiDateMin2, SIGNAL(dateChanged(QDate)), this, SLOT(updateBegin()));
    connect(uiDateMin2, SIGNAL(dateChanged(QDate)), this, SLOT(slotDateChanged()));

    connect(uiDateMax2, SIGNAL(dateChanged(QDate)), this, SLOT(updateBegin()));
    connect(uiDateMax2, SIGNAL(dateChanged(QDate)), this, SLOT(slotDateChanged()));


    uiDateMin2->setMinimumDate(GET_ALLMINDAY);
    uiDateMin2->setMaximumDate(GET_ALLMAXDAY);
    uiDateMax2->setMinimumDate(GET_ALLMINDAY);
    uiDateMax2->setMaximumDate(GET_ALLMAXDAY);
    uiDateMin2->setDate(dateLimits.day1);
    uiDateMax2->setDate(dateLimits.day2);

    slotUpdate(); //tämä jossain tapauksessa kaataa softan eikä näköjään edes tarvita, miksi tää oli tässä?

}

// -----------------------------------------------------------------------------------------------------------

int PersonTableModel2::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    const int tmp = GENE.indiLastUsed + 1;

    return tmp;
}

int PersonTableModel2::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant PersonTableModel2::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    QVariant result;

    result = QVariant();

    if (index.isValid()) {
        if ( role == Qt::BackgroundColorRole ) {
            if (INDI(row).sex == FEMALE ) {
                if (INDI(row).selected) result = ( QColor(255, 240, 240));
                else result = ( QColor(255, 199, 199) );
            }
            if (INDI(row).sex == MALE ) {
                if (INDI(row).selected) result = ( QColor(240, 240, 255));
                else result = ( QColor(199, 199, 225) );
            }
            if (INDI(row).sex == UNKNOWN) {
                if (INDI(row).selected) result = ( QColor(240, 240, 240));
                else result = ( QColor(180, 180, 180) );
            }
        }
        if ( role == Qt::TextAlignmentRole ) { result =  ( Qt::AlignLeft ) ;  }
        if ( role == Qt::DisplayRole ) {
            if (index.column() == 0) result = INDI(row).nameFamily;
            if (index.column() == 1) result =  INDI(row).nameFirst;
            if (index.column() == 2) result =  INDI(row).estimatedBirth.toString(true);

        }
    }

    return result;
}

bool PersonTableModel2::setData(const QModelIndex &index, const QVariant &value, int role)
{
     if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();
        int column = index.column();
        if ( column == 0 ) INDI(row).nameFamily = value.toString();
        if ( column == 1 ) INDI(row).nameFirst = value.toString();

        emit(dataChanged(index, index));
        return true;
    }
     return false;
}

void PersonTableModel2::updateBegin()
{
    emit(this->layoutAboutToBeChanged());
}

void PersonTableModel2::slotUpdate()
{
    this->layoutChanged();
}

QModelIndex PersonTableModel2::index( int row, int column, const QModelIndex & parent ) const
{
    Q_UNUSED(parent);
    return this->createIndex(row, column, row);
}

Qt::ItemFlags PersonTableModel2::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
 }

QVariant PersonTableModel2::headerData(int section, Qt::Orientation orientation, int role) const
 {
     if (role != Qt::DisplayRole) return QVariant();

     if (orientation == Qt::Horizontal) {
         switch (section) {
             case 0: return "Family Name";
             case 1: return "First Name";
             case 2: return "Birth Day";

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

bool MyProxyModel2::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    // sourcerow equals to id-number of the individual

    return p->acceptRow(sourceRow);
}

