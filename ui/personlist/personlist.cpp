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

#include "personlist.h"
#include "ui_personlist.h"
#include "data/persondata.h"
#include "data/familydata.h"
#include <QDialog>
#include <QPainter>
#include <QBrush>
#include <QSettings>
#include "macro.h"
#include "ui/person.h"

extern  QSettings       sets;
extern  GeneData        * geneData;

void PersonList::addChild(quint16 relative)
// Child voidaan lisätä myös muualta kuin "childlist" luokasta, eli lähinnä myös "siblinglist" luokasta.
// Siksi addChild methodi on tässä (eikä ChildList:ssä)
// relative muuttujassa on "current" henkilön id, eli se henkilö joka on UI:ssä keskellä
{

    // Tarkastetaan, ettei isä ja äiti ole liian nuoria vanhemmiksi. Esim, jos jompikumpi vanhemmista on
    // lapsi joka on syntynyt jokunen vuosi sitten suhteessa systeemipvm, tätä uutta lasta ei voida lisätä.
    int mother, father;
    if ( (INDI(relative).sex == MALE) || ( INDI(relative).sex == UNKNOWN) ) {
        father = relative;
        mother = INDI(relative).getSpouse();
    } else {
        mother = relative;
        father = INDI(relative).getSpouse();
    }


    QString message = "";
    if (father && (INDI(father).estimatedBirth.day1.daysTo((QDate::currentDate())) < GET_AGE_FATHERCHILD_MIN*356)) {
    //if (father && (INDI(father).getEntryDaybyType(true, BIRTH).day1.daysTo((QDate::currentDate())) < GET_AGE_FATHERCHILD_MIN*356)) {
        qDebug() << QDate::currentDate();
        qDebug() << GET_AGE_FATHERCHILD_MIN;
        qDebug() << father;
        qDebug() << INDI(father).getEntryDaybyType(true, BIRTH).day1;
        qDebug() << INDI(father).estimatedBirth;
        qDebug() << INDI(father).getEntryDaybyType(true, BIRTH).day1.daysTo((QDate::currentDate()));
        message = "Father would be too young.";
    }

    if (mother && (INDI(mother).estimatedBirth.day1.daysTo((QDate::currentDate())) < GET_AGE_MOTHERCHILD_MIN*356)) {
    //if (mother && (INDI(mother).getEntryDaybyType(true, BIRTH).day1.daysTo((QDate::currentDate())) < GET_AGE_MOTHERCHILD_MIN*356)) {
        message += "Mother would be too young.";
    }

    if (message != "") {
        QMessageBox msgBox;
        msgBox.setText("Error occured. " + message + " Calculation done based on system date. If you want to add this child, check the minimum age between a child and parents in the settings, parent birthdays, or system date.");
        msgBox.setInformativeText("Would you like to add this child anyway?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();

        if (ret == QMessageBox::No) return;
    }

// isä ja äiti ei ole liian nuoria vanhemmiksi suhteessa systeemin päivämäärään.

    GENE.copyToEditUndo(relative);

    GENE.editAddIndi = GENE.indiLastUsed+1;

    GENE.indiSpace(GENE.editAddIndi);

    if (INDI(relative).getFamS() == 0 ) {
        // no fd for this child. Create it.
        GENE.editAddFam = GENE.famiLastUsed + 1;
        GENE.famiSpace(GENE.editAddFam);
        INDI(relative).addFamS(GENE.editAddFam);

        if (INDI(relative).sex == MALE) {
            FAM(GENE.editAddFam).husb = relative;
            if (INDI(relative).getSpouse() != 0) FAM(GENE.editAddFam).wife = INDI(relative).getSpouse();
        } else {

            FAM(GENE.editAddFam).wife = relative;
            if (INDI(relative).getSpouse() != 0) FAM(GENE.editAddFam).husb = INDI(relative).getSpouse();
        }
    }

    INDI(GENE.editAddIndi).famc.value = INDI(relative).getFamS();
    INDI(GENE.editAddIndi).setRin();

    if (!FAM(INDI(relative).getFamS()).childs.contains(GENE.editAddIndi)) FAM(INDI(relative).getFamS()).childs.append(GENE.editAddIndi);

    if ( GET_SUGGEST_FAMILYNAME && ( INDI(GENE.editAddIndi).getFather() != 0 ) ) {

        INDI(GENE.editAddIndi).nameFamily = INDI(INDI(GENE.editAddIndi).getFather()).nameFamily;
    }

    Person ps(false, false, &GENE.editAddIndi);

    int result = ps.exec();

    if (!result) {
        GENE.pasteFromEditUndo();
        emit(sigUpdate());

        /*
         Lasta ei tallennetakaan. Perutaan edellÃ¤ tehdyt muutokset.
        */
    } else {
        GENE.demonRun.dateAccuracyTest  = true;
        GENE.demonRun.errors            = true;
        GENE.demonRun.estimatedBirths   = true;
        GENE.demonRun.familyRelations   = true;
        GENE.run();
    }
    GENE.clearEditUndo();
}

void PersonList::enableAddButton(bool enabled)
{
    if (enabled) this->uiAdd->setHidden(false);
    else this->uiAdd->setHidden(true);    

}

void PersonList::slotNoStepRelatives()
{
    if (uiOnlyCurrentFam->isChecked()) {
        this->noStepRelativesChecked = true;
        emit noStepRelatives(true);
    }
    else {
        this->noStepRelativesChecked = false;
        emit noStepRelatives(false);
    }
}

void PersonList::clicked(QModelIndex qmi)
{
    int row = qmi.row();
    emit(sigUpdate(personList->at(row)));
}

void PersonList::trans(QString title, QString checkText)
{
    this->uiTitle->setText(title);
    this->uiOnlyCurrentFam->setText(checkText);
}

void PersonList::setup(QList<quint16> *pL, QString title, bool addButton, QString checkBoxText, QWidget *parent)// : QWidget(parent)
{
    Q_UNUSED(parent);
    setupUi(this);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    p = this->palette();
    this->setAutoFillBackground(true);
    p.setColor(QPalette::Window, Qt::lightGray);
    this->setPalette(p);

    if (!addButton) this->uiAdd->setHidden(true);    
    else uiAdd->setStyleSheet("QPushButton { background-color: rgb(150, 150, 150); }");

    personModel = new PersonListModel;
    personList = pL;
    personModel->personList = pL;
    uiPLT->setModel(personModel);
    uiTitle->setText(title);
    uiTitle->setFont(GeneData::panelFontBold);

    uiOnlyCurrentFam->setText(checkBoxText);
    uiOnlyCurrentFam->setFont(GeneData::panelFontBold);

    //uiPLT->setColumnWidth(0, 80);
    //uiPLT->setColumnWidth(1, 53);

    connect(uiPLT, SIGNAL(clicked(QModelIndex)), this, SLOT(clicked(QModelIndex)));
    connect(uiOnlyCurrentFam, SIGNAL(clicked()), this, SLOT(slotNoStepRelatives()));

    uiPLT->setFont(GeneData::panelFont);
}

void PersonList::update()
{    
    personModel->updt();    
}

void PersonList::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

//------------------


void PersonListModel::updt()
{
   this->layoutChanged();
}

int PersonListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    const int tmp = personList->size();

    return tmp;
}


int PersonListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant PersonListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int id = personList->at(row);

    if (!index.isValid()) return QVariant(); // tää oli jossain esimerkissä, mitä tämä tekee?
    if ( role == Qt::BackgroundColorRole ) {
        if (INDI(id).sex == FEMALE ) {
            if (INDI(id).selected) return ( QColor(255, 240, 240) );
            else return ( QColor(255, 199, 199) );
        }
        if (INDI(id).sex == MALE ) {
            if (INDI(id).selected) return ( QColor(240, 240, 255) );
            else return ( QColor(199, 199, 225) );
        }
        if (INDI(id).sex == UNKNOWN) {
            if (INDI(id).selected) return ( QColor(240, 240, 240) );
            else return ( QColor(180,180,180) );
    }
    }
    if ( role == Qt::TextAlignmentRole ) { return ( Qt::AlignLeft ) ;  }
    if ( role == Qt::DecorationRole ) QVariant();
    if ( role == Qt::DisplayRole ) {
        QString name;
        name = INDI(personList->at(row)).nameFirst;
        if ( INDI(id).name2nd != "" ) name += " " + INDI(personList->at(row)).name2nd.left(1) + ".";
        if ( INDI(id).name3rd != "" ) name +=  INDI(personList->at(row)).name3rd.left(1) + ".";

        if (INDI(id).isDirectAncestor) name = "*" + name;
        //if (!INDI(id).familyRelation.contains("c") && !INDI(id).familyRelation.contains("s")) name = "*" + name;

        if (index.column() == 0) return name;
        if (index.column() == 1) {
            Date birth = INDI(personList->at(row)).ientry(BIRTH).day;
            birth.setAcc(ABT);
            return birth.toString(false);
        }

        return row;
    }
    return QVariant();
}

 bool PersonListModel::setData(const QModelIndex &index, const QVariant &value, int role)
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

Qt::ItemFlags PersonListModel::flags(const QModelIndex &index) const
 {
    if (!index.isValid())
         return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled;
 }

 QVariant PersonListModel::headerData(int section, Qt::Orientation orientation, int role) const
 {
     if (role != Qt::DisplayRole) return QVariant();

     if (orientation == Qt::Horizontal) {
         switch (section) {
             case 0: return tr("Name");
             case 1: return tr("Birth");

             default:
                 return QVariant();
         }
     }
     if (orientation == Qt::Vertical) {
         return section;
     }
     return QVariant();
 }
