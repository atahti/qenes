/*
 *
 *  qenes genealogy software
 *
 *  Copyright (C) 2009-2018 Ari T�hti
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License veasion 2 as
 *  published by the Free Software Foundation.
 */

// void Person::slotRestrictionChanged()
#include <QMessageBox>
#include "person.h"
#include "data/persondata.h"
#include "data/familydata.h"
#include "macro.h"
#include <QPixmap>
#include <QFile>
#include <QFileDialog>
#include "dbview2.h"
#include <QInputDialog>
#include <QSettings>
#include <QToolTip>

#define ITEM_AT(grid, x,y)          grid->itemAtPosition(y, x)->widget()
#define LINE_HEIGHT  20
#define ENTRYS                      entryBox->par->visibleTabWidget->entrys
#define bINDI                        &(entryBox->par->visibleTabWidget->indi)
#define ID                          entryBox->par->id

#define SPACERLINE_HEIGHT 1
#define SHOWMETHODS true
#define DE_BUG if (SHOWMETHODS) qDebug() <<

extern  QStringListModel    quayModel, accModel, adopModel;
extern  QStringList         usrEvents, famEvents;
extern  GeneData        * geneData;
extern  SourceRecord*   sour;
extern  QSettings       sets;
extern  QList<QString>  boyNames;
extern  QList<QString>  girlNames;

extern ENTRYELEMENT line2days[9];
extern ENTRYELEMENT line1days[9];
extern ENTRYELEMENT line0days[9];
extern ENTRYELEMENT lineAddnew[9];
extern ENTRYELEMENT line0Adop[9];
extern ENTRYELEMENT line1Adop[9];
extern ENTRYELEMENT line2Adop[9];
extern ENTRYELEMENT line0Attr[9];
extern ENTRYELEMENT line1Attr[9];
extern ENTRYELEMENT line2Attr[9];
extern ENTRYELEMENT line0Usre[9];
extern ENTRYELEMENT line1Usre[9];
extern ENTRYELEMENT line2Usre[9];
extern ENTRYELEMENT lineEmpty[9];
extern ENTRYELEMENT lineSpace[9];
extern ENTRYELEMENT lineAddress[9];
extern ENTRYELEMENT linePostCity[9];
extern ENTRYELEMENT lineStateCountry[9];
extern ENTRYELEMENT lineSourcePgQlty[9];
extern ENTRYELEMENT lineSourceText[9];
extern ENTRYELEMENT lineAddEntryMore[9];
extern ENTRYELEMENT lineAddEntryNotes[9];
extern ENTRYELEMENT lineRestrictions[9];
extern ENTRYELEMENT linePhotos[9];

// *** voisiko nuo muuttujaniemet tuossa alla alkaa jossain yhdenmukaisella esim entrylinetype_foo
ENTRYELEMENT line2days[9] =  { TYPECOMBO, ACCCOMBO,  DAY1,      DAY2,       PLACE,      PLACE,      PLACE,      BUTTONM,    BUTTONX };
ENTRYELEMENT line1days[9] =  { TYPECOMBO, ACCCOMBO,  DAY1,      PLACE,      PLACE,      PLACE,      PLACE,      BUTTONM,    BUTTONX };
ENTRYELEMENT line0days[9] =  { TYPECOMBO, ACCCOMBO,  DAY1,      PLACE,      PLACE,      PLACE,      PLACE,      BUTTONM,    BUTTONX };
ENTRYELEMENT lineAddnew[9]=  { NEWCOMBO,  DELETE,    DELETE, DELETE,  DELETE,  DELETE,  DELETE,  NOELEMENT,  NOELEMENT};
ENTRYELEMENT line0Adop[9] =  { TYPECOMBO, ACCCOMBO,  DAY1,      NOELEMENT,  NOELEMENT,  NOELEMENT,  NOELEMENT,  BUTTONM,    BUTTONX};
ENTRYELEMENT line1Adop[9] =  { TYPECOMBO, ACCCOMBO,  DAY1,      ADOPCOMBO,  ADOPCOMBO,  ADOPCOMBO,  ADOPCOMBO,  BUTTONM,    BUTTONX};
ENTRYELEMENT line2Adop[9] =  { TYPECOMBO, ACCCOMBO,  DAY1,      DAY2,       ADOPCOMBO,  ADOPCOMBO,  ADOPCOMBO,  BUTTONM,    BUTTONX};
ENTRYELEMENT line0Attr[9] =  { TYPECOMBO, ACCCOMBO,  DAY1,      VALUE,      VALUE,      VALUE,      PLACE,      BUTTONM,    BUTTONX};
ENTRYELEMENT line1Attr[9] =  { TYPECOMBO, ACCCOMBO,  DAY1,      VALUE,      VALUE,      VALUE,      PLACE,      BUTTONM,    BUTTONX};
ENTRYELEMENT line2Attr[9] =  { TYPECOMBO, ACCCOMBO,  DAY1,      DAY2,       VALUE,      VALUE,      PLACE,      BUTTONM,    BUTTONX};
ENTRYELEMENT line0Usre[9] =  { TYPECOMBO, ACCCOMBO,  USRCOMBO,  PLACE,   PLACE,      PLACE,      PLACE,      BUTTONM,    BUTTONX};
ENTRYELEMENT line1Usre[9] =  { TYPECOMBO, ACCCOMBO,  DAY1,      USRCOMBO,   PLACE,      PLACE,      PLACE,      BUTTONM,    BUTTONX};
ENTRYELEMENT line2Usre[9] =  { TYPECOMBO, ACCCOMBO,  DAY1,      DAY2,       USRCOMBO,   VALUE,      VALUE,      BUTTONM,    BUTTONX};

ENTRYELEMENT lineEmpty[9] =   {NOELEMENT, NOELEMENT, NOELEMENT, NOELEMENT,  NOELEMENT,  NOELEMENT,  NOELEMENT,  NOELEMENT,  NOELEMENT};
ENTRYELEMENT lineSpace[9] =   {SPACELINE, SPACELINE, SPACELINE, SPACELINE,  SPACELINE,  SPACELINE,  SPACELINE,  SPACELINE,  SPACELINE};
ENTRYELEMENT lineAddress[9] = {MORE_LABEL, MORE_LABEL, MORE_LABEL,ADDRLINE,   ADDRLINE,   ADDRLINE,   ADDRLINE,   NOELEMENT,  BUTTONX};
ENTRYELEMENT linePostCity[9]  =   {MORE_LABEL, MORE_LABEL, MORE_LABEL,POST,       CITY,       CITY,       CITY,       NOELEMENT,  BUTTONX};
ENTRYELEMENT lineStateCountry[9] =    {MORE_LABEL, MORE_LABEL, MORE_LABEL,STATE,      STATE,      COUNTRY,    COUNTRY,    NOELEMENT,  BUTTONX};
ENTRYELEMENT lineSourcePgQlty[9]=  {MORE_LABEL, MORE_LABEL, MORE_LABEL,SOURCE_QLTY,SOURCE,     SOURCE,     SOURCE ,    NOELEMENT,  BUTTONX};
ENTRYELEMENT lineSourceText[9]  = {MORE_LABEL, MORE_LABEL, MORE_LABEL,SOURCE_TEXT,SOURCE_TEXT,SOURCE_TEXT,SOURCE_TEXT,NOELEMENT,  BUTTONX};
ENTRYELEMENT lineAddEntryMore[9]= {MORE_ADD,  MORE_ADD,  MORE_ADD,  MORE_ADD,   MORE_ADD,   MORE_ADD,   MORE_ADD,   NOELEMENT,   NOELEMENT};
ENTRYELEMENT lineAddEntryNotes[9]={NOTES, NOTES, NOTES,NOTES,      NOTES,      NOTES,      NOTES,      NOELEMENT,  BUTTONX};
ENTRYELEMENT lineRestrictions[9] ={MORE_LABEL, MORE_LABEL, MORE_LABEL,RESTRICTION,RESTRICTION,RESTRICTION,RESTRICTION,NOELEMENT,  BUTTONX};
ENTRYELEMENT linePhotos[9]      = {PHOTO,PHOTOFILE,PHOTOFILE,PHOTONOTES,PHOTONOTES,PHOTONOTES,PHOTONOTES,NOELEMENT,BUTTONX};

TextEdit2::TextEdit2(QWidget *parent) : QPlainTextEdit(parent)
{

}

QSize TextEdit2::minimumSizeHint() const
{
    return QSize(100,21);
}

QSize TextEdit2::sizeHint() const
{
    quint16 h;

    int fontSizeHeight = QFontMetrics(this->document()->defaultFont()).height();
    QSizeF docSize = this->document()->size();
    h = ((docSize.height() * fontSizeHeight) +8);

    return QSize(600,h);

}

quint8 EntryBox::getType()
{
    if (!this->entry) return 0;
    return (quint8)entry->type;
}

EntryBox::EntryBox(Person *p)
{
    DE_BUG "entrybox::entrybox";

    entryId = 0;
    photoId = 255; // 255 tarkoittaa -1 => ei kuvaa

    par = p;
    grid = new QGridLayout();
    this->setLayout(grid);
    entry               = nullptr;
    uiComboType         = nullptr;
    uiDate1             = nullptr;
    uiDate2             = nullptr;
    boxId               = (quint8)par->visibleTabWidget->pEntryBox.size();
    usrEventModel = &par->indiEventModel;// onko n�m� samat sek� indi ett� fam tiedoille??
    ownEvents     = &usrEvents;

    grid->setColumnStretch(0,1);    // type combo
    grid->setColumnStretch(1,1);    // date accuracy combo
    grid->setColumnStretch(2,1);    // date1 combo
    grid->setColumnStretch(3,1);    // date2 combo
    grid->setColumnStretch(4,50);   // value1
    grid->setColumnStretch(5,50);   // value
    grid->setColumnStretch(6,1);    // + button
    grid->setColumnStretch(7,1);    // x button
}


PhotoWidget::PhotoWidget(QString fn)
{
    DE_BUG "(in) PhotoWidget::PhotoWidget";

    fileName = fn;

    if (fileName.left(2) != "\\\\") {
        if (!bmpOrginal.load(fileName, ".jpg", Qt::AutoColor)) { qDebug() << "failed to download photo @ person " << fileName; } // t�ss� oli return false
        else {
            bmpScaled = bmpOrginal.scaled(100, 100, Qt::KeepAspectRatio);
            setFixedSize(bmpScaled.width(), bmpScaled.height()); // ilman t�t� voi widgetin koko olla joskus liian iso
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            picture = new QGraphicsScene();
            picture->addPixmap(bmpScaled);
            setScene(picture);
        }
    }
    DE_BUG "PhotoWidget::PhotoWidget (out)";
}


QDialog2::QDialog2(QWidget *parent) : QDialog(parent)
{
    DE_BUG "qdialog2::qdialog2";
}

void QDialog2::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED( event);
    this->deleteLater();
}

void PhotoWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED( event );

    QDialog2 *w = new QDialog2(this);
    QGraphicsView *imgW = new QGraphicsView(w);

    imgW->setFixedSize(bmpOrginal.width(), bmpOrginal.height()); // ilman t�t� voi widgetin koko olla joskus liian iso
    imgW->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    imgW->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsScene *pic = new QGraphicsScene();

    pic->addPixmap(bmpOrginal);
    imgW->setScene(pic);
    w->show();
}

void Person::privacyPolicyChanged(int index)
{
    DE_BUG "Person::privacyPolicyChanged" << index;
    if (visibleTabWidget->indi)
            INDI(id).privacyPolicy = index;
    else FAM(INDI(id).getFamS()).privacyPolicy = index;

    backGroundColorBoxes();
    DE_BUG "out Person::privacyPolicyChanged";
}


void Person::slotNextTab()
{
    uiTabGroup->setCurrentIndex(uiTabGroup->currentIndex()+1);
}

void Person::slotPrevTab()
{
    uiTabGroup->setCurrentIndex(uiTabGroup->currentIndex()-1);
}

void Person::slotFocus()
{
    DE_BUG "person::slotfocus";
    setFocus();
    //uiMessage->setText("b:Birth, D:Death, S:save, f1-f4:names, f:female, m:male");
}

 quint8 Person::guiBoxOfEntryType(quint8 entryType)
{
     DE_BUG "person::guiboxofentrytype";
    for (quint8 b = 0 ; b < visibleTabWidget->pEntryBox.size() ; b++)
        if ( visibleTabWidget->pEntryBox.at(b)->getType() == entryType) return b;

    return 255;
}

void Person::slotShortcut1() { shortCutToEventN(1); }

void Person::slotShortcut2() { shortCutToEventN(2); }

void Person::slotShortcut3() { shortCutToEventN(3); }

void Person::slotShortcut4() { shortCutToEventN(4); }

void Person::slotShortcut5() { shortCutToEventN(5); }

void Person::slotShortcut6() { shortCutToEventN(6); }

void Person::slotShortcut7() { shortCutToEventN(7); }

void Person::slotShortcut8() { shortCutToEventN(8); }

void Person::slotShortcut9() { shortCutToEventN(9); }

void Person::slotShortcut0() { shortCutToEventN(10); }

