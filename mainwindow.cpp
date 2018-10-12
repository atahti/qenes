/*
 *
 *  qenes genealogy software
 *
 *  Copyright (C) 2009-2010 Ari Tï¿½hti
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License veasion 2 as
 *  published by the Free Software Foundation.
 */


#include <QLabel>
#include <QMessageBox>
#include <stdio.h>
#include <QResizeEvent>
#include <QFileDialog>
#include <QSettings>
#include <QClipboard>
#include <QDesktopWidget>
#include "mainwindow.h"
#include "macro.h"
#include "ui/person.h"
#include "io/reporter.h"
#include "data/genedata.h"
#include "dbview2.h"
#include "data/date.h"
#include "sourceview.h"
//#include "ui/descendants.h"

#include <QMetaEnum>
#include <QRegExp>
#include <QPainter>

QStringListModel quayModel, accModel, adopModel;
QStringList     usrEvents, famEvents;
QList<QString>  patronyms;
QList<QString>  boyNames;
QList<QString>  girlNames;
QList<QString>  places;

#define SHOWMETHODS false
#define DE_BUG if (SHOWMETHODS) qDebug() <<

extern GeneData        * geneData;
bool Settings::changed = false;

QSettings       sets("qenes", "Qenes");



// --------------------------------------------------

void MainWindow::slotLoadLastFile()
{

    QString fileName = sets.value("lastFile").toString();

    if (fileName != "") {
        this->slotStatusBarMessage("Load last file : " + fileName);
        file.setFileName(fileName);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox qmb;
            qmb.setText(tr("Could not open file"));
            qmb.exec();
            return;
        }

        importStream(pLoadStream, true, fileName);
        GENE.isSaved = true;
    } else {

    }


}

bool MainWindow::gdSpace(quint8 requiredSpace)
{    
    // varmistetaan ettei taustaprosessit pyöri. Ko prosessi voi olla esim vikojen hakua
    if ( GeneData::gdLastUsed != 255) for (quint8 i=0 ; i<GeneData::gdLastUsed ; i++) {
        geneData[i].stopDemonNow = true;
        geneData[i].wait(10000);
    }

    if (requiredSpace > GeneData::gdLastUsed) GeneData::gdLastUsed = requiredSpace;
    if (( GeneData::gdCount - requiredSpace) < 1) {
        quint8 x;

        // disconnect varmuuden vuoksi. Ei tiedetä vielä, pitääkö gdSpaceïä suurentaa vai ei.
        // Jos pitää suurentaa, geneData osoite on muuttunut.

        for (x = 0 ; x < GeneData::gdCount ; x++) {
            disconnect(&geneData[x], SIGNAL(sigUpdate(quint16)), this, SLOT(update(quint16)));
            disconnect(&geneData[x], SIGNAL(sigUpdate()), this, SLOT(update()));
            disconnect(&geneData[x], SIGNAL(sigStatusBarMessage(QString)), this, SLOT(slotStatusBarMessage(QString)));
        }

        qDebug() << "a1" << GeneData::currentGd;
        bool answer = GeneData::gdSpace(requiredSpace); // huom kutsutaan GeneData:n gdspacea, ei tätä samaa funktiota uuelleen
qDebug() << "a2" << GeneData::currentGd;

        for (x = 0 ; x < GeneData::gdCount ; x++) {
            connect(&geneData[x], SIGNAL(sigUpdate(quint16)), this, SLOT(update(quint16)));
            connect(&geneData[x], SIGNAL(sigUpdate()), this, SLOT(update()));
            connect(&geneData[x], SIGNAL(sigStatusBarMessage(QString)), this, SLOT(slotStatusBarMessage(QString)));
        }

        return answer;

    }
    return false;
}

void MainWindow::makeDefaultSetupValues()
{
    Settings::makeDefaultValues();
}

void MainWindow::useDefaultSetupValues()
{
    QMessageBox msgBox;
    msgBox.setText(tr("Setupvalues not found. First time use? Using defaults. "));
    msgBox.exec();

    sets.setValue("fileDirectory", QCoreApplication::applicationDirPath());

    Settings::resetSettings();

    sets.setValue("INSTALLED", 1);
}

void MainWindow::setGd(int gdId)
{    
    DE_BUG "MainWindow::setGd";
    GeneData::currentGd = gdId;

    this->slotStatusBarMessage("switch to database " + QString::number(GeneData::currentGd));

    setWindowTitle(GENE.fileName);
    update(GENE.currentId);
}

int MainWindow::debug_int(bool *newCol, int child, QList<int> *rivit, QList<int> *henk, QString *output)
{
    int childCount = 0;

    if (*newCol) {
        *output += "<tr>\n";
        *newCol = false;
    }

    QString tag = QString("%1").arg(QString::number(child), 5, '0');
    *output += "<td style=\"border: 1pt solid rgb(187,187,187);\" valign=\"top\" rowspan = ROWS" + tag + " bgcolor=\" COLOR" + tag + "\"> NAME" + tag + " </td>\n";

    for (int c=0 ; c<INDI(child).childIdList.size() ; c++) childCount += debug_int(newCol, INDI(child).childIdList.at(c), rivit, henk, output);

    // tï¿½ssï¿½hï¿½n lasketaan rivejï¿½. Jos henkilï¿½llï¿½ on 0 lasta, hï¿½n tarvii yhden rivin itseï¿½ï¿½n varten, jos hï¿½nellï¿½ on
    // 1 tai useampi lasta, hï¿½n tarvii yhtï¿½ monta riviï¿½ kuin lapsia on
    if (childCount == 0) {
        childCount = 1;
        // tï¿½llï¿½ henkilï¿½llï¿½ ei ollut enï¿½ï¿½ lapsia, eli sarake loppuu
        *output += "</tr>\n";
        *newCol = true;
    }

    rivit->append(childCount);
    henk->append(child);

    return childCount;
}

void MainWindow::debug1()
{
/*
    quint16 id=1;

    Descendants d(&id);

    d.exec();
  */
}

QString MainWindow::getFileEnd(QString fileName)
{ // tää on kopioitu reporter :sta, tarkoitus olisi että debug2 toiminta joka tätä käyttää menisi reporteriin. ota tämä sitten pois kun änin on taphatunut
    if (fileName.contains("/") || fileName.contains("\\")) {
        // filename is in form "c;\directory\diipadaapa\file.name"
        // This will return "file.name"
        QRegExp qre("[.*\\\\|.*\\/]([^\\\\.|^\\/]+\\.\\S{3})");
        qre.indexIn(fileName);
        return qre.cap(1);
    }
    return fileName;
}

