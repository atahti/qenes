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


#include "dialogdbview.h"
#include <QDebug>
#include "mainwindow.h"
#include <QSettings>
#include "macro.h"

extern PersonData * pd;
extern FamilyData * fd;
extern GeneData        gd;

extern QSettings        sets;
extern QList<int>       browseHistory;

int DialogDbView::getId()
{
    int id;

    if ( !this->uiTableView->selectionModel()->selection().isEmpty() ) {
        QModelIndex qmi = proxyModel->mapToSource(this->uiTableView->selectionModel()->selection().indexes().at(0));
        id = qmi.row();
    } else id = 0;

    return id;
}

void DialogDbView::slotToBrowseHistory()
{
    int id = getId();
    if (id) { browseHistory.removeAll(id); browseHistory.prepend(id); }    
}

void DialogDbView::slotGoTo()
{    
    int id = getId();
    this->done(id);
}

void DialogDbView::slotSearch(QString search)
{
    proxyModel->setFilterRegExp(QRegExp(QRegExp(search, Qt::CaseInsensitive, QRegExp::FixedString)));    
    familyModel->slotUpdate();
}

void DialogDbView::slotUiSearchFamilyName()
{
    if (uiSearchFamilyName->isChecked()) searchFamilyName = true; else searchFamilyName = false;
    personModel->slotUpdate();
    familyModel->slotUpdate();
}

void DialogDbView::slotUiSearchFirstName()
{
    if (uiSearchFirstName->isChecked()) searchFirstName = true; else searchFirstName = false;
    personModel->slotUpdate();
    familyModel->slotUpdate();
}

void DialogDbView::slotUpdate()
{
    qDebug() << "q";
    personModel->slotUpdate();
    familyModel->slotUpdate();
}

void DialogDbView::slotUiSearchIsCaseSensitive(bool isit)
{    
    if (isit) proxyModel->setFilterCaseSensitivity(Qt::CaseSensitive);
    else proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    personModel->slotUpdate();
    familyModel->slotUpdate();
}

DialogDbView::DialogDbView(Date dateLimits, bool male, bool female, QWidget *parent) : QDialog(parent)
{
    this->setup(dateLimits, male, female);
}

DialogDbView::DialogDbView(QWidget *parent) : QDialog(parent)
{
    Date date;
    date.setDate1(GET_ALLMINDAY);
    date.setDate2(GET_ALLMAXDAY);
    this->setup(date, true, true);
}

bool DialogDbView::acceptRow(int sourceRow)
{
    QString firstNameSearh  = this->uiSearch->text();
    QString familyNameSearh = this->uiSearchFamilyName->text();
    bool    ancestorsOnly   = this->uiAncestorsOnly->isChecked();
    bool    descentantsOnly = this->uiDescentantsOnly->isChecked();

    return true;

    if (pd[sourceRow].deleted) return false;

    bool firstNameMatch =     pd[sourceRow].nameFirst.contains(firstNameSearh)
                          || (pd[sourceRow].name2nd.contains(firstNameSearh))
                          || (pd[sourceRow].name3rd.contains(firstNameSearh));

    bool familyNameMatch = pd[sourceRow].nameFamily.contains(firstNameSearh);

    if (ancestorsOnly && !ancestors[sourceRow]) return false;
    if (descentantsOnly && !descentants[sourceRow]) return false;

    if (firstNameSearh == "" && searchFirstName) firstNameMatch = true;
    if (familyNameSearh == "" && searchFamilyName) familyNameMatch = true;

    if (!uiFemale->isChecked() && pd[sourceRow].sex == FEMALE) return false;
    if (!uiMale->isChecked() && pd[sourceRow].sex == MALE) return false;

    if ( !( ( pd[sourceRow].estimatedBirth.day1 >= uiDateMin->date() && pd[sourceRow].estimatedBirth.day1 <= uiDateMax->date() )
       || ( pd[sourceRow].estimatedBirth.day2 >= uiDateMin->date() && pd[sourceRow].estimatedBirth.day2 <= uiDateMax->date() ) ) )
        return false;

    if ( ( searchFamilyName) && ( searchFirstName ) ) return (familyNameMatch || firstNameMatch);
    if ( ( searchFamilyName) && (!searchFirstName ) ) return familyNameMatch;
    if ( ( searchFirstName ) && (!searchFamilyName) ) return firstNameMatch;
    if ( (!searchFamilyName) && (!searchFirstName ) ) return false;
    return false;

}