void Person::shortCutToEventN(quint8 n)
{
    if ( n<INDI(id).entry.size()) {
        ENTRY entryType = (ENTRY)INDI(id).entry.at(n-1).type;
        indiEntryShortcutSelected(entryType);
    }
}

void EntryLine::newEventPhoto()
{
    DE_BUG "void EntryLine::newEventPhoto()";

    if (GENE.fileName.left(4) == "<new") {

        QMessageBox msgBox;
        msgBox.setText("Save the document, add the photos to the\nsame directory, and finally add the photos\nto the database.");
        msgBox.exec();

        return;
    }

    QString fileName = NULL;
    if (this->getPhotoFile(&fileName)) {
        this->entryBox->entry->multiMedia.file = fileName; //        pEvent->multiMedia.file = fileName;
    }
    deleteUiWidgets();
    makeUi(this->entryBox->grid);

    DE_BUG "out void EntryLine::newEventPhoto()";
}

EventFrame::EventFrame(TabWidget *ea)
{
    tabWidget = ea;
}


void EntryLine::slotChangePhoto()
{
    DE_BUG "void EntryLine::slotChangePhoto()" << ID;

    QString fileName = "";//NULL;

    if (getPhotoFile(&fileName)) {
        INDI(ID).multiMedia[entryBox->photoId].file = fileName;
        deleteUiWidgets();
        makeUi(entryBox->grid);
    }

    DE_BUG "out void Person::slotChangePhoto()";
}

void EntryLine::slotNewPhoto()
{
    DE_BUG "void EntryLine::slotNewPhoto()" << ID << this->entryBox->photoId;
    QString fileName = NULL;

    MultiMedia mm;

    if (getPhotoFile(&fileName)) {
        mm.file = fileName;
        entryBox->photoId = INDI(ID).multiMedia.size();
        INDI(ID).multiMedia.append(mm);

        deleteUiWidgets();
        makeUi(entryBox->grid);
    }

    DE_BUG "out void Person::slotNewPhoto()";
}

void Person::indiEntryShortcutSelected(ENTRY entryType)
{
    // isIEntry palauttaa tiedon onko entryType jo k�yt�ss�. Palautusarvo on entry numero
    quint8 entryId = INDI(this->id).isIEntry(entryType);
    quint8 t = this->visibleTabWidget->pEntryBox.last()->eventMapping.indexOf(entryType);

    if ( entryId  == 255) {
/* kutsutaan alimman boxin (eli sen jossa on "add entry") ensimm�isen entrylinen
 * (eli se joka on se addentry) metodia "addEntry"
 */
        visibleTabWidget->pEntryBox.last()->pEntryLine.at(0)->addEntry(t);
    }
}

void Person::famEntryShortcutSelected(ENTRYf entryType)
{
    quint8 entryId = FAM(INDI(id).getFamS()).isFEntry(entryType);
    quint8 t = this->visibleTabWidget->pEntryBox.last()->eventMapping.indexOf(entryType);

    if ( entryId == 255) {
        visibleTabWidget->pEntryBox.last()->pEntryLine.at(0)->addEntry(t);
    }
}

void Person::slotShortcutB()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 0) indiEntryShortcutSelected(BIRTH);
}

void Person::slotShortcutD()
{
    quint8 tab = uiTabGroup->currentIndex();

    if (tab == 0) indiEntryShortcutSelected(DEATH);
    if (tab > 0) famEntryShortcutSelected(DIVORCE);

    debugEvents();
}

void Person::slotShortcutO()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 0) indiEntryShortcutSelected(OCCUPATION); // muuttuu resident (18), po 14
}

void Person::slotShortcutE()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 0) indiEntryShortcutSelected(EMIGRATION);
    if (tab == 1) famEntryShortcutSelected(ENGAGEMENT);
}

void Person::slotShortcutC()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 0) indiEntryShortcutSelected(CHRISTENING);
    if (tab == 1) famEntryShortcutSelected(MARRIAGE_CONTRACT);
}

void Person::slotShortcutG()
{    quint8 tab = uiTabGroup->currentIndex();
     if (tab == 0) indiEntryShortcutSelected(GRADUATION);
}

void Person::slotShortcutA()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 0) indiEntryShortcutSelected(ADOPTATION); // muuttuu retirement 13, po 12
    if (tab == 1) famEntryShortcutSelected(ANNULMENT);
}

void Person::slotShortcutU()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 0) indiEntryShortcutSelected(USEREVENT);
}

void Person::slotShortcutW()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 0) indiEntryShortcutSelected(WILL); // muuttuu graduated (9), po 8
}

void Person::slotShortcutR()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 0) indiEntryShortcutSelected(RETI); // muuttuu occupation 14, po 13
}

void Person::slotShortcutP()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 0) indiEntryShortcutSelected(PROPERTY); // kr�sh
}

void Person::slotShortcutN()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 0) indiEntryShortcutSelected(NATIONALITY); // muuttuu phy desc (20) po 16
}

void Person::slotShortcutT()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 0) indiEntryShortcutSelected(TITLE); // ckr�s
}

void Person::slotShortcutMM()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 1) famEntryShortcutSelected(MARRIAGE);
}

void Person::slotShortcutI()
{
    quint8 tab = uiTabGroup->currentIndex();
    if (tab == 0) indiEntryShortcutSelected(IMMIGRATION);
}

void Person::slotShortcutF1()
{
    uiNameFamily->setFocus();
}

void Person::slotShortcutF2()
{
    uiName1st->setFocus();
}

void Person::slotShortcutF3()
{
    uiName2nd->setFocus();
}

void Person::slotShortcutF4()
{
    uiName3rd->setFocus();
}

void Person::slotShortcutEnter() { clearFocus(); }

void Person::breakRelationshipToFather()
{
    DE_BUG "void Person::breakRelationshipToFather()" << id;

    GENE.copyToEditUndo(INDI(id).getFather());
    INDI(id).assignFather(0);
    emit sigUpdate(0);

    DE_BUG "out void Person::breakRelationshipToFather()";
}

void Person::breakRelationshipToMother()
{
    DE_BUG "void Person::breakRelationshipToMother()" << id;

    GENE.copyToEditUndo(INDI(id).getMother());
    INDI(id).assignMother(0);
    emit sigUpdate(0);

    DE_BUG "out void Person::breakRelationshipToMother()";
}

void Person::breakRelationshipToSpouse()
{
    DE_BUG "void Person::breakRelationshipToSpouse()" << id;

    GENE.copyToEditUndo(INDI(id).getSpouse());
    if (FAM(INDI(id).getFamS()).nchi) {
        QMessageBox qmb;
        qmb.setText("This couble has children. Break first relationship of the children to their parents.");
        qmb.exec();
        return;
    }
    if (INDI(id).sex == FEMALE) {
        INDI(INDI(id).getSpouse()).famSList.removeAt(INDI(id).currentSpouse);
        FAM(INDI(id).getFamS()).husb = 0;
    }
    if (INDI(id).sex ==   MALE) {
        INDI(INDI(id).getSpouse()).famSList.removeAt(INDI(id).currentSpouse);
        FAM(INDI(id).getFamS()).wife = 0;
    }
    FAM(INDI(id).getFamS()).deleted = true;
    INDI(id).famSList.removeAll(INDI(id).getFamS());
    emit sigUpdate(0);

    DE_BUG "out void Person::breakRelationshipToSpouse()";
}

void Person::assignNewPersoninDb()
{
    DE_BUG "void Person::assignNewPersoninDb()" << id;

    DbView2 dbv(INDI(id).estimatedBirth, showMales, showFemales);

    quint16 newId = dbv.exec();

    if (newId) {
        // aiemmin on luotu uusi henkil�, jota ei nyt k�ytet�k��n. Tyhjennet��n siis viimeisin tietokantaolento
        INDI(GENE.indiLastUsed).clear(geneData);
        GENE.indiLastUsed--;
        GENE.copyToEditUndo(newId);
        if (INDI(newId).getSpouse()) GENE.copyToEditUndo(INDI(newId).getSpouse());
        this->id = newId;
        emit sigPersonAssigned(newId);
        emit sigUpdate(0);
        setValues();
    }

    DE_BUG "out void Person::assignNewPersoninDb()";
}

void EntryLine::deleteThisPerson()
{
    quint32 id;
    quint16 i=0;

    id = this->entryBox->par->id;
    DE_BUG "void Person::deleteThisPerson()" << id;

    // Called by "delete button". Delete this person.


    // etsit��n henkil�� joka pistet��n current box:n

    //if (GENE.currentId == id) {
        if (INDI(id).getSpouse() != 0) i = INDI(id).getSpouse();
        else if (INDI(id).getFather() != 0) i = INDI(id).getFather();
        else if (INDI(id).getMother() != 0) i = INDI(id).getMother();
        else {
            QList<quint16> childIdList;
            INDI(id).getChilds(&childIdList, false);

            if (childIdList.size()>0) i=childIdList.at(0);
        }
    //}
    //else i = GENE.currentId;

    INDI(id).deleteThis();

    if (i==0) do { i++; } while (INDI(i).deleted);

    // Koska t�m� henkil� tuhottiin, suljetaan my�s person-ikkuna ja annetaan return arvona
    // sen henkil�n nimi joka pistet��n currentiksi. Jos t�m� henkil� oli current kun h�net tuhottiin,
    // pit�� siis currentiksi pist�� joku toinen henkil� joka arvottiin tossa just �skett�in.

    DE_BUG "out void Person::deleteThisPerson()" << i;

    this->entryBox->par->done(i);
    //done(i);
}

// -------------------------------------------------------------------------------------------------

bool EntryLine::getPhotoFile(QString *fileName)
{
    DE_BUG "void Person::getPhotoFile(QString *fileName)" << ID;

    if (!GENE.isSaved) {
        QMessageBox msgBox;
        msgBox.setText("Save the document, add the photos to the\nsame directory, and finally add the photos\nto the database.");
        msgBox.exec();

        return false;
    }

    *fileName = QFileDialog::getOpenFileName(
            entryBox->par,
            tr("Open Picture"),
            GET_DIRECTORY,
            tr("(*.*)"));

    if (fileName->contains(" ")) {
        QMessageBox msgBox;
        msgBox.setText("Oops, I don't like filenames that has a spacebar in it. Please go and change the name so that there's no space bars.");
        msgBox.exec();
        return false;
    }

    // Lets separate default dir from a filename and save only filename.
    // e.g. fileDiractory = c:\dir\ecotry, file = c:\dir\ecotry\pic\tures, lets find "pic\tures" and save only that.

    if ( fileName->indexOf(GET_DIRECTORY) != -1 )
        *fileName = fileName->right(fileName->length() - GET_DIRECTORY.length()-1);

    if (fileName != NULL) return true; else return false;

    DE_BUG "out void Person::getPhotoFile(QString *fileName)";
}


void EntryBox::makeEntryLine(ENTRYLINETYPE elt)
{
    DE_BUG "entrybox::makeentryline";
    // lis�t��n yksi lis�rivi.

    EntryLine *line = new EntryLine(this);
    line->entryLineType = elt;
    line->row = pEntryLine.size();

    pEntryLine.append(line);
    line->makeUi(grid);
}

void Person::makeEntryBox(quint8 i, quint8 i2, ENTRYLINETYPE elt)
{
    DE_BUG "Person::menuEntryBox " << i << i2 << elt;
    EntryBox *box = new EntryBox(this);
    visibleTabWidget->pEntryBox.append(box);
 //   box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    box->entryId = i;
    box->photoId = i2;
    if (elt==ENTRYINFO_BASIC && i!=255) box->entry = &(*visibleTabWidget->entrys)[i];

    box->makeEntryLine(elt);

    if (i != 255)  {
        Entry entry = *box->entry;

        if (entry.address.line != "" || entry.showAddress0Line) box->makeEntryLine(ENTRYINFO_ADDRLINE);
        if (entry.address.post != "" || entry.address.city != "" || entry.showPostCityLine ) box->makeEntryLine(ENTRYINFO_POST_CITY);
        if (entry.address.state != "" || entry.address.country != "" || entry.showstCnrPhoLine ) box->makeEntryLine(ENTRYINFO_STATE_COUNTRY);
        if (entry.source.note.text != "" || entry.showSourceTextLine) box->makeEntryLine(ENTRYINFO_SOURCE_TEXT);
        if (entry.source.id || entry.source.page != "" || entry.source.quay || entry.showSourcePgQltyLine ) box->makeEntryLine(ENTRYINFO_SOURCE_PAGE_QUALITY);
        if (entry.note.text != "" || entry.showNotes) box->makeEntryLine(ENTRYINFO_NOTES);
        if (entry.multiMedia.used()) box->makeEntryLine(ENTRYINFO_PHOTO);
        if (entry.privacyPolicy) box->makeEntryLine(ENTRYINFO_RESTRICTIONS);
        if (entry.showAddEntryMore) box->makeEntryLine(ENTRYINFO_ADD);
    }

    addEntryBox(box);
}

