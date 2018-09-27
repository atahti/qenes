/*
 *
 *  qenes genealogy software
 *
 *  Copyright (C) 2009-2010 Ari T√§hti
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License veasion 2 as
 *  published by the Free Software Foundation.
 */


#include "mainwindow.h"
#include "ui/box/current.h"
#include "ui/person.h"
#include "ui/box/panel.h"
#include "macro.h"
#include <QSettings>

extern  QSettings       sets;
extern  GeneData        * geneData;

QPoint Current::rightPos(QPoint global)
{
    QPoint posMain = this->mapToGlobal(QPoint(0, 0));
    QPoint returnValue;

    returnValue.setX(  posMain.x() - global.x()  + this->width() );
    returnValue.setY( (posMain.y() - global.y()) + this->height()/2  );

    return returnValue;
}

Current::Current(QMainWindow *parent)
{    
    oldHeight = 0;
    setupUi(this);
    p = this->palette();
    this->setAutoFillBackground(true);
    p.setColor(QPalette::Window, Qt::lightGray);
    this->setPalette(p);

    current = new PanelCommonAll();
    current->setup(5, false, false); // center button / spouse selector
    this->grid->addWidget(current);

    spouse = new PanelCommonAll();
    spouse->setup(1, false, true);
    spouse->setMinimumHeight(80);
    this->grid->addWidget(spouse);

    setParent(parent);
    current->id = 0;

    //current->setIcon(QIcon("c://icon.jpg"));

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(current, SIGNAL(clicked()), this, SLOT(slotShowPerson()));
    connect(spouse->uiAddSpouse, SIGNAL(clicked()), this, SLOT(slotAddSpouse()));
    connect(spouse, SIGNAL(clicked()), this, SLOT(slotShowSpouse()));
    connect(spouse->uiSpouseList, SIGNAL(activated(int)), this, SLOT(slotSpouseClicked(int)));
}

// -----------------------------------------------------------------


void Current::picture(QString currentFile)
{
    QPixmap bmpCurrent;

    pictureCurrentScene.clear();

    if (currentFile != "") {
        currentFile = GET_DIRECTORY + "/" + currentFile;
        if (currentFile.left(2) != "\\\\") { // ei verkkolevyj√§

            if (!bmpCurrent.load(currentFile, ".jpg", Qt::AutoColor)) qDebug() << "failed to download @ current " << currentFile;
            else {
                pictureCurrentScene.setSceneRect(0, 0, bmpCurrent.width(), bmpCurrent.height());
                pictureCurrentScene.addPixmap(bmpCurrent);
                uiPicture->setScene(&pictureCurrentScene);
                uiPicture->fitInView(0, 0, bmpCurrent.width(), bmpCurrent.height(), Qt::KeepAspectRatio);
            }
        }
    }
}

/*void Current::resizeEvent(QResizeEvent *event)
{    
    Q_UNUSED( event );
    if (this->current->heightId != 3 && this->height() < 200) this->current->setup(3, false, false);
    if (this->current->heightId != 4 && (this->height()>200 && this->height() < 270 ) ) this->current->setup(4, false, false);
    if (this->current->heightId != 5 && this->height() > 270) this->current->setup(5, false, false);

    uiPicture->fitInView(0, 0, this->pictureCurrentScene.width(), this->pictureCurrentScene.height(), Qt::KeepAspectRatio);
}*/

QString Current::place(QString input)
{
    QRegExp nameExp("(.[^\\/]*)?/?([^\\/].[^\\/]*)?/?([^\\/].[^\\/]*)?");
    nameExp.indexIn(input);
    return nameExp.cap(1);    
}

void Current::slotSpouseClicked(int i)
{// t‰m‰ on kahteen kertaan, toinen kerta person:sa *!*

    //qDebug() << "Current::slotSpouseClicked(" << i << ")";
    INDI(current->id).currentSpouse = i;
    emit sigUpdate(0);
}

void Current::slotShowSpouse()
{
    qDebug() << "Current::slotShowSpouse";
    quint16 spouseId = INDI(current->id).getSpouse();

    if (spouseId) emit(this->sigUpdate(spouseId));
}

void Current::slotSpouseAssigned(quint16 newSpouse)
{    
    // k‰ytt‰j‰ valitsi puolison db:sta

    GENE.copyToEditUndo(newSpouse);

    GENE.demonRun.familyRelations = true;
    // rapatessa roiskuu... jossain luodaan jostainsyyst‰ famsList:n arvo 0. Siistit‰‰n myˆs indi 0 ja fam 0
    INDI(current->id).famSList.removeAll(0);
    INDI(0).clear(&GENE);
    FAM(0).clear(&GENE);

    // T‰ss‰ v‰liss‰ famiLastUsed:a ei tarvi pienent‰‰, koska sit‰ pienennettiin jo slotpersonassignedssa.

    int familyID = GENE.famiLastUsed;// + 1;

    GENE.famiSpace(familyID);
    INDI(newSpouse).famSList.append(familyID);

    if (INDI(current->id).sex == MALE) {
        FAM(familyID).husb = current->id;
        FAM(familyID).wife = newSpouse;
    }
    if (INDI(current->id).sex == FEMALE) {
        FAM(familyID).husb = newSpouse;
        FAM(familyID).wife = current->id;
    }
}

