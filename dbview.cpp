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


#include "dbview.h"
#include <QDebug>
#include "mainwindow.h"
#include <QSettings>
#include "macro.h"

extern  GeneData        * geneData;
extern QSettings        sets;

int DbView::getId()
{
    int id;

    if ( !this->uiTableView->selectionModel()->selection().isEmpty() ) {
        QModelIndex qmi = proxyModel->mapToSource(this->uiTableView->selectionModel()->selection().indexes().at(0));
        id = qmi.row();
    } else id = 0;

    return id;
}

void DbView::slotToBrowseHistory()
{
    int id = getId();
    if (id) { GENE.browseHistory.removeAll(id); GENE.browseHistory.prepend(id); }
}

void DbView::slotGoTo()
{    
    int id = getId();
    this->done(id);
}

void DbView::slotSearch(QString search)
{
    proxyModel->setFilterRegExp(QRegExp(QRegExp(search, Qt::CaseInsensitive, QRegExp::FixedString)));
    this->slotUpdate();
}

void DbView::slotUiSearchFamilyName()
{
    if (uiSearchFamilyName->isChecked()) searchFamilyName = true; else searchFamilyName = false;
    this->slotUpdate();
}

void DbView::slotUiSearchFirstName()
{
    if (uiSearchFirstName->isChecked()) searchFirstName = true; else searchFirstName = false;
    this->slotUpdate();
}

void DbView::slotUpdate()
{    
    personModel->slotUpdate();
    familyModel->slotUpdate();
}

void DbView::slotUiSearchIsCaseSensitive(bool isit)
{    
    if (isit) proxyModel->setFilterCaseSensitivity(Qt::CaseSensitive);
    else proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    this->update();
}

DbView::DbView(Date dateLimits, bool male, bool female, QWidget *parent) : QDialog(parent)
{
    searchFamilyName = true;
    searchFirstName = true;
    setup(dateLimits, male, female);
}

DbView::DbView(QWidget *parent) : QDialog(parent)
{
    Date date;
    date.setDate1(GET_ALLMINDAY);
    date.setDate2(GET_ALLMAXDAY);
    searchFamilyName = true;
    searchFirstName = true;
    setFont(GeneData::panelFont);
    this->setup(date, true, true);
}

bool DbView::acceptRow(int sourceRow)
{
    QString firstNameSearh  = this->uiSearch->text();
    QString familyNameSearh = this->uiSearchFamilyName->text();
    bool    ancestorsOnly   = this->uiAncestorsOnly->isChecked();
    bool    descentantsOnly = this->uiDescentantsOnly->isChecked();
    bool    ancDecOnly      = this->uiAncDec->isChecked();

    if (INDI(sourceRow).deleted) return false;

    if (ancestorsOnly || descentantsOnly || ancDecOnly) {
        if (!INDI(sourceRow).selected) return false;
    }

    bool firstNameMatch =     INDI(sourceRow).nameFirst.contains(firstNameSearh)
                          || (INDI(sourceRow).name2nd.contains(firstNameSearh))
                          || (INDI(sourceRow).name3rd.contains(firstNameSearh));

    bool familyNameMatch = INDI(sourceRow).nameFamily.contains(firstNameSearh);

    if (firstNameSearh == "" && searchFirstName) firstNameMatch = true;
    if (familyNameSearh == "" && searchFamilyName) familyNameMatch = true;

    if (!uiFemale->isChecked() && INDI(sourceRow).sex == FEMALE) return false;
    if (!uiMale->isChecked() && INDI(sourceRow).sex == MALE) return false;
    if ( uiTwins->isChecked() && !(GENE.twinAlfa.contains(sourceRow) || GENE.twinBeta.contains(sourceRow))) return false;

    if ( !( ( INDI(sourceRow).estimatedBirth.day1 >= uiDateMin->date() && INDI(sourceRow).estimatedBirth.day1 <= uiDateMax->date() )
       || ( INDI(sourceRow).estimatedBirth.day2 >= uiDateMin->date() && INDI(sourceRow).estimatedBirth.day2 <= uiDateMax->date() ) ) )
        return false;

    if ( ( searchFamilyName) && ( searchFirstName ) ) return (familyNameMatch || firstNameMatch);
    if ( ( searchFamilyName) && (!searchFirstName ) ) return familyNameMatch;
    if ( ( searchFirstName ) && (!searchFamilyName) ) return firstNameMatch;
    if ( (!searchFamilyName) && (!searchFirstName ) ) return false;
    return false;
}

void DbView::slotClose()
{
    INDI(GENE.currentId).selectNone();
    close();
}

void DbView::updateBegin()
{
    this->familyModel->updateBegin();
    this->personModel->updateBegin();
}

void DbView::slotSelection()
{
    INDI(GENE.currentId).selectNone();

    if (uiAncDec->isChecked())              INDI(GENE.currentId).selectAncestorsAll();
    if (uiDescentantsOnly->isChecked())     INDI(GENE.currentId).selectDescendants();
    if (uiAncestorsOnly->isChecked())       INDI(GENE.currentId).selectAncestors();
}