void Person::addIndiEvents()
{
    // t�t� kutsutaan silloin, kun indi-tapahtumia ei viel� ole ja ne pit�isi kaikki lis�t�. T�ss� on looppi joka lis�� kaikki. Loppuun list��n viel�
    // addevent combo

    DE_BUG "void Person::addIndiEvents()" << id;

    quint8 i;

    if (INDI(id).note.used()) makeEntryBox(255, 255, ENTRYINFO_NOTES);
    for (i=0 ; i<INDI(id).multiMedia.size() ; i++) makeEntryBox(255, i, ENTRYINFO_PHOTO);
    if (INDI(id).source.page != "" || INDI(id).source.quay  || INDI(id).source.id ) makeEntryBox(255, 255, ENTRYINFO_SOURCE_PAGE_QUALITY);
    if (INDI(id).source.note.text != "") makeEntryBox(255, 255, ENTRYINFO_SOURCE_TEXT);
    for (i=0 ; i<INDI(id).entry.size() ; i++) makeEntryBox(i, 255, ENTRYINFO_BASIC);

    addNewEntryComboAndBox();

    DE_BUG "out void Person::addIndiEvents()";
}

void EntryBox::setBackGroundColour()
{
    quint8 privacy, entryPrivacy;
    if (par->visibleTabWidget->indi) privacy = INDI(par->id).privacyPolicy; else privacy = FAM(INDI(par->id).getFamS()).privacyPolicy;

    if (this->entry)
        entryPrivacy = entry->privacyPolicy;
    else entryPrivacy = 0;

    if ( (privacy != 0) || (entryPrivacy != 0)  )
        // T�m� boxi on jonkun privacyPolicyn vaikutuksen alainen. Piiirret��n punaiset reunukset.
         if (boxId & 1) setStyleSheet("background-color:rgb(255,255,153);border-style: outset;border-width: 4px;border-color: red;");
         else setStyleSheet("background-color:rgb(255,255,190);border-style: outset;border-width: 4px;border-color: red;");
    else {
        // Ihan normiboxi. Piirret��n harmaat reunukset.
         if (boxId & 1) setStyleSheet("background-color:rgb(255,255,153);border-style: outset;border-width: 2px;border-color: gray;");
         else setStyleSheet("background-color:rgb(255,255,190);border-style: outset;border-width: 2px;border-color: gray;");
    }
}

/* If userEdit is true, go through all boxes and colour them
 */
void Person::backGroundColorBoxes()
{
    quint8 b;
    for (b=0 ; b<this->visibleTabWidget->pEntryBox.size() ; b++) {
        visibleTabWidget->pEntryBox.at(b)->boxId = b;
        visibleTabWidget->pEntryBox.at(b)->setBackGroundColour();
    }
}

void Person::addEntryBox(EntryBox *box)
{    
    DE_BUG "person::addentrybox";
    visibleTabWidget->eventFrame->layout()->addWidget(box);
    backGroundColorBoxes();
}

void Person::addFamilyEvents()
{
    // t�t� kutsutaan, kun family eventtej� ei viel� ole ja ne pit�� lis�t�. T�ss� on looppi joka lis�� kaikki. Loppuun list��n viel�
    // addevent combo
    DE_BUG "person::addfamilyevents" << id;

    if (FAM(INDI(id).getFamS()).note.used()) makeEntryBox(255, 255, ENTRYINFO_NOTES);
    for (quint8 f=0 ; f<FAM(INDI(id).getFamS()).multiMedia.size() ; f++) makeEntryBox(255, f, ENTRYINFO_PHOTO);
    if (FAM(INDI(id).getFamS()).source.page != "" || FAM(INDI(id).getFamS()).source.quay  || FAM(INDI(id).getFamS()).source.id ) makeEntryBox(255, 255, ENTRYINFO_SOURCE_PAGE_QUALITY);
    if (FAM(INDI(id).getFamS()).source.text != "") makeEntryBox(255, 255, ENTRYINFO_SOURCE_TEXT);
    for (quint8 e=0 ; e<FAM(INDI(id).getFamS()).entry.size() ; e++ ) makeEntryBox(e, 255, ENTRYINFO_BASIC);

    // *A* uiFamRestriction->setCurrentIndex((int)FAM(INDI(id).getFamS()).privacyPolicy);

    if (INDI(id).getFamS() != 0) this->uiBreakSpouse->setEnabled(true);
    addNewEntryComboAndBox();

    DE_BUG "out void Person::addFamilyEvents()";
}


void Person::save()
{
    DE_BUG "void Person::save()" << id;

    SEX proposedSex = INDI(id).sex;

    GENE.indiSpace(id);

    if (!uiMale->isChecked() && !uiFemale->isChecked() && GET_SUGGEST_SEX) {
        if (boyNames.contains( uiName1st->text() )) INDI(id).sex = MALE;
        if (girlNames.contains( uiName1st->text() )) INDI(id).sex = FEMALE;
    }
    else {
        if (uiFemale->isChecked()) INDI(id).sex = FEMALE;
        if (uiMale->isChecked()) INDI(id).sex = MALE;
    }



// ----------------------------- check if user has changed sex from proposed --------------------------
// --- if current family and his/her spouse do not have other spouses, the sex of the spouse will be
// --- changed to correspond to the change. However, if there are more spouses, the change wont be done.
// --- For a user it might be a bit tricky to change all coubles to right gender.


      if ( INDI(id).sex != proposedSex) {
          if ( INDI(id).sex == MALE ) INDI(INDI(id).getSpouse()).sex = FEMALE; else INDI(INDI(id).getSpouse()).sex = MALE;
          if ( ( INDI(id).sex == MALE && INDI(id).getWife() == id ) || ( INDI(id).sex == FEMALE && INDI(id).getHusb() == id ) ) {
              quint16 w = INDI(id).getWife();
              quint16 h = INDI(id).getHusb();
              INDI(id).setWife(h);
              INDI(id).setHusb(w);
        }
    }


// ------------------------- fams, famc, busb, wife data is ok, lets fill the persondata table --------------

    QRegExp spaceRemove("(\\S+)");
	
    //spaceRemove.indexIn(uiName1st->text()) ; INDI(id).nameFirst = spaceRemove.cap(1);
    //spaceRemove.indexIn(uiName2nd->text()) ; INDI(id).name2nd = spaceRemove.cap(1);
    //spaceRemove.indexIn(uiName3rd->text()) ; INDI(id).name3rd = spaceRemove.cap(1);
    INDI(id).nameFirst = uiName1st->text().replace(" ","_");
	INDI(id).name2nd = uiName2nd->text().replace(" ","_");
	INDI(id).name3rd = uiName3rd->text().replace(" ","_");
	spaceRemove.indexIn(uiNameFamily->text()) ; INDI(id).nameFamily = spaceRemove.cap(1);

    for (quint8 b=0 ; b<visibleTabWidget->pEntryBox.size() ; b++) visibleTabWidget->pEntryBox.at(b)->deleteLater();

    // otetaan lisan pohjalla oleva "add" pois
    usrEvents.removeLast();
    famEvents.removeLast();

    INDI(id).changed.setDate(QDate::currentDate());
    INDI(id).changed.setTime(QTime::currentTime());

    // Estimated birthday on laskettu jokatapauksessa. Se voi olla laskettu perustuen
    // sukulaistietoihin, jos k�ytt�j� on kuitenkin antanut erikseen syntym�p�iv�n, on
    // t�llainen sukulaistietoihin perustuva arvio nyt tarpeeton. Tuhontaan siis se,
    // estimatedBirth lasketaan uudelleen seuraavaksi kun db demonia kutsutaan
    if (INDI(id).ientry(BIRTH).day.known) INDI(id).estimatedBirth.clear();

    // t�m� tallentaa valitun entryn

    // annetaan palautusarvona henkil�n id

    DE_BUG "out void Person::save()" << id;

    this->done(id);
}

void Person::closeWithoutSaving()
{
    DE_BUG "void Person::closeWithoutSaving()" << id;

    // nuo *** merkityt oli k�yt�ss�, mutta tarvitaanko niit�?
    //***for (quint8 b=0 ; b<visibleTabWidget->pEntryBox.size() ; b++) visibleTabWidget->pEntryBox.at(b)->deleteLater();
    // otetaan lisan pohjalla oleva "add" pois
    //***usrEvents.removeLast();
    //***famEvents.removeLast();

    // annetaan palautusarvona 0, joka merkitsee sit� ettei tallennettu

    DE_BUG "out void Person::closeWithoutSaving()" << 0;

    this->done(0);
}

// ------------------------- entryline--------------------------------------------


/* This slot will be called when a user press + button for adding new info into an event
 * A combo box for selecing new info class will be added to the ui
 */
void EntryLine::slotEntryMore()
{
    DE_BUG "void EntryLine::slotEntryMore()";

    // t�� on melkein samanlainen kuin EntryBox::makeUi, mutta em. on tarkoitettu koko boxin
    // uudelleenpiirtoon. Jos sit� k�ytt�isi t�ss�, tulisi kaikki event rivit toiseen kertaan
    EntryLine *line = new EntryLine(entryBox);
    line->entryLineType = ENTRYINFO_ADD;
    line->row = this->entryBox->pEntryLine.size();
    entryBox->pEntryLine.append(line);
    entryBox->entryId = 0;
    entryBox->photoId = 255;
    line->makeUi(line->entryBox->grid);

    DE_BUG "out void EntryLine::slotEntryMore()";
}

/* Remove widgets in this box, but leave the box
 */
void EntryLine::deleteUiWidgets()
{
    DE_BUG "entrylne::deleteuiwidgets";
    for (quint8 w=0 ; w<uiWidgets.size() ; w++) {
        uiWidgets.at(w)->deleteLater();

    }
    uiWidgets.clear();
}

/* Called when this entryLine UI should be deleted. For example, when user change the type of this event
 */
void EntryLine::deleteUi()
{
    DE_BUG "void EntryLine::deleteUi()" << uiWidgets.size();

    deleteUiWidgets();

    if (entryBox->pEntryLine.size() == 1) {
        entryBox->deleteLater();
        entryBox->par->backGroundColorBoxes();
    }

    DE_BUG "out void EntryLine::deleteUi()";
}

/* Called, when user change the event type by changing the uiComboType selection
 */
void EntryLine::slotTypeChanged(int newType)
{    
    DE_BUG "entryline::slotTypechanged" << newType << this->entryBox->getType();

    if (entryBox->getType() == 0) this->addEntry(newType); // on painettu "add event":�
    else
    if (newType >= 0) { // t�ss� oli ennest�� jo joku eventti, jonka tyyppi muutetaan

        entryBox->entry->type = entryBox->eventMapping.at(newType);
        deleteUiWidgets();
        makeUi(entryBox->grid); // luoda uudelleen, koska uudessa event tyypiss� voi olla eri rivin sis�lt�
        entryBox->par->updateTypes();
    }

    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotTypeChanged(int newType)";
}

void EntryLine::slotDeleteEntry()
{    
    DE_BUG "void EntryLine::slotDeleteEntry()";

    if (*bINDI) INDI(ID).entry.removeAt(entryBox->entryId);
    else FAM(INDI(ID).getFamS()).entry.removeAt(entryBox->entryId);

    quint8 boxId = entryBox->par->guiBoxOfEntryType(entryBox->getType());
    entryBox->par->visibleTabWidget->pEntryBox.removeAt(boxId);
    entryBox->deleteLater();

    entryBox->par->visibleTabWidget->pEntryBox.removeAt(entryBox->boxId);

    DE_BUG "out void EntryLine::slotDeleteEntry()";
}

void EntryLine::slotAccChanged(int acc)
{
    DE_BUG "entryline::slotaccchanged";

    entryBox->entry->day.setAcc((ACCURACY)acc);
    if (acc == 0) {
        entryBox->entry->day.day1 = QDate();
        entryBox->entry->day.day2 = QDate();
        entryBox->entry->day.known = false;
    }
    else if (acc != 7) {
        entryBox->entry->day.day2 = entryBox->entry->day.day1;
    }


    //enum ACCURACY { 0 NA, 1 ABT, 2 CAL, 3 EST, 4 BEF, 5 AFT, 6 EXA, 7 FRO };

    deleteUiWidgets();
    makeUi(entryBox->grid);

    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotAccChanged(int acc)";
}

/* User has selected add new entry combo for selecting a new entry. So lets do that new entry and draw UI for it
 */
