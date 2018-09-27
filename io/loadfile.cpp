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

#include "loadfile.h"
#include "data/genedata.h"
#include "macro.h"

#include <QList>
#include <QDateTime>
#include <data/setupvalues.h>
#include <QMessageBox>

extern  GeneData        * geneData;
extern QStringList     usrEvents, famEvents;

// ei lue kuin yhden submitterin, joka on headerin submitter

/* How this works.
   File is read line by line. The level number, command and paramet will be extracted. eg,
    0 @indi1234@ INDI => level = 0, param = @indi1234@, command = INDI

   The commands will be saved in a command[] table, so that command of a level will be saved on a same
   index. For example.
       0 @indi1234@ INDI
       1 NAME Joe Juser

   Then command[0] = INDI, command[1] = NAME. Next step, what is being read will be analyzed in
   analyse****record method. These methods interpret the commands saved on a command[] and will save
   the param in a temporary variable. There are many temp variables for each use, eg tmpPd for saving
   person data, tmpFd for saving family data, tmpAddress for saving address data etc... for example,
   in the previous example analyseIndiRecord would save the param (=Joe Juser) into tmpFd.name.
   If the record would continue
       0 @indi1234@ INDI
       1 NAME Joe Juser
       2 Address osoite
   Then the param (osoite) would be saved in a tmpAddress.line = param. In this phase, the
    analyse****record methods wont know where the tmp** variables will be used.

    The next logical step is to find out that a record has ended. For example, if the reading would
    continue
       0 @indi1234@ INDI
       1 NAME Joe Juser
       2 Address osoite
       1 BIRT 1.2.3

   Then the address record has ended. For that purpose, commandOld[] saves the commands of the previous
   line. In this example, we have thus found that now we read BIRT event, and the previous address has
    ended. Therefore, tmpAddress should be saved, in this case the commandOld = INDI/NAME/ADDR which can
    be used to find out to save tmpFd.address = tmpAddress.

    When the reading will reach the level 0 again, eg
       0 @indi1234@ INDI
       1 NAME Joe Juser
       2 Address osoite
       1 BIRT 1.2.3
       0 @fam6789@ FAM

   This indicates that record has ended. Now, we use again commandOld to find out what we were just
   reading. We'll find that commandOld[0] = INDI, and know that tmpPd should be saved finally to the
   table (after making sure that there's space in the table). Immediately after that, the content of
    a new line will be checked, eg. now we'll find that command[0] = FAM. The id number will be given
    to tmp variable to make sure, that the final record in the table will have its id number (that
    may be needed later).

    The very last and messy step is to convert gedcom tags into integer references in the table.
    For example, the if the "@indi1234@" was saved in INDI[4], that info will be saved in indList and
    all @indi1234@ tags should be converted into integer 4.

  */

QList<QString> monthsFi, monthsEn;

extern QStringList famEvents;

MULTIMEDIA_FORMAT Load::mmFormat(QString str)
{
    if (str == "jpeg")  return JPEG_MM;
    if (str == "JPEG")  return JPEG_MM;
    
    if (str == "gif")   return GIF_MM;
    if (str == "GIF")   return GIF_MM;
    
    if (str == "bmp")   return BMP_MM;
    if (str == "BMP")   return BMP_MM;
        
    if (str == "ole")   return OLE_MM;
    if (str == "OLE")   return OLE_MM;
    
    if (str == "pcx")   return PCX_MM;
    if (str == "PCX")   return PCX_MM;
    
    if (str == "tiff")  return TIFF_MM;
    if (str == "TIFF")  return TIFF_MM;
    
    if (str == "wav")   return WAV_MM;    
    if (str == "WAV")   return WAV_MM;
    return NO_MM;
}

QString Load::pedi(PEDI_TYPE pt)
{
    if (pt == ADOPTED_PEDI) return "adopted";
    if (pt == BIRTH_PEDI)   return "birth";
    if (pt == FOSTER_PEDI)  return "foster";
    if (pt == SEALING_PEDI) return "sealing";
    return "";
}

PEDI_TYPE Load::pedi(QString str)
{
    if (str == "adopted")   return ADOPTED_PEDI;
    if (str == "birth")     return BIRTH_PEDI;
    if (str == "foster")    return FOSTER_PEDI;
    if (str == "sealing")   return SEALING_PEDI;
    return NO_PEDI;
}

QUALITY Load::quality(QString str)
{
    if (str == "0") return QUAY0;
    if (str == "1") return QUAY1;
    if (str == "2") return QUAY2;
    if (str == "3") return QUAY3;
    return NO_QUAY;
}

QString Load::familyEventRole(ENTRYf_ROLE fer)
{
    if (fer == CHIL_ROLE) return "child";
    if (fer == HUSB_ROLE) return "husband";
    if (fer == WIFE_ROLE) return "wife";
    if (fer == MOTH_ROLE) return "mother";
    if (fer == FATH_ROLE) return "father";
    if (fer == SPOU_ROLE) return "spouse";
    return "";
}

ENTRYf_ROLE Load::familyEventRole(QString fer)
{
    if (fer == "child") { return CHIL_ROLE; }
    if (fer == "husband") { return HUSB_ROLE; }
    if (fer == "wife") { return WIFE_ROLE; }
    if (fer == "mother") { return MOTH_ROLE; }
    if (fer == "father") { return FATH_ROLE; }
    if (fer == "spouse") { return SPOU_ROLE; }
    return NO_FAMILY_EVENT_ROLE;
}

void Load::headRecordEnded()
{
    if (GENE.header.language == "") {} // onko tämä tarpeellinen??
}

bool Load::findGedComTag(QString str, QString* pTag)
{
    // skjdfhskld@1233456@skjfhskdhfskd => palauttaa @1233456@
    if (str.count("@") != 2) return false;
    *pTag = str.mid(str.indexOf("@"), 1+str.indexOf("@", str.indexOf("@")+1) - str.indexOf("@"));
    return true;
}

void Load::gedComTimeAnalyse(QString* line, QTime *time)
{
    bool ok;
    QRegExp rx("([^0-9][0-9]|[0-1][0-9]|[2][0-3]):([0-6][0-9]):([0-9][0-9])(.|"")([0-9][0-9]|"")");
    rx.indexIn(*line);
    time->setHMS(rx.cap(1).toInt(&ok), rx.cap(2).toInt(&ok), rx.cap(3).toInt(&ok), rx.cap(5).toInt(&ok));
}

void Load::gedComDayAnalyze(QString* dayString, Date* date)
{
    int mo1, mo2, da1, da2;
    date->setAcc(NA);
    bool ok;
    //QString kk;

    //QRegExp dayExp("(NA|ABT|CAL|EST|BEF|AFT|EXA|FROM)? ?(\\d{1,2} )? ?(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC|TAM|HEL|MAA|HUH|TOU|KES|HEI|ELO|SYY|LOK|MAR|JOU)? ?(\\d{4})? ?T?O? ?(\\d{1,2} )? ?(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)? ?(\\d{4})?", Qt::CaseInsensitive);
    QRegExp dayExp("(NA|ABT|CAL|EST|BEF|AFT|EXA|FROM)? ?(\\d{1,2} )? ?(\\S{3})? ?(\\d{4})? ?T?O? ?(\\d{1,2} )? ?(\\S{3})? ?(\\d{4})?", Qt::CaseInsensitive);

    dayExp.indexIn(*dayString);

    mo1 = 0; mo2 = 0; da1 = 0; da2 = 0;


    // mo1 = joko ainoa kuukausi tai "from" kuukausi
    // mo2 = "to" kuukausi

    QString monthFrom = dayExp.cap(3).toLower();
    QString monthTo = dayExp.cap(6).toLower();

    if (monthLanguage == MO_FINNISH) {
        mo1 = monthsFi.indexOf(monthFrom) + 1;
        mo2 = monthsFi.indexOf(monthTo) + 1;
    }

    if (monthLanguage == MO_ENGLISH) {
        mo1 = monthsEn.indexOf(monthFrom) + 1;
        mo2 = monthsEn.indexOf(monthTo) + 1;
    }

    if (mo1 == 0) mo1 = 1;
    if (mo2 == 0) mo2 = 1;

    da1 = dayExp.cap(2).toInt(&ok, 10);
    da2 = dayExp.cap(5).toInt(&ok, 10);

    if (da1 == 0) da1 = 1;
    if (da2 == 0) da2 = 1;

    date->setDate1(dayExp.cap(4).toInt(&ok, 10),
                   mo1,
                   da1);

    if (dayExp.cap(7) == "") date->setDate2(date->day1);
    else date->setDate2(dayExp.cap(7).toInt(&ok, 10),
                   mo2,
                   da2);

    date->setAcc((ACCURACY)Date::accuracyFromGedCom(dayExp.cap(1))); //
    if ((date->getAcc() == -1) && (date->known) ) date->setAcc(EXA);

    if (date->getAcc() != FRO) {
        if (dayExp.cap(2) == "") date->setAcc(EST);
        if (dayExp.cap(3) == "") date->setAcc(ABT);
    }
    if (date->day1.year() == 0) cntErrorsDate++;
    cntDates++;
//    qDebug() << date->day1 << *dayString << cntErrorsDate;
}