void DialogDbView::setup(Date dateLimits, bool male, bool female)
{
    setupUi(this);

    uiPersonCount->setText(QString::number(PersonData::count));
    uiPersonLastID->setText(QString::number(PersonData::lastUsed));
    uiFamilyCount->setText(QString::number(FamilyData::count));
    uiFamilyLastID->setText(QString::number(FamilyData::lastUsed));

    connect(uiExit, SIGNAL(clicked()), this, SLOT(close()));

 // create a person view model and install it

    personModel = new PersonTableModel(this);
    proxyModel = new MyProxyModel(personModel, this);
    proxyModel->setSourceModel(personModel);
    uiTableView->setModel(proxyModel);
    proxyModel->setFilterKeyColumn(0);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    uiTableView->setColumnWidth(0, 80); // familyname
    uiTableView->setColumnWidth(1, 100); // first name
    uiTableView->setColumnWidth(2, 65); // birthday
    uiTableView->setColumnWidth(3, 36); // fams0
    uiTableView->setColumnWidth(4, 36); // fams1
    uiTableView->setColumnWidth(5, 36); // fams2
    uiTableView->setColumnWidth(6, 36); // fams3

    for (int i=1 ; i<PersonData::lastUsed+1 ; i++) idList.append(i);

// Create a family view model and install it

    familyModel = new FamilyTableModel();
    familyProxyModel = new FamilyProxyModel(familyModel, this);
    familyProxyModel->setSourceModel(familyModel);
    uiFamilyView->setModel(familyProxyModel);

    uiFamilyView->setColumnWidth(0, 30); // husb
    uiFamilyView->setColumnWidth(1, 30); // first name
    uiFamilyView->setColumnWidth(2, 70); // birthday    
// connect signals & slots

    connect(uiSearch, SIGNAL(textChanged(QString)), this, SLOT(slotSearch(QString)));
    connect(uiSearchIsCaseSensitive, SIGNAL(toggled(bool)), this, SLOT(slotUiSearchIsCaseSensitive(bool)));
    connect(uiSearchFamilyName, SIGNAL(toggled(bool)), this, SLOT(slotUiSearchFamilyName()));
    connect(uiSearchFirstName, SIGNAL(toggled(bool)), this, SLOT(slotUiSearchFirstName()));
    connect(uiGoTo, SIGNAL(clicked()), this, SLOT(slotGoTo()));           
    connect(uiMale, SIGNAL(clicked()), this, SLOT(slotUpdate()));
    connect(uiFemale, SIGNAL(clicked()), this, SLOT(slotUpdate()));
    connect(uiDateMin, SIGNAL(dateChanged(QDate)), this, SLOT(slotUpdate()));
    connect(uiDateMax, SIGNAL(dateChanged(QDate)), this, SLOT(slotUpdate()));
    connect(uiToBrowseHistory, SIGNAL(clicked()), this, SLOT(slotToBrowseHistory()));
    connect(uiAncestorsOnly, SIGNAL(clicked()), this, SLOT(slotUpdate()));
    connect(uiDescentantsOnly, SIGNAL(clicked()), this, SLOT(slotUpdate()));

// init check boxes

    if (searchFamilyName) uiSearchFamilyName->setChecked(true); else uiSearchFamilyName->setChecked(false);
    if (searchFirstName) uiSearchFirstName->setChecked(true); else uiSearchFirstName->setChecked(false);
    uiDateMin->setMinimumDate(GET_ALLMINDAY);
    uiDateMin->setMaximumDate(GET_ALLMAXDAY);
    uiDateMax->setMinimumDate(GET_ALLMINDAY);
    uiDateMax->setMaximumDate(GET_ALLMAXDAY);
    uiDateMin->setDate(dateLimits.day1);
    uiDateMax->setDate(dateLimits.day2);

    if (male) uiMale->setChecked(true); else uiMale->setChecked(false);
    if (female) uiFemale->setChecked(true); else uiFemale->setChecked(false);

    ancestors = new bool[PersonData::lastUsed+1];
    descentants = new bool[PersonData::lastUsed+1];

    for (int i=0 ; i<PersonData::lastUsed ; i++ ) {
        ancestors[i] = false;
        descentants[i] = false;
    }

    int id = *gd.currentId;

    pd[id].ancestors(ancestors);
    pd[id].descendantStep(descentants, id);

    //slotUpdate();// tämä jossain tapauksessa kaataa softan eikä näköjään edes tarvita, miksi tää oli tässä?
}