void EntryLine::addEntry(int index)
{
    DE_BUG "void EntryLine::addEntry(int index)" << index;

    // uiComboType asetettiin kun t�h�n boxiin piirrettiin "add new" combo. Kuitenkin, t�t� uiComboType:� k�ytet��n
    // my�hemmin ainakin updatetype:ss�, ja jos nyt valitaan jokin tieto johon ei liity entry�, eli uiComboType:� ei m��ritet�
    // toimii updatetype v��rin jos se luulee t�ss� olevan "add new":n
    entryBox->uiComboType = NULL;
    if (index == -1) return; // -1 tulee jos model:a muutetaan

    quint8 selection = entryBox->eventMapping.at(index);

    if ( selection > 27 ) {
        // n�ihin entry lineihin ei litty entry muuttujaa
        // 28 == common photo
        // 29 == common note
        // 30 == common source
        // 31 == common source id / pg / quality
        if ( selection == 28 ) { // photo

            entryLineType = ENTRYINFO_PHOTO;
        }
        if ( selection == 29 ) {
            if (*bINDI) INDI(ID).note.refn = "used"; // t�m� on v�liaikainen nootti, jotta tiedet��n ett� k�ytt�j� haluaa k�ytt�� notea
            else FAM(INDI(ID).getFamS()).note.refn = "used";
            entryLineType = ENTRYINFO_NOTES;
        }
        if ( selection == 30 ) { // source
            entryLineType = ENTRYINFO_SOURCE_TEXT;
            if (*bINDI) INDI(ID).source.refn = "used"; // sama kuin yll�
            else FAM(INDI(ID).getFamS()).source.refn = "used";
        }
        if ( selection == 31 ) { // source
            entryLineType = ENTRYINFO_SOURCE_PAGE_QUALITY;
            if (*bINDI) INDI(ID).source.refn = "used"; // sama kuin yll�
            else FAM(INDI(ID).getFamS()).source.refn = "used";
        }

        deleteUiWidgets();
        makeUi(entryBox->grid);
        entryBox->par->addNewEntryComboAndBox();
        entryBox->par->updateTypes();
    } else {
        Entry entry;
        entry.type = selection;
        entry.indi = *bINDI;
        entryBox->entryId = ENTRYS->size();
        ENTRYS->append(entry);
        entryBox->entry = &(ENTRYS->last());
        deleteUiWidgets();
        entryLineType = ENTRYINFO_BASIC;
        makeUi(entryBox->grid);
        entryBox->par->addNewEntryComboAndBox();
        entryBox->par->updateTypes();
    }

    bool foo; INDI(ID).estimatedBirth = INDI(ID).getEstimatedBirth(&foo);

    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::AddEntry(int index)" << index;
}

void EntryLine::date1changed(QDate date)
{
    DE_BUG "entryline::date1changed";

    if ( entryBox->entry->day.getAcc() != CAL ) {
        entryBox->entry->day.setDate1(date.year(), date.month(), date.day());
        if ( entryBox->entry->day.getAcc() != FRO ) entryBox->entry->day.setDate2(date.year(), date.month(), date.day());
    }

    // t�m� aiheuttaa sen, ett� kun jotain p�iv�m��r�� ollaan muuttamassa, asettaa se itse itselleen rajoitukssen

    bool foo; INDI(ID).estimatedBirth = INDI(ID).getEstimatedBirth(&foo);
    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::date1changed(QDate date)";
}

void EntryLine::date2changed(QDate date)
{
    DE_BUG "entryline::date2changed";

    entryBox->entry->day.setDate2(date.year(), date.month(), date.day());

    bool foo;
    INDI(ID).estimatedBirth = INDI(ID).getEstimatedBirth(&foo);

    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::date2changed(QDate date)";
}


void EntryLine::placeChanged()
{
    DE_BUG "entryline::placechanged";

    QString value = getChangedText((TextEdit2*)(QObject::sender()));
    entryBox->entry->place = value;
    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::placeChanged(QString place)";
}

void EntryLine::attrValueChanged()
{
    DE_BUG "void EntryLine::attrValueChanged()";

    QString value = getChangedText((TextEdit2*)(QObject::sender()));
    entryBox->entry->attrText = value;
    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::attrValueChanged()";
}

void Person::updateDates()
{
    // Update min/max limits for widgets and update the calculated date values
    // Called by quite many..
    DE_BUG "person::updatedates" << id;
    int i, type;
    QDateEdit *date;

    for (i=0 ; i<visibleTabWidget->pEntryBox.size() ; i++) {
        if ( ( visibleTabWidget->pEntryBox.at(i)->getType() ) && (visibleTabWidget->pEntryBox.at(i)->pEntryLine.at(0)->entryLineType == ENTRYINFO_BASIC) ) {
            Entry entry;

            entry = *visibleTabWidget->pEntryBox.at(i)->entry;

            type = entry.type;

            if ( type > 0 ) {                
                QComboBox *typeCombo = visibleTabWidget->pEntryBox.at(i)->uiComboType;

                QPalette p = typeCombo->palette();
                if (    entry.privacyPolicy ||
                     (  visibleTabWidget->indi && INDI(id).privacyPolicy ) ||
                     ( !visibleTabWidget->indi && FAM(INDI(id).getFamS()).privacyPolicy ) ) {
                    p.setColor(QPalette::Base, QColor(255,100,100));
                    typeCombo->setPalette(p);
                }
                else {
                    p.setColor(QPalette::Base, Qt::white);
                    typeCombo->setPalette(p);
                }

                QComboBox *acc = visibleTabWidget->pEntryBox.at(i)->uiComboAcc; //visibleTabWidget->pEntryLine.at(i)->uiComboAcc;

                if (acc->currentIndex() != NA) {
                    date = visibleTabWidget->pEntryBox.at(i)->uiDate1;

                    if (acc->currentIndex() == CAL ) {
                        if (visibleTabWidget->indi && type == BIRTH ) {
                            date->setDate(INDI(id).getCalBirth());
                        } else {
                            QDate cDay;

                            // this returns either a real or estimated birth
                            QDate birth = INDI(id).getEntryDaybyType(visibleTabWidget->indi, BIRTH).day1;

                            int x;

                            if (visibleTabWidget->indi) x = GET_I_CAL(type);
                            else x = GET_F_CAL(type);

                            cDay = birth.addYears(x); //   cDay.setDate(birth.year() + x, birth.month(), birth.day());
                            date->setDate(cDay);
                        }
                    }
                    if (acc->currentIndex() == FRO ) {
                        //date2 = (QDateEdit*)(ITEM_AT(visibleTabWidget->pEntryLine.at(i)->grid, 4, row));
                        //date2->setMinimumDate(entry.day.day1);
                    }
                }
            }
        }
    }

    DE_BUG "out void Person::updateDates()";
}

void EntryLine::adopChanged(int newValue)
{
    DE_BUG "void EntryLine::adopChanged(int newValue)" << newValue;

    INDI(ID).adoptedBy = (ADOPTED_BY)newValue;

    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::adopChanged(int newValue)";
}

void EntryLine::slotUsrChanged(int newType)
{
    DE_BUG "void EntryLine::slotUsrChanged(int newType)" << newType;

    ACCURACY acc = entryBox->entry->getAcc();
    QComboBox *comboUsr;
    int comboColumn;

    if (acc == NA) comboColumn = 2;
    else if (acc != FRO) comboColumn = 3;
    else comboColumn = 5;

    comboUsr = (QComboBox*)ITEM_AT(entryBox->grid, comboColumn, this->row);

    if (newType == 255) {
        // on lis�tty uusi eventtyyppi valitsemalla add. Se muutttaa combon listaa, joka saa current indexin muuttumaan
        // arvoon -1. *!* t�t� voi varmaan soveltaa myös event typess�
        int index = entryBox->ownEvents->indexOf((*ENTRYS)[entryBox->entryId].attrType);
        comboUsr->blockSignals(true);
        comboUsr->setCurrentIndex(index);
        comboUsr->blockSignals(false);
    } else {
        if (comboUsr->currentText() == "Add") {
            QString input = QInputDialog::getText(0, tr("New Event"), tr("Enter Family Event Type"), QLineEdit::Normal);

            entryBox->ownEvents->removeLast();
            entryBox->ownEvents->append(input);
            entryBox->ownEvents->append("Add");
            entryBox->usrEventModel->setStringList(*entryBox->ownEvents);
            // t�m� pist�� current indexin juuri luotuun uuteen event-typeen
            comboUsr->setCurrentIndex(entryBox->ownEvents->size()-2);

        } else {
            entryBox->entry->attrType = comboUsr->currentText();
        }
    }

    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotUsrChanged(int newType)";
}

void EntryLine::slotAddrLineChanged()
{
    DE_BUG "void EntryLine::slotAddrLineChanged()";

    QString value = getChangedText((TextEdit2*)(QObject::sender()));

    entryBox->entry->address.line = value;
    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotAddrLineChanged()";
}

void EntryLine::slotPostChanged()
{
    DE_BUG "void EntryLine::slotPostChanged()";

    QString value = getChangedText((TextEdit2*)(QObject::sender()));

    entryBox->entry->address.post = value;

    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotPostChanged()";
}

void EntryLine::slotCityChanged()
{
    DE_BUG "void EntryLine::slotCityChanged()";

    QString value = getChangedText((TextEdit2*)(QObject::sender()));

    entryBox->entry->address.city = value;

    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotCityChanged()";
}

void EntryLine::slotStateChanged()
{
    DE_BUG "void EntryLine::slotStateChanged()";

    QString value = getChangedText((TextEdit2*)(QObject::sender()));
    entryBox->entry->address.state = value;
    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotStateChanged()";
}

void EntryLine::slotCountryChanged()
{
    DE_BUG "void EntryLine::slotCountryChanged()";

    QString value = getChangedText((TextEdit2*)(QObject::sender()));
    entryBox->entry->address.country = value;
    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotCountryChanged()";
}

void EntryLine::slotPhoneChanged()
{
    //Q_UNUSED( value );

}

void EntryLine::slotSourceChanged(int index)
{
    DE_BUG "void EntryLine::slotSourceChanged ";
    if (entryBox->entry) entryBox->entry->source.id = index;
    else {
        if (*bINDI) INDI(ID).source.id = index;
        else FAM(INDI(ID).getFamS()).source.id = index;
    }
    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotSourceChanged(int index)";
}

void EntryLine::slotSourcePageChanged()
{
    DE_BUG "void EntryLine::slotSourcePageChanged()";

    QString value = getChangedText((TextEdit2*)(QObject::sender()));

    if (entryBox->entry) entryBox->entry->source.page = value;

    else {
        if (*bINDI) INDI(ID).source.page = value;
        else FAM(INDI(ID).getFamS()).source.page = value;
    }
    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotSourcePageChanged()";
}

void EntryLine::slotSourceTextChanged()
{
    DE_BUG "void EntryLine::slotSourceTextChanged()";

    QString value = getChangedText((TextEdit2*)(QObject::sender()));

    if (entryBox->entry) entryBox->entry->source.note.text = value;

    else {
        if (*bINDI) INDI(ID).source.note.text = value;
        else FAM(INDI(ID).getFamS()).source.note.text = value;
    }
    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotSourceTextChanged()";
}

void EntryLine::slotSourceQualityChanged(int index)
{
    DE_BUG "void EntryLine::slotSourceQualityChanged(int index)";
    if (entryBox->entry) entryBox->entry->source.quay = (QUALITY)(index);
    else {
        if (*bINDI) INDI(ID).source.quay = (QUALITY)(index);
        else FAM(INDI(ID).getFamS()).source.quay = (QUALITY)(index);
    }
    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotSourceQualityChanged(int index)";
}


QString EntryLine::getChangedText(TextEdit2 *qte)
{
    qte->updateGeometry();
    return qte->toPlainText();
}

void EntryLine::slotPhotoNotesChanged()
{
    DE_BUG "void EntryLine::slotPhotoNotesChanged()";

    QString value = getChangedText((TextEdit2*)(QObject::sender()));

    if (entryBox->entry) entryBox->entry->multiMedia.note.text = value; // eventin kuvanootti
    else {
        if (entryBox->photoId == 255) return;
        if (*bINDI) INDI(ID).multiMedia[entryBox->photoId].note.text = value;
        else FAM(INDI(ID).getFamS()).multiMedia[entryBox->photoId].note.text = value;
    }
}

void EntryLine::slotNotesChanged()
{
    DE_BUG "void EntryLine::slotNotesChanged()";

    TextEdit2 * qpte = (TextEdit2*)(QObject::sender());
    QString value = getChangedText(qpte);

    if (this->entryBox->entry) entryBox->entry->note.text = value; // eventin notti
    else {
        // ei ole eventti, eli on oltava yleinen nootti
        if (*bINDI) INDI(ID).note.text = value;   // indin nootti
        else FAM(INDI(ID).getFamS()).note.text = value; // fam:n nootti
    }

    DE_BUG "out void EntryLine::slotNotesChanged()";
}

void EntryLine::slotPrivacyPolicyChanged(int value)
{
    DE_BUG "void EntryLine::slotPrivacyPolicyChanged(int value)";
    entryBox->entry->privacyPolicy = value;
    entryBox->setBackGroundColour();

    DE_BUG "out void EntryLine::slotPrivacyPolicyChanged(int value)";
}

void EntryLine::slotEntryAddMore(int index)
{
    DE_BUG "void EntryLine::slotEntryAddMore";
    switch (index) {
        case 0 : entryLineType = ENTRYINFO_NULL; break;
        case 1 : entryLineType = ENTRYINFO_ADDRLINE; break;
        case 2 : entryLineType = ENTRYINFO_POST_CITY; break;
        case 3 : entryLineType = ENTRYINFO_STATE_COUNTRY; break;
        case 4 : entryLineType = ENTRYINFO_SOURCE_PAGE_QUALITY; break;
        case 5 : entryLineType = ENTRYINFO_SOURCE_TEXT; break;
        case 6 : entryLineType = ENTRYINFO_NOTES; break;
        case 7 : entryLineType = ENTRYINFO_PHOTO; break;
        case 8 : entryLineType = ENTRYINFO_RESTRICTIONS; break;
    }
    entryBox->entry->clearEntryMoreShowFlags();

    deleteUiWidgets();
    makeUi(entryBox->grid);

    DE_BUG "out void EntryLine::slotEntryAddMore(int index)";
}