void MainWindow::debug2()
{
    QFile file("/home/ari/qenes.json");

    if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QString saveString = "[\n";
        for (int i = 1; i<=GENE.indiLastUsed ; i++) {
            saveString += "{\n\"name\" : \"" + INDI(i).nameFirst + " " + INDI(i).nameFamily + "\",\n";
            if (INDI(i).note.used()) saveString += "\"notes\" : \"" + INDI(i).note.text.simplified().remove("\"") + "\",\n";
            saveString += "\"rela\" : \"" + INDI(i).getShortFamilyRelation() + "\",\n";
            saveString += "\"reportid\" : \"" + QString::number(i) + "\",\n";
            if (INDI(i).changed.toString("dd.MM.yyyy") != "") saveString += "\"changed\" : \"" + INDI(i).changed.toString("dd.MM.yyyy") + "\",\n";
            if (INDI(i).multiMedia.size()>0) saveString += "\"photo\" : \"" + getFileEnd(INDI(i).multiMedia.value(0).file) + "\",\n";
            saveString += "\"mother\" : \"" + QString::number(FAM(INDI(i).famc.value).wife-1) + "\",\n";
            saveString += "\"father\" : \"" + QString::number(FAM(INDI(i).famc.value).husb-1) + "\",\n";
            saveString += "\"sex\" : \"";
            if (INDI(i).sex == FEMALE) saveString += "f\"";
            else if (INDI(i).sex == MALE) saveString += "m\"";
            else saveString += "u\"";
            saveString += ",\n";
            saveString += "\"spouses\" : \"" + QString::number(INDI(i).famSList.size())  + "\",";

            if (INDI(i).famSList.size() > 0) {
                saveString += "\n\"spouse\" : [\n";
                for (quint8 s = 0 ; s<INDI(i).famSList.size() ; s++) {
                    saveString += "\t{\n";
                    saveString += "\t\"id\" : \"" + QString::number(INDI(i).getSpouse(s)) + "\",\n";
                    if (FAM(INDI(i).famSList.at(s)).note.used()) saveString += "     \"note\" : \"" + FAM(INDI(i).famSList.at(s)).note.text.simplified().remove("\"") + "\",\n";

                    saveString += "\t\"entries\" : \"" + QString::number(FAM(INDI(i).famSList.at(s)).entry.size())  + "\"";
                    if (FAM(INDI(i).famSList.at(s)).entry.size() > 0) {
                        saveString += ",\n\t\"entry\" : [\n";
                        for (quint8 e = 0 ; e<FAM(INDI(i).famSList.at(s)).entry.size() ; e++) {
                            saveString += "\t\t{\n";
                            saveString += "\t\t\"type\" : \"" + FAM(INDI(i).famSList.at(s)).entry.value(e).typeString() + "\"";
                            if (FAM(INDI(i).famSList.at(s)).entry.at(e).day.known) {
                                saveString += ",\n\t\t\"day\" : \"" + FAM(INDI(i).famSList.at(s)).entry.value(e).day.toString(true) + "\"";

                                if (FAM(INDI(i).famSList.at(s)).entry.value(e).day.dayNameOfWeek() != "") saveString += ",\n\t\t\"weekday\" : \"" + FAM(INDI(i).famSList.at(s)).entry.value(e).day.dayNameOfWeek()  + "\"";
                                if (INDI(FAM(INDI(i).famSList.at(s)).wife).getAge(FAM(INDI(i).famSList.at(s)).entry.value(e).day) != "") saveString += ",\n\t\t\"wag\" : \"" + INDI(FAM(INDI(i).famSList.at(s)).wife).getAge(FAM(INDI(i).famSList.at(s)).entry.value(e).day)  + "\"";
                                if (INDI(FAM(INDI(i).famSList.at(s)).husb).getAge(FAM(INDI(i).famSList.at(s)).entry.value(e).day) != "") saveString += ",\n\t\t\"hag\" : \"" + INDI(FAM(INDI(i).famSList.at(s)).husb).getAge(FAM(INDI(i).famSList.at(s)).entry.value(e).day)  + "\"";
                            }

                            if (FAM(INDI(i).famSList.at(s)).entry.value(e).note.used()) saveString += ",\n\t\t\"note\" : \"" + FAM(INDI(i).famSList.at(s)).entry.value(e).note.text.simplified().remove("\"") +"\"";
                            if (!FAM(INDI(i).famSList.at(s)).entry.value(e).place.isEmpty()) saveString += ",\n\t\t\"place\" : \"" + FAM(INDI(i).famSList.at(s)).entry.value(e).place.simplified().remove("\"")  + "\"";
                            if (!FAM(INDI(i).famSList.at(s)).entry.value(e).printValue().isEmpty()) saveString += ",\n\t\t\"value\" : \"" + FAM(INDI(i).famSList.at(s)).entry.value(e).attrType.simplified().remove("\"")  + "\"";

                            saveString += "\n\t\t}";

                            if (e<FAM(INDI(i).famSList.at(s)).entry.size()-1) saveString += ",";
                            saveString += "\n";
                        }
                        saveString += "\t\t],\n";
                    } else saveString += ",\n";

                    saveString += "\t\"childs\" : \"" + QString::number((FAM(INDI(i).famSList.at(s)).childs.size())) + "\"";
                    if (FAM(INDI(i).famSList.at(s)).childs.size() > 0) {
                        QString tmp;
                        quint16 childID;
                        saveString += ",\n\t\"child\" : [\n";
                        for (quint8 c = 0 ; c<FAM(INDI(i).famSList.at(s)).childs.size() ; c++) {
                            childID = FAM(INDI(i).famSList.at(s)).childs.at(c);

                            saveString += "\t\t{\n";
                            saveString += "\t\t\"id\" : \"" + QString::number(childID) + "\"";
                            tmp = QString(INDI(INDI(childID).getFather()).getAge(INDI(childID).ientry(BIRTH).day));
                            if (tmp != "") saveString += ",\n\t\t\"fag\" : \"" + tmp + "\"";
                            tmp = QString(INDI(INDI(childID).getMother()).getAge(INDI(childID).ientry(BIRTH).day));
                            if (tmp != "") saveString += ",\n\t\t\"mag\" : \"" + tmp + "\"";
                            saveString += ",\n\t\t\"death\" : \"" + QString::number(666) + "\"";
                            tmp = INDI(childID).ientry(BIRTH).place;
                            if (tmp != "") saveString += ",\n\t\t\"place\" : \"" + tmp + "\"";
                            tmp = INDI(childID).ientry(BIRTH).day.toString(true) + " " + INDI(childID).ientry(BIRTH).day.dayNameOfWeek();
                            if (tmp != " ") saveString += ",\n\t\t\"birth\" : \"" + tmp + "\"";

                            saveString += "\n\t\t}";
                            if (c<FAM(INDI(i).famSList.at(s)).childs.size()-1) saveString += ",";
                            saveString += "\n";
                        }
                        saveString += "\t\t]\n";
                    } else saveString += "\n";

                    saveString += "\t}";
                    if (s<INDI(i).famSList.size()-1) saveString += ",";
                    saveString += "\n";
                }
                saveString += "\t],\n";
            } else saveString += "\n";


            saveString += "\"entries\" : \"" + QString::number(INDI(i).entry.size())  + "\"";
            if (INDI(i).entry.size() > 0) {
                saveString += ",\n\"entry\" : [\n";
                for (quint8 e = 0 ; e<INDI(i).entry.size() ; e++) {
                    saveString += "\t{\n";
                    saveString += "\t\"type\" : \"" + INDI(i).entry.value(e).typeString() + "\"";
                    if (INDI(i).entry.at(e).day.known) {
                        saveString += ",\n\t\"day\" : \"" + INDI(i).entry.value(e).day.toString(true)  + "\"";
                        saveString += ",\n\t\"age\" : \"" + INDI(i).getAge(INDI(i).entry.value(e).day) + "\"";
                        if (INDI(i).entry.value(e).day.dayNameOfWeek() != "") saveString += ",\n\t\"weekday\" : \"" + INDI(i).entry.value(e).day.dayNameOfWeek()  + "\"";
                    }
                    if (INDI(i).entry.value(e).note.used()) saveString += ",\n\t\"note\" : \"" + INDI(i).entry.value(e).note.text.simplified().remove("\"") +"\"";
                    if (!INDI(i).entry.at(e).place.isEmpty()) saveString += ",\n\t\"place\" : \"" + INDI(i).entry.at(e).place.simplified().remove("\"")  + "\"";
                    if (!INDI(i).entry.at(e).attrText.isEmpty()) saveString += ",\n\t\"attrText\" : \"" + INDI(i).entry.at(e).attrText.simplified().remove("\"")  + "\"";
                    if (!INDI(i).entry.at(e).attrType.isEmpty()) saveString += ",\n\t\"attrType\" : \"" + INDI(i).entry.at(e).attrType.simplified().remove("\"")  + "\"";

                    saveString += "\n\t}";
                    if (e<INDI(i).entry.size()-1) saveString += ",";
                    saveString += "\n";
                }
                saveString += "\t]\n";
            } else saveString += "\n";

            saveString += "}";

            if (i != GENE.indiLastUsed) saveString += ",\n"; else saveString += "\n]";
        }

        file.write(saveString.toLocal8Bit());

        file.close();
    }
    qDebug() << "done";
}