void DbView::setup(Date dateLimits, bool male, bool female)
{
    setupUi(this);

    uiPersonCount->setText(QString::number(GENE.indiCount));
    uiPersonLastID->setText(QString::number(GENE.indiLastUsed));
    uiFamilyCount->setText(QString::number(GENE.famiCount));
    uiFamilyLastID->setText(QString::number(GENE.famiLastUsed));

    if (male) uiMale->setChecked(true); else uiMale->setChecked(false);
    if (female) uiFemale->setChecked(true); else uiFemale->setChecked(false);

    connect(uiExit, SIGNAL(clicked()), this, SLOT(slotClose()));

 // create a person view model and install it
    personModel = new PersonTableModel(this);
    proxyModel = new MyProxyModel(personModel, this);
    proxyModel->setSourceModel(personModel);
    uiTableView->setModel(proxyModel);

    uiTableView->setColumnWidth(0, 80); // familyname
    uiTableView->setColumnWidth(1, 100); // first name
    uiTableView->setColumnWidth(2, 200); // birthday

    for (quint16 i=1 ; i<=GENE.indiLastUsed+1 ; i++) idList.append(i);

    proxyModel->setFilterKeyColumn(0);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

// Create a family view model and install it
    familyModel = new FamilyTableModel();
    familyProxyModel = new FamilyProxyModel(familyModel, this);
    familyProxyModel->setSourceModel(familyModel);
    uiFamilyView->setModel(familyProxyModel);

    uiFamilyView->setColumnWidth(0, 100); // husb
    uiFamilyView->setColumnWidth(1, 100); // wife
    uiFamilyView->setColumnWidth(2, 70); // marr day
// connect signals & slots
    connect(uiSearch, SIGNAL(textChanged(QString)), this, SLOT(updateBegin()));
    connect(uiSearch, SIGNAL(textChanged(QString)), this, SLOT(slotSearch(QString)));

    connect(uiSearchIsCaseSensitive, SIGNAL(toggled(bool)), this, SLOT(updateBegin()));
    connect(uiSearchIsCaseSensitive, SIGNAL(toggled(bool)), this, SLOT(slotUiSearchIsCaseSensitive(bool)));

    connect(uiSearchFamilyName, SIGNAL(toggled(bool)), this, SLOT(updateBegin()));
    connect(uiSearchFamilyName, SIGNAL(toggled(bool)), this, SLOT(slotUiSearchFamilyName()));

    connect(uiSearchFirstName, SIGNAL(toggled(bool)), this, SLOT(updateBegin()));
    connect(uiSearchFirstName, SIGNAL(toggled(bool)), this, SLOT(slotUiSearchFirstName()));

    connect(uiGoTo, SIGNAL(clicked()), this, SLOT(slotGoTo()));           

    connect(uiMale, SIGNAL(clicked()), this, SLOT(updateBegin()));
    connect(uiMale, SIGNAL(clicked()), this, SLOT(slotUpdate()));

    connect(uiFemale, SIGNAL(clicked()), this, SLOT(updateBegin()));
    connect(uiFemale, SIGNAL(clicked()), this, SLOT(slotUpdate()));

    connect(uiTwins, SIGNAL(clicked()), this, SLOT(updateBegin()));
    connect(uiTwins, SIGNAL(clicked()), this, SLOT(slotUpdate()));


    connect(uiDateMin, SIGNAL(dateChanged(QDate)), this, SLOT(updateBegin()));
    connect(uiDateMin, SIGNAL(dateChanged(QDate)), this, SLOT(slotUpdate()));

    connect(uiDateMax, SIGNAL(dateChanged(QDate)), this, SLOT(updateBegin()));
    connect(uiDateMax, SIGNAL(dateChanged(QDate)), this, SLOT(slotUpdate()));

    connect(uiToBrowseHistory, SIGNAL(clicked()), this, SLOT(slotToBrowseHistory()));

    connect(uiDescentantsOnly, SIGNAL(toggled(bool)), this, SLOT(updateBegin()));
    connect(uiDescentantsOnly, SIGNAL(toggled(bool)), this, SLOT(slotSelection()));
    connect(uiDescentantsOnly, SIGNAL(toggled(bool)), this, SLOT(slotUpdate()));

    connect(uiAncestorsOnly, SIGNAL(toggled(bool)), this, SLOT(updateBegin()));
    connect(uiAncestorsOnly, SIGNAL(toggled(bool)), this, SLOT(slotSelection()));
    connect(uiAncestorsOnly, SIGNAL(toggled(bool)), this, SLOT(slotUpdate()));

    connect(uiAncDec, SIGNAL(toggled(bool)), this, SLOT(updateBegin()));
    connect(uiAncDec, SIGNAL(toggled(bool)), this, SLOT(slotSelection()));
    connect(uiAncDec, SIGNAL(toggled(bool)), this, SLOT(slotUpdate()));

// init check boxes
//    if (searchFamilyName) uiSearchFamilyName->setChecked(true); else uiSearchFamilyName->setChecked(false);
//    if (searchFirstName) uiSearchFirstName->setChecked(true); else uiSearchFirstName->setChecked(false);

    uiDateMin->setMinimumDate(GET_ALLMINDAY);
    uiDateMin->setMaximumDate(GET_ALLMAXDAY);
    uiDateMax->setMinimumDate(GET_ALLMINDAY);
    uiDateMax->setMaximumDate(GET_ALLMAXDAY);
    uiDateMin->setDate(dateLimits.day1);
    uiDateMax->setDate(dateLimits.day2);

    slotUpdate(); //tämä jossain tapauksessa kaataa softan eikä näköjään edes tarvita, miksi tää oli tässä?

}