void EntryLine::slotEraseAddrLine()
{
    DE_BUG "void EntryLine::slotEraseAddrLine";
    entryBox->entry->address.line = "";
    deleteUi();

    DE_BUG "out void EntryLine::slotEraseAddrLine()";
}

void EntryLine::slotEraseNotes()
{
    DE_BUG "void EntryLine::slotEraseNotes()";
    if (entryBox->entryId == 255) {
        if (*bINDI) INDI(ID).note.clear();
        else FAM(INDI(ID).getFamS()).note.clear();
    } else entryBox->entry->note.clear();

    entryBox->par->visibleTabWidget->pEntryBox.removeAt(entryBox->boxId);
    deleteUi();

    DE_BUG "out void EntryLine::slotEraseNotes()";
}

void EntryLine::slotErasePostCity()
{
    DE_BUG "void EntryLine::slotEraseStateCountry()";
    entryBox->entry->address.post = "";
    entryBox->entry->address.city = "";

    deleteUi();

    entryBox->par->visibleTabWidget->pEntryBox.removeAt(entryBox->boxId);

    DE_BUG "out void EntryLine::slotErasePostCity()";

}

void EntryLine::slotEraseStateCountry()
{
    DE_BUG "void EntryLine::slotEraseStateCountry()";

    entryBox->entry->address.state = "";
    entryBox->entry->address.country = "";

    deleteUi();

    entryBox->par->visibleTabWidget->pEntryBox.removeAt(entryBox->boxId);

    DE_BUG "out void EntryLine::slotEraseStateCountry()";
}

/*
void EntryLine::slotErasePhoneChanged()
{

}*/

void EntryLine::slotErasesourePgQlty()
{
    DE_BUG "void EntryLine::slotErasesourePgQlty()";
    if (entryBox->entryId == 255) {
        if (*bINDI) INDI(ID).source.clear();
        else FAM(INDI(ID).getFamS()).source.clear();
    } else {
        if (*bINDI) {
            entryBox->entry->source.page = "";
            entryBox->entry->source.quay = (QUALITY)0;
            entryBox->entry->source.id = 0;
        } else {
            FAM(INDI(ID).getFamS()).source.page = "";
            FAM(INDI(ID).getFamS()).source.quay = (QUALITY)0;
            FAM(INDI(ID).getFamS()).source.id = 0;
        }
    }
    deleteUi();

    entryBox->par->visibleTabWidget->pEntryBox.removeAt(entryBox->boxId);

    DE_BUG "out void EntryLine::slotErasesourePgQlty()";
}

void EntryLine::slotEraseSourceText()
{    
    DE_BUG "void EntryLine::slotEraseRestriction()";
    if (entryBox->entryId == 255) {
        // jos t�m�n rivin notti ei liity mihink��n eventtiin, eli liittyy kaikkiin yhteisesti, on
        // noten tiedot tallennettu INDI muuttujaan. Muuten tiedot on tallennettu eventin omaan muuttujaan.
        if (*bINDI) {
            INDI(ID).source.note.text = "";
        } else {
            FAM(INDI(ID).getFamS()).source.note.text = "";
        }
    } else {
        entryBox->entry->source.note.text = "";//(*entrys)[entryBox->entryId].source.note.text = "";
    }
    deleteUi();

    entryBox->par->visibleTabWidget->pEntryBox.removeAt(entryBox->boxId);

    DE_BUG "out void EntryLine::slotEraseSourceText()";
}

void EntryLine::slotEraseRestriction()
{
    DE_BUG "void EntryLine::slotEraseRestriction()";
    if (*bINDI) entryBox->entry->privacyPolicy = 0;
    else FAM(INDI(ID).getFamS()).privacyPolicy = 0;
    deleteUi();

    entryBox->par->visibleTabWidget->pEntryBox.removeAt(entryBox->boxId);

    DE_BUG "out void EntryLine::slotEraseRestriction()";
}

void EntryLine::slotEraseEntryPhoto()
{
    DE_BUG "void EntryLine::slotEraseEntryPhoto()";

    // jos entryId on 255, k�sitell��n henkil�n varsinaista kuvaa eik� siis entry:n (kuten kuolema) kuvaa
    if (entryBox->entryId == 255) INDI(ID).multiMedia.value(entryBox->photoId).clear();
        else entryBox->entry->multiMedia.clear();
    deleteUi();

    entryBox->par->visibleTabWidget->pEntryBox.removeAt(entryBox->boxId);

    DE_BUG "out void EntryLine::slotEraseEntryPhoto()";
}

