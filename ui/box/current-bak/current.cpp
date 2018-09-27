/*
 *
 *  qenes genealogy software
 *
 *  Copyright (C) 2009-2010 Ari TÃ¤hti
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License veasion 2 as
 *  published by the Free Software Foundation.
 */


#include "mainwindow.h"
#include "ui/box/current.h"
#include "ui/person.h"
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
    setupUi(this);
    p = this->palette();
    this->setAutoFillBackground(true);
    p.setColor(QPalette::Window, Qt::lightGray);
    this->setPalette(p);

    setParent(parent);
    id = 0;

    this->id=0;    

    this->uiName->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->uiBirthDay->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->uiBirthPlace->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->uiDeathDay->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->uiDeathPlace->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->uiMarrDay->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->uiMarrPlace->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->uiRelation->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->uiSpouseBirthDate->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->uiSpouseBirthPlace->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->uiSpouseDeathDate->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->uiSpouseDeathPlace->setAttribute(Qt::WA_TransparentForMouseEvents);

    uiOpen->setIcon(QIcon("c://icon.jpg"));

    connect(uiOpen, SIGNAL(clicked()), this, SLOT(slotShowPerson()));
    connect(uiAddSpouse, SIGNAL(clicked()), this, SLOT(slotAddSpouse()));
    connect(uiOpenSpouse, SIGNAL(clicked()), this, SLOT(slotShowSpouse()));
    connect(uiSpouseList, SIGNAL(activated(int)), this, SLOT(slotSpouseClicked(int)));
}

// -----------------------------------------------------------------

void Current::picture(QString currentFile)
{
    QPixmap bmpCurrent;

    pictureCurrentScene.clear();

    if (currentFile != "") {
        currentFile = GET_DIRECTORY + "/" + currentFile;
        if (currentFile.left(2) != "\\\\") { // ei verkkolevyjÃ¤

            if (!bmpCurrent.load(currentFile, ".jpg", Qt::AutoColor)) qDebug() << "failed to download " << currentFile;
            else {
                pictureCurrentScene.setSceneRect(0, 0, bmpCurrent.width(), bmpCurrent.height());
                pictureCurrentScene.addPixmap(bmpCurrent);
                uiPicture->setScene(&pictureCurrentScene);
                uiPicture->fitInView(0, 0, bmpCurrent.width(), bmpCurrent.height(), Qt::KeepAspectRatio);
            }
        }
    }
}

QString Current::place(QString input)
{
    QRegExp nameExp("(.[^\\/]*)?/?([^\\/].[^\\/]*)?/?([^\\/].[^\\/]*)?");
    nameExp.indexIn(input);
    return nameExp.cap(1);
}

void Current::slotSpouseClicked(int i)
{// tämä on kahteen kertaan, toinen kerta person:sa *!*
    INDI(id).famSId = i;
    emit sigUpdate(0);
}

void Current::slotShowSpouse()
{
    int spouse = INDI(id).getSpouse();

    if (spouse) emit(this->sigUpdate(spouse));
}

void Current::slotSpouseAssigned(int newSpouse)
{    
    // käyttäjä valitsi puolison db:sta
    GENE.copyToEditUndo(newSpouse);

    GENE.demonRun.familyRelations = true;
    // rapatessa roiskuu... jossain luodaan jostainsyystä famsList:n arvo 0. Siistitään myös indi 0 ja fam 0
    INDI(id).famSList.removeAll(0);
    INDI(0).clear(&GENE);
    FAM(0).clear(&GENE);
    INDI(GENE.indiLastUsed).clear(&GENE); GENE.indiLastUsed--;

    int familyID = GENE.famLastUsed;// + 1;

    GENE.famSpace(familyID);
    INDI(newSpouse).famSList.append(familyID);
    //INDI(id).famSList.append(familyID);

    if (INDI(id).sex == MALE) {
        FAM(familyID).husb = id;
        FAM(familyID).wife = newSpouse;
    }
    if (INDI(id).sex == FEMALE) {
        FAM(familyID).husb = newSpouse;
        FAM(familyID).wife = id;
    }
}