bool Load::gedComLineAnalyze(QString* ln, int* level, QString* cmd, QString* param)
{
    *level = ln->left(2).toInt();

    QRegExp gedComLine;

    if (ln->right(1) == "\n")
        *ln = ln->left(ln->length()-1);

    switch (*level) {
        case 0 : gedComLine.setPattern("0 (\\@.+\\@)? ?(\\S{3,4})"); break;        
        default : gedComLine.setPattern("\\d (\\S{3,4})(\\s(.*))?"); break;
    }

    gedComLine.indexIn(*ln);

    switch (*level) {
        case 0 : {
            *cmd = gedComLine.cap(2);
            *param = gedComLine.cap(1);            
            if (param->right(1) == "\n") *param = param->left(param->length()-1);                        
        }; break;
        default : {
            *cmd = gedComLine.cap(1);
            *param = gedComLine.cap(3);
            if (param->right(1) == "\n") *param = param->left(param->length()-1);            
        }; break;
    }

    if (*cmd != "") return true; else { *cmd = "ERROR" ; return false; }
}

int Load::commandLevel(QString cmd[], QString command)
{
    for (int i=0 ; i<9 ; i++) if (cmd[i] == command) return i;
    return -1;
}

bool Load::isCommand(QString cmd[], QString cmd0, QString cmd1, QString cmd2)
{
    int i = 0;

    if (cmd0 != "**") {
        if ( ( (cmd[0] == cmd0) || (cmd0 == "*") ) &&
             ( (cmd[1] == cmd1) || (cmd1 == "*") ) && //) return true;
             ( (cmd[2] == cmd2) || (cmd2 == "*") ) ) return true;
    } else {
        while ( ( cmd[i] != cmd1) && (i<8) ) i++;
        if ( (cmd[i] == cmd1) && ( ( cmd2 == cmd[i+1] ) || ( cmd2 == "*" ) ) ) return true;
    }

    return false;
}

bool Load::isEntry(QString cmd[])
{
    if (cmd[0] == "INDI") if (Entry::gedComToInt(cmd[1], true )) return true;
    if (cmd[0] == "FAM")  if (Entry::gedComToInt(cmd[1], false)) return true;
    return false;
}

bool Load::hasRecordEnded(QString cmd, QString commandOld[], QString command[])
{    

    /* Tässä tsekataan onko record vaihtunut niin, että tietyn tyyppisen recordin jälkeen tulee toinen samantyyppinen
     * record.
     * INDI OBJE FILE file1
     * INDI OBJE
     */

    if ( isCommand(commandOld, "**", cmd, "*") && (isCommand(command, "**", cmd, "") ) ) return true;

    /* onko note record loppunut
      INDI NOTE
      INDI CHAR */

    if ( isCommand(commandOld, "**", cmd, "*") && !isCommand(command, "**", cmd, "*") ) return true;

    /* Tässä tarkastetaan, onko command level lopusta lukien vaihtanut paikkaa, eli esim.
       Old : INDI NOTE OBJE NOTE
       New : INDI NOTE OBJE
       Huomataan että ennen NOTE löytyi command[3]:sta (lopustapäin lukien), ja jälkeen se löytyy vain command[1]:stä.
       Jos lukeminen alotettaisiin alusta, todettaisiin vain että ennen ja jälkeen NOTE löytyy command[1]:stä eikä huomattaisi
       hierarkissa myöhemmin olevan NOTE:n loppumista.
      */

    for (int i=8 ; i>=0 ; i--) {
        if (commandOld[i] == cmd) {

            if (command[i] != cmd) return true;
            else return false;
        }
    }

    return false;
}

void Load::noteRecordEnded()
{
    GENE.indiSpace(idIndi);
    GENE.famiSpace(idFam);
    if ( isCommand(commandOld, "INDI", "NOTE", "*") ) {
    if (tmpNote.text == "QENES ROOT") { GENE.rootId = idIndi; used = true; }
        else {
            tmpPd.note = tmpNote;
            tmpNote.clear();
            used = true;
        }
    }
    if ( isCommand(commandOld, "HEAD", "NOTE", "*") ) { GENE.header.note = tmpNote.text; tmpNote.clear(); used = true; }
    if ( isCommand(commandOld, "**", "SOUR", "NOTE") ) { tmpSource.note = tmpNote; tmpNote.clear(); used = true;}
    if ( isCommand(commandOld, "FAM", "NOTE", "*") ) {
        if (tmpNote.text == "QENES ROOT") { GENE.rootFams = idFam; used = true; }
        else {
            tmpFd.note = tmpNote;
            tmpNote.clear();
            used = true;
        }
    }
    if ( isCommand(commandOld, "INDI", "NAME", "NOTE") ) { tmpPd.nameNote = tmpNote; tmpNote.clear(); used = true;}
    if ( isCommand(commandOld, "**", "REPO", "NOTE") ) { tmpRepo.note = tmpNote; tmpNote.clear(); used = true;}
    if ( isCommand(commandOld, "**", "OBJE", "NOTE") ) { tmpMultimedia.note = tmpNote; tmpNote.clear(); used = true;}

    if (isEntry(commandOld)) { tmpEvent.note = tmpNote; tmpNote.clear(); used = true; }
}

void Load::addrRecordEnded()
{
    if (isCommand(commandOld, "HEAD", "SOUR", "CORP") ) { tmpSource.address = tmpAddress; used = true; tmpAddress.clear(); used = true;}
    if (isCommand(commandOld, "SUBM", "ADDR", "*") ) { GENE.subm.address = tmpAddress; used = true; tmpAddress.clear(); used = true;}
    if (isCommand(commandOld, "REPO", "ADDR", "*") ) { tmpRepo.address = tmpAddress; used = true; tmpAddress.clear(); used = true;}

    if (isEntry(commandOld)) { tmpEvent.address = tmpAddress; tmpAddress.clear(); used = true; }
}

void Load::objeRecordEnded()
{
    // tarkastetaan vielä onko FORM tod näk oikein
    if (tmpMultimedia.format == NO_MM) {
        if (tmpMultimedia.file != "" ) {
            // { NO_MM, BMP_MM, GIF_MM, JPEG_MM, OLE_MM, PCX_MM, TIFF_MM, WAV_MM };
            tmpMultimedia.format = mmFormat(tmpMultimedia.file.right(3));

        }
    }
    if ( isCommand(commandOld, "FAM", "OBJE", "*") ) { tmpFd.multiMedia.append(tmpMultimedia); tmpMultimedia.clear(); used = true; } // tmpFd.multiMedia = tmpMultimedia; used = true; tmpMultimedia.clear(); used = true; }
    if ( isCommand(commandOld, "**", "SOUR", "OBJE") ) { tmpSource.multiMedia = tmpMultimedia; used = true; tmpMultimedia.clear();used = true; }
    if ( isCommand(commandOld, "SUBM", "OBJE", "*") ) { GENE.subm.multimedia = tmpMultimedia;  used = true; tmpMultimedia.clear(); used = true;}
    if ( isCommand(commandOld, "INDI", "OBJE", "*") ) { tmpPd.multiMedia.append(tmpMultimedia); tmpMultimedia.clear(); used = true;}
    if ( isEntry(commandOld)) {tmpEvent.multiMedia = tmpMultimedia; tmpMultimedia.clear(); used = true;}
}