void EntryLine::makeUi(QGridLayout *grid)
// grid = sen eventboxin grid, johon eventit piirret��n
//## pit��k� t�ss� olla grid parametrin�, eik� se ole aina boxin grid
{
    DE_BUG "entryline::makeui" << entryBox->entryId << this->entryLineType << this->entryBox->photoId;

    // mik�s t�m� ele2 nyt olikaan???
    ENTRYELEMENT ele[9];
    ENTRYELEMENT *ele2;
    int i = 0;
    int add=0;        

    Entry *entry;

    entry = nullptr;
    ele2 = lineEmpty;
    DE_BUG "makeUI a" << *bINDI;
    if (entryBox->entryId != 255) {
        entry = &(*ENTRYS)[entryBox->entryId];
    };
    DE_BUG "makeui b";
    if (this->entryLineType == ENTRYINFO_BASIC) {
        if (entryBox->getType()>=18) {
            if (entry->day.getAcc() == 7) ele2 = line2Attr;
            if (entry->day.getAcc() >0 && entry->day.getAcc()<7) ele2 = line1Attr;
            if ( (entry->day.getAcc() == 0) && (entryBox->getType() != 0 )) ele2 = line0Attr;
        }
        else if ( (entryBox->getType() == 7 && *bINDI) || (entryBox->getType() == 11 && !(*bINDI))) {
            if (entry->day.getAcc() == 7 ) ele2 = line2Usre;
            if (entry->day.getAcc() >0 && entry->day.getAcc()<7) ele2 = line1Usre;
            if ( (entry->day.getAcc() == 0) && (entryBox->getType() != 0 )) ele2 = line0Usre;
        } else if (entryBox->getType() == 0) {
            ele2 = lineAddnew;
        } else if (entryBox->getType() != 13) {
            if (entry->day.getAcc() == 7) ele2 = line2days;
            if (entry->day.getAcc() >0 && entry->day.getAcc()<7) ele2 = line1days;
            if ( (entry->day.getAcc() == 0) && (entryBox->getType() != 0 )) ele2 = line0days;
        } else {
            if (entry->day.getAcc() == 7) ele2 = line2Adop;
            if (entry->day.getAcc() >0 && entry->day.getAcc()<7) ele2 = line1Adop;
            if ( (entry->day.getAcc() == 0) && (entryBox->getType() != 0 )) ele2 = line0Adop;
        }
    }

    // ele2 muuttujassa on se alussa m��ritetty array, jossa listataan mit� ui elementtej� t�h�n riviin tulee.
    DE_BUG "makeui c";
    if ( this->entryLineType == ENTRYINFO_ADDRLINE  )               ele2 = lineAddress;
    if ( this->entryLineType == ENTRYINFO_POST_CITY )               ele2 = linePostCity;
    if ( this->entryLineType == ENTRYINFO_STATE_COUNTRY )           ele2 = lineStateCountry;
    if ( this->entryLineType == ENTRYINFO_SOURCE_PAGE_QUALITY )     ele2 = lineSourcePgQlty;
    if ( this->entryLineType == ENTRYINFO_SOURCE_TEXT )             ele2 = lineSourceText;
    if ( this->entryLineType == ENTRYINFO_ADD )                     ele2 = lineAddEntryMore;
    if ( this->entryLineType == ENTRYINFO_NOTES )                   ele2 = lineAddEntryNotes;
    if ( this->entryLineType == ENTRYINFO_RESTRICTIONS )            ele2 = lineRestrictions;
    if ( this->entryLineType == ENTRYINFO_PHOTO )                   ele2 = linePhotos;
    if ( this->entryLineType == ENTRYINFO_LINE )                    ele2 = lineSpace;
    DE_BUG "makeui d";
    // miksi t�m� pit�� kopioida n�in?
    for (quint8 x=0 ; x<9 ; x++) ele[x] = ele2[x];
    DE_BUG "makeui e";
    // k�yd��n jokainen ele l�pi ja piirret��n ele:n koodattu ui elementti
    do {
        add = -1;

        // tutkitaan onko ele taulukossa on monta samaa ui elementti� per�kk�in. Per�kk�in olevien samojen
        // elementtien lukum��r� on add, ja add muuttujaa k�ytet��n ui elementtien strethaamiseen gridiss�
        do { add++; } while ( (ele[i] == ele[i+add+1] ) && (i+add !=8 ) );
    DE_BUG "makeui f" << i;
    DE_BUG ele[i];
        // ------------------------------------------------------------------------------------------------
        // Insert comboboxes of different types. Also fill in the value, if any.
        // ------------------------------------------------------------------------------------------------
        if (       ele[i] == TYPECOMBO
                || ele[i] == NEWCOMBO
                || ele[i] == ACCCOMBO
                || ele[i] == USRCOMBO
                || ele[i] == ADOPCOMBO
                || ele[i] == SOURCE
                || ele[i] == SOURCE_QLTY
                || ele[i] == RESTRICTION
                || ele[i] == MORE_ADD
                ) {
            QComboBox *combo = new QComboBox(entryBox->par);
            combo->blockSignals(true);
            combo->setFixedHeight(LINE_HEIGHT);
            combo->setStyleSheet("QComboBox { border-style: outset; border-width: 1px; border-color: gray; selection-color: white; selection-background-color: black;}");

            if (add==0) grid->addWidget(combo, row, i, 0); else grid->addWidget(combo, row, i, 1, 1+add, 0);

            if (ele[i] == TYPECOMBO ) {
                combo->setModel(&entryBox->typeModel);
                combo->setCurrentIndex(entryBox->eventMapping.indexOf(entryBox->getType()));
                connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotTypeChanged(int)));
                entryBox->uiComboType = combo;
                if (entry->privacyPolicy != PRIVACY_0_SHOW) {
                    QPalette p = combo->palette();
                    p.setColor(QPalette::Base, QColor(255,200,200));
                    combo->setPalette(p);
                }

            }

            if (ele[i] == NEWCOMBO ) {
                combo->setModel(&entryBox->typeModel);
                connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotTypeChanged(int)));
                entryBox->uiComboType = combo;
            }

            if (ele[i] == ACCCOMBO) {
                combo->setModel(&accModel);
                combo->setCurrentIndex(entry->day.getAcc());
                combo->setMinimumWidth(50);
                combo->setMaximumWidth(50);
                connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotAccChanged(int)));
                entryBox->uiComboAcc = combo;
            }

            if (ele[i] == USRCOMBO) {
                DE_BUG "usrCombo";
                combo->setModel(entryBox->usrEventModel);
                combo->setCurrentIndex( entryBox->ownEvents->indexOf( entryBox->entry->attrType));
                connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotUsrChanged(int)));
            }

            if (ele[i] == ADOPCOMBO) {
                combo->setModel(&adopModel);
                combo->setCurrentIndex((int)INDI(ID).adoptedBy);
                connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(adopChanged(int)));
            }

            if (ele[i] == SOURCE) {
                combo->setModel(&entryBox->par->sourceModel);
                int index;

                if (entryBox->entry) index = entry->source.id; else {
                    if (*bINDI) index = INDI(ID).source.id;
                    else index = FAM(INDI(ID).getFamS()).source.id;
                }
                combo->setCurrentIndex(index);
                connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSourceChanged(int)));
            }

            if (ele[i] == SOURCE_QLTY) {
                combo->setModel(&quayModel);
                int index;
                if (entryBox->entry) index = (int)entry->source.quay; else {
                    if (*bINDI) index = INDI(ID).source.quay;
                    else index = FAM(INDI(ID).getFamS()).source.quay;
                }
                combo->setCurrentIndex(index);
                connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSourceQualityChanged(int)));
            }

            if (ele[i] == RESTRICTION) {
                combo->setModel(&entryBox->par->entryRestModel);
                int index;
                if (entryBox->getType()) index = (int)entry->privacyPolicy; else {
                    if (*bINDI) index = INDI(ID).privacyPolicy;
                    else index = FAM(INDI(ID).getFamS()).privacyPolicy;
                }
                combo->setCurrentIndex(index);
                connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotPrivacyPolicyChanged(int)));
            }

            if (ele[i] == MORE_ADD) {
                combo->setModel(&entryBox->par->entryMoreModel);
                combo->setCurrentIndex(0);
                connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotEntryAddMore(int)));
            }
            combo->blockSignals(false);
            uiWidgets.append(combo);
        }

        // ------------------------------------------------------------------------------------------------
        // Adding day fields, and add the day values too and the min max limits for the boxes
        // ------------------------------------------------------------------------------------------------
        if (ele[i] == DAY1 ) {
            DE_BUG "day1" << ID;
            QDate toDay;
            toDay = QDate::currentDate();

            entryBox->uiDate1 = new QDateEdit(entryBox->par);
            entryBox->uiDate1->blockSignals(true);

            QDate date;

            // lasketaan tapahtuman pienimm�t ja suurimmat sallitut p�iv�t riippuen
            // ymp�rill� olevien ihmisten i�st� ja tapahtumap�ivist�

            Date datePeriod;

            INDI(ID).eventPeriod(&datePeriod, entryBox->getType(), *bINDI);

            if (entry->date1() < datePeriod.day1 && !entry->date1().isNull()) datePeriod.day1 = entry->date1();
            if (entry->date1() > datePeriod.day2 && !entry->date1().isNull()) datePeriod.day2 = entry->date1();

            // jos p�iv�� ei ole annettu, mutta tarkkuus on != CAL, pit�� n�ytt�� joku p�iv� eli CAL p�iv�.
            // Pit�� silloin kans pist�� tietokantaan vastaava p�iv�m��r�
            if (   entryBox->entry->date1().isNull() && entryBox->entry->getAcc() != NA) {

                if (*bINDI && entryBox->getType() == BIRTH) {
                    date = INDI(ID).getEntryDaybyType(true, BIRTH).day1;
                    if (date.isNull()) date = INDI(ID).getCalBirth();
                } else {
                    if (*bINDI) date = INDI(ID).getCalBirth().addYears(GET_I_CAL(entryBox->getType()));
                    else date = INDI(ID).getCalBirth().addYears(GET_F_CAL(entryBox->getType()));
                }

                if (date.isNull())
                    // jos t�m� on 1. henkil�, mit��n arvausp�iv�m��r�� ei ole voitu laskea,
                    // annetaan t�m� p�iv�
                    date = QDate::currentDate();

                // t�ss� kohtaa pistet��n laskettu p�iv�m��r�ehdotus myös kantaan

                if (( entryBox->entry->getAcc() != CAL) ) {
                    entryBox->entry->day.setDate1(date);
                    entryBox->entry->day.setDate2(date);
                }
            } else {
                DE_BUG "day1d";
                DE_BUG *bINDI;
                DE_BUG entryBox->entryId;
                date = INDI(ID).getEntryDay(*bINDI, entryBox->entryId).day1;
                DE_BUG "a";
            }

            entryBox->uiDate1->setDate(date);
            entryBox->uiDate1->setStyleSheet("QDateEdit { border-style: outset; border-width: 1px; border-color: gray; }");

            entryBox->uiDate1->setToolTip(
                        "Possible dates between " +
                        datePeriod.day1.toString("dd.MM.yyyy") +
                        " (" + datePeriod.day1explanation + ") and " +
                        datePeriod.day2.toString("dd.MM.yyyy") +
                        "(" + datePeriod.day2explanation + ")");

            entryBox->uiDate1->setFixedHeight(LINE_HEIGHT);
            entryBox->uiDate1->setFixedWidth(100);

            if (entry->day.getAcc() == 1) entryBox->uiDate1->setDisplayFormat(GET_ACC_ABT);
            if (entry->day.getAcc() == 2) entryBox->uiDate1->setDisplayFormat(GET_ACC_CAL);
            if (entry->day.getAcc() == 3) entryBox->uiDate1->setDisplayFormat(GET_ACC_EST);
            if (entry->day.getAcc() == 4) entryBox->uiDate1->setDisplayFormat(GET_ACC_BEF);
            if (entry->day.getAcc() == 5) entryBox->uiDate1->setDisplayFormat(GET_ACC_AFT);
            if (entry->day.getAcc() == 6) entryBox->uiDate1->setDisplayFormat(GET_ACC_EXA);
            if (entry->day.getAcc() == 7) entryBox->uiDate1->setDisplayFormat(GET_ACC_FRO);

            connect(entryBox->uiDate1, SIGNAL(dateChanged(QDate)), this, SLOT(date1changed(QDate)));

            grid->addWidget(entryBox->uiDate1, row, i, Qt::AlignLeft);

            if (entry->day.getAcc() == 0) entryBox->uiDate1->setEnabled(false); else entryBox->uiDate1->setEnabled(true);
            entryBox->uiDate1->blockSignals(false);

            uiWidgets.append(entryBox->uiDate1);
        }

        if (ele[i] == DAY2 ) {
            entryBox->uiDate2 = new QDateEdit(entryBox->par);
            entryBox->uiDate2->blockSignals(true);

            QDate date;
            if (entry->date2().isNull()) date = entry->date1();
            else date = entry->date2();

            entryBox->uiDate2->setFixedHeight(LINE_HEIGHT);
            entryBox->uiDate2->setDisplayFormat(GET_ACC_FRO);
            entryBox->uiDate2->setCalendarPopup(true);
            entryBox->uiDate2->setStyleSheet("QDateEdit { border-style: outset; border-width: 1px; border-color: gray; }");

            entryBox->uiDate2->setDate(date);
            entryBox->entry->day.setDate2(date);

            grid->addWidget(entryBox->uiDate2, row, i, Qt::AlignLeft);

            connect(entryBox->uiDate2, SIGNAL(dateChanged(QDate)), this, SLOT(date2changed(QDate)));
            entryBox->uiDate2->blockSignals(false);
            //uiDate2 = entryBox->uiDate2;
            uiWidgets.append(entryBox->uiDate2);
        }

        // ------------------------------------------------------------------------------------------------
        // Spacer between events, ENTRYINFO_LINE
        // ------------------------------------------------------------------------------------------------

        if (ele[i] == SPACELINE) {
            QFrame *linea1 = new QFrame(entryBox->par); // <<< this does the trick
            linea1->setLineWidth(SPACERLINE_HEIGHT);
            linea1->setMidLineWidth(0);
            linea1->setFixedHeight(SPACERLINE_HEIGHT+8);
            linea1->setFrameShape(QFrame::HLine);
            linea1->setFrameShadow(QFrame::Raised);
            grid->addWidget(linea1, row, i, 1, 1+add, 0);
        }

        // ------------------------------------------------------------------------------------------------
        // Text fields
        // ------------------------------------------------------------------------------------------------

        if (ele[i] == TO ) {
            QLabel *uiLabelTo = new QLabel(entryBox->par);

            uiLabelTo->setText("<=>");
            uiLabelTo->setFixedHeight(LINE_HEIGHT);
            grid->addWidget(uiLabelTo, row, i, Qt::AlignLeft);
            uiWidgets.append(uiLabelTo);
        }

        if (ele[i] == MORE_LABEL ) {
            QLabel *uiAddLineLabel = new QLabel(entryBox->par);

            uiAddLineLabel->setText("(label)");
            uiAddLineLabel->setFixedHeight(LINE_HEIGHT);
            uiAddLineLabel->setFixedWidth(280);
            uiAddLineLabel->setStyleSheet("QLabel { border-style: outset; border-width: 1px; border-color: gray; }");
            if (add==0) grid->addWidget(uiAddLineLabel, row, i, Qt::AlignLeft); else grid->addWidget(uiAddLineLabel, row, i, 1, 1+add, Qt::AlignLeft);

            // t�ss� list�t��n lis�rivin selostus. Lis�riviss� on aina selostuksen j�lkeen kaksi tyhj�� solua (n�ytt�� paljon
            // selke�mm�lt�, siksi, tutkitaan mik� on solun tyyppi kohdassa x+3 ja sen mukaan kirjoitetaan selostusteksti
            if (ele[i+3] == ADDRLINE) uiAddLineLabel->setText("Street Address");
            if (ele[i+3] == NOTES) uiAddLineLabel->setText("Notes");
            if (ele[i+3] == SOURCE_TEXT) uiAddLineLabel->setText("Source Text");
            if (ele[i+3] == STATE) uiAddLineLabel->setText("State/Country");
            if (ele[i+3] == POST) uiAddLineLabel->setText("Post/City");
            if (ele[i+3] == SOURCE_QLTY) uiAddLineLabel->setText("Src pg/Src/Qua");
            if (ele[i+3] == SOURCE_TEXT) uiAddLineLabel->setText("Source Note");
            if (ele[i+3] == RESTRICTION) uiAddLineLabel->setText("Privacy");
            uiWidgets.append(uiAddLineLabel);
        }

        // ------------------------------------------------------------------------------------------------
        // QTextEdit fields
        // ------------------------------------------------------------------------------------------------

        if (    ele[i] == ADDRLINE ||
                ele[i] == POST ||
                ele[i] == CITY ||
                ele[i] == STATE ||
                ele[i] == COUNTRY ||
                ele[i] == PHONE ||
                ele[i] == SOURCE_PAGE ||
                ele[i] == SOURCE_TEXT ||
                ele[i] == NOTES ||
                ele[i] == PHOTONOTES ||
                //ele[i] == PHOTO ||
                ele[i] == PLACE ||
                ele[i] == VALUE

                ) {

            TextEdit2   *edit       = new TextEdit2(entryBox);

            edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            edit->setSizeAdjustPolicy(TextEdit2::AdjustToContentsOnFirstShow); // t�m� auttaa plaintexteditin resize oikean kokoiseksi kun se avataan
            edit->setStyleSheet("TextEdit2 { border-style: outset; border-width: 1px; border-color: gray; }");
            edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            QString     text;

            if (ele[i] == ADDRLINE) {
                text = entry->address.line;
                connect(edit, SIGNAL(textChanged()), this, SLOT(slotAddrLineChanged()));
            }

            if (ele[i] == POST) {
                text = entry->address.post;
                connect(edit, SIGNAL(textChanged()), this, SLOT(slotPostChanged()));
            }

            if (ele[i] == CITY) {
                text = entry->address.city;
                connect(edit, SIGNAL(textChanged()), this, SLOT(slotCityChanged()));
            }

            if (ele[i] == COUNTRY) {
                text = entry->address.country;
                connect(edit, SIGNAL(textChanged()), this, SLOT(slotCountryChanged()));
            }
            if (ele[i] == STATE) {
                text = entry->address.state;
                connect(edit, SIGNAL(textChanged()), this, SLOT(slotStateChanged()));
            }

            if (ele[i] == SOURCE_PAGE) {
                if (entryBox->entry) text = entry->source.page; else {
                    if (*bINDI) text = INDI(ID).source.page;
                    else text = FAM(INDI(ID).getFamS()).source.page;
                }
                connect(edit, SIGNAL(textChanged()), this, SLOT(slotSourcePageChanged()));
            }

            if (ele[i] == SOURCE_TEXT) {
                if ( entryBox->entry ) text = entry->source.note.text; else {
                    if (*bINDI) text = INDI(ID).source.note.text;
                    else text = FAM(INDI(ID).getFamS()).source.note.text;
                }
                connect(edit, SIGNAL(textChanged()), this, SLOT(slotSourceTextChanged()));
            }

            if (ele[i] == PHOTONOTES) {
                // joko portrait kuvan tain etnry kuvan nootit
                if ( entryBox->entry ) text = entry->multiMedia.note.text; // entry kuva
                else {
                    if (entryBox->photoId != 255) text = INDI(ID).multiMedia[entryBox->photoId].note.text; // t�m� antaa arvon indi-tason kuva notelle
                }
                connect(edit, SIGNAL(textChanged()), this, SLOT(slotPhotoNotesChanged()));

                QPalette p = edit->palette();
                p.setColor(QPalette::Base, QColor(255,255,200));
                edit->setPalette(p);
            }

            if (ele[i] == NOTES) {
                if (this->entryLineType == ENTRYINFO_BASIC) {
                    text = entry->note.text;
                }

                if (this->entryLineType == ENTRYINFO_NOTES) {
                    if ( entryBox->entry ) text = entry->note.text; // t�m� rivi on joku eventti. Luetaan eventin nootti
                    else {
                        if (*bINDI) text = INDI(ID).note.text; // indi recordin yleinen nootti
                        else text = FAM(INDI(ID).getFamS()).note.text; // fam recordin yleinen nootti
                    }
                }

                connect(edit, SIGNAL(textChanged()), this, SLOT(slotNotesChanged()));

                QPalette p = edit->palette();
                p.setColor(QPalette::Base, QColor(255,255,200));
                edit->setPalette(p);

            }

            if (ele[i] == PLACE ) {
                edit->setToolTip("Place. Eg. Country/City/Village/House");
                text = entry->place;

                connect(edit, SIGNAL(textChanged()), this, SLOT(placeChanged()));

                edit->setStyleSheet("TextEdit2 { background-color:rgb(255,228,181); border-style: outset; border-width: 1px; border-color: gray; }");

            }

            if (ele[i] == VALUE ) {
                edit->setToolTip("Value");
                text = entry->attrText;
                connect(edit, SIGNAL(textChanged()), this, SLOT(attrValueChanged()));
            }

            if (add==0) grid->addWidget(edit, row, i, 0); else grid->addWidget(edit, row, i, 1, 1+add, 0);

            edit->setPlainText(text);
            uiWidgets.append(edit);
        }

        // ------------------------------------------------------------------------------------------------
        // Buttons
        // ------------------------------------------------------------------------------------------------

        if (ele[i] == BUTTONX ) {
            QPushButton *uiXButton    = new QPushButton(entryBox->par);

            uiXButton->setStyleSheet("QPushButton { border-style: outset; border-width: 1px; border-color: gray; background-color:red;}");

            uiXButton->setMaximumWidth(23);
            uiXButton->setText("X");
            uiXButton->setFixedHeight(LINE_HEIGHT);
            grid->addWidget(uiXButton, row, i, Qt::AlignRight);
            uiWidgets.append(uiXButton);

            if ( entryLineType == ENTRYINFO_PHOTO )                 connect(uiXButton, SIGNAL(clicked()), this, SLOT(slotEraseEntryPhoto()));
            if ( entryLineType == ENTRYINFO_BASIC )                 connect(uiXButton, SIGNAL(clicked()), this, SLOT(slotDeleteEntry()));
            if ( entryLineType == ENTRYINFO_RESTRICTIONS )          connect(uiXButton, SIGNAL(clicked()), this, SLOT(slotEraseRestriction()));
            if ( entryLineType == ENTRYINFO_NOTES )                 connect(uiXButton, SIGNAL(clicked()), this, SLOT(slotEraseNotes()));
            if ( entryLineType == ENTRYINFO_SOURCE_TEXT )           connect(uiXButton, SIGNAL(clicked()), this, SLOT(slotEraseSourceText()));
            if ( entryLineType == ENTRYINFO_SOURCE_PAGE_QUALITY )   connect(uiXButton, SIGNAL(clicked()), this, SLOT(slotErasesourePgQlty()));
            if ( entryLineType == ENTRYINFO_STATE_COUNTRY )         connect(uiXButton, SIGNAL(clicked()), this, SLOT(slotEraseStateCountry()));
            if ( entryLineType == ENTRYINFO_POST_CITY )             connect(uiXButton, SIGNAL(clicked()), this, SLOT(slotErasePostCity()));
            if ( entryLineType == ENTRYINFO_ADDRLINE )              connect(uiXButton, SIGNAL(clicked()), this, SLOT(slotEraseAddrLine()));
        }

        if (ele[i] == BUTTONM ) {
            QPushButton *uiMButton = new QPushButton(entryBox->par);

            uiMButton->setStyleSheet("QPushButton { border-style: outset; border-width: 1px; border-color: gray; background-color:QColor(190,240,190);}");
            uiMButton->setMaximumWidth(23);
            uiMButton->setText("+");
            uiMButton->setFixedHeight(LINE_HEIGHT);
            grid->addWidget(uiMButton, row, i, Qt::AlignRight);
            uiWidgets.append(uiMButton);

            connect(uiMButton, SIGNAL(clicked()), this, SLOT(slotEntryMore()));
        }

        if (ele[i] == DELETE ) {
            QPushButton *uiDeleteMButton = new QPushButton(entryBox->par);
//***
            uiDeleteMButton->setStyleSheet("QPushButton { border-style: outset; border-width: 1px; border-color: gray; }"); //background-color:QColor(255,200,200);}");

            if (*bINDI) {
                uiDeleteMButton->setText("Delete this person from database. Leave spouse(s) and children in database.");
                connect(uiDeleteMButton, SIGNAL(clicked()), this, SLOT(deleteThisPerson()));

            }
            else uiDeleteMButton->setText("Break marriage relationship, but leave persons in database.");

            uiDeleteMButton->setFixedHeight(LINE_HEIGHT);

            grid->addWidget(uiDeleteMButton, row, i, 1, 1+add, 0);
            uiWidgets.append(uiDeleteMButton);
        }

        if (ele[i] == PHOTO ) {
            QString fileName = "";
            if (entryBox->entry) {
                // piirret��n jonkun tapahtuman kuva
                if ( entryBox->entry->multiMedia.used()) fileName = entryBox->entry->multiMedia.file;
            } else {
                // entry muuttujaa ei ole, joten t�m� on varmaan joku "common" kuva
                if ( entryBox->photoId != 255 ) fileName = INDI(ID).multiMedia.at(entryBox->photoId).file;
            }

            if (fileName != "") fileName = GET_DIRECTORY + "/" + fileName;

            PhotoWidget *imageWidget = new PhotoWidget(fileName);

            grid->addWidget(imageWidget, row, i, Qt::AlignLeft);
            uiWidgets.append(imageWidget);

        }

        if (ele[i] == PHOTOFILE  ) {
            QPushButton *uiFileNameButton = new QPushButton(entryBox->par);

            QString text;

            uiFileNameButton->setFixedHeight(LINE_HEIGHT);

            if ( entryBox->entry) {
                if ( entryBox->entry->multiMedia.used() ) { // tapahtuma on, ja sille tapahtumalle on m��ritetty kuva
                    text = entryBox->entry->multiMedia.file;
                    connect(uiFileNameButton, SIGNAL(clicked()), this, SLOT( newEventPhoto() ));
                } else { // tapahtuma on, mutta sille ei ole m��r�tty kuvaa.
                    text = tr("Select a photo");
                    connect(uiFileNameButton, SIGNAL(clicked()), this, SLOT( newEventPhoto() ));
                }
            } else { // tapahtumaa ei ole, joten t�m� on portrait-kuva

                if (this->entryBox->photoId == 255) { // t�ss� boxissa ei ole kuvaa. T�h�n siis halutaan kuva...
//                if (INDI(id).multiMedia.size() == 0) {
                    // portrait kuvaa ei aiemmin ole, luodaan se
                    text = tr("Select a photo");
                    connect(uiFileNameButton, SIGNAL(clicked()), this, SLOT( slotNewPhoto() ));
                } else { // portait kuva on jo aiemmin, vaihdetaan se
                    text = INDI(ID).multiMedia.at(entryBox->photoId).file;
                    connect(uiFileNameButton, SIGNAL(clicked()), this, SLOT( slotChangePhoto() ));
                }

            }
            uiFileNameButton->setText(text);

            if (add==0) grid->addWidget(uiFileNameButton, row, i, 0); else grid->addWidget(uiFileNameButton, row, i, 1, 1+add, 0);
            uiWidgets.append(uiFileNameButton);
        }


        i+= add;
        i++;

    } while (i<9);    
    DE_BUG "out void EntryLine::makeUi()";
}