void Current::slotAddSpouse()
{
    bool showMale, showFemale;

    if ( id == 0) {
        QMessageBox qmb;
        qmb.setText(tr("Cannot give a spouse because current person has not been entered. Give the current person first."));
        qmb.exec();
    }
    else {
        GENE.copyToEditUndo(id);
        int newId;
        int familyID = 0; // *!* uninitialized

        {
            newId = GENE.indiLastUsed + 1;
            GENE.indiSpace(newId);

            if ( ( INDI(id).getSpouse() == 0) && (INDI(id).getFamS() == 0 ) ) {
                familyID = GENE.famLastUsed + 1; // ensimmäinen puoliso, ei lapsia
                GENE.famSpace(familyID);
                INDI(id).famSId = 0;
            }

            if ( (INDI(id).getFamS() != 0) && ( INDI(id).getSpouse() != 0 ) ) { // adding a spouse
                familyID = GENE.famLastUsed + 1; // ei ensimmäinen puoliso
                GENE.famSpace(familyID);
                INDI(id).famSId = INDI(id).famSId + 1;
            }

            if ( (INDI(id).getFamS() != 0) && (INDI(id).getSpouse() == 0 ) ) {
                // the very first spouse & kids

                familyID = GENE.famLastUsed + 1;
                GENE.famSpace(familyID);
            }

            SEX spouseSex = INDI(id).sex;

            // tallennetaan uuden ID:n estimatedbirth:n arvio puolison sallitusta syntymävälistä, jos käyttäjä
            // haluaa valita puolison databaseta
            QDate allowedDateMin(INDI(id).estimatedBirth.day1.year() - GET_AGE_SPOUSES_MAX,
                                 INDI(id).estimatedBirth.day1.month(),
                                 INDI(id).estimatedBirth.day1.day());
            QDate allowedDateMax(INDI(id).estimatedBirth.day2.year() + GET_AGE_SPOUSES_MAX,
                                 INDI(id).estimatedBirth.day2.month(),
                                 INDI(id).estimatedBirth.day2.day());

            INDI(newId).estimatedBirth.setDate1(allowedDateMin);
            INDI(newId).estimatedBirth.setDate2(allowedDateMax);

            if (spouseSex == FEMALE) {
                showMale = true;
                showFemale = false;
                INDI(newId).sex = MALE;
                FAM(familyID).husb = newId;
                FAM(familyID).wife = id;
            } else {
                showMale = false;
                showFemale = true;
                INDI(newId).sex = FEMALE;
                FAM(familyID).husb = id;
                FAM(familyID).wife = newId;
            }
            if (spouseSex == UNKNOWN) INDI(newId).sex = UNKNOWN;

            //qDebug() << id << newId << familyID << INDI(id).famSList.contains(familyID) << INDI(newId).famSList.contains(familyID);
            if (!INDI(id).famSList.contains(familyID)) INDI(id).addFamS(familyID);
            if (!INDI(newId).famSList.contains(familyID)) INDI(newId).addFamS(familyID);
            int i ; for (i=1 ; i<newId ; i++) {
                if ( INDI(i).isParent(newId) ) {
                    // henkilön i isä tai äiti on id. Jo aiemmin olleessa perheessä oli joko isä tai äiti ja lapsia, nyt annettiin toinen
                    // vanhempi. Varmistetaan että hlö:n i lapsiperheen isä ja äiti on oikein.
                    if (spouseSex == MALE) {
                        INDI(i).setMother(newId);
                        INDI(i).setFather(id);
                    } else {
                        INDI(i).setFather(newId);
                        INDI(i).setMother(id);
                    }
                }
            }
            INDI(i).setRin();
        }  // end of adding a spouse

        // lisätään siis puolisoa
        Person ps(showMale, showFemale, &newId, &id);        
        connect(&ps, SIGNAL(sigPersonAssigned(int)), this, SLOT(slotSpouseAssigned(int)));
        int result = ps.exec();

        if ( !result ) {
            // käyttäjä joko painoi exit without save tai valitsi puolison db:sta
            GENE.stop();
            GENE.pasteFromEditUndo();
            if ( GENE.wasRunningWhenStopped ) GENE.start(QThread::NormalPriority);

        } else {
            GENE.stop();
            GENE.demonRun.dateAccuracyTest = true;
            GENE.demonRun.familyRelations = true;
            GENE.demonRun.estimatedBirths = true;
            GENE.demonRun.errors = true;
            GENE.start(QThread::NormalPriority);
        }

        INDI(0).clear(&GENE); // varmuuden vuoksi
    }
}