void Current::slotAddSpouse()
{
    bool showMale, showFemale;

    if ( current->id == 0) {
        QMessageBox qmb;
        qmb.setText(tr("Cannot give a spouse because current person has not been entered. Give the current person first."));
        qmb.exec();
    }
    else {

        GENE.copyToEditUndo(current->id);
        quint16 newId;
        quint16 familyID = 0; // *!* uninitialized

        {
            newId = GENE.indiLastUsed + 1;
            GENE.indiSpace(newId);

            if ( ( INDI( current->id ).getSpouse() == 0) && (INDI( current->id ).getFamS() == 0 ) ) {
                familyID = GENE.famiLastUsed + 1; // ensimm‰inen puoliso, ei lapsia
                GENE.famiSpace(familyID);
                INDI( current->id ).currentSpouse = 0;
            }

            if ( (INDI( current->id ).getFamS() != 0) && ( INDI( current->id ).getSpouse() != 0 ) ) { // adding a spouse
                familyID = GENE.famiLastUsed + 1; // ei ensimm‰inen puoliso
                GENE.famiSpace(familyID);
                INDI( current->id ).currentSpouse = INDI( current->id ).currentSpouse + 1;
            }

            if ( (INDI( current->id ).getFamS() != 0) && (INDI( current->id ).getSpouse() == 0 ) ) {
                // the very first spouse & kids

                familyID = GENE.famiLastUsed + 1;
                GENE.famiSpace(familyID);
            }

            SEX spouseSex = INDI( current->id ).sex;

            // tallennetaan uuden ID:n estimatedbirth:n arvio puolison sallitusta syntym‰v‰list‰, jos k‰ytt‰j‰
            // haluaa valita puolison databaseta
            /*
            QDate allowedDateMin(INDI( current->id ).estimatedBirth.day1.year() - GET_AGE_SPOUSES_MAX,
                                 INDI( current->id ).estimatedBirth.day1.month(),
                                 INDI( current->id ).estimatedBirth.day1.day());
            QDate allowedDateMax(INDI( current->id ).estimatedBirth.day2.year() + GET_AGE_SPOUSES_MAX,
                                 INDI( current->id ).estimatedBirth.day2.month(),
                                 INDI( current->id ).estimatedBirth.day2.day());

            INDI(newId).estimatedBirth.setDate1(allowedDateMin);
            INDI(newId).estimatedBirth.day1explanation = "Spouse birth (early estimation " + INDI( current->id ).estimatedBirth.day1.toString("dd.MM.yyyy") + " - spouse max age difference (" + QString::number(GET_AGE_SPOUSES_MAX) + ")";
            INDI(newId).estimatedBirth.setDate2(allowedDateMax);
            INDI(newId).estimatedBirth.day2explanation = "Spouse birth (late estimation " + INDI( current->id ).estimatedBirth.day2.toString("dd.MM.yyyy") + " + spouse max age difference (" + QString::number(GET_AGE_SPOUSES_MAX) + ")";

            if (INDI(newId).estimatedBirth.day2 > QDate::currentDate()) {
                INDI(newId).estimatedBirth.day2 = QDate::currentDate();
                INDI(newId).estimatedBirth.day2explanation = "This day";
            }
            n‰ytt‰isi toimivan mainiosti estimatedbirthill‰, joka haetaan parikyt‰ rivi‰ alasp‰in
*/


            if (spouseSex == FEMALE) {
                showMale = true;
                showFemale = false;
                INDI(newId).sex = MALE;
                FAM(familyID).husb = newId;
                FAM(familyID).wife = current->id;
            } else {
                showMale = false;
                showFemale = true;
                INDI(newId).sex = FEMALE;
                FAM(familyID).husb = current->id;
                FAM(familyID).wife = newId;
            }
            if (spouseSex == UNKNOWN) INDI(newId).sex = UNKNOWN;

            if (!INDI( current->id ).famSList.contains(familyID)) INDI( current->id ).addFamS(familyID);
            if (!INDI(newId).famSList.contains(familyID)) INDI(newId).addFamS(familyID);
            quint16 i ; for (i=1 ; i<newId ; i++) {
                if ( INDI(i).isParent(newId) ) {
                    // henkilˆn i is‰ tai ‰iti on id. Jo aiemmin olleessa perheess‰ oli joko is‰ tai ‰iti ja lapsia, nyt annettiin toinen
                    // vanhempi. Varmistetaan ett‰ hlˆ:n i lapsiperheen is‰ ja ‰iti on oikein.
                    if (spouseSex == MALE) {
                        INDI(i).setMother(newId);
                        INDI(i).setFather( current->id );
                    } else {
                        INDI(i).setFather(newId);
                        INDI(i).setMother( current->id );
                    }
                }
            }
            INDI(i).setRin();
        }  // end of adding a spouse

        // lis‰t‰‰n siis puolisoa

        bool foo; INDI(newId).estimatedBirth = INDI(newId).getEstimatedBirth(&foo);

        Person ps(showMale, showFemale, &newId);//###
        //Person ps(showMale, showFemale, &newId, &current->id);//###
        connect(&ps, SIGNAL(sigPersonAssigned(quint16)), this, SLOT(slotSpouseAssigned(quint16)));
        int result = ps.exec();
        // Result : ssa on uuden puolison numero, joko uusi tai listasta valittu. Jos 0, k‰ytt‰j‰ painoi
        // exit without save

        if ( !result ) {
            // k‰ytt‰j‰ joko painoi exit without save
            GENE.stop();
            GENE.pasteFromEditUndo();

            // Siilt‰ varalta, ett‰  oltiin lis‰‰m‰ss‰ puolisoa joka sitten peruttiinkin, asetetaan currentidex
            // arvoon nolla jossa suurimmalla todenn‰kˆisyydell‰ on joku
            current->uiSpouseList->setCurrentIndex(0); INDI(GENE.currentId).currentSpouse = 0;

            if ( GENE.wasRunningWhenStopped && RUN_CHECK ) GENE.start(QThread::NormalPriority);

        } else {
            GENE.stop();
            GENE.demonRun.dateAccuracyTest = true;
            GENE.demonRun.familyRelations = true;
            GENE.demonRun.estimatedBirths = true;
            GENE.demonRun.errors = true;
            if (RUN_CHECK) GENE.start(QThread::NormalPriority);
        }

        GENE.clearEditUndo();
        INDI(0).clear(&GENE); // varmuuden vuoksi
    }
}