void Person::updateTypes()
{
    DE_BUG "Person::updateTypes" << id;
    // When a new event will be added, the available-events variable will be changed (for example if user adds "death",
    // will event type "death" be removed from the list since there can be only one death for a person. This change will
    // cause that all previous event-type combos will be changed to type-id 0 and they will appear empty. This function
    // will rewrite the event-type's.

    // p�ivitet��n kaikki selectable eventit

    for (int b = 0 ; b<visibleTabWidget->pEntryBox.size() ; b++) {
        if (visibleTabWidget->pEntryBox.at(b)->uiComboType) {
            visibleTabWidget->pEntryBox.at(b)->uiComboType->blockSignals(true);
            visibleTabWidget->pEntryBox.at(b)->makeSelectableEvents();
            visibleTabWidget->pEntryBox.at(b)->uiComboType->setCurrentIndex(visibleTabWidget->pEntryBox.at(b)->eventMapping.indexOf(visibleTabWidget->pEntryBox.at(b)->getType()));
            visibleTabWidget->pEntryBox.at(b)->uiComboType->blockSignals(false);
        }
        // k�yd��n jokainen boxi l�pi. Yhdes�s boxissa on aina yksi tapahtuma tai merkint�, esim syntym�, valokuva, note...
    }


    DE_BUG "out void Person::updateTypes(bool ind)";
}

EntryLine::EntryLine(EntryBox *box)
{    
    DE_BUG "EntryLine::EntryLine(Person *p, tabWidget *ea)";
    entryBox = box;
    row = 0;

    DE_BUG "out EntryLine::EntryLine(Person *p, TabWidget *ea)";
}

EntryLine::~EntryLine()
{
    DE_BUG "EntryLine::~EntryLine()";
}

/* Add a combobox for adding new entry.
 * */
void Person::addNewEntryComboAndBox()
{
    DE_BUG "person::addNewEntryComboandBox" << id;

    if (visibleTabWidget->eventFrame->spacerItem) visibleTabWidget->eventFrame->grid->removeItem(visibleTabWidget->eventFrame->spacerItem);
    makeEntryBox(255, 255, ENTRYINFO_BASIC);
    visibleTabWidget->eventFrame->addSpacerItem();

    DE_BUG "out void Person::addNewEntryComboandBox(bool indi)";
}

bool EntryBox::eventUsed(quint8 typ)
{
// t�t� k�ytt�� vain EntryBox::makeSelectableEvents
    DE_BUG "bool EntryLine::eventUsed(int typ, bool indi)" << typ;

    for (quint8 b = 0 ; b<par->visibleTabWidget->pEntryBox.size() ; b++) {
        if ( par->visibleTabWidget->pEntryBox.at(b)->getType() == typ) return true;
    }

    return false;
}

void EntryBox::makeSelectableEvents()
{
    DE_BUG "emakeselectableevents";// << entryBox->entryId << this->type() << par->id << this->indi << this->par->visibleTabWidget->indi;
    // update the available events for combo boxes.
    // Called by many functions

    selectableEvents.clear();
    eventMapping.clear();

    quint8 j = 0;
    do {
        bool use, many;
        if (par->visibleTabWidget->indi) {
            use = GET_I_USE(j);
            many = GET_I_MANY(j);
        } else {
            use = GET_F_USE(j);
            many = GET_F_MANY(j);
        }

        if (    ( this->getType() == j ) ||
                ( use && ( !eventUsed( j ) || many ) ) ) {
            selectableEvents.append(Entry::typeString(j, par->visibleTabWidget->indi) );
            eventMapping.append(j);
        }

        j++;
    } while (Entry::typeString(j, par->visibleTabWidget->indi) != "");

    /*
jos t�m� "entryline" on "add event", lis�t��n viel� n�m� kolme vaihtoehtoa. N�m� on v�h�n poikkeuksia,
koska n�m� eiv�t liity mihink��n tiettyyn eventtiin kuten kaikki muut. Siksi jos k�ytt�j� haluaa vaihtaa
eventtityypin toiseen (esim marriage => engagement) menee v�h�n monimutkaiseksi vaihtaa esim yleinen source
tieto jonkun eventin source tiedoksi. Siksi sit� ei nyt jaksa t�ss� mahdollista. *** JOs k�ytt�j� haluaa vaihtaa
esim yleisen source tiedon marriage source tiedoksi, pit�� ensin deletoida ja luoda source tieto sitten uudelleen
marriagen alle. ja sillee....*/

    if (entryId == 255) {
        selectableEvents << tr("Portrait Photo") << tr("Common Note") << tr("Common Source Note") << tr("Source / pg / quality");
        eventMapping.append(28);
        eventMapping.append(29);
        eventMapping.append(30);
        eventMapping.append(31);
    }

    typeModel.setStringList(selectableEvents);

    DE_BUG "out void EntryLine::makeSelectableEvents()";
}


void Person::debugEvents()
{
    DE_BUG "Person::debug";
    //this->visibleTabWidget->eventFrame->update();
    this->visibleTabWidget->eventFrame->repaint();

    //this->visibleTabWidget->eventScrollArea->widget()->update();

/*
    QString fileName = QFileDialog::getOpenFileName(
            qApp->activeWindow(),
            tr("Open Picture"),
            GET_DIRECTORY,
            tr("(*.*)"));
            */

    // *!* for development purposes, not called
    /*
    int i;
    for (i=0 ; i< this->visibleTabWidget->pEntryLine.size() ; i++) {
        qDebug() << i << visibleTabWidget->pEntryLine.at(i)->indi << visibleTabWidget->pEntryLine.at(i)->type() << visibleTabWidget->pEntryLine.at(i)->entryId << visibleTabWidget->pEntryLine.at(i)->debug;
    }
    qDebug() << "count " << visibleTabWidget->pEntryLine.size();
*/

this->visibleTabWidget->grid->update();
    //this->visibleTabWidget->update();
    //this->visibleTabWidget->updateGeometry();
}

TabWidget::TabWidget() {

}

TabWidget::TabWidget(QTabWidget *qtw)
{
    Q_UNUSED( qtw );
}

void TabWidget::set()
{
    DE_BUG "void TabWidget::set()";
    grid->setVerticalSpacing(2);
    grid->setHorizontalSpacing(2);
    grid->setContentsMargins(8, 3, 3, 3);

    DE_BUG "out void TabWidget::set()";
}

void Person::nameEdited(QString name) {
    DE_BUG "void Person::nameEdited(name)" << name << id;
    // t�ss� kohtaa t�m� voi aiheuttaa v��ri� sexip��telmi�, esim jos pis�� andersdotter, voi johtop��t� olla ett� kyseess� on mies

    /*
    if (GET_SUGGEST_SEX)
    {
        if (boyNames.contains(name)) this->uiMale->setChecked(true);
        if (girlNames.contains(name)) this->uiFemale->setChecked(true);
    }
    */

    DE_BUG "out void Person::nameEdited(QString name) {";
}