void Load::sourRecordEnded()
{
    if ( isCommand(commandOld, "FAM", "SOUR", "*") ) //{ idSourCit = tmpFd.listSourCit.size(); tmpFd.listSourCit.append(tmpSource); }
        { tmpFd.sourCitations.append(tmpSource); used = true; }
    // tätä ei voi käyttää jos level0 on vaihtunut
    if ( isCommand(commandOld, "HEAD", "SOUR", "*")) { GENE.header.source = tmpSource; tmpSource.clear(); used = true; }
    if ( isCommand(commandOld, "INDI", "NAME", "SOUR")) { tmpPd.nameSource = tmpSource; tmpSource.clear(); used = true; }
    if ( isCommand(commandOld, "INDI", "SOUR", "*")) { tmpPd.source = tmpSource; tmpSource.clear(); used = true; }
    if ( isCommand(commandOld, "FAM", "SOUR", "*")) { tmpFd.source = tmpSource; tmpSource.clear(); used = true; }
    // ***
    if (isEntry(commandOld)) { tmpEvent.source = tmpSource; tmpSource.clear(); used = true; }
}

void Load::famEventEnded()
{
    ENTRYf eventType = NO_FAMILY_EVENT;
    GENE.famiSpace(idFam);
    //qDebug() << "fameventended" << commandOld[0] << commandOld[1] << commandOld[2] << commandOld[3];
    if ( isCommand(commandOld, "FAM", "MARR", "*" ) ) eventType = MARRIAGE;
    if ( isCommand(commandOld, "FAM", "ANUL", "*" ) ) eventType = ANNULMENT;
    if ( isCommand(commandOld, "FAM", "CENS", "*" ) ) eventType = CENSUS;
    if ( isCommand(commandOld, "FAM", "DIV", "*" ) )  eventType = DIVORCE;
    if ( isCommand(commandOld, "FAM", "DIVF", "*" ) ) eventType = DIVORCE_FILED;
    if ( isCommand(commandOld, "FAM", "ENGA", "*" ) ) eventType = ENGAGEMENT;
    if ( isCommand(commandOld, "FAM", "MARB", "*" ) ) eventType = MARRIAGE_BANN;
    if ( isCommand(commandOld, "FAM", "MARC", "*" ) ) eventType = MARRIAGE_CONTRACT;
    if ( isCommand(commandOld, "FAM", "MARL", "*" ) ) eventType = MARRIAGE_LICENSE;
    if ( isCommand(commandOld, "FAM", "MARS", "*" ) ) eventType = MARRIAGE_SETTLEMENT;
    if ( isCommand(commandOld, "FAM", "EVEN", "*" ) ) eventType = EVEN;
    if (eventType) {        
        tmpEvent.type = (int)eventType;
        tmpEvent.indi = false;
        tmpFd.entry.append(tmpEvent);
        used = true;
    }
    tmpEvent.clear();
}

void Load::indiEventEnded()
{
    GENE.indiSpace(idIndi);
    ENTRY eventType = NOTYPE;
    if ( isCommand(commandOld, "INDI", "BIRT", "*") ) eventType = BIRTH;
    if ( isCommand(commandOld, "INDI", "DEAT", "*") ) eventType = DEATH;
    if ( isCommand(commandOld, "INDI", "BURI", "*") ) eventType = BURIED;
    if ( isCommand(commandOld, "INDI", "CHR", "*") )  eventType = CHRISTENING;
    if ( isCommand(commandOld, "INDI", "IMMI", "*") ) eventType = IMMIGRATION;
    if ( isCommand(commandOld, "INDI", "CENS", "*") ) eventType = ICENSUS;
    if ( isCommand(commandOld, "INDI", "WILL", "*") ) eventType = WILL;
    if ( isCommand(commandOld, "INDI", "GRAD", "*") ) eventType = GRADUATION;
    if ( isCommand(commandOld, "INDI", "EMIG", "*") ) eventType = EMIGRATION;
    if ( isCommand(commandOld, "INDI", "EVEN", "*") ) eventType = USEREVENT;
    if ( isCommand(commandOld, "INDI", "CREM", "*") ) eventType = CREM;
    if ( isCommand(commandOld, "INDI", "ADOP", "*") ) eventType = ADOPTATION;
    if ( isCommand(commandOld, "INDI", "CONF", "*") ) eventType = CONFIRMATION;
    if ( isCommand(commandOld, "INDI", "RETI", "*") ) eventType = RETI;
    if ( isCommand(commandOld, "INDI", "PROB", "*") ) eventType = PROB;
    if ( isCommand(commandOld, "INDI", "NATU", "*") ) eventType = NATU;
    if ( isCommand(commandOld, "INDI", "BLES", "*") ) eventType = BLESSING;

    if ( isCommand(commandOld, "INDI", "OCCU", "*") ) eventType = OCCUPATION;
    if ( isCommand(commandOld, "INDI", "EDUC", "*") ) eventType = EDUCATION;
    if ( isCommand(commandOld, "INDI", "RESI", "*") ) eventType = RESIDENCE;
    if ( isCommand(commandOld, "INDI", "TITL", "*") ) eventType = TITLE;
    if ( isCommand(commandOld, "INDI", "NATI", "*") ) eventType = NATIONALITY;
    if ( isCommand(commandOld, "INDI", "PROP", "*") ) eventType = PROPERTY;
    if ( isCommand(commandOld, "INDI", "RELI", "*") ) eventType = RELIGION;
    if ( isCommand(commandOld, "INDI", "SSN", "*") )  eventType = SOCIAL_SECURITY_NRO;
    if ( isCommand(commandOld, "INDI", "CAST", "*") ) eventType = CAST;
    if ( isCommand(commandOld, "INDI", "DSCR", "*") ) eventType = DSCR;
    if ( isCommand(commandOld, "INDI", "IDNO", "*") ) eventType = IDNO;
    if ( isCommand(commandOld, "INDI", "NCHI", "*") ) eventType = NCHI;
    if ( isCommand(commandOld, "INDI", "NMR", "*") )  eventType = NMR;

    if (eventType != NOTYPE) {
        tmpEvent.type = eventType;
        tmpEvent.indi = true;
        tmpPd.entry.append(tmpEvent);
        used = true;
    }

    tmpEvent.clear();
}

void Load::famcRecordEnded()
{
    // jos ennestään ei ole famc.tä, tai jos ennestään on joku sikasäkä-adoptio, kirjoitetaan yli
    if (tmpPd.pedi == ADOPTED_PEDI) { tmpPd.famcSecondary = tmpFamc; } //*pedi
    else tmpPd.famc = tmpFamc;
    tmpFamc.clear();
}

void Load::analyseNoteRecord()
{
    if ( isCommand(command, "**", "NOTE", "")) {
        if (param.count("@") == 2) tmpNote.tag = param;
        else tmpNote.text = param;        
        used = true;
    }
    if ( isCommand(command, "**", "NOTE", "CONC"))  { conc(&tmpNote.text, param); used = true; }
    if ( isCommand(command, "**", "NOTE", "CONT"))  { cont(&tmpNote.text, param); used = true; }
    if ( isCommand(command, "**", "REFN", ""))      { tmpNote.refn = param; used = true; }
    if ( isCommand(command, "**", "REFN", "TYPE"))  { tmpNote.refnType = param; used = true; }
    if ( isCommand(command, "**", "REFN", "RIN"))   { tmpNote.rin = param; used = true; }
    if ( isCommand(command, "**", "CHAN", "DATE")) { tmpNote.change.setDate(date.day1); used = true; }
    if ( isCommand(command, "**", "DATE", "TIME")) { tmpNote.change.setTime(time); used = true; }
}