void Current::slotShowPerson()
{    
    //qDebug() << "void Current::slotShowPerson()";

    // center-henkilˆ‰ on klikattu

    if (GENE.locked) {
        QMessageBox qmb;
        qmb.setText("Not editable during output operation. The output might get screwed.");
        qmb.exec();
        return;
    }

    GENE.browseHistory.removeAll(current->id);
    if (!INDI(current->id).deleted && current->id != 0 ) GENE.browseHistory.prepend( current->id );

    if ( current->id == 0) {
        // Lets create a very first person in the database.

        current->id = GENE.indiLastUsed+1;

        GENE.indiSpace( current->id );

        if (INDI( current->id ).deleted) INDI( current->id ).clear(&GENE);

        INDI( current->id ).setRin();
    }

    GENE.copyToEditUndo(current->id);
    //Person ps(false, false, &current->id, &current->id);//###
    Person ps(false, false, &current->id);//###
    quint16 result = ps.exec();
    //qDebug() << "piip" << result;
    if (!result) {
        GENE.pasteFromEditUndo();

        // Jos ihan ensimm‰isen henkilˆn lis‰ys peruttiin, v‰h‰n erikoisk‰sittely‰...:
        if ( GENE.currentId == 0 ) {
            GENE.indiLastUsed = 0;
            this->current->id = 0;
        }

    } else {
        GENE.demonRun.dateAccuracyTest = true;
        GENE.demonRun.familyRelations = true;
        GENE.demonRun.estimatedBirths = true;
        GENE.demonRun.errors = true;
        if (RUN_CHECK) GENE.start(QThread::NormalPriority);
        emit(sigUpdate( result ));
    }

    GENE.clearEditUndo();
}

void Current::update(int c)
{

    current->id = c;

    QString thisPicture;

    if (current->id != 0) thisPicture = INDI( current->id ).multiMedia.value(0).file; else thisPicture = "";

    picture(thisPicture);

    current->setIcon(QIcon());

    for (int i = 0 ; i<GENE.problems.size() ; i++ ) {
        if ( GENE.problems.at(i).id == current->id ) current->setIcon(QIcon(":/icon/ui/error.jpg"));
    }

    if ( current->id == 0 ) {
        // T‰ss‰ boxissa olevaa henkilˆ‰ ei ole syˆtetty aiemmin.

        current->setIcon(QIcon(":/icon/ui/benefits.png"));

        current->setDisabled(false);
        spouseNameList.clear();
        spouseIdList.clear();
        spouseModel.setStringList(spouseNameList);

        this->current->clear();
        this->spouse->clear();

    } else { // henkilˆtiedot on annettu aiemmin ja niit‰ voidaan muuttaa
        current->updt();

        spouseNameList.clear();
        spouseIdList.clear();

        INDI( current->id ).spouses(&spouseIdList);
        INDI( current->id ).makeNameList(&spouseNameList, &spouseIdList);

        spouseModel.setStringList(spouseNameList);
        //for (int x=0 ; x<spouseNameList.size() ; x++) qDebug() << "spouse" << x << spouseNameList.at(x);
        //qDebug() << "current" << INDI( current->id ).famSId;
        spouse->uiSpouseList->setModel(&spouseModel);
//qDebug() << "current::update" << INDI(current->id).currentSpouse << current->id;
        spouse->uiSpouseList->setCurrentIndex(INDI( current->id ).currentSpouse);

        int spouseId = INDI( current->id ).getSpouse();

        if (spouseId) spouse->updt(spouseId); else spouse->clear();

    }
}