// -----------------------------------------------------------------------------------------------------------

int PersonTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    const int tmp = GENE.indiLastUsed + 1;

    return tmp;
}

int PersonTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant PersonTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    QVariant result;

    result = QVariant();

    if (index.isValid()) {
        if ( role == Qt::BackgroundColorRole ) {
            if (INDI(row).sex == FEMALE ) {
                //if (INDI(row).selected) result = ( QColor(255, 240, 240)); else
                result = ( QColor(255, 199, 199) );
            }
            if (INDI(row).sex == MALE ) {
                // if (INDI(row).selected) result = ( QColor(240, 240, 255)); else
                result = ( QColor(199, 199, 225) );
            }
            if (INDI(row).sex == UNKNOWN) {
                // if (INDI(row).selected) result = ( QColor(240, 240, 240)); else
                result = ( QColor(180,180,180));// Qt::gray );
            }
        }
        if ( role == Qt::TextAlignmentRole ) { result =  ( Qt::AlignLeft ) ;  }
        if ( role == Qt::DisplayRole ) {
            if (index.column() == 0) result =  INDI(row).nameFamily;
            if (index.column() == 1) result =  INDI(row).nameFirst;
            if (index.column() == 2) result =  INDI(row).estimatedBirth.toString(true);

        }
    }

    return result;
}

bool PersonTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
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

void PersonTableModel::updateBegin()
{
    emit(this->layoutAboutToBeChanged());
}

void PersonTableModel::slotUpdate()
{
    this->layoutChanged();
}

QModelIndex PersonTableModel::index( int row, int column, const QModelIndex & parent ) const
{
    Q_UNUSED(parent);
    return this->createIndex(row, column, row);
}

Qt::ItemFlags PersonTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
 }

QVariant PersonTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

 // -----------------------------------------------------------------------------------------------------------

void FamilyTableModel::updateBegin()
{
    emit(this->layoutAboutToBeChanged());
}

int FamilyTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    const int tmp = GENE.famiLastUsed + 1;

    return tmp;
}

int FamilyTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant FamilyTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    QVariant result = QVariant();

    if (index.isValid()) {
        if ( role == Qt::TextAlignmentRole ) { result = ( Qt::AlignLeft ) ;  }
        if ( role == Qt::DisplayRole ) {
            if (index.column() == 0) result = INDI(FAM(row).husb).nameFamily + " " + INDI(FAM(row).husb).nameFirst;
            if (index.column() == 1) result = INDI(FAM(row).wife).nameFamily + " " + INDI(FAM(row).wife).nameFirst;
            if (index.column() == 2) {
                Entry marr = FAM(row).fentry(MARRIAGE);
                if (marr.day.known) result = marr.day.toString(true);
                else result = "Not known";
            }

        }
    }

    return result;
}

bool FamilyTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);

    return false;
}

Qt::ItemFlags FamilyTableModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled;
}

QVariant FamilyTableModel::headerData(int section, Qt::Orientation orientation, int role) const
 {
     if (role != Qt::DisplayRole) return QVariant();

     if (orientation == Qt::Horizontal) {
         switch (section) {
            case 0: return "Husb";
            case 1: return "Wife";
            case 2: return "Marr day";

             default:
                 return QVariant();
         }
     }
     if (orientation == Qt::Vertical) {
         return section;
     }
     return QVariant();
 }

QModelIndex FamilyTableModel::index( int row, int column, const QModelIndex & parent ) const
{
    Q_UNUSED(parent);

    return this->createIndex(row, column, row);
}

void FamilyTableModel::slotUpdate()
{       
    this->layoutChanged();
}

// --------------------------------------------------------------------------------------------------------

bool MyProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    // sourcerow equals to id-number of the individual

    return p->acceptRow(sourceRow);
}

bool FamilyProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{    
    Q_UNUSED(sourceParent);

    return (!FAM(sourceRow).deleted && (p->acceptRow(FAM(sourceRow).wife) || p->acceptRow(FAM(sourceRow).husb)));

}