void Load::analyseObjeRecord()
{
// -------------------- multimedia field --------------------------------
    if ( isCommand(command, "**", "OBJE", "")) { tmpMultimedia.tag = param; used = true; }
    if ( isCommand(command, "**", "FORM", "*")) { tmpMultimedia.format = mmFormat(param); used = true; }
    if ( isCommand(command, "**", "TITL", "*")) { tmpMultimedia.title = param; used = true; }
    //if ( isCommand(command, "**", "OBJE", "*")) { tmpMultimedia.obje = param; used = true; }
    if ( isCommand(command, "**", "FILE", "*")) { tmpMultimedia.file = param; used = true; }
    if ( isCommand(command, "**", "REFN", "*")) { tmpMultimedia.refn = param; used = true; }
    if ( isCommand(command, "**", "REFN", "TYPE")) { tmpMultimedia.refnType = param; used = true; }
    if ( isCommand(command, "**", "RIN", "*")) { tmpMultimedia.rin = param; used = true; }
    if ( isCommand(command, "**", "CHAN", "DATE") ) { tmpMultimedia.change.setDate(date.day1); used = true; }
    if ( isCommand(command, "**", "DATE", "TIME")) { tmpMultimedia.change.setTime(time); used = true; }
}

void Load::analyseAddrRecord()
{
// -------------------- address field --------------------------------
    if ( isCommand(command, "**", "ADDR", "") ) { tmpAddress.line = param; used = true; }
    if ( isCommand(command, "**", "ADDR", "CONT") ) { cont(&tmpAddress.line, param); used = true; }
    if ( isCommand(command, "**", "ADDR", "CONC") ) { conc(&tmpAddress.line, param); used = true; }
    if ( isCommand(command, "**", "ADR1", "*") ) { tmpAddress.line1 = param; used = true; }
    if ( isCommand(command, "**", "ADR2", "*") ) { tmpAddress.line2 = param; used = true; }
    if ( isCommand(command, "**", "CITY", "*") ) { tmpAddress.city = param; used = true; }
    if ( isCommand(command, "**", "STAE", "*") ) { tmpAddress.state = param; used = true; }
    if ( isCommand(command, "**", "POST", "*") ) { tmpAddress.post = param; used = true; }
    if ( isCommand(command, "**", "CTRY", "*") ) { tmpAddress.country = param; used = true; }
    if ( isCommand(command, "**", "EMAI", "*") ) { tmpAddress.email = param; used = true; }
}

void Load::analyseSourRecord()
{
// -------------------- source citation ------------------------------ s.34
    // luetaan source muuttujaan ilman harmainta aavistusta, mihin se tulee käyttöön

    if ( isCommand(command, "**", "SOUR", "" ) ) {
        if (tag != "") tmpSource.tag = tag;
        else tmpSource.text = param;
        used = true;
    }
    //if ( isCommand(command, "**", "SOUR", "CONT") ) cont(&tmpSource.dataText, param);
    //if ( isCommand(command, "**", "SOUR", "CONC") ) conc(&tmpSource.dataText, param);
    if ( isCommand(command, "**", "QUAY", "" ) ) { tmpSource.quay = quality(param); used = true; }
    if ( isCommand(command, "**", "PAGE", "" ) ) { tmpSource.page = param; used = true; }
    if ( isCommand(command, "**", "PAGE", "CONC" ) ) { conc(&tmpSource.page, param); used = true; }
    if ( isCommand(command, "**", "PAGE", "CONT" ) ) { cont(&tmpSource.page, param); used = true; }
    if ( isCommand(command, "**", "EVEN", "") ) { tmpSource.event.type = param; used = true; }
    if ( isCommand(command, "**", "EVEN", "ROLE") ) { tmpSource.event.role = familyEventRole(param); used = true; }
    if ( isCommand(command, "**", "EVEN", "DATE") ) { tmpSource.event.date.setDates(date); used = true; }
    if ( isCommand(command, "**", "EVEN", "PLAC") ) { tmpSource.event.place = param; used = true; }
    // multimedia luetaan omassa rakenteessaan
    if ( isCommand(command, "**", "AUTH", "") ) { tmpSource.author = param; used = true; }
    if ( isCommand(command, "**", "AUTH", "CONT") ) { cont(&tmpSource.author, param); used = true; }
    if ( isCommand(command, "**", "AUTH", "CONC") ) { conc(&tmpSource.author, param); used = true; }
    if ( isCommand(command, "**", "TITL", "") ) { tmpSource.title = param; used = true; }
    if ( isCommand(command, "**", "TITL", "CONT") ) { cont(&tmpSource.title, param); used = true; }
    if ( isCommand(command, "**", "TITL", "CONC") ) { conc(&tmpSource.title, param); used = true; }
    // -- used by HEAD SOUR
    if ( isCommand(command, "HEAD", "SOUR", "") ) { tmpSource.approvedSystemID = param; }
    if ( isCommand(command, "**", "SOUR", "VERS") ) { tmpSource.version = param; used = true; }
    if ( isCommand(command, "**", "SOUR", "NAME") ) { tmpSource.productName = param; used = true; }
    if ( isCommand(command, "**", "CORP", "") ) { tmpSource.corporation = param; used = true; }
    // address luetaan omassa funktiossa lukuunottamatta phone
    if ( isCommand(command, "**", "PHON", "*") ) { tmpSource.phone = param; used = true; }
    // multimedia sama juttu
    if ( isCommand(command, "**", "DATA", "") ) { tmpSource.name = param; used = true;}
    if ( isCommand(command, "**", "DATA", "AGNC") ) { tmpSource.event.agency = param; }
    //if ( isCommand(command, "**", "DATA", "DATE") ) tmpSource.date.setDates(date);
    if ( isCommand(command, "**", "TEXT", "") ) { tmpSource.text = param; used = true; }
    if ( isCommand(command, "**", "TEXT", "CONT") ) { cont(&tmpSource.text, param); used = true; }
    if ( isCommand(command, "**", "TEXT", "CONC") ) { conc(&tmpSource.text, param); used = true; }
    if ( isCommand(command, "**", "DATA", "COPR") ) tmpSource.copyright = param;
    //if ( isCommand(command, "**", "PHON", "") ) tmpSource.phone = param;
    if ( isCommand(command, "**", "ABBR", "") ) { tmpSource.abbreviation = param; used = true; }
    if ( isCommand(command, "**", "PUBL", "") ) { tmpSource.publication = param; used = true; }
    if ( isCommand(command, "**", "PUBL", "CONT") ) { cont(&tmpSource.publication, param); used = true; }
    if ( isCommand(command, "**", "PUBL", "CONC") ) { conc(&tmpSource.publication, param); used = true; }
    if ( isCommand(command, "**", "REFN", "") ) tmpSource.refn = param;
    if ( isCommand(command, "**", "REFN", "TYPE") ) tmpSource.refnType = param;
    if ( isCommand(command, "**", "RIN", "") ) {tmpSource.rin = param; used = true; }
    if ( isCommand(command, "**", "CHAN", "DATE") ) { tmpSource.changed.setDate(date.day1); used = true; }
    if ( isCommand(command, "**", "DATE", "TIME")) { tmpSource.changed.setTime(this->time); used = true; }
    if ( isCommand(command, "**", "MEDI", "")) { used = true; } // *** ei käytetty
}

void Load::analyseEvent()
{
// ------------------- family or indi event -----------------------------------
    if ( command[0] == "INDI" && Entry::gedComToInt(command[1], true) > 17 && level == 1 ) tmpEvent.attrText = param;
    if ( isCommand(command, "INDI", "*", ""  ) ) { used = true; }
    if ( isCommand(command, "FAM", "*", ""  ) ) { used = true; } // !!!
    if ( isCommand(command, "*", "*", "DATE" ) ) { tmpEvent.day.setDates(date); used = true; }
    if ( isCommand(command, "*", "*", "PLAC" ) ) { tmpEvent.place = param; used = true; }
    if ( isCommand(command, "*", "*", "AGE"  ) ) { bool ok ; tmpEvent.age = param.toInt(&ok, 10); used = true; }
    if ( isCommand(command, "*", "*", "AGNC" ) ) { tmpEvent.agency = param; used = true; }
    if ( isCommand(command, "*", "*", "CAUS" ) ) { tmpEvent.cause = param; used = true; }
    if ( isCommand(command, "*", "*", "RESN" ) ) {
        if (param == "confidential") { tmpEvent.privacyPolicy = 1; used = true; }
        if (param == "locked") { tmpEvent.privacyPolicy = 2; used = true; }
        if (param == "privacy") { tmpEvent.privacyPolicy = 3; used = true; }
    }
    if ( isCommand(command, "*", "*", "TYPE" ) ) {
        if (command[0] == "INDI" && command[1] == "EVEN" && !usrEvents.contains(param) ) { usrEvents.append(param); used = true; }
        if (command[0] == "FAM" && command[1] == "EVEN" && !famEvents.contains(param) ) { famEvents.append(param); used = true; }
        tmpEvent.attrType = param;
        used = true;
    } // *!*
    if ( isCommand(command, "**", "ADOP", "*") ) {
        if (param == "Y")       { tmpPd.adoptedBy = YES_ADOP; used = true; }
        if (param == "HUSB")    { tmpPd.adoptedBy = HUSB_ADOP; used = true; }
        if (param == "WIFE")    { tmpPd.adoptedBy = WIFE_ADOP; used = true; }
        if (param == "BOTH")    { tmpPd.adoptedBy = BOTH_ADOP; used = true; }

    }
    if ( param == "Y" || param == "y" ) { tmpEvent.entryLineYes = true; used = true; }
}