void Current::slotShowPerson()
{    
    if (GENE.locked) {
        QMessageBox qmb;
        qmb.setText("Not editable during output operation");
        qmb.exec();
        return;
    }

    GENE.browseHistory.removeAll(id);
    if (!INDI(id).deleted && id != 0 ) GENE.browseHistory.prepend(id);

    GENE.copyToEditUndo(id);

    if (id == 0) {
        // Lets create a very first person in the database.

        id = GENE.indiLastUsed+1;

        GENE.indiSpace(id);    // tämän jälkeen gd.indiLastUsed = id;

        if (INDI(id).deleted) INDI(id).clear(&GENE);

        INDI(id).setRin();
    }

    Person ps(false, false, &id, &id);
    //connect(&ps, SIGNAL(sigPersonAssigned(int)), this, SLOT(slotPersonAssigned(int)));

    if (!ps.exec()) {

        GENE.pasteFromEditUndo();

        if (id==1) GENE.currentId = 0;
    } else {
        GENE.demonRun.dateAccuracyTest = true;
        GENE.demonRun.familyRelations = true;
        GENE.demonRun.estimatedBirths = true;
        GENE.demonRun.errors = true;
        GENE.start(QThread::NormalPriority);
    }

    emit(sigUpdate(id));


//    //GENE.indiCount++; // counter needs to be increased one step, because when pdRelative will
                         // be destructed while leaving this method, the counter would be wrong

}

void Current::clear()
{
    this->uiName->setText("");
    uiRelation->setText("");
    //if (uiPhoto) uiPhoto->setVisible(false);
    if (uiBirthPlace) uiBirthPlace->setText("");
    if (uiDeathPlace) uiDeathPlace->setText("");
    if (uiDeathDay) uiDeathDay->setText("");
    if (uiBirthDay) uiBirthDay->setText("");
}