void MainWindow::debug3()
{
    for (int i=0 ; i<=GENE.indiLastUsed ; i++) {
        qDebug() << "---------------------";
        qDebug() << i << INDI(i).nameFamily << INDI(i).nameFirst;
        for (int e=0 ; e<INDI(i).entry.size() ; e++) {
            qDebug() << e << INDI(i).entry.value(e).typeString() << INDI(i).entry.value(e).day << INDI(i).entry.value(e).day.accuracyString();
        }
    }
}
void MainWindow::debug4()
{
    Date d1, d2;

    d1.setDate1(1900,1,1);      d1.setAcc(EXA);
    d2.setDate1(1900,12,30);    d2.setAcc(EXA);
    qDebug() << (d1<d2) << (d1>d2);

    d2.setDate1(1900,12,30);    d2.setAcc(EST);
    qDebug() << (d1<d2) << (d1>d2);

    d2.setDate1(1900,12,30);    d2.setAcc(ABT);
    qDebug() << (d1<d2) << (d1>d2);

    d1.setAcc(ABT); d2.setAcc(EXA);
    qDebug() << (d1<d2) << (d1>d2);

    d1.setAcc(EST); d2.setAcc(EXA);
    qDebug() << (d1<d2) << (d1>d2);

    d1.setDate1(1900,1,1);  d1.setDate2(1900,12,31);    d1.setAcc(FRO);
    d2.setDate1(1900,2,1);  d2.setDate2(1900,3,1);      d2.setAcc(FRO);
    qDebug() << (d1<d2) << (d1>d2);

    d1.setDate1(1900,1,1);  d1.setDate2(1900,12,31);    d1.setAcc(FRO);
    d2.setDate1(1900,2,1);  d2.setDate2(1902,3,1);      d2.setAcc(FRO);
    qDebug() << (d1<d2) << (d1>d2);

    d1.setDate1(1900,1,1);  d1.setDate2(1900,12,31);    d1.setAcc(FRO);
    d2.setDate1(1902,2,1);  d2.setDate2(1903,3,1);      d2.setAcc(FRO);
    qDebug() << (d1<d2) << (d1>d2);
}

void MainWindow::debug5()
{
    qDebug() << FAM(43).childs.at(0);
    qDebug() << FAM(43).childs.at(1);
}

void MainWindow::debug6() {
    INDI(22).deleteThis();
}

void MainWindow::hideGgggps(bool visi)
{
    for (int i=0 ; i<32 ; i++) this->panelGGGGp[i].setHidden(visi);
    this->generation[6].setHidden(visi);
}

void MainWindow::hideGggps(bool visi)
{;
    for (int i=0 ; i<16 ; i++) this->panelGGGp[i].setHidden(visi);
    this->generation[5].setHidden(visi);
}

void MainWindow::hideGgps(bool visi)
{
    for (int i=0 ; i<8 ; i++) this->panelGGp[i].setHidden(visi);
    this->generation[4].setHidden(visi);
}

void MainWindow::hideGps(bool visi)
{
    for (int i=0 ; i<4 ; i++) this->panelGp[i].setHidden(visi);

    this->generation[3].setHidden(visi);
    cousinList->setVisible(visi);    

}

void MainWindow::settings()
{    
    DE_BUG "MainWindow::settings";
    Settings set(0);

    //set.startTab = 0;
    set.exec();
    update(panelCenter->current->id);
}

void MainWindow::slotPrint()
{    
    if (GENE.reporter) {// reporteriï¿½ ei vï¿½lttï¿½mï¿½ttï¿½ ole
        if (GENE.reporter->isRunning()) {
            QMessageBox qmb;
            qmb.setText("Report ongoing. Wait until it has finished");
            qmb.exec();
            return;
        }
    }

    printSettings.clear();
    ReporterDialog repDialog(&printSettings);
    if (repDialog.exec()) {

        // reporterille pitï¿½ï¿½ kertoa mitï¿½ genedata-currentGd:tï¿½ ollaan tulostamassa, jotta
        // myï¿½hemmin osataan tuhota oikea reporter tulostuksen pï¿½ï¿½tteeksi, sekï¿½ katkoa signal/slotit
        GENE.reporter = new Reporter(GeneData::currentGd, &printSettings);

        connect(GENE.reporter, SIGNAL(reportComplete(int)),this, SLOT(slotReportComplete(int)));
        connect(GENE.reporter, SIGNAL(reportFail(int, int)), this, SLOT(slotReportFail(int, int)));
        connect(GENE.reporter, SIGNAL(sigStatusBarMessage(QString)), this, SLOT(slotStatusBarMessage(QString)));

        GENE.locked = true;
        actionPrint->setDisabled(true);
        actionNew->setDisabled(true);
        actionOpen->setDisabled(true);
        GENE.reporter->start(QThread::HighestPriority);
    }
}

void MainWindow::slotSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(
            this,
            tr("Save File"),
            GET_DIRECTORY,
            tr("Gedcom 5.5 files (*.ged)"));

    if (fileName.isEmpty()) return;

    GENE.fileName = fileName;
    GENE.canBeSaved = true;
    GENE.isSaved = true;

    slotSave();    
}

void MainWindow::slotSave()
{
    GENE.printPolicy = PRINTPOLICY_1_EVERYTHING; // tï¿½mï¿½ vaikuttaa mm siihen, miten security attribuutit tallennetaan ja
    // mitï¿½ tietoja jï¿½tetï¿½ï¿½n tallentamatta. Kï¿½ytetï¿½ï¿½n eri tavalla kun tehdï¿½ï¿½n raportteja
    
    if (GENE.canBeSaved) Save::file(GENE.fileName);
    GENE.isSaved = true;
    slotStatusBarMessage("Saved as " + GENE.fileName);
}

void MainWindow::importStream(QTextStream * stream, bool activateAfterLoad, QString workName)
{
    loadThread = new Load(stream, workName, activateAfterLoad, this);

    connect(loadThread, SIGNAL(loadComplete()), this, SLOT(slotLoadComplete()));
    connect(loadThread, SIGNAL(loadFail(int)), this, SLOT(slotLoadFail(int)));
    connect(loadThread, SIGNAL(loadDialog(int, int, int, int, int, int, int, int, int)), this, SLOT(slotLoadDialog(int,int, int, int, int, int, int, int, int)));
    connect(loadThread, SIGNAL(sigMessage(QString)), this, SLOT(slotStatusBarMessage(QString)));
    connect(loadThread, SIGNAL(update(quint16)), this, SLOT(update(quint16)));

    loadThread->start(QThread::NormalPriority);
    slotStatusBarMessage("Importing...");
}

void MainWindow::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Open File"),
            GET_DIRECTORY,
            tr("Gedcom 5.5 files (*.ged)"));

    QRegExp dir("(.*)(\\/|\\\\)[\\w\\-]*\\.\\w*");

    dir.indexIn(fileName);

    SET_DIRECTORY(dir.cap(1));
    //qDebug() << "heihou" << dir.cap(1) << fileName;

    if (fileName != "") {
        file.setFileName(fileName);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox qmb;
            qmb.setText(tr("Could not open file"));
            qmb.exec();
            return;
        }

        importStream(pLoadStream, true, fileName);
    }

    GENE.isSaved = true;
}

void MainWindow::slotLoadComplete()
{
    statBar->showMessage("Load complete");    
    updateProjectMenu();
    update(GENE.rootId);
    GENE.start(QThread::NormalPriority);

    file.close();

    loadThread->deleteLater();
}

void MainWindow::slotLoadFail(int code)
{
    if (code == 1) {
        QMessageBox msgBox;
        msgBox.setText(tr("Error : The lines should be null-terminated. Could read only one huge line."));
        msgBox.exec();
        filenew();
        file.close();
    }
}

void MainWindow::slotLoadDialog(int header, int indi, int fam, int multi, int note, int repo, int sour, int subm, int prog)
{    
    if (header==-1) {
        lp = new LoadProgress();
        lp->show();
        return;
    }

    if (header==-2) {
        lp->close();
        delete lp;
        return;
    }

    lp->set(header, indi, fam, multi, note, repo, sour, subm, prog);
}