void Load::analyseRepoRecord()
{
    if ( isCommand(command, "*", "*", "NAME") ) { tmpRepo.name = param; used = true; }
    if ( isCommand(command, "*", "*", "REFN") ) { tmpRepo.refn = param; used = true; }
    if ( isCommand(command, "**", "REFN", "TYPE") ) { tmpRepo.refnType = param; used = true; }
    if ( isCommand(command, "*", "*", "RIN") ) { tmpRepo.rin = param; used = true; }
    if ( isCommand(command, "**", "CHAN", "DATE") ) { tmpRepo.changed.setDate(date.day1); used = true; }
    if ( isCommand(command, "**", "DATE", "TIME")) { tmpRepo.changed.setTime(time); used = true; }
}

void Load::analyseSubnRecord()
{
    /*if ( isCommand(command, "*", "*", "FAMF") ) { gd.submission.famf = param; }
    if ( isCommand(command, "*", "*", "ANCE") ) { gd.submission.ance = param; }
    if ( isCommand(command, "*", "*", "DESC") ) { gd.submission.desc = param; }
    if ( isCommand(command, "*", "*", "ORDI") ) { gd.submission.ordi = param; }
    if ( isCommand(command, "*", "*", "RIN") ) { gd.submission.rin = param; }*/
}

void Load::analyseHeadRecord()
{    
    //qDebug() << command[0] << command[1] << command[2];
    if ( isCommand(command, "HEAD", "FILE", "*") ) { GENE.header.file = param; used = true; }
    if ( isCommand(command, "HEAD", "CHAR", "") ) { GENE.header.charset.name = param; used = true; }
    if ( isCommand(command, "HEAD", "CHAR", "VERS") ) { GENE.header.charset.version = param; used = true; }
    if ( isCommand(command, "HEAD", "DEST", "*") ) { GENE.header.receivingSystemName = param; used = true;}
    if ( isCommand(command, "HEAD", "DATE", "") ) { GENE.header.dateTransmission = date.day1; used = true; }
    if ( isCommand(command, "HEAD", "DATE", "TIME") ) { GENE.header.time = time; used = true; }
    if ( isCommand(command, "HEAD", "SUBM", "*") ) { GENE.header.submTag = param; used = true;}
    if ( isCommand(command, "HEAD", "SUMN", "*") ) { GENE.header.subnTag = param; used = true;}
    if ( isCommand(command, "HEAD", "COPR", "*") ) { GENE.header.copyright = param; used = true;}
    if ( isCommand(command, "HEAD", "LANG", "*") ) { GENE.header.language = param; used = true;}
    if ( isCommand(command, "HEAD", "PLAC", "FORM") ) { GENE.header.placForm = param; used = true; }
    if ( isCommand(command, "HEAD", "GEDC", "") ) used = true;    
    if ( isCommand(command, "HEAD", "GEDC", "VERS") ) { GENE.header.gedComVersion = param; used = true; }
    if ( isCommand(command, "HEAD", "GEDC", "FORM") ) { GENE.header.gedComForm = param; used = true; }
    // if ( isCommand(command, "HEAD", "NOTE", "") ) note tallennetaan hasNoteEnded funktiossa
    if ( isCommand(command, "HEAD", "NOTE", "CONT") ) { cont(&GENE.header.note, param); used = true;}
    if ( isCommand(command, "HEAD", "NOTE", "CONC") ) { conc(&GENE.header.note, param); used = true;}
}

void Load::analyseSubmRecord()
{
    if ( isCommand(command, "SUBM", "NAME", "*")) { GENE.subm.name = param; used = true; }
    if ( isCommand(command, "SUBM", "LANG", "*")) { GENE.subm.language = param; used = true; }
    if ( isCommand(command, "SUBM", "RFN", "*")) { GENE.subm.rfn = param; used = true; }
    if ( isCommand(command, "SUBM", "RIN", "*")) { GENE.subm.rin = param; used = true; }
    if ( isCommand(command, "SUBM", "CHAN", "DATE")) { GENE.subm.changed.setDate(date.day1); used = true; }
    if ( isCommand(command, "**", "DATE", "TIME")) { GENE.subm.changed.setTime(time); used = true; }

    // multimedia & address in their own funcion
}

void Load::analyseIndiRecord()
{
    GENE.indiSpace(idIndi);
    // huom esim INDI SOUR sijoitus tehdään sourcRecordEnend:ssä
    if (isCommand(command, "INDI", "NAME", "") ) { tmpPd.nameGedCom = param; used = true; }
    if (isCommand(command, "INDI", "NAME", "SURN") ) { used = true; } // ei käytetä vielä
    if (isCommand(command, "INDI", "NAME", "GIVN") ) { used = true; } // ei käytetä vielä
    if (isCommand(command, "INDI", "NAME", "NICK") ) { used = true; } // ei käytetä vielä
    if (isCommand(command, "INDI", "NAME", "SPFX") ) { used = true; } // ei käytetä vielä
    if (isCommand(command, "INDI", "NAME", "CONT") ) { cont(&tmpPd.nameGedCom, param) ; used = true; }
    if (isCommand(command, "INDI", "NAME", "CONC") ) { conc(&tmpPd.nameGedCom, param) ; used = true; }
    if (isCommand(command, "INDI", "FAMS", "*")) { tmpParam = param; used = true; }


    if (isCommand(command, "INDI", "FAMS", "NOTE")) { } // *!* mix tä on tyhjä
    else if (isCommand(command, "INDI", "FAMS", "*") ) { tmpPd.famsTags.append(param); used = true; }
    if (isCommand(command, "INDI", "SUBM", "*")) { tmpPd.submTags.append(param); used = true; }
    if (isCommand(command, "INDI", "SEX", "*")) {
        if (param == "M") { tmpPd.sex = MALE; used = true; }
        if (param == "F") { tmpPd.sex = FEMALE; used = true; }
    }
    if (isCommand(command, "INDI", "RFN", "")) { tmpPd.rfn = param; used = true; }
    if (isCommand(command, "INDI", "AFN", "")) { tmpPd.afn = param; used = true; }
    if (isCommand(command, "INDI", "REFN", "")) { tmpPd.refn = param; used = true; }
    if (isCommand(command, "INDI", "REFN", "TYPE")) { tmpPd.refnType = param; used = true; }
    if (isCommand(command, "INDI", "RIN", "")) { bool ok; tmpPd.rin = param.toInt(&ok, 10); used = true; }
    if (isCommand(command, "INDI", "CHAN", "")) used = true;
    if (isCommand(command, "INDI", "CHAN", "DATE")) {
        if (command[3] == "" ) { tmpPd.changed.setDate(date.day1); used = true; }
        if (command[3] == "TIME") { tmpPd.changed.setTime(time); used = true; }
    }
    if ( isCommand(command, "INDI", "RESN", "" ) ) {
        if (param == "confidential") { tmpPd.privacyPolicy = 1; used = true; }
        if (param == "locked") { tmpPd.privacyPolicy = 2; used = true; }
        if (param == "privacy")  { tmpPd.privacyPolicy = 3;  used = true; }
    }

    if (isCommand(command, "INDI", "ANCI", "*") ) { tmpPd.anciTag = param; used = true; }
    if (isCommand(command, "INDI", "DECI", "*") ) { tmpPd.desiTag = param; used = true; }
}