void Current::update(int c)
{
    id = c;

    QString thisPicture;

    if (id != 0) thisPicture = INDI(id).multiMedia.value(0).file; else thisPicture = "";
    /*
    if (INDI[currentID].getSpouse() != 0)
        spousePicture = INDI[INDI[currentID].getSpouse()].multiMedia.value(0).file;
    else spousePicture = "";
    */
    picture(thisPicture);

    QString birth, death;

    uiOpen->setIcon(QIcon());

    for (int i = 0 ; i<GENE.problems.size() ; i++ ) {
        if ( GENE.problems.at(i).id == id ) {
            QIcon icon;
            icon.addFile(QString::fromUtf8(":/icons/error.jpg"), QSize(), QIcon::Normal, QIcon::Off);
            uiOpen->setIcon(icon);
        }
    }


    if ( id==0 ) {
        // Tässä boxissa olevaa henkilöä ei ole syötetty aiemmin.

        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/benefits.png"), QSize(), QIcon::Normal, QIcon::Off);
        uiOpen->setIcon(icon);

        uiOpen->setDisabled(false);

        uiOpen->setStyleSheet(YELLOWSTYLE);
        uiOpenSpouse->setStyleSheet(UNKNOWNSTYLE);

        spouseNameList.clear();
        spouseIdList.clear();
        spouseModel.setStringList(spouseNameList);
        uiMarrDay->setText("");
        uiMarrPlace->setText("");
        uiSpouseBirthDate->setText("");
        uiSpouseBirthPlace->setText("");
        uiSpouseDeathDate->setText("");
        uiSpouseDeathPlace->setText("");

        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/addspouse.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        this->uiOpenSpouse->setIcon(icon2);
        clear();
        uiOpenSpouse->setDisabled(true);
    } else { // henkilötiedot on annettu aiemmin ja niitä voidaan muuttaa

        if ( INDI(id).sex == MALE) {
            if (INDI(id).selected) uiOpen->setStyleSheet(MALESTYLE_SELECTED);
            else uiOpen->setStyleSheet(MALESTYLE);
        }
        if ( INDI(id).sex == FEMALE) {
            if (INDI(id).selected) uiOpen->setStyleSheet(FEMALESTYLE_SELECTED);
            else uiOpen->setStyleSheet(FEMALESTYLE);
        }
        if ( INDI(id).sex == UNKNOWN) {
            if (INDI(id).selected) uiOpen->setStyleSheet(UNKNOWNSTYLE_SELECTED);
            else uiOpen->setStyleSheet(UNKNOWNSTYLE);
        }

        if (INDI(id).getSpouse()) {
            if (INDI(INDI(id).getSpouse()).sex == MALE) {
                if (INDI(INDI(id).getSpouse()).selected) this->uiOpenSpouse->setStyleSheet(MALESTYLE_SELECTED);
                else this->uiOpenSpouse->setStyleSheet(MALESTYLE);
            }
            if (INDI(INDI(id).getSpouse()).sex == FEMALE) {
                if (INDI(INDI(id).getSpouse()).selected) this->uiOpenSpouse->setStyleSheet(FEMALESTYLE_SELECTED);
                else this->uiOpenSpouse->setStyleSheet(FEMALESTYLE);
            }
        } else this->uiOpenSpouse->setStyleSheet(UNKNOWNSTYLE);

        birth = birthString(id);
        death = deathString(id);

        uiRelation->setText(INDI(id).getShortFamilyRelation());
        uiName->setText(INDI(id).nameFamily + " " + INDI(id).nameFirst );

        uiBirthDay->setText(birth);
        uiDeathDay->setText(death);

        if (uiBirthPlace) uiBirthPlace->setText(place(INDI(id).ievent(BIRTH).place));
        if (uiDeathPlace) uiDeathPlace->setText(place(INDI(id).ievent(DEATH).place));
        uiOpen->setText(""); // open
        uiOpen->setDisabled(false);

        if ( INDI(id).sex == UNKNOWN) uiOpen->setStyleSheet(UNKNOWNSTYLE);

        spouseNameList.clear();
        spouseIdList.clear();

        INDI(id).spouses(&spouseIdList);
        INDI(id).makeNameList(&spouseNameList, &spouseIdList);

        spouseModel.setStringList(spouseNameList);
        uiSpouseList->setModel(&spouseModel);
        uiSpouseList->setCurrentIndex(INDI(id).famSId);

        uiMarrDay->setText(INDI(id).fevent(MARRIAGE).day.getDateToString(true));
        uiMarrPlace->setText(INDI(id).fevent(MARRIAGE).place);

        int spouse = INDI(id).getSpouse();
        if (spouse) {
            uiSpouseBirthDate->setText(birthString(spouse));
            uiSpouseBirthPlace->setText(place(INDI(spouse).ievent(BIRTH).place));
            uiSpouseDeathDate->setText(deathString(spouse));
            uiSpouseDeathPlace->setText(place(INDI(spouse).ievent(DEATH).place));
            uiOpenSpouse->setDisabled(false);
            this->uiOpenSpouse->setIcon(QIcon());
        } else {
            uiMarrDay->setText("");
            uiMarrPlace->setText("");
            uiSpouseBirthDate->setText("");
            uiSpouseBirthPlace->setText("");
            uiSpouseDeathDate->setText("");
            uiSpouseDeathPlace->setText("");
            uiOpenSpouse->setDisabled(true);

            QIcon icon;
            icon.addFile(QString::fromUtf8(":/icons/addspouse.jpg"), QSize(), QIcon::Normal, QIcon::Off);
            this->uiOpenSpouse->setIcon(icon);
        }
    }
}

/*
Current::Current()
{
    id = 0;
}
*/