void MainWindow::slotShowHeader()
{
    HeaderDialog hd(GENE.header);
    hd.exec();
}

void MainWindow::slotStatusBarMessage(QString str)
{
    //qDebug() << str;
    messagePanel->addMessage(str);
}

void MainWindow::updateChildren(quint16 id)
{

    DE_BUG "MainWindow::updateChildren";
    if (id==0) {        
        childIdList.clear();
        childList->update();

        siblingIdList.clear();
        siblingList->update();

        cousinIdList.clear();
        cousinList->update();
    } else {
        INDI(id).getChilds(&childIdList, childList->noStepRelativesChecked);
        GENE.arrangePersons(&childIdList);
        childList->update();

        INDI(id).getSiblings(&siblingIdList, siblingList->noStepRelativesChecked);
        GENE.arrangePersons(&siblingIdList);
        siblingList->update();

        INDI(id).getCousins(&cousinIdList, cousinList->noStepRelativesChecked);
        GENE.arrangePersons(&cousinIdList);
        cousinList->update();
    }

    this->searchAndHistory->updt();

}

void MainWindow::updateProjectMenu()
{
    QMenu *qmn = this->uiMenuProjects;
    qmn->clear();

    for (quint8 x = 0 ; x<GeneData::gdCount ; x++) {
        qDebug() << "menu " << x << geneData[x].fileName;
        if (geneData[x].fileName != "") {
            QAction * act = qmn->addAction(geneData[x].fileName);
            connect(act, SIGNAL(triggered(bool)), projectSignalMapper, SLOT(map()));
            projectSignalMapper->setMapping(act, x);
        }
    }
}

void MainWindow::filenew()
{
    GeneData::gdLastUsed++;
    GeneData::currentGd = GeneData::gdLastUsed;

    this->slotStatusBarMessage("Create new database " + QString::number(GeneData::currentGd));

    qDebug() << "test " << GeneData::currentGd << GeneData::gdLastUsed;
    this->gdSpace(GeneData::currentGd) ;
qDebug () << "-----------";
qDebug() << "adding gd nr " << GeneData::currentGd << GeneData::gdLastUsed;
    GENE.format();
qDebug() << "format complete" << GeneData::currentGd << GeneData::gdCount;

    GENE.fileName = "<new project " + QString::number(GeneData::currentGd) + ">";
    panelCenter->current->id = 0;
    panelFather->id = 0;
    panelMother->id = 0;
    usrEvents.clear();
    famEvents.clear();
    GENE.currentId = 0;
    GENE.rootFams  = 0;
    usrEvents << "" << "Web";
    famEvents << "" << "Honey moon";

    int i;
    for (i=0 ; i<4 ; i++)  panelGp[i].id = 0;
    for (i=0 ; i<8 ; i++)  panelGGp[i].id = 0;
    for (i=0 ; i<16 ; i++) panelGGGp[i].id = 0;

    updateProjectMenu();

    setGd(GeneData::currentGd);

}

void MainWindow::langEnglish()
{
    trans->load(":/lang/lang/english.qm");

    childList->trans(tr("Childs"), tr("No step-childs"));
    cousinList->trans(tr("Cousins"), tr("No step-cousins"));
    siblingList->trans(tr("Siblings"), tr("No step-sisters"));
    retranslateUi(this);
    slotStatusBarMessage("English");
}

void MainWindow::langSuomi()
{    
    trans->load(":/lang/lang/suomi.qm");

    childList->trans(tr("Childs"), tr("No step-childs"));
    cousinList->trans(tr("Cousins"), tr("No step-cousins"));
    siblingList->trans(tr("Siblings"), tr("No step-sisters"));
    retranslateUi(this);
    slotStatusBarMessage("Suomeksi");
}

void MainWindow::update()
{
    DE_BUG "MainWindow::update" << QObject::sender();

    update(0);
}