void Load::analyseFamRecord()
{
    // huom esim INDI SOUR sijoitus tehdään sourcRecordEnend:ssä
    GENE.famiSpace(idFam);
    // alla varmistetaan että wife, husb ja childs tiedoista käytetään vain ensimmäinen rivi joka on aina tag
    // Em kentät voi sisältää myös muita määreitä, joita ei käytetä
    if ( isCommand(command, "FAM", "HUSB", "*" ) ) { if (tmpFd.gedComTag.husb == "") { tmpFd.gedComTag.husb = param; used = true; } }
    if ( isCommand(command, "FAM", "WIFE", "*" ) ) { if (tmpFd.gedComTag.wife == "") { tmpFd.gedComTag.wife = param; used = true; } }
    if ( isCommand(command, "FAM", "CHIL", "*" ) ) { tmpFd.gedComTag.childs.append(param); used = true; }
    if ( isCommand(command, "FAM", "NCHI", "*" ) ) { tmpFd.nchi = param.toInt(); used = true; }
    if ( isCommand(command, "FAM", "CHAN", "" ) ) used = true;
    if ( isCommand(command, "FAM", "CHAN", "DATE" ) ) {
        if ( command[3] == "" ) { tmpFd.changed.setDate(date.day1); used = true; }
        if ( command[3] == "TIME" ) { tmpFd.changed.setTime(time); used = true; }
    }
    if ( isCommand(command, "FAM", "SUBM", "*" ) ) { tmpFd.gedComTag.submitters.append(param); used = true; }
    if ( isCommand(command, "FAM", "REFN", "" ) ) { tmpFd.refn = param; used = true; }
    if ( isCommand(command, "FAM", "REFN", "TYPE" ) ) { tmpFd.refnType = param; used = true; }
    if ( isCommand(command, "FAM", "RIN", "*" ) ) { tmpFd.rin = param; used = true; }
    if ( isCommand(command, "FAM", "RESN", "" ) ) {
        if (param == "confidential") { tmpFd.privacyPolicy = 1; used = true; }
        if (param == "locked") { tmpFd.privacyPolicy = 2; used = true; }
        if (param == "privacy") { tmpFd.privacyPolicy = 3; used = true; }
    }
}

void Load::analyseFamcRecord()
{
    if (level == 1) { tmpFamc.tag = tag; used = true; }
    if ( isCommand(command, "**", "FAMC", "PEDI") ) { tmpPd.pedi = pedi(param); used = true; }
}

void Load::whichMonthUsed()
{
    // tämä on luova tapa arvata, millä kielellä kuukaudennimet on tallennettu. Jotkut systeemit kirjoittaa kuukaudet esim
    // suomeksi, eli esim 01 Mar 1999 on 1. Marraskuuta. Kun taas englanniksi sama olisi 1. Maaliskuuta (Mars). Gedcom speksi on
    // vähän epäselvä. Tässä siis luetaan koko paska muistiin, lasketaan eri kuukausien määrät eri kielillä ja se kuka voittaa, on
    // käytettävä kieli.
    QString all;


    inputStream->setCodec("ISO 8859-1"); // UTF-8

    all = inputStream->readAll().toLower();

    quint16 cntFin = all.count(" tam ") + all.count(" hel ") + all.count(" maa ") + all.count(" huh ") + all.count(" tou ") + all.count(" kes ") + all.count(" hei ") + all.count(" elo ") + all.count(" syy ") + all.count(" lok ") + all.count(" mar ") + all.count(" jou ");
    quint16 cntEng = all.count(" jan ") + all.count(" feb ") + all.count(" mar ") + all.count(" apr ") + all.count(" may ") + all.count(" jun ") + all.count(" jul ") + all.count(" aug ") + all.count(" sep ") + all.count(" oct ") + all.count(" nov ") + all.count(" dec ");

    /*
    if (all.count("ä") > all.count("�")) {
        emit this->sigMessage("Looks like UTF-8 encoding works better than ISO8851. So, using UTF8.");
        inputStream->setCodec("UTF-8"); // UTF-8
    }
    */
    qDebug() << "����" << all.count("�") << all.count("�") << all.count("ä");
    if (cntFin > cntEng) {
        emit this->sigMessage("Finnish month names found and used in date decoding");
        monthLanguage = MO_FINNISH;
    }
    if (cntFin <= cntEng) {
        emit this->sigMessage("English month names found and used in date decoding");
        monthLanguage = MO_ENGLISH;
    }

    qDebug() << monthLanguage;
    inputStream->seek(0);
}