void Person::setValues()
{
    DE_BUG "void Person::setValues()" << id;
    uiName1st->setText(INDI(id).nameFirst.replace("_"," "));
    uiName2nd->setText(INDI(id).name2nd.replace("_"," "));
    uiName3rd->setText(INDI(id).name3rd.replace("_"," "));
    uiNameFamily->setText(INDI(id).nameFamily);

    uiLcdNumber->display((int)id);

    if (!showFemales && !showMales ) {
        uiLinkNewPersontoDB->setEnabled(false);
    }

    if ( INDI(id).sex == FEMALE ) {
        uiMale->setChecked(false);
        uiFemale->setChecked(true);
    }

    if ( INDI(id).sex == MALE ) {
        uiMale->setChecked(true);
        uiFemale->setChecked(false);
    }

    if ( !INDI(id).getMother() ) this->uiBreakMother->setEnabled(false);
    if ( !INDI(id).getFather() ) this->uiBreakFather->setEnabled(false);
    if ( !INDI(id).getSpouse() ) this->uiBreakSpouse->setEnabled(false);

    if (INDI(id).getFather() != 0) this->uiBreakFather->setEnabled(true);
    if (INDI(id).getMother() != 0) this->uiBreakMother->setEnabled(true);

    uiChanged->setText(tr("Changed : ") + INDI(id).changed.toString("dd.MM.yyyy   hh:mm:ss"));

    /*
    int j;
    QString webUrl;

    for (j=0 ; j<INDI(id).entry.size() ; j++) {
        if (INDI(id).entry.at(j).type == USEREVENT && INDI(id).entry.at(j).attrType == "Web") webUrl = INDI(id).entry.at(j).place;
    }

    ***webview removed in this version***
    webView->load(webUrl);
    uiWebUrl->setText(webUrl);
    */
    updateDates();

    DE_BUG "out void Person::setValues()";
}

/*
void Person::slotPrivacyPolicyChanged()
{    
    DE_BUG "void Person::slotPrivacyPolicyChanged()" << id;

    //INDI(id).privacyPolicy = this->uiIndiRestriction->currentIndex();
    // *A* FAM(INDI(id).getFamS()).privacyPolicy = this->uiFamRestriction->currentIndex();

    this->updateDates();

    DE_BUG "out void Person::slotPrivacyPolicyChanged()";
}
*/

void Person::slotTabChanged(int current)
{
    DE_BUG "Person::slotTabChanged" << current;
    if ((current>0) && current<=INDI(id).famSList.size()) INDI(id).currentSpouse = current-1;
qDebug() << INDI(id).currentSpouse;
    visibleTabWidget = (TabWidget*)(uiTabGroup->currentWidget());
}

Person::Person(bool sM, bool sF, quint16 *p1, QWidget *parent) : QDialog(parent)
{
    DE_BUG "Person::person" << id;

    setupUi(this);

    id = *p1;

    this->showFemales = sF;
    setFont(GeneData::panelFont);
    this->showMales = sM; // n�it� tarvitaan silloin jos n�ytet��n person-dialog uuden ihmisen valintaa varten, ja valitaankin uuden ihmisen sijasta joku db:ssa oleva

    connect(uiTabGroup,   SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));

    usrEvents.append("Add");
    famEvents.append("Add");
    indiEventModel.setStringList(usrEvents);
    famEventModel.setStringList(famEvents);

    // ---------------------------------------------------------------------------

    for ( quint16 s=0 ; s<=GENE.sourLastUsed ; s++) {
        QString sourceStr;
        sourceStr = QString::number(s) + " " + GENE.sour[s].title.left(20);
        if (GENE.sour[s].author != "") sourceStr += " / " + GENE.sour[s].author.left(20);
        sourceList << sourceStr;
    }

    sourceModel.setStringList(sourceList);

    QStringList entryMoreList;
    entryMoreList << tr("Add more info") << tr("Address") << tr("Post Code / city") << tr("State / Country / Phone") << tr("Source / Pg / Qual.") << tr("Source text") << tr("Notes") << tr("Photo") << tr("Privacy in reporting");
    entryMoreModel.setStringList(entryMoreList);

    QString policyText[5] = { "Show",
                              "Show Name, plur details, show genre",
                              "Show Name, no details, no genre",
                              "Plur Name, no details, no genre",
                              "Hide all"};

    QStringList entryRestList;
    for (int x=0 ; x<4 ; x++) entryRestList.append(policyText[s_privacyMethod(x)]);

    entryRestModel.setStringList(entryRestList);


    // ----------------------------------------------------------------------

    //EntryLine::person       = &INDI(id);

    // luodaan indi tab
    TabWidget *newTab = new TabWidget(this->uiTabGroup);

    QGridLayout *layout = new QGridLayout(newTab); // koko tabin layout
    newTab->setLayout(layout);

    QComboBox   *privacyPolicyCombo = new QComboBox(newTab);       layout->addWidget(privacyPolicyCombo, 0, 0, 1, 9, 0); // combo box
    QLCDNumber  *qlcd = new QLCDNumber(newTab);     layout->addWidget(qlcd, 0, 9, 1, 1, 0); // lcd

    newTab->eventScrollArea = new QScrollArea(newTab);     // se event widget jonka k�ytt�j� n�kee ruudulla
    newTab->eventFrame = new EventFrame(newTab);  // se widget, johon eventit piirret��n ja joka voi olla isompi kuin se jonka k�ytt�j� n�kee
    newTab->eventScrollArea->setWidget(newTab->eventFrame);
    newTab->eventFrame->grid = new QVBoxLayout;
    newTab->eventFrame->setLayout(newTab->eventFrame->grid);
    layout->addWidget(newTab->eventScrollArea, 1, 0, 10, 10, 0);    // eventtienkatselu vidget
    newTab->eventScrollArea->setWidgetResizable(true);
    uiTabGroup->insertTab(0, newTab, "Personal Data");

    visibleTabWidget = newTab;

    newTab->grid          = layout; // t�m� ei mene samalla tavalla kuin ennen
    newTab->indi          = true;
    newTab->entrys        = &INDI(id).entry;
    newTab->set();

    addIndiEvents();

    updateTypes();

    privacyPolicyCombo->setModel(&entryRestModel);
    privacyPolicyCombo->setCurrentIndex(INDI(id).privacyPolicy);

    retranslateUi(this);

    setValues();    

    new QShortcut(Qt::Key_S, this, SLOT(save()));
    new QShortcut(Qt::Key_1, this, SLOT(slotShortcut1()));
    new QShortcut(Qt::Key_2, this, SLOT(slotShortcut2()));
    new QShortcut(Qt::Key_3, this, SLOT(slotShortcut3()));
    new QShortcut(Qt::Key_4, this, SLOT(slotShortcut4()));
    new QShortcut(Qt::Key_5, this, SLOT(slotShortcut5()));
    new QShortcut(Qt::Key_6, this, SLOT(slotShortcut6()));
    new QShortcut(Qt::Key_7, this, SLOT(slotShortcut7()));
    new QShortcut(Qt::Key_8, this, SLOT(slotShortcut8()));
    new QShortcut(Qt::Key_9, this, SLOT(slotShortcut9()));
    new QShortcut(Qt::Key_0, this, SLOT(slotShortcut0()));
    new QShortcut(Qt::Key_B, this, SLOT(slotShortcutB()));
    new QShortcut(Qt::Key_D, this, SLOT(slotShortcutD()));
    new QShortcut(Qt::Key_E, this, SLOT(slotShortcutE()));
    new QShortcut(Qt::Key_O, this, SLOT(slotShortcutO()));
    new QShortcut(Qt::Key_G, this, SLOT(slotShortcutG()));
    new QShortcut(Qt::Key_C, this, SLOT(slotShortcutC()));
    new QShortcut(Qt::Key_A, this, SLOT(slotShortcutA()));
    new QShortcut(Qt::Key_U, this, SLOT(slotShortcutU()));
    new QShortcut(Qt::Key_W, this, SLOT(slotShortcutW()));
    new QShortcut(Qt::Key_R, this, SLOT(slotShortcutR()));
    new QShortcut(Qt::Key_P, this, SLOT(slotShortcutP()));
    new QShortcut(Qt::Key_N, this, SLOT(slotShortcutN()));
    new QShortcut(Qt::Key_T, this, SLOT(slotShortcutT()));
    new QShortcut(Qt::Key_I, this, SLOT(slotShortcutI()));
    new QShortcut(Qt::Key_M, uiMale, SLOT(click())); // male
    new QShortcut(Qt::Key_F, uiFemale, SLOT(click())); // female

    new QShortcut(Qt::Key_F1, this, SLOT(slotShortcutF1())); //familyname
    new QShortcut(Qt::Key_F2, this, SLOT(slotShortcutF2())); // 1st name
    new QShortcut(Qt::Key_F3, this, SLOT(slotShortcutF3())); // 2nd name
    new QShortcut(Qt::Key_F4, this, SLOT(slotShortcutF4())); // 3rd name

    new QShortcut(Qt::Key_Left, this, SLOT(slotPrevTab()));
    new QShortcut(Qt::Key_Right, this, SLOT(slotNextTab()));
    new QShortcut(Qt::SHIFT + Qt::Key_M, this, SLOT(slotShortcutMM()));


    connect(uiNameFamily,    SIGNAL(returnPressed()), this, SLOT(slotShortcutF2()));
    connect(uiName1st,       SIGNAL(returnPressed()), this, SLOT(slotShortcutF3()));
    connect(uiName2nd,       SIGNAL(returnPressed()), this, SLOT(slotShortcutF4()));
    connect(uiName3rd,       SIGNAL(returnPressed()), this, SLOT(slotShortcutF1()));

    connect(uiSave,             SIGNAL(clicked()), this, SLOT(save()));
    connect(uiClose,            SIGNAL(clicked()), this, SLOT(closeWithoutSaving()));
    //connect(uiDelete,           SIGNAL(clicked()), this, SLOT(slotDeleteThis()));
    connect(uiName1st,          SIGNAL(textEdited(QString)), this, SLOT(nameEdited(QString)));
    connect(uiName2nd,          SIGNAL(textEdited(QString)), this, SLOT(nameEdited(QString)));
    connect(uiName3rd,          SIGNAL(textEdited(QString)), this, SLOT(nameEdited(QString)));
    connect(uiLinkNewPersontoDB,SIGNAL(clicked()), this, SLOT(assignNewPersoninDb()));
    connect(uiBreakFather,      SIGNAL(clicked()), this, SLOT(breakRelationshipToFather()));
    connect(uiBreakMother,      SIGNAL(clicked()), this, SLOT(breakRelationshipToMother()));
    connect(uiBreakSpouse,      SIGNAL(clicked()), this, SLOT(breakRelationshipToSpouse()));
    connect(privacyPolicyCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(privacyPolicyChanged(int)));
    //connect(uiIndiRestriction,  SIGNAL(currentIndexChanged(int)), this, SLOT(slotPrivacyPolicyChanged()));

    for (quint8 f=0 ; f<INDI(id).famSList.size() ; f++) {
        // onko puoliso todellakin olemassa? Jossain tilanteissa henkil�ll� voi olla spouse perhe-
        // numero, joka on siis annettu my�s lapsen perhenumeroksi, mutta toista puolisoa ei olekaan
        // ts is� tai �iti on tuntematon. Ei silloin luoda perhe-tab:a. Jos k�ytt�j� haluaa antaa
        // tietoa t�st� tuntemattomasta puolisosta, pit�� ko puolisolle luoda INDI tietue
        INDI(id).currentSpouse = f;
        if (INDI(id).getSpouse()) {

            TabWidget *newTab = new TabWidget(this->uiTabGroup);

            QGridLayout *layout = new QGridLayout(newTab);
            newTab->setLayout(layout);

            QComboBox *privacyPolicyCombo = new QComboBox(newTab);     layout->addWidget(privacyPolicyCombo, 0, 0, 1, 9, 0); // combo box
            QLCDNumber *qlcd = new QLCDNumber(newTab);  layout->addWidget(qlcd, 0, 9, 1, 1, 0); // lcd

            newTab->eventScrollArea = new QScrollArea(newTab);
            newTab->eventFrame = new EventFrame(newTab);
            newTab->eventScrollArea->setWidget(newTab->eventFrame);
            newTab->eventFrame->grid = new QVBoxLayout;
            newTab->eventFrame->setLayout(newTab->eventFrame->grid); // boxit menee vertikaalisesti eventFrameen
            layout->addWidget(newTab->eventScrollArea, 1, 0, 10, 10, 0); // scrollarea
            newTab->eventScrollArea->setWidgetResizable(true);
            privacyPolicyCombo->setModel(&entryRestModel);
            privacyPolicyCombo->setCurrentIndex(FAM(INDI(id).getFamS()).privacyPolicy);

            uiTabGroup->insertTab(1, newTab, "Family with " + INDI(INDI(id).getSpouse(f)).nameFirst + " " + INDI(INDI(id).getSpouse(f)).nameFamily);

            INDI(id).currentSpouse = f;

            visibleTabWidget = newTab;

            newTab->grid          = layout; // t�m� ei mene samalla tavalla kuin ennen
            newTab->indi          = false;
            newTab->entrys        = &FAM(INDI(id).getFamS()).entry;
            newTab->set();
            addFamilyEvents();
            updateTypes();
            connect(privacyPolicyCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(privacyPolicyChanged(int)));
        }
    }

    uiTabGroup->setCurrentIndex(0);
    DE_BUG "out Person::Person";
    backGroundColorBoxes();
    setFocus();

    QPalette palette = QToolTip::palette();
    palette.setColor(QPalette::Inactive, QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::Inactive, QPalette::ToolTipText, Qt::black);
    QToolTip::setPalette(palette);QFont serifFont("Times", 14, QFont::Bold);
    QToolTip::setFont(serifFont);
}

Person::~Person()
{
    DE_BUG "~person";
}