void MainWindow::update(quint16 currentID)
{
    DE_BUG "(in) MainWindow::update(" << currentID << ")" << QObject::sender();
    // demoni oli terminoitu koska pGd oli vaihdettu kesken demonin ajon
    // ajetaan siis demoni uudelleen jonka pï¿½ï¿½tteksi tulee uusi update sinkku

    if (currentID) { GENE.browseHistory.removeAll(currentID); if (!INDI(currentID).deleted ) GENE.browseHistory.prepend(currentID); }

    if (currentID==0) currentID = GENE.currentId;
    else {
        if (GENE.currentId != currentID) slotStatusBarMessage("Person " + QString::number(currentID) + ", " + INDI(currentID).nameFamily + " " + INDI(currentID).nameFirst);
        GENE.currentId = currentID;
    }

    // tï¿½mï¿½ pï¿½ivittï¿½ï¿½ keskipaneelin : keskihenkilï¿½n, puolison, puolisolistan, kuvan
    panelCenter->update(currentID);

    // alla panelFather ja panelMother pï¿½ivitys pï¿½ivittï¿½ï¿½ rekursiivisesti kaikki "jï¿½lkelï¿½is panel:t"
    // niinkauan ettï¿½ jï¿½lkelï¿½ispanelien osoite on NULL, eli niitï¿½ ei ole.
    int f=INDI(currentID).getFather(); panelFather->updtAll(f);
    int m=INDI(currentID).getMother(); panelMother->updtAll(m);

    if (f || m) siblingList->enableAddButton(true); else siblingList->enableAddButton(false);
    if (currentID) childList->enableAddButton(true); else childList->enableAddButton(false);

    for (int i=0 ; i<7 ; i++) generation[i].setup();

    updateChildren(currentID);

    if (!GENE.canBeSaved) this->actionSave->setDisabled(true);
    else this->actionSave->setDisabled(false);

    if (GENE.header.note != "") this->uiTitle->setText(GENE.header.note);

    ((LineWidget*)(this->gridLayout->itemAtPosition(0,3)->widget()))->repaint();

    ((LineWidget*)(this->gridLayout->itemAtPosition(0,5)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(16,5)->widget()))->repaint();

    ((LineWidget*)(this->gridLayout->itemAtPosition(0,7)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(8,7)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(16,7)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(24,7)->widget()))->repaint();

    ((LineWidget*)(this->gridLayout->itemAtPosition(0,9)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(4,9)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(8,9)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(12,9)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(16,9)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(20,9)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(24,9)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(28,9)->widget()))->repaint();

    ((LineWidget*)(this->gridLayout->itemAtPosition(0,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(2,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(4,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(6,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(8,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(10,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(12,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(14,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(16,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(18,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(20,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(22,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(24,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(26,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(28,11)->widget()))->repaint();
    ((LineWidget*)(this->gridLayout->itemAtPosition(30,11)->widget()))->repaint();

    DE_BUG "(out) MainWindow::update(" << currentID << ")";
}

void MainWindow::slotDatabases()
{
    DbView dbv;
    update(dbv.exec());
}

void MainWindow::problemFinder()
{
    DE_BUG "problemFinder";
    GENE.wait(50000); // varmistetaan ettï¿½ demoni on suorittanut itsensï¿½ loppuun
    ProblemFinder pf;    
    update(pf.exec());
}

void MainWindow::setRootFams()
{
    DE_BUG "setRootFams";
    int rootFams = INDI(panelCenter->current->id).getFamS();

    if (rootFams == 0 || FAM(rootFams).childs.size() == 0) {
        QMessageBox msgBox;
        msgBox.setText(tr("Select a person who has at least one child."));
        msgBox.exec();
    }

    else {
        for (quint16 i=0 ; i<=GENE.indiLastUsed ; i++ ) {
            INDI(i).familyRelation = "";
        }
        GENE.rootFams = rootFams;
    }

    //update(); *** miksi tämä on tässä
}

void MainWindow::setRootIndi()
{
    if (panelCenter->current->id) GENE.rootId = panelCenter->current->id;
}

void MainWindow::slotSelectAncestors()
{
    DE_BUG "MainWindow:slotSelectAncestors";
    slotStatusBarMessage("Select ancestors");
    INDI(panelCenter->current->id).selectAncestors();
    update(0);
}

void MainWindow::slotSelectDescendants()
{
    slotStatusBarMessage("Select descendants");
    INDI(panelCenter->current->id).selectDescendants();
    update(0);
}

void MainWindow::slotSelectDescendantsAll()
{
    slotStatusBarMessage("Select ancestors with all cousins");
    INDI(panelCenter->current->id).selectAncestorsAll();
    update(0);
}

void MainWindow::slotSelectAll()
{
    slotStatusBarMessage("Select all");
    INDI(panelCenter->current->id).selectAll();
    update(0);
}

void MainWindow::slotSelectThis()
{
    slotStatusBarMessage("Select this");
    INDI(panelCenter->current->id).selected = true;
    update(0);
}

void MainWindow::slotSelectNone()
{
    slotStatusBarMessage("Select none");
    INDI(panelCenter->current->id).selectNone();
    update(0);
}

void MainWindow::slotCopy()
{
    slotStatusBarMessage("Copy to clipboard");
    QClipboard *cb = QApplication::clipboard();
    QString toClip;

    Save::gedComCoder(&toClip);

    cb->setText(toClip);
    //slotStatusBarMessage("Copy to clipboard");
}

void MainWindow::slotPasteAsNewProject()
{
    slotStatusBarMessage("Paste clipboard to a new project");
    QClipboard *cb = QApplication::clipboard();
    pasteString = cb->text();
    filenew();
    pPasteStream->seek(0);
    importStream(pPasteStream, true, "<new project " + QString::number(GeneData::currentGd) + ">");
}

void MainWindow::slotPaste()
{
    int x;

    slotStatusBarMessage("Paste clipboard to this project");
    int currentGd = GeneData::currentGd;

//-----------------
    GeneData::currentGd = GeneData::gdLastUsed + 1;
    int pasteGd = GeneData::currentGd;

    this->gdSpace(GeneData::currentGd) ;

    //GENE.clear();
    GENE.format();

    QClipboard *cb = QApplication::clipboard();
    pasteString = cb->text();
    pPasteStream->seek(0);
    importStream(pPasteStream, false, "");

//--------------------


    if (!this->loadThread->wait(10000)) {
        QMessageBox qmb;
        qmb.setText(tr("Paste failed"));
        qmb.exec();
        return;
    }

    // now the copy'ed file is entrirely available at geneData[pasteGd]. Lets copy it in the end of geneData[currentGd]

    int deltaIndi       = geneData[currentGd].indiLastUsed;
    int deltaFam        = geneData[currentGd].famiLastUsed;
    int deltaSource     = geneData[currentGd].sourLastUsed;
    int deltaRepo       = geneData[currentGd].repoLastUsed;
    int deltaMm         = geneData[currentGd].objeLastUsed;
    int deltaNote       = geneData[currentGd].noteLastUsed;
    int pasteIndiCnt    = geneData[pasteGd].indiLastUsed;
    int pasteFamCnt     = geneData[pasteGd].famiLastUsed;
    int pasteSourceCnt  = geneData[pasteGd].sourLastUsed;
    int pasteRepoCnt    = geneData[pasteGd].repoLastUsed;
    int pasteMmCnt      = geneData[pasteGd].objeLastUsed;
    int pasteNoteCnt    = geneData[pasteGd].noteLastUsed;

    GeneData::currentGd = currentGd;
    for (int i=1 ; i <= pasteIndiCnt ; i++) {
        geneData[currentGd].indiSpace( deltaIndi + i );
        geneData[currentGd].indi[deltaIndi + i] = geneData[pasteGd].indi[i];
        if (geneData[currentGd].indi[deltaIndi + i].famc.value) geneData[currentGd].indi[deltaIndi + i].famc.value += deltaFam;
        geneData[currentGd].indi[deltaIndi + i].id += deltaIndi;
        geneData[currentGd].indi[deltaIndi + i].rin += deltaIndi;

        for (x=0 ; x<geneData[currentGd].indi[deltaIndi + i].famSList.size() ; x++)
            geneData[currentGd].indi[deltaIndi + i].famSList[x] = geneData[currentGd].indi[deltaIndi + i].famSList.at(x) + deltaFam;
    }

    for (quint16 i = 0 ; i<=geneData[currentGd].indiLastUsed ; i++) geneData[currentGd].indi[i].parent = &geneData[currentGd];

    for (int f=1 ; f <= pasteFamCnt ; f++) {
        GENE.famiSpace( deltaFam + f );

        geneData[currentGd].fami[deltaFam + f] = geneData[pasteGd].fami[f];

        if (geneData[pasteGd].fami[f].husb) geneData[currentGd].fami[deltaFam + f].husb += deltaIndi;
        if (geneData[pasteGd].fami[f].wife) geneData[currentGd].fami[deltaFam + f].wife += deltaIndi;

        geneData[currentGd].fami[deltaFam + f].id += deltaFam;
        geneData[currentGd].fami[deltaFam + f].rin += deltaFam;

        for (x=0 ; x<geneData[currentGd].fami[deltaFam + f].childs.size() ; x++ )
            geneData[currentGd].fami[deltaFam + f].childs[x] = geneData[currentGd].fami[deltaFam + f].childs.at(x) + deltaIndi;
    }

    for (quint16 f = 0 ; f<=geneData[currentGd].famiLastUsed ; f++) geneData[currentGd].fami[f].parent = &geneData[currentGd];

    for (int s=0 ; s<pasteSourceCnt ; s++) {
        geneData[currentGd].sourSpace( deltaSource + s);
        geneData[currentGd].sour[deltaSource + s] = geneData[pasteGd].sour[s];
        geneData[currentGd].sour->id += deltaSource;
    }

    for (int r=0 ; r < pasteRepoCnt ; r++) {
        geneData[currentGd].repo[r + deltaRepo] = geneData[pasteGd].repo[r];
        if (geneData[currentGd].repo[r + deltaRepo].note.id) geneData[currentGd].repo[r + deltaRepo].note.id += deltaNote;
    }

    for (int o=0 ; o < pasteMmCnt ; o++) {
        geneData[currentGd].obje[o + deltaMm] = geneData[pasteGd].obje[o];
        if (geneData[currentGd].obje[o + deltaMm].id) geneData[currentGd].obje[o + deltaMm].id += deltaMm;
        if (geneData[currentGd].obje[o + deltaMm].note.id) geneData[currentGd].obje[o + deltaMm].note.id += deltaNote;
    }

    for (int n=0 ; n < pasteNoteCnt ; n++) {
        geneData[currentGd].note[n + deltaNote] = geneData[pasteGd].note[n];
        if (geneData[currentGd].note[n + deltaNote].id) geneData[currentGd].note[n + deltaNote].id += deltaNote;
    }

    GeneData::currentGd = currentGd;

    if (!GENE.currentId) GENE.currentId = 1;

    update();

    GENE.demonRun.dateAccuracyTest = true;
    GENE.demonRun.estimatedBirths = true;
    GENE.demonRun.familyRelations = true;
    GENE.demonRun.errors = true;
    if (RUN_CHECK) GENE.start(QThread::NormalPriority);
}

void MainWindow::slotReportComplete(int id)
{
    QMessageBox qmb;
    qmb.setText("Report complete");
    qmb.exec();
    geneData[id].locked = false; //   GENE.locked = false;

    this->actionPrint->setDisabled(false);
    this->actionNew->setDisabled(false);
    this->actionOpen->setDisabled(false);

    disconnect(geneData[id].reporter, SIGNAL(reportComplete(int)),this, SLOT(slotReportComplete(int)));
    disconnect(geneData[id].reporter, SIGNAL(reportFail(int, int)), this, SLOT(slotReportFail(int, int)));

    delete geneData[id].reporter;
    geneData[id].reporter = NULL;
}

void MainWindow::slotReportFail(int code, int gd)
{
    QMessageBox qmb;
    qmb.setText("Report error " + QString::number(code));
    qmb.exec();

    disconnect(geneData[gd].reporter, SIGNAL(reportComplete(int)),this, SLOT(slotReportComplete(int)));
    disconnect(geneData[gd].reporter, SIGNAL(reportFail(int, int)), this, SLOT(slotReportFail(int, int)));

    delete geneData[gd].reporter;
    geneData[gd].reporter = NULL;
}

void MainWindow::slotStepChildSelected(bool selection)
{
    INDI(GENE.currentId).getChilds(&childIdList, selection);
    childList->update();
}

/*
void MainWindow::slotStepSiblingsSelected(bool selection)
{

}
*/

void MainWindow::slotClose()
{    
    if (GENE.fileName.left(1) != "<") sets.setValue("lastFile", GENE.fileName);
    sets.setValue("geometry", saveGeometry());
    sets.setValue("windowState", saveState());
    close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    slotClose();
}

void MainWindow::slotRunGene()
{
    GENE.demonRun.dateAccuracyTest = true;
    GENE.demonRun.errors = true;
    GENE.demonRun.estimatedBirths = true;
    GENE.demonRun.familyRelations = true;
    GENE.run();
}

void MainWindow::slotSources()
{
    Sourceview sv;
    sv.exec();
}

void MainWindow::slotSearchReplace()
{
    SearchReplace sp;

    sp.exec();

}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    Settings::changed = false;
    QStringList accList; accList << "NA" << "~~" << "#" << "~" << "<" << ">" << "=" << "...";
    QStringList adopList; adopList << tr("Not Adopted") << tr("By Father") << tr("By Mother") << tr("By Both") << tr("Yes, not known which parent");
    QStringList quayList; quayList << tr("Quality NA") << tr("uncertain") << tr("questionable") << tr("second evidence") << tr("primary evidence");

    //usrEvents << "Web";
    //famEvents << "Honey moon";

    quayModel.setStringList(quayList);
    accModel.setStringList(accList);
    adopModel.setStringList(adopList);

    //reporter = NULL;

    setupUi(this);

    PANELTYPE panelType;

    pLoadStream     = new QTextStream(&file);
    pPasteStream    = new QTextStream(&pasteString);
    GeneData::currentGd = 0;

    generation      = new Generation[7];
    panelGGGGp      = new Panel[32];
    panelGGGp       = new Panel[16];
    panelGGp        = new Panel[8];
    panelGp         = new Panel[4];

    int i;

// --------------------------------------------------------------------------------------------------
// Column 0
    childList = new ChildList(&childIdList, this);

    gridLayout->addWidget(childList, 9, 0, 14, 1, 0);
    connect(childList, SIGNAL(sigUpdate(quint16)), this, SLOT(update(quint16)));
    connect(childList, SIGNAL(sigUpdate()), this, SLOT(update()));
    connect(childList, SIGNAL(noStepRelatives(bool)), this, SLOT(slotStepChildSelected(bool)));

    searchAndHistory = new SearchAndHistory();

    gridLayout->addWidget(searchAndHistory, 24, 0, 7, 3, 0);


// --------------------------------------------------------------------------------------------------
// Column 1 : Spacer
// Culumn 2 : Current panel
    panelCenter = new Current(this);
    gridLayout->addWidget(panelCenter, 9, 2, 14, 1, 0);
    panelCenter->current->pChildId = NULL;
    panelCenter->spouse->pChildId = NULL;
    panelCenter->current->panelType = PANEL_CURRENT;
    panelCenter->spouse->panelType = PANEL_SPOUSE;

// --------------------------------------------------------------------------------------------------
// Column 3 : LineWidget
// Column 4 : Mother, Father, sibligns

    panelFather = new Panel();
    panelMother = new Panel();

    panelFather->setup(5, true, false);
    panelMother->setup(5, true, false);

    siblingList = new SiblingList(&siblingIdList, this);
    connect(siblingList, SIGNAL(sigUpdate(quint16)), this, SLOT(update(quint16)));
    connect(siblingList, SIGNAL(sigUpdate()), this, SLOT(update()));

    gridLayout->addWidget(panelFather,      2,  4, 6, 1, 0); // columns 0, 1, 2
    gridLayout->addWidget(siblingList,      9, 4, 14, 1, 0); // columns 3, 4, 5, 6
    gridLayout->addWidget(panelMother,      24, 4, 6, 1, 0);

    panelFather->panelType = PANEL_MALE;
    panelMother->panelType = PANEL_FEMALE;

    panelFather->pChildId = &panelCenter->current->id;
    panelMother->pChildId = &panelCenter->current->id;

    panelFather->setStyleSheet(MALESTYLE);
    panelMother->setStyleSheet(FEMALESTYLE);
    gridLayout->addWidget(new LineWidget(panelCenter, panelFather, panelMother), 0, 3, 32, 1, 0);

// --------------------------------------------------------------------------------------------------
// Column 5 : LineWidget
// Column 6 : gp
    cousinList = new CousinList(&cousinIdList, this);
    connect(cousinList, SIGNAL(sigUpdate(quint16)), this, SLOT(update(quint16)));
    connect(cousinList, SIGNAL(sigUpdate()), this, SLOT(update()));

    panelType = PANEL_MALE;
    for (i=0 ; i<4 ; i++) {
        panelGp[i].panelType = panelType;
        panelGp[i].setup(5, true, false);
        if ( panelType == PANEL_MALE ) panelType = PANEL_FEMALE ; else panelType = PANEL_MALE;
        connect(&panelGp[i], SIGNAL(sigUpdate(quint16)), this, SLOT(update(quint16)));
        connect(&panelGp[i], SIGNAL(sigStatusBarMessage(QString)), this, SLOT(slotStatusBarMessage(QString)));
    }
    panelGp[0].pChildId = &panelFather->id;
    panelGp[1].pChildId = &panelFather->id;
    panelGp[2].pChildId = &panelMother->id;
    panelGp[3].pChildId = &panelMother->id;

    panelFather->pFather = &panelGp[0];
    panelFather->pMother = &panelGp[1];
    panelMother->pFather = &panelGp[2];
    panelMother->pMother = &panelGp[3];

    gridLayout->addWidget(&panelGp[0], 0,  6, 4, 1, 0);
    gridLayout->addWidget(&panelGp[1], 5,  6, 4, 1, 0);
    gridLayout->addWidget(cousinList,  10, 6, 12, 1, 0);
    gridLayout->addWidget(&panelGp[2], 23, 6, 4, 1, 0);
    gridLayout->addWidget(&panelGp[3], 28, 6, 4, 1, 0);

    gridLayout->addWidget(new LineWidget(panelFather, &panelGp[0], &panelGp[1]), 0, 5, 16, 1, 0);
    gridLayout->addWidget(new LineWidget(panelMother, &panelGp[2], &panelGp[3]), 16, 5, 16, 1, 0);

// --------------------------------------------------------------------------------------------------
// Column 7 : LineWidget
// Column 8 : GGp
    panelType = PANEL_MALE;

    int heightGGp;
    if (this->size().height() > 650) heightGGp = 4;
    if (this->size().height() < 650) heightGGp = 3;

    for (i=0 ; i<8 ; i++) {
        panelGGp[i].panelType = panelType;
        panelGGp[i].setup(heightGGp, true, false);
        if ( panelType == PANEL_MALE ) panelType = PANEL_FEMALE ; else panelType = PANEL_MALE;
        gridLayout->addWidget(&panelGGp[i], i*4, 8, 4, 1, 0);
        connect(&panelGGp[i], SIGNAL(sigUpdate(quint16)), this, SLOT(update(quint16)));
        connect(&panelGGp[i], SIGNAL(sigStatusBarMessage(QString)), this, SLOT(slotStatusBarMessage(QString)));
        panelGGp[i].pChildId = &panelGp[(int)(i/2)].id;
    }

    for (i=0 ; i<4 ; i++) {
        panelGp[i].pFather = &panelGGp[i*2];
        panelGp[i].pMother = &panelGGp[i*2 + 1];

        gridLayout->addWidget(new LineWidget(&panelGp[i], &panelGGp[i*2], &panelGGp[i*2+1]),  i*8, 7, 8, 1, 0);
    }


// --------------------------------------------------------------------------------------------------
// Column 9  : LineWidget
// Column 10 : GGGp
    panelType = PANEL_MALE;

    int heightGGGp;
    if (this->size().height() > 910) heightGGGp = 3;
    if (this->size().height() < 910) heightGGGp = 1;

    for (i=0 ; i<16 ; i++) {
        panelGGGp[i].panelType = panelType;
        panelGGGp[i].setup(heightGGGp, true, false);
        if ( panelType == PANEL_MALE ) panelType = PANEL_FEMALE; else panelType = PANEL_MALE;
        gridLayout->addWidget(&panelGGGp[i], i*2, 10, 2, 1, 0);
        connect(&panelGGGp[i], SIGNAL(sigUpdate(quint16)), this, SLOT(update(quint16)));
        connect(&panelGGGp[i], SIGNAL(sigStatusBarMessage(QString)), this, SLOT(slotStatusBarMessage(QString)));
        //if (panelType == PANEL_MALE) panelGGGp[i].setStyleSheet(MALESTYLE);

        panelGGGp[i].pChildId = &panelGGp[(int)(i/2)].id;
    }

    for (i=0 ; i<8 ; i++) {
        panelGGp[i].pFather = &panelGGGp[i*2];
        panelGGp[i].pMother = &panelGGGp[i*2 + 1];

        gridLayout->addWidget(new LineWidget(&panelGGp[i], &panelGGGp[i*2], &panelGGGp[i*2+1]),  i*4, 9, 4, 1, 0);
    }

// --------------------------------------------------------------------------------------------------
// column 11 : LineWidget
// Column 12 : GGGGp

    panelType = PANEL_MALE;
    for (i=0 ; i<32 ; i++) {
        panelGGGGp[i].panelType = panelType;
        if ( panelType == PANEL_MALE ) panelType = PANEL_FEMALE ; else panelType = PANEL_MALE;
        panelGGGGp[i].setup(1, true, false);
        gridLayout->addWidget(&panelGGGGp[i], i, 12, 1, 1, 0);
        connect(&panelGGGGp[i], SIGNAL(sigUpdate(quint16)), this, SLOT(update(quint16)));
        connect(&panelGGGGp[i], SIGNAL(sigStatusBarMessage(QString)), this, SLOT(slotStatusBarMessage(QString)));
        //if (panelType == PANEL_MALE) panelGGGGp[i].setStyleSheet(MALESTYLE);

        panelGGGGp[i].pChildId = &panelGGGp[(int)(i/2)].id;
    }

    for (i=0 ; i<16 ; i++) {
        panelGGGp[i].pFather = &panelGGGGp[i*2];
        panelGGGp[i].pMother = &panelGGGGp[i*2 + 1];

        gridLayout->addWidget(new LineWidget(&panelGGGp[i], &panelGGGGp[i*2], &panelGGGGp[i*2+1]),  i*2, 11, 2, 1, 0);
    }

    for (i=0 ; i<32 ; i++) {
        panelGGGGp[i].pFather = nullptr;
        panelGGGGp[i].pMother = nullptr;
    }

// --------------------------------------------------------------------------------------------------


    for (i=0 ; i<7 ; i++) {
        generation[i].generationOnUi = i;
        gridLayout->addWidget(&generation[i], 33, i*2);
    }
    gridLayout->setRowMinimumHeight(33, 22);
    gridLayout->setRowStretch(33,2);

    messagePanel = new MessagePanel(this);
    gridLayout->addWidget(messagePanel, 0, 0, 6, 3, 0);

    uiTitle = new QLabel("Qenes v18.8");
    uiTitle->setFrameStyle(1);
    uiTitle->setFrameShadow(QFrame::Raised);
    uiTitle->setFont(QFont("Times", 15, 5, false));
    uiTitle->setStyleSheet("QLabel { background-color: rgb(170, 170, 170); }");
    uiTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(uiTitle, 6, 0, 2, 3, 0);


    gridLayout->setColumnMinimumWidth(3,5);
    gridLayout->setColumnMinimumWidth(5,5);
    gridLayout->setColumnMinimumWidth(7,5);
    gridLayout->setColumnMinimumWidth(9,5);

    //for (int i=0 ; i<12 ; i++) gridLayout->setColumnStretch(i,1);
    for (int i=0 ; i<=12 ; i+=2) gridLayout->setColumnStretch(i,100);
    for (int i=1 ; i<11 ; i+=2) gridLayout->setColumnStretch(i,5);
    gridLayout->setColumnStretch(1,1);

    debug1key           = new QShortcut(Qt::CTRL + Qt::Key_1, this, SLOT(debug1()));
    debug2key           = new QShortcut(Qt::CTRL + Qt::Key_2, this, SLOT(debug2()));
    debug3key           = new QShortcut(Qt::CTRL + Qt::Key_3, this, SLOT(debug3()));
    debug4key           = new QShortcut(Qt::CTRL + Qt::Key_4, this, SLOT(debug4()));
    debug5key           = new QShortcut(Qt::CTRL + Qt::Key_5, this, SLOT(debug5()));
    debug6key           = new QShortcut(Qt::CTRL + Qt::Key_6, this, SLOT(debug6()));

    keyRunDemon_P       = new QShortcut(Qt::CTRL + Qt::Key_P, this, SLOT(slotRunGene()));
    keyUpdate_U         = new QShortcut(Qt::CTRL + Qt::Key_U, this, SLOT(update()));
    keyCopy             = new QShortcut(Qt::CTRL + Qt::Key_C, this, SLOT(slotCopy()));
    keyPaste            = new QShortcut(Qt::CTRL + Qt::Key_V, this, SLOT(slotPaste()));
    keySelectAncestors  = new QShortcut(Qt::CTRL + Qt::Key_A, this, SLOT(slotSelectAncestors()));

    connect(panelFather,        SIGNAL(sigUpdate(quint16)),  this, SLOT(update(quint16)));
    connect(panelFather,        SIGNAL(sigStatusBarMessage(QString)), this, SLOT(slotStatusBarMessage(QString)));
    connect(panelMother,        SIGNAL(sigUpdate(quint16)),  this, SLOT(update(quint16)));
    connect(panelMother,        SIGNAL(sigStatusBarMessage(QString)), this, SLOT(slotStatusBarMessage(QString)));
    connect(panelCenter,        SIGNAL(sigUpdate(quint16)),  this, SLOT(update(quint16)));
    connect(actionNew,          SIGNAL(triggered(bool)), this, SLOT(filenew()));
    connect(actionOpen,         SIGNAL(triggered(bool)), this, SLOT(loadFile()));
    connect(actionExit,         SIGNAL(triggered(bool)), this, SLOT(slotClose()));
    connect(actionDatabases,    SIGNAL(triggered(bool)), this, SLOT(slotDatabases()));
    connect(actionHeader,       SIGNAL(triggered(bool)), this, SLOT(slotShowHeader()));
    connect(actionSave,         SIGNAL(triggered(bool)), this, SLOT(slotSave()));
    connect(actionSaveAs,       SIGNAL(triggered(bool)), this, SLOT(slotSaveAs()));
    connect(actionPrint,        SIGNAL(triggered(bool)), this, SLOT(slotPrint()));
    connect(actionSettigns,     SIGNAL(triggered(bool)), this, SLOT(settings()));
    connect(actionProblemFinder,SIGNAL(triggered(bool)), this, SLOT(problemFinder()));            
    connect(actionEnglish,      SIGNAL(triggered(bool)), this, SLOT(langEnglish()));
    connect(actionSuomi,        SIGNAL(triggered(bool)), this, SLOT(langSuomi()));
    connect(actionRelationship_Calculator, SIGNAL(triggered(bool)), this, SLOT(setRootFams()));
    connect(actionSet_root_individual,  SIGNAL(triggered(bool)), this, SLOT(setRootIndi()));
    connect(actionAncestors,    SIGNAL(triggered(bool)), this, SLOT(slotSelectAncestors()));
    connect(actionDescentants,  SIGNAL(triggered(bool)), this, SLOT(slotSelectDescendants()));
    connect(actionAncestors_and_all_their_Descentats, SIGNAL(triggered(bool)), this, SLOT(slotSelectDescendantsAll()));
    connect(actionAll,          SIGNAL(triggered(bool)), this, SLOT(slotSelectAll()));
    connect(actionUnselect,     SIGNAL(triggered(bool)), this, SLOT(slotSelectNone()));
    connect(actionThis,         SIGNAL(triggered(bool)), this, SLOT(slotSelectThis()));
    connect(searchAndHistory,   SIGNAL(update(quint16)),     this, SLOT(update(quint16)));
    connect(actionCopy_Selected,SIGNAL(triggered(bool)), this, SLOT(slotCopy()));
    connect(actionPaste_as_new_project, SIGNAL(triggered(bool)), this, SLOT(slotPasteAsNewProject()));
    connect(actionPaste,        SIGNAL(triggered(bool)), this, SLOT(slotPaste()));
    connect(actionSources,      SIGNAL(triggered(bool)), this, SLOT(slotSources()));
    connect(actionSearch_Replace,SIGNAL(triggered(bool)),this, SLOT(slotSearchReplace()));
    connect(actionMake_Default_Setupvalues, SIGNAL(triggered()), this, SLOT(makeDefaultSetupValues()));
    connect(actionUse_Default_Setupvalues, SIGNAL(triggered()), this, SLOT(useDefaultSetupValues()));

    projectSignalMapper = new QSignalMapper(this);
    connect(projectSignalMapper, SIGNAL(mapped(int)), this, SLOT(setGd(int)));

    setStatusBar(statBar);
    statBar->setEnabled(true);
    QString message;
    message = "qenes v" + QDate::currentDate().toString("dd.MM.yyyy") + " (c) Ari Tahti 2009, 2010, 2011. Source code availabe from sourceforge.org under gpl2";
    statBar->showMessage(message);


    // --------------------------------------------------------------------------------

    GeneData::gdLastUsed = -1; // myï¿½hemmin kutsutaan filenew, jossa ensimmï¿½isen taulukon jï¿½rjestys nostetaan 0:n

    QFile file;
    QTextStream in;

    file.setFileName("malenames.ini");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    in.setDevice(&file);
    in.setCodec("ISO 8859-1");
    while (!in.atEnd()) boyNames.append(in.readLine().trimmed());
    file.close();

    file.setFileName("femalenames.ini");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    in.setDevice(&file);
    in.setCodec("ISO 8859-1");
    while (!in.atEnd()) girlNames.append(in.readLine().trimmed());
    file.close();

    file.setFileName("patronyms.ini");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    in.setDevice(&file);
    in.setCodec("ISO 8859-1");
    while (!in.atEnd()) patronyms.append(in.readLine().trimmed());
    file.close();

    file.setFileName("places.ini");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    in.setDevice(&file);
    in.setCodec("ISO 8859-1");
    while (!in.atEnd()) places.append(in.readLine().trimmed());
    file.close();

    if (!sets.contains("INSTALLED")) this->useDefaultSetupValues();

    printSettings.clear();

    restoreGeometry(sets.value("geometry").toByteArray());
    restoreState(sets.value("windowState").toByteArray());

    Settings::makeEventMappings();

    filenew();

    new QShortcut(Qt::Key_Return, panelCenter, SLOT(slotShowPerson()));
    new QShortcut(Qt::Key_Left, childList, SLOT(slotAdd()));
    new QShortcut(Qt::Key_Up, panelFather, SLOT(center()));
    new QShortcut(Qt::Key_Down, panelMother, SLOT(center()));
    new QShortcut(Qt::Key_Escape, this, SLOT(close()));
    new QShortcut(Qt::Key_L, this, SLOT(slotLoadLastFile()));

    setWindowIcon(QIcon(":/icon/ui/qenes-icon.jpg"));

    QString fileName = sets.value("lastFile").toString();

    if (fileName != "") this->slotStatusBarMessage("Press l to load " + fileName);

}

#define G4pMinHeight  760
#define G4pMinWidth  1155

void MainWindow::resizeEvent(QResizeEvent *event)
{    
    Q_UNUSED(event);
//    qDebug() << this->size().width() << this->size().height();

        int i;

    // Tutkitaan mitï¿½ on tilaa leveyssuunnassa nï¿½yttï¿½ï¿½, ja nï¿½ytetï¿½ï¿½n jos tilaa on
    if ( ( this->size().width() <= G4pMinWidth ) || ( this->size().height() <= G4pMinHeight ) ) this->hideGgggps(true);
    else if ( ( this->size().width() >= G4pMinWidth ) || ( this->size().height() >= G4pMinHeight ) ) this->hideGgggps(false);

    if ( size().width() <= 1000 ) this->hideGggps(true);
    if ( size().width() >= 1000 ) this->hideGggps(false);

    if ( size().width() <= 825  ) this->hideGgps(true);
    if ( size().width() >= 825  ) this->hideGgps(false);

    if ( panelGGGp[0].heightId != 3 && size().height() > 920) for (i=0 ; i<16 ; i++) this->panelGGGp[i].setup(3, true, false);
    if ( panelGGGp[0].heightId != 2 && (size().height() > 700 && size().height() < 920) ) for (i=0 ; i<16 ; i++) panelGGGp[i].setup(2, true, false);
    if ( panelGGGp[0].heightId != 1 && (size().height() > 0 && size().height() < 650) ) for (i=0 ; i<16 ; i++) panelGGGp[i].setup(1, true, false);

    if ( panelGGp[0].heightId != 5 && size().height() > 880) for (i=0 ; i<8 ; i++) panelGGp[i].setup(5, true, false);
    if ( panelGGp[0].heightId != 4 && (size().height() > 620 && size().height() < 830) ) for (i=0 ; i<8 ; i++) panelGGp[i].setup(4, true, false);
    if ( panelGGp[0].heightId != 3 && (size().height() > 520 && size().height() < 620) ) for (i=0 ; i<8 ; i++) panelGGp[i].setup(3, true, false);
    if ( panelGGp[0].heightId != 2 && (size().height() > 0 && size().height() < 520) ) for (i=0 ; i<8 ; i++) panelGGp[i].setup(2, true, false);

    if ( panelGp[0].heightId != 5 && size().height() > 800) for (i=0 ; i<4 ; i++) panelGp[i].setup(5, true, false);
    if ( panelGp[0].heightId != 4 && (size().height() > 580 && size().height() < 800) ) for (i=0 ; i<4 ; i++) panelGp[i].setup(4, true, false);
    if ( panelGp[0].heightId != 3 && (size().height() > 520 && size().height() < 580) ) for (i=0 ; i<4 ; i++) panelGp[i].setup(3, true, false);
    if ( panelGp[0].heightId != 2 && (size().height() > 0 && size().height() < 520) ) for (i=0 ; i<4 ; i++) panelGp[i].setup(2, true, false);

    if ( panelFather->heightId !=5 && size().height() > 500) { panelFather->setup(5, true, false); panelMother->setup(5, true, false); }
    if ( panelFather->heightId !=4 && (size().height() > 0 && size().height() < 500 ) ) { panelFather->setup(4, true, false); panelMother->setup(4, true, false); }

}

MainWindow::~MainWindow()
{
    if (Settings::changed) Settings::saveFromRamToDisk();
    delete [] panelGGGGp;
    delete [] panelGGGp;
    delete [] panelGGp;
    delete [] panelGp;
    delete panelCenter;
    delete panelFather;
    delete panelMother;
    delete childList;
    delete siblingList;
    delete cousinList;
    delete messagePanel;
    delete searchAndHistory;
    delete uiTitle;
    delete [] generation;
}