void Load::run()
{    
    tmpEvent.clear();
    tmpSource.clear();
    tmpAddress.clear();
    tmpMultimedia.clear();
    tmpNote.clear();
    tmpRepo.clear();
    tmpPd.clear(&GENE);
    tmpFd.clear(&GENE);
    int lnro    = 0;
    int i       = 0;
    line        = "";
    idHead      = 0;
    idIndi      = 0;
    idFam       = 0;
    idObje      = 0;
    idNote      = 0;
    idRepo      = 0;
    idSubm      = 0;
    idSour      = 0;

    bool    warnedAboutUsrTags = false;

    param = "";

    GENE.indiCount--; GENE.famiCount--; // luotiin tmp muuuttujat

    monthsFi << "tam" << "hel" << "maa" << "huh" << "tou" << "kes" << "hei" << "elo" << "syy" << "lok" << "mar" << "jou";
    monthsEn << "jan" << "feb" << "mar" << "apr" << "may" << "jun" << "jul" << "aug" << "sep" << "oct" << "nov" << "dec";
    //montshFr << "" << "" << "" << "" << "" << "" << "" << "" << "" << "" << "" << "";

    emit(loadDialog(-1, 0, 0, 0, 0, 0, 0, 0, 0));
    used = false;
    //inputStream->setCodec("UTF-8"); // UTF-8
    inputStream->setCodec("ISO 8859-1"); // UTF-8

    whichMonthUsed();

// ------------------- read a new line, "break" it into parts for later analyze ---------------------------

    while (!inputStream->atEnd()) {
        if (!used) {
            emit this->sigMessage("Load : Unprocessed line : " + QString::number(lnro) + " " + line);
        }
        lnro++; tag = ""; used = false;
        line = inputStream->readLine();

        // tarkastetaan voiko tiedostoa lukea
        if (true) {
            if (line.length() > 2000) {
                emit(loadFail(1));
                this->exit();
            }
        }        

        //for (i=0 ; i<9 ; i++) commandOld[i] = command[i];
        level = 0; cmd = ""; param = "";
        gedComLineAnalyze(&line, &level, &cmd, &param);

        if (cmd.left(1) == "_") {
            used = true; // tämä on usr tag
            if (!warnedAboutUsrTags) emit this->sigMessage("Warning : gedcom file has user tags. They will be ingonerd.");
            warnedAboutUsrTags = true;
        } else {
            for (i=0 ; i<9 ; i++) commandOld[i] = command[i];
            // tämä on user defined tagi, joita qenes ei käytä. Merkataan simppelisti "used = true" jotta ei tule varoituksia, mutta ei käytetä tätä mihinkään
            // käyttämättä jääneitä user tageistä voisi ojtenkin fiksusti varoittaa

            for (i=level ; i<9 ; i++) command[i] = "" ;
            command[level] = cmd;
 //qDebug() << lnro << line << "level:" << level << "cmd:" << cmd << "0:" << command[0] << "1:" << command[1] << "2:" << command[2] << "3:" << command[3];
            findGedComTag(line, &tag);
            if (cmd == "DATE") { gedComDayAnalyze(&param, &date); used = true; }
            if (cmd == "TIME") { gedComTimeAnalyse(&line, &time); used = true; }

            emit(loadDialog(idHead, idIndi, idFam, idObje, idNote, idRepo, idSour, idSubm, 0));

            // muista tässä järjestys. Syvimmällä leveleissä oleva pitää tallentaa ensin, esim jos rakenne
            // INDI SOUR ADDR loppuu, pitää ensin tallentaa ADDR tieto SOUR.n, sitten vasta SOUR tieto INDI:n
            // Tallennusjärjestys .
            // ensin : NOTE ja ADDR (ei väliä kumpi ensin)
            // - OBJE
            // - SOUR
            // - EVENTS JA ATTRIBUTES

    /*  Has reading of !=0 level record ended? The record has been saved in a temp variable (tmpNote, tmpEvent, tmpObje...)
        and now it should be saved somewhere. These functions are supposed to read the commands and find out the next
        level command a right place where to save, eg. should we save the tmp variable into indi, fam, into tmpNote or
        somewhere else.

        0 level records will be handeled later.
        */
            if ( hasRecordEnded("NOTE", commandOld, command) ) noteRecordEnded();
            if ( hasRecordEnded("ADDR", commandOld, command) ) addrRecordEnded();
            if ( hasRecordEnded("OBJE", commandOld, command) ) objeRecordEnded();
            if ( hasRecordEnded("SOUR", commandOld, command) ) sourRecordEnded();
            if ( hasRecordEnded("HEAD", commandOld, command) ) headRecordEnded();
            if ( commandOld[0] == "INDI" && isEntry(commandOld) && level <= 1 ) indiEventEnded();
            if ( commandOld[0] == "FAM"  && isEntry(commandOld) && level <= 1 ) famEventEnded();

            if ( isCommand(commandOld, "INDI", "FAMC", "*") && ( level <= 1 ) ) famcRecordEnded();

    /*  If the level is zero.
        - check what was the previous record, if necessary, create a new table index and save a tmp into that index
        - check what is the new record. Update counter, and save the tag info for later processing.
        */

            if (level==0) {
                // allaolevat tapaukset luetaan samalla tavalla "citation":ksi ja omaksi recordikseen.
                if ( isCommand(command, "", "", "")) used = true;
                if ( isCommand(command, "HEAD", "", "")) used = true;
                if ( isCommand(command, "TRLR", "", "")) used = true;
                if (commandOld[0] == "INDI") { GENE.indiSpace(idIndi);  INDI(idIndi) = tmpPd ; tmpPd.clear(&GENE); used = true; }
                if (commandOld[0] == "FAM" ) { GENE.famiSpace(idFam);   FAM(idFam)  = tmpFd  ; tmpFd.clear(&GENE); used = true; }
                if (commandOld[0] == "SOUR") { GENE.sourSpace(idSour);  GENE.sour[idSour].copyDataFrom(tmpSource) ; tmpSource.clear(); used = true; }
                if (commandOld[0] == "OBJE") { GENE.objeSpace(idObje);  GENE.obje[idObje].copyDataFrom(tmpMultimedia); tmpMultimedia.clear(); used = true; }
                if (commandOld[0] == "NOTE") { GENE.noteSpace(idNote);  GENE.note[idNote].copyDataFrom(tmpNote); tmpNote.clear(); used = true; }
                if (commandOld[0] == "REPO") { GENE.repoSpace(idRepo) ; GENE.repo[idRepo] = tmpRepo; tmpRepo.clear(); used = true; }
                // subn record will be saved directly into database in "analysing methods"
                // header, the same
                // subm, the same
                if (cmd == "FAM" ) if ( tag != "" ) { idFam++;  famList.append(tag);    tmpFd.id = idFam; used = true; }
                if (cmd == "INDI") if ( tag != "" ) { idIndi++; indList.append(tag);    tmpPd.id = idIndi; used = true;}
                if (cmd == "OBJE") if ( tag != "" ) { idObje++; objeList.append(tag);   tmpMultimedia.id = idObje; used = true; }
                if (cmd == "NOTE") if ( tag != "" ) { idNote++; noteList.append(tag);   tmpNote.id = idNote; used = true;}
                if (cmd == "REPO") if ( tag != "" ) { idRepo++; repoList.append(tag);   tmpRepo.id = idRepo; used = true;}
                if (cmd == "SUBM") if ( tag != "" ) { idSubm++; submList.append(tag); used = true;}
                if (cmd == "SOUR") if ( tag != "" ) { idSour++; sourList.append(tag);   tmpSource.id = idSour; used = true;}
                // since there are only one subn, header, subm, they are not here... why subm is *!*
            } else {

    /* Level is not zero.
       In this phase we know what record we're reading (that's in command[0...]) The methods below are responsible for
       scanning the command[] variables and saving the param variable in the right place.
      7 For example, if commands are 0 = INDI, 1= RFN, and param holds the RFN value, then save tmpPd.rfn = param.
       Saving will happen always in tmp variables, which will be saved in the table in later phases (the code above).

       The order makes a difference, its important eg to make sure that indi and fam are in the end so that all records that
       are inside those, has been saved. That's why we use else's.

       Subn, header and subm are not saved in a tmp record but directly into database. There's only 1 of each.
       */
                if ( isCommand(command, "**", "NOTE", "*") ) analyseNoteRecord(); else
                if ( isCommand(command, "**", "OBJE", "*") ) analyseObjeRecord(); else
                if ( isCommand(command, "**", "ADDR", "*") ) analyseAddrRecord(); else
                if ( isCommand(command, "**", "SOUR", "*" ) ) analyseSourRecord(); else
                if ( isEntry(command)) analyseEvent(); else
                if ( isCommand(command, "**", "FAMC", "*") ) analyseFamcRecord(); else
                if ( isCommand(command, "**", "REPO", "*") ) analyseRepoRecord(); else
                if ( isCommand(command, "**", "SUBN", "*") ) analyseSubnRecord(); else
                if ( isCommand(command, "HEAD", "*", "*")) analyseHeadRecord(); else
                if ( isCommand(command, "SUBM", "*", "*")) analyseSubmRecord(); else
                if ( isCommand(command, "INDI", "*", "*")) analyseIndiRecord(); else
                if ( isCommand(command, "FAM", "*", "*")) analyseFamRecord();
            }
        } // ei ollut user tagi joka alkaa _:llä
    } // while loop loppuu

    // -------------------------------- READ ENDS ------------------------------------------

    int y;

    // *!* Not a masterpiece QRegexp... how to make regexp that last name is cap(4).
    // Now the problem is I dont know how to catch e.g. name from /name/ ...

    QRegExp nameExp("([^/]\\S*[^/ ]|\\?)? ?([^ /]\\S*[^/ ])? ?([^ /]\\S*[^/ ])? ?(\\/(.*)?\\/)?");

    // For all indi's, lets change gedcom ref-tag eg. FAMC @family@ to FAMC 12
    // tag value @family@ is saved on gedComFamc, and famList has mapping 12 <=> @family@.
    // For famS values, each family record can have more than one famS, the number will be acquired
    // from lastUsedFamsN. Gedcom famS tags are saved in each fd in a separate map.

    int x = 0;
    for (i=0 ; i<=idIndi ; i++) {
        emit(loadDialog(idHead, idIndi, idFam, idObje, idNote, idRepo, idSour, idSubm, i));
        if (famList.indexOf(INDI(i).famc.tag) != -1) {
            INDI(i).famc.value = 1 + famList.indexOf(INDI(i).famc.tag);
        }
        INDI(i).nameNote.id = 1 + noteList.indexOf(INDI(i).nameNote.tag);

        INDI(i).note.id = 1 + noteList.indexOf(INDI(i).note.tag);

        if (famList.indexOf(INDI(i).famcSecondary.tag) != -1) INDI(i).famcSecondary.value = 1 + famList.indexOf(INDI(i).famcSecondary.tag);
        for (y=0 ; y<INDI(i).famsTags.count() ; y++)
            if (famList.indexOf(INDI(i).getFamsTag(y)) != -1) INDI(i).addFamS(1 + famList.indexOf(INDI(i).getFamsTag(y)));

        // Muunnetaan henkilön i eventtien source tiedot täg arvosta => int, sourList:n avulla.
        // esim INDI(i) - syntymä - source = @source1234@.
        // sourList kertoo, että @source1234@ on tallennettu paikkaan 12.
        // Arvo 12 haetaan ja tallennetaan INDI(i) - syntymä - sourceID, jota arvoa käytetään myöhemmin.
        for (y=0 ; y<INDI(i).entry.size() ; y++) {
            INDI(i).entry[y].source.id = 1 + sourList.indexOf(INDI(i).entry.at(y).source.tag);
            INDI(i).entry[y].multiMedia.id = 1 + objeList.indexOf(INDI(i).entry.at(y).multiMedia.tag);
            INDI(i).entry[y].multiMedia.note.id = 1 + noteList.indexOf(INDI(i).entry.at(y).multiMedia.tag);
            INDI(i).entry[y].note.id = 1 + noteList.indexOf(INDI(i).entry.at(y).note.tag);
        }

        nameExp.indexIn(INDI(i).nameGedCom);
        INDI(i).nameFirst = nameExp.cap(1);
        INDI(i).name2nd   = nameExp.cap(2);
        INDI(i).name3rd   = nameExp.cap(3);
        INDI(i).nameFamily = nameExp.cap(5);

        for (y=0 ; y<= INDI(i).multiMedia.count() ; y++) {
            x = objeList.indexOf(INDI(i).multiMedia.value(y).tag);
            if ( x!= -1) {
                INDI(i).multiMedia[y].id = 1+x;
                INDI(i).multiMedia[y].note.id = 1 + noteList.indexOf(INDI(i).multiMedia.value(y).note.tag);
            }
            x = 0;
        }

        x = noteList.indexOf(INDI(i).note.tag);
        if ( x != -1 ) FAM(i).note.id = 1 + x;

        INDI(i).source.id = 1 + sourList.indexOf(INDI(i).source.tag);

        for (y=0 ; y<= INDI(i).submTags.count(); y++) {
            x = submList.indexOf(INDI(i).submTags.value(y));
            if ( x != -1 ) INDI(i).submitters.append(1 + x);
        }

        INDI(i).source.multiMedia.id = 1 + objeList.indexOf(INDI(i).source.multiMedia.tag);
        INDI(i).source.note.id = 1 + noteList.indexOf(INDI(i).source.note.tag);

    }

    // as wife and husb values are saved as HUSB @individual1@ and WIFE @indivudual2@, shall each
    // family record be converted so that mapping @individual@ <=> 13 will be used. The gedcom tags
    // are saved on FAM[].gedcomwife and the actual mapping on FAM[].wife.
    for (i=0 ; i<=idFam ; i++) {        
        emit(loadDialog(idHead, idIndi, idFam, idObje, idNote, idRepo, idSour, idSubm, idIndi+i));
        FAM(i).wife = 1 + indList.indexOf(FAM(i).gedComTag.wife);
        //if (FAM(i).wife < 1) qDebug() << "vaimo issue " << FAM(i).gedComTag.wife << i; else qDebug() << "vaimo ok " << FAM(i).gedComTag.wife << i;

        //if ( FAM(i).wife == -1 ) qDebug() << "Wife tag " << FAM(i).gedComTag.wife << " not found";
        FAM(i).husb = 1 + indList.indexOf(FAM(i).gedComTag.husb);
        //if ( FAM(i).husb == -1 ) qDebug() << "Husb tag " << FAM(i).gedComTag.husb << " not found";

        int count = FAM(i).sourCitations.count();
        for (y=0 ; y<count ; y++) FAM(i).sourCitations[y].id = 1 + sourList.indexOf(FAM(i).sourCitations[y].tag);


        count = FAM(i).gedComTag.submitters.count();
        for (y=0 ; y<count ; y++) {
            QString submTag = FAM(i).gedComTag.submitters.value(y);
            int sourceId = 1 + submList.indexOf(submTag);
            FAM(i).subM.append(sourceId);
        }

        count = FAM(i).gedComTag.childs.count();
        for (y=0 ; y<count ; y++) {
            QString childTag = FAM(i).gedComTag.childs.value(y);
            int childID = 1 + indList.indexOf(childTag);
            FAM(i).childs.append(childID);
        }

        for (y=0 ; y<FAM(i).entry.size() ; y++) {            
            FAM(i).entry[y].source.id =  1 + sourList.indexOf(FAM(i).entry.value(y).source.tag);
            FAM(i).entry[y].multiMedia.id = 1 + objeList.indexOf(FAM(i).entry.value(y).multiMedia.tag);
            FAM(i).entry[y].multiMedia.note.id = 1 + noteList.indexOf(FAM(i).entry.value(y).multiMedia.tag); //*** mikä siis tää o?
            FAM(i).entry[y].note.id = 1 + noteList.indexOf(FAM(i).entry.value(y).note.tag);
        }

        FAM(i).note.id = 1 + noteList.indexOf(FAM(i).note.tag);

        x = sourList.indexOf(FAM(i).source.tag); if (x!= -1) FAM(i).source.id = 1+x;

        for (int f=0 ; f<= FAM(i).multiMedia.count() ; f++) {
            int x = objeList.indexOf(INDI(i).multiMedia.value(f).tag);
            if ( x!= -1) {
                INDI(i).multiMedia[f].id = 1+x;
                INDI(i).multiMedia[f].note.id = 1 + noteList.indexOf(INDI(i).multiMedia.value(f).note.tag);
            }
            x = 0;
        }
    }

    for (i=0 ; i<=idIndi ; i++) {
        if ( (!FAM(INDI(i).famc.value).wife) && (FAM(INDI(i).famcSecondary.value).wife) ) {
            FAM(INDI(i).famc.value).wife = FAM(INDI(i).famcSecondary.value).wife;
            FAM(INDI(i).famcSecondary.value).wife = 0;
            INDI(i).adoptedBy = WIFE_ADOP;
            if (FAM(INDI(i).famc.value).childs.indexOf(i) == -1) FAM(INDI(i).famc.value).childs.append(i);
            FAM(INDI(i).famcSecondary.value).childs.removeAll(i);
        }

        if ( (!FAM(INDI(i).famc.value).husb) && (FAM(INDI(i).famcSecondary.value).husb) ) {
            FAM(INDI(i).famc.value).husb = FAM(INDI(i).famcSecondary.value).husb;
            FAM(INDI(i).famcSecondary.value).husb = 0;
            if (INDI(i).adoptedBy == WIFE_ADOP ) INDI(i).adoptedBy = BOTH_ADOP;
            else INDI(i).adoptedBy = HUSB_ADOP;
            if (FAM(INDI(i).famc.value).childs.indexOf(i) == -1) FAM(INDI(i).famc.value).childs.append(i);
            FAM(INDI(i).famcSecondary.value).childs.removeAll(i);
        }
    }

    GENE.header.subm = 1 + submList.indexOf(GENE.header.submTag);
    GENE.fileName = fileName;
    GENE.canBeSaved = true;
    GENE.arrangeEvents();
    //if (GENE.rootId==0) GENE.rootId = 1;

    emit(loadDialog(-2, 0, 0, 0, 0, 0, 0, 0, 0));

    if (cntErrorsDate > 0) {
        QMessageBox qmb;
        qmb.setText(tr("Errors while loading the file. ") + QString::number(cntErrorsDate) + tr(" dates had errors. Application may crash."));
        qmb.exec();
    }

/*
 * t�m� otettu t�st� pois ku tuntuu ett� n�ytt� olisi parasta p�ivitt�� jossain my�hemmin. Varmaan virheentarkastuksen j�lkeen.
    if (activateAfterLoad) {
        if (GENE.rootId != 0) {
            emit (update(GENE.rootId));
        }
        else {
            GENE.currentId = 1;
            emit (update(1));
        }

    }
*/
    // tässä vaanii pieni vaara, tämä aiheuttaa mainwindow::slotloadcomplete metodin ajamisen, joka lopputyökseen tuhoaa tämän
    // threadin. Kun tämä oli aikaisemmin, tuli joskus virhe että thread tuhotaan kun sitä ollaan vielä ajamassa
    // kokeiltu silksi loadThread.deleteLater


    emit(loadComplete());

}

Load::Load(QTextStream *qts, QString projectName, bool aal, QObject *parent) : QThread(parent)
{
    cntErrorsDate = 0;
    cntDates = 0;
    activateAfterLoad = aal;
    inputStream = qts;
    if (projectName != "") fileName = projectName;
}

Load::~Load()
{

}

void Load::cont(QString * pData, QString add) {
    if (add != "") pData->append("\n").append(add);
}

void Load::conc(QString * pData, QString add) {
    if (add != "") pData->append(add);
}