// -----------------------------------------------------------------------------------------------------------

int PersonTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    const int tmp = PersonData::lastUsed+1;

    return tmp;

}

int PersonTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 7;
}

QVariant PersonTableModel::data(const QModelIndex &index, int role) const
{    

    int row = index.row();
    QVariant result;

    result = QVariant();

    if (index.isValid()) {
        if ( role == Qt::BackgroundColorRole ) {
            if (pd[row].sex == FEMALE ) result = ( QColor(255, 199, 199) );
            if (pd[row].sex == MALE ) result =  ( QColor(199, 199, 225) );
            if (pd[row].sex == UNKNOWN) result =  ( Qt::gray );
        }
        if ( role == Qt::TextAlignmentRole ) { result =  ( Qt::AlignLeft ) ;  }
        //if ( role == Qt::DecorationRole ) QVariant();
        if ( role == Qt::DisplayRole ) {
            if (index.column() == 0) result = pd[row].nameFamily;
            if (index.column() == 1) result =  pd[row].nameFirst;
            if (index.column() == 2) result =  pd[row].estimatedBirth.day1;
            if (index.column() == 3) result =  pd[row].estimatedBirth.day2;
            if (index.column() == 4) result =  pd[row].famSList.value(0);
            if (index.column() == 5) result =  pd[row].famSList.value(1);
            if (index.column() == 6) result =  pd[row].famSList.value(2);
        }
    }

    return result;

}

 bool PersonTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
 {
     if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();
        int column = index.column();
        if ( column == 0 ) pd[row].nameFamily = value.toString();
        if ( column == 1 ) pd[row].nameFirst = value.toString();
        //if ( column == 2 ) pd[row].birthDay = value.toDate(); does not work??
        emit(dataChanged(index, index));
        return true;
    }

     return false;
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
             case 3: return "famc";
             case 4: return "fams0";
             case 5: return "fams1";
             case 6: return "fams2";

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

int FamilyTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    const int tmp = FamilyData::lastUsed+1;

    return tmp;


}

int FamilyTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5; // muuta tätä niin columnien määrä muuttuu samalla
}

QVariant FamilyTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    QVariant result = QVariant();

    if (index.isValid()) {
        if ( role == Qt::TextAlignmentRole ) { result = ( Qt::AlignLeft ) ;  }
        //if ( role == Qt::DecorationRole ) QVariant();
        if ( role == Qt::DisplayRole ) {
            if (index.column() == 0) result = fd[row].husb;
            if (index.column() == 1) result = fd[row].wife;
            if (index.column() == 2) {
                Entry marr = fd[row].fentry(MARRIAGE);
                if (marr.day.known) result = marr.date1();
                else result = "Not known";
            }
            if (index.column() == 3) result = pd[fd[row].husb].nameFamily + " " + pd[fd[row].husb].nameFirst;
            if (index.column() == 4) result = pd[fd[row].wife].nameFamily + " " + pd[fd[row].wife].nameFirst;
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
            case 3: return "H Name";
            case 4: return "W Name";

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

    return (p->acceptRow(fd[sourceRow].wife) || p->acceptRow(fd[sourceRow].husb));
}
