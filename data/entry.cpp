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

//#include "persondata.h"
#include "data/entry.h"
#include "QDate"
#include "setupvalues.h"
#include "data/genedata.h"
#include "macro.h"
#include <QSettings>
#include <QStringList>

extern GeneData        * geneData;
extern QSettings    sets;

void Entry::addPrintLine(QString *line, QString add)
{
    if (*line == "") *line += add;
    else if (add != "") *line += "<br>" + add;
}

void Entry::addPrintLine(QString *result, QString comment, QString add)
{
    if (add != "") *result += comment + add;

}

QString Entry::addMmCitation()
{
//    QString result = "<img src=\"FILENAME\" height = \"200\" width=\"200\">";
//    result.replace("FILENAME", GeneData::getFileEnd(multiMedia.file));

    QString result = "<b>Photo : </b>";

    addPrintLine(&result, ", ", multiMedia.title);
    addPrintLine(&result, ", ", multiMedia.note.text);

    return result;
}

QString Entry::addAddressCitation()
{
    QString result = "";

    if (address.used()) {
        QString result = "<b>Address : </b>";

        addPrintLine(&result, " ", address.line);
        addPrintLine(&result, " ", address.line1);
        addPrintLine(&result, " ", address.line2);
        addPrintLine(&result, " ", address.city);
        addPrintLine(&result, " ", address.post);
        addPrintLine(&result, " ", address.state);
        addPrintLine(&result, " ", address.country);
        result += "\n";
    }

    return result;
}

QString Entry::addSourceCitation()
{
    QString result = "";

    if (source.used()) {
        result = "<b>Source : </b>";

        addPrintLine(&result, " ",   GENE.sour[source.id].name);
        addPrintLine(&result, " / ", GENE.sour[source.id].title);
        addPrintLine(&result, " / ", GENE.sour[source.id].author);
        addPrintLine(&result, " / ", GENE.sour[source.id].text);
        addPrintLine(&result, " / ", GENE.sour[source.id].publication);
        addPrintLine(&result, "(c)", GENE.sour[source.id].corporation);
        addPrintLine(&result, " <b>Citation : </b>pg.", source.page);
        addPrintLine(&result, " / ", source.note.text);
        addPrintLine(&result, " / ", source.text);
        addPrintLine(&result, " / ", source.printQuality());

        result += "<br>\n";
    }

    return result;
}

/*
QString Entry::printResn()
{
    if (GENE.printPolicy == PRINTPOLICY_1_EVERYTHING) return "";

    if ( ( this->indi) && (GENE.printPolicy == PRINTPOLICY_3_PUBLIC) && (INDI(indiId).isLiving())) GeneData::resn(1);
    if ( (!this->indi) && (GENE.printPolicy == PRINTPOLICY_3_PUBLIC) && (FAM(famiId).isLiving())) GeneData::resn(1);

    if ( effectivePrivacyMethod() == PRIVACY_0_SHOW ) return "";

    return GeneData::resn(1);

}
*/

QString Entry::printPlace()
{
    //if ( ( this->indi) && (GENE.printPolicy == PRINTPOLICY_3_PUBLIC) && (INDI(indiId).isLiving())) return "";
    //if ( (!this->indi) && (GENE.printPolicy == PRINTPOLICY_3_PUBLIC) && (FAM(famiId).isLiving())) return "";
/*
    PRIVACY_0_SHOW          0 "Show"
    PRIVACY_1_BLUR_DETAILS  1 "Show Name, *** details, show genre"
    PRIVACY_2_HIDE_DETAILS  2 "Show name, ___ details"
    PRIVACY_3_BLUR_NAME     3 "***  Name, ___ details"
    PRIVACY_4_HIDE          4 "Hide"
          */
    if ( place == "" ) return "";
    PRIVACYMETHOD epm = this->effectivePrivacyMethod();

    if ( epm == PRIVACY_1_BLUR_DETAILS ) return "***";
    if ( epm == PRIVACY_0_SHOW ) return place;
    return "";

    /*
     0 "Show"
     1 "Show Name, plur details, show genre"
     2 "Show name, no details"
     3 "Plur Name, hide details"
     4 "Hide"
          */
}

QString Entry::printValue()
{
    if ( attrText == "" ) return "";

    PRIVACYMETHOD epm = this->effectivePrivacyMethod();

    if ( epm == PRIVACY_1_BLUR_DETAILS ) return "***";
    if ( epm == PRIVACY_0_SHOW ) return attrText;
    return "";
}

QString Entry::printNote()
{
     QString notes = note.text;

     if ( (GENE.printPolicy == PRINTPOLICY_1_EVERYTHING) || (effectivePrivacyMethod() == PRIVACY_0_SHOW) ) {
        if ( address.used() ) addPrintLine(&notes, addAddressCitation());
        if ( source.used() ) addPrintLine(&notes, addSourceCitation());
        if ( multiMedia.used() ) addPrintLine(&notes, addMmCitation()); // tässä pitää vielä kopsata kuvat pdf:n tai dirikkaan. SE tehdään reporter luokassa.
     }

    if ( notes == "" ) return "";

    PRIVACYMETHOD epm = this->effectivePrivacyMethod();

    if ( epm == PRIVACY_1_BLUR_DETAILS ) return "***";
    if ( epm == PRIVACY_0_SHOW ) return notes;
    return "";
}

QString Entry::printWeekday()
{
    if ( effectivePrivacyMethod() == PRIVACY_0_SHOW ) return day.dayNameOfWeek();
    return "";
}

QString Entry::printColor()
{
    if ( INDI(indiId).privacyPolicy != 0) return COLOR_PRINTRED;
    if (  indi ) if ( !GET_I_PRINT(type)) return COLOR_PRINTRED;
    if ( !indi ) if ( !GET_F_PRINT(type)) return COLOR_PRINTRED;

    if ( this->privacyPolicy == 0 ) return COLOR_PRINTYELLOW;

    return COLOR_PRINTRED;
}

QString Entry::printDay()
{
    if ( ( this->indi) && (GENE.printPolicy == PRINTPOLICY_3_PUBLIC) && (INDI(indiId).isLiving())) return "";
    if ( (!this->indi) && (GENE.printPolicy == PRINTPOLICY_3_PUBLIC) && (FAM(famiId).isLiving())) return "";

    if ( (GET_PRINT_ESTIMATEDBIRTHS) && !day.known && (effectivePrivacyMethod() == PRIVACY_0_SHOW)) {
        // annetaan arviopäivä, jos käyttäjä niin haluaa ja jos varsinaista päivää ei ole

        Date date;
        INDI(indiId).eventPeriod(&date, this->type, this->indi);
        return "#" + date.toString();
    }

    if ( !day.known ) return "";
    if ( effectivePrivacyMethod() == PRIVACY_1_BLUR_DETAILS ) return "***";
    if ( effectivePrivacyMethod() == PRIVACY_2_HIDE_DETAILS ) return "";
    if ( effectivePrivacyMethod() == PRIVACY_3_BLUR_NAME ) return "";
    if ( effectivePrivacyMethod() == PRIVACY_4_HIDE ) return "";

    return day.toString();

}

PRIVACYMETHOD Entry::effectivePrivacyMethod()
{
    // palauttaa printtausluokalle tämän entryn privacy policyn, joka on otettava printtauksessa huomioon.
    // Eli käytännössä tutkitaan, mikä on entryn oma, entryn "omistajan" eli indiN.n, tai jos entryn omistaja on fam niin
    // famin, perheen vaimon tai miehen privacy policy, ja palautetaan näistä se kaikista tiukin policy jota käyttäjä nyt
    // haluaa käyttää.
    // SHOW 0
    // DONT_SHOW 1
    // CHANGE_TO_TEXT 2

    if (GENE.printPolicy == PRINTPOLICY_1_EVERYTHING ) return PRIVACY_0_SHOW;

    // print policy on joko PRIVACY_1_BLUR_DETAILS, PRIVACY_2_HIDE_DETAILS, PRIVACY_3_BLUR_NAME tai PRIVACY_4_HIDE
    // Jos setup arvoissa on asetettu, että tämäntyyppistä eventtiä ei tulosteta, palauta privacy-hide
    if (indi) if (!GET_I_PRINT(this->type)) return PRIVACY_4_HIDE;
    if (!indi) if (!GET_F_PRINT(this->type)) return PRIVACY_4_HIDE;

    // nämä on ehdottomia arvoja, jos käyttäjä on nämä asettanut niin pulinat pois. EffectivePrivacyPolicy ottaa huomioon myös,
    // jos ko henkilö on niin nuori että hänen policy muuttuu sen takia.
    PRIVACYMETHOD p1, p2, p3, p4, p;
    p1 = p2 = p3 = p4 = p = PRIVACY_0_SHOW;
    // calculated p.o. o.k. koska ne on laskettu reporterissa, joka kutsuu tätä metodia
    if (indiId) p1 = INDI(indiId).effectivePrivacyMethod();
    if (famiId) p2 = FAM(famiId).effectivePrivacyMethod();
    p3 = day.whichAutoPrivacyMethodApplies();

    //if ( GENE.printPolicy == PRINTPOLICY_3_PUBLIC )
    p4 = (PRIVACYMETHOD)s_privacyMethod(this->privacyPolicy);

//if (this->indiId == 1) qDebug() << this->day << p1 << p2 << p3 << p4 << GENE.printPolicy << this->privacyPolicy;
    p = MAX(p1, p2);
    p = MAX(p,  p3);
    p = MAX(p,  p4);

    return p;
}

QString Entry::typeGedCom()
{
    return typeGedCom(type, indi);
}

QString Entry::typeGedCom(int type, bool indi)
{
    if (indi) {
        switch (type) {
            case 0 : return ""; break;
            case 1 : return "BIRT"; break;
            case 2 : return "CHR"; break;
            case 3 : return "CONF"; break;
            case 4 : return "DEAT"; break;
            case 5 : return "EMIG"; break;
            case 6 : return "IMMI"; break;
            case 7 : return "EVEN"; break;
            case 8 : return "CENS"; break;
            case 9 : return "WILL"; break;
            case 10 : return "GRAD"; break;
            case 11 : return "BURI"; break;
            case 12 : return "CREM"; break;
            case 13 : return "ADOP"; break;
            case 14 : return "RETI"; break;
            case 15 : return "PROB"; break;
            case 16 : return "NATU"; break;
            case 17 : return "BLES"; break;
            case 18 : return "OCCU"; break;
            case 19 : return "EDUC"; break;
            case 20 : return "NATI"; break;
            case 21 : return "TITL"; break;
            case 22 : return "PROP"; break;
            case 23 : return "RELI"; break;
            case 24 : return "RESI"; break;
            case 25 : return "SSN"; break;
            case 26 : return "CAST"; break;
            case 27 : return "DSCR"; break;
            case 28 : return "NCHI"; break;
            case 29 : return "NMR"; break;
            case 30 : return "INDO"; break;
        }
    }
    if (!indi) {
        switch (type) {
            case 0 : return ""; break;
            case 1 : return "MARR"; break;
            case 2 : return "DIV"; break;
            case 3 : return "ANUL"; break;
            case 4 : return "CENS"; break;
            case 5 : return "DIVF"; break;
            case 6 : return "ENGA"; break;
            case 7 : return "MARB"; break;
            case 8 : return "MARC"; break;
            case 9 : return "MARL"; break;
            case 10 : return "MARS"; break;
            case 11 : return "EVEN"; break;
        }
    }
    return "";
}

int Entry::gedComToInt(QString type, bool indi)
{
    if (!indi) {
        if (type == "MARR") return 1;
        if (type == "DIV")  return 2;
        if (type == "ANUL") return 3;
        if (type == "CENS") return 4;
        if (type == "DIVF") return 5;
        if (type == "ENGA") return 6;
        if (type == "BARB") return 7;
        if (type == "MARC") return 8;
        if (type == "AMRL") return 9;
        if (type == "MARS") return 10;
        if (type == "EVEN") return 11;
        return 0;
    }
    if (indi) {
        if (type == "BIRT") return 1;
        if (type == "CHR")  return 2;
        if (type == "CONF") return 3;
        if (type == "DEAT") return 4;
        if (type == "EMIG") return 5;
        if (type == "IMMI") return 6;
        if (type == "EVEN") return 7;
        if (type == "CENS") return 8;
        if (type == "WILL") return 9;
        if (type == "GRAD") return 10;
        if (type == "BURI") return 11;
        if (type == "CREM") return 12;
        if (type == "ADOP") return 13;
        if (type == "RETI") return 14;
        if (type == "PROB") return 15;
        if (type == "NATU") return 16;
        if (type == "BLES") return 17;

        if (type == "OCCU") return 18;
        if (type == "EDUC") return 19;
        if (type == "NATI") return 20;
        if (type == "TITL") return 21;
        if (type == "PROP") return 22;
        if (type == "RELI") return 23;
        if (type == "RESI") return 24;
        if (type == "SSN")  return 25;
        if (type == "CAST") return 26;
        if (type == "DSCR") return 27;
        if (type == "NCHI") return 28;
        if (type == "NMR")  return 29;
        if (type == "INDO") return 30;
        return 0;
    }
    return -1;
}

QString Entry::typeString()
{
    return typeString(type, indi);
}

QString Entry::typeString(int type, bool indi)
{
    if (indi) {
        switch (type) {
            case 0 : return QObject::tr("Add"); break;
            case 1 : return QObject::tr("Birth"); break;
            case 2 : return QObject::tr("Christening"); break;
            case 3 : return QObject::tr("Confirmation"); break;
            case 4 : return QObject::tr("Death"); break;
            case 5 : return QObject::tr("Emigration"); break;
            case 6 : return QObject::tr("Immigration"); break;
            case 7 : return QObject::tr("User event"); break;
            case 8 : return QObject::tr("Census"); break;
            case 9 : return QObject::tr("Will"); break;
            case 10 : return QObject::tr("Graduation"); break;
            case 11 : return QObject::tr("Buried"); break;
            case 12 : return QObject::tr("Chremat."); break;
            case 13 : return QObject::tr("Adopted"); break;
            case 14 : return QObject::tr("Retirement"); break;
            case 15 : return QObject::tr("Probate"); break;
            case 16 : return QObject::tr("Naturalization"); break;
            case 17 : return QObject::tr("Blessing"); break;
            case 18 : return QObject::tr("Occupation"); break;
            case 19 : return QObject::tr("Education"); break;
            case 20 : return QObject::tr("Nationality"); break;
            case 21 : return QObject::tr("Title"); break;
            case 22 : return QObject::tr("Property"); break;
            case 23 : return QObject::tr("Religion"); break;
            case 24 : return QObject::tr("Residence"); break;
            case 25 : return QObject::tr("SS number"); break;
            case 26 : return QObject::tr("Caste"); break;
            case 27 : return QObject::tr("Physical descr."); break;
            case 28 : return QObject::tr("Children count"); break;
            case 29 : return QObject::tr("Marriage count"); break;
            case 30 : return QObject::tr("Indent number"); break;
        }
        return "";
    } else {
        switch (type) {
            case 0 : return QObject::tr("Add");
            case 1 : return QObject::tr("Marriage");
            case 2 : return QObject::tr("Divorce");
            case 3 : return QObject::tr("Annulment");
            case 4 : return QObject::tr("Census");
            case 5 : return QObject::tr("Divo. filed");
            case 6 : return QObject::tr("Engagement");
            case 7 : return QObject::tr("Marr. Bann");
            case 8 : return QObject::tr("Marr. Contract");
            case 9 : return QObject::tr("Marr. License");
            case 10 : return QObject::tr("Marr. Settlement");
            case 11 : return QObject::tr("Usr Event");
        }
        return "";
    }
}

bool Entry::publishable()
{
    /*
    PRIVACYMETHOD

    PRIVACY_0_SHOW          0 "Show"
    PRIVACY_1_BLUR_DETAILS  1 "Show Name, *** details, show genre"
    PRIVACY_2_HIDE_DETAILS  2 "Show name, ___ details"
    PRIVACY_3_BLUR_NAME     3 "***  Name, ___ details"
    PRIVACY_4_HIDE          4 "Hide"
          */

    /*
    PRINTPOLICY_0_NONE
    PRINTPOLICY_1_EVERYTHING
    PRINTPOLICY_2_GENE
    PRINTPOLICY_3_PUBLIC
    */


    PRIVACYMETHOD pm = this->effectivePrivacyMethod();

    if ((pm == PRIVACY_0_SHOW ) || (pm == PRIVACY_1_BLUR_DETAILS)) return true;
    return false;

/*

    PRIVACYMETHOD pm = this->effectivePrivacyMethod();

    this->
    if (this->indi) pm = INDI(this->indiId).effectivePrivacyMethod();
    else pm = FAM(this->famId).effectivePrivacyMethod();

    pm = MAX(this->effectivePrivacyMethod(), pm);

    if (GENE.printPolicy != PRINTPOLICY_1_EVERYTHING) {
        if (  this->indi ) if ( !GET_I_PRINT(type)) return false;
        if ( !this->indi ) if ( !GET_F_PRINT(type)) return false;
    }

    quint8 i;
    if (  this->indi ) i = INDI(this->indiId).privacyPolicy;
    if ( !this->indi ) i = FAM(this->famiId).privacyPolicy;

    if  ( (i == PRIVACY_2_HIDE_DETAILS) || (i == PRIVACY_3_BLUR_NAME) || (i == PRIVACY_4_HIDE) ) return false;

    PRIVACYMETHOD pm;
    pm = effectivePrivacyMethod();
    if ( (pm == PRIVACY_2_HIDE_DETAILS) || (pm == PRIVACY_3_BLUR_NAME) || (pm == PRIVACY_4_HIDE) ) return false;

    return true;
    */
}

QDate Entry::date1()
{
    return this->day.day1;
}

QDate Entry::date2()
{
    return this->day.day2;
}

ACCURACY Entry::getAcc()
{
    return this->day.getAcc();
}

bool Entry::used()
{
    if ( ( day.getAcc() != NA) || address.used() || age != 0 || agency != "" ||
         cause != "" || day.known || multiMedia.used() ||
         note.used() || place != "" || source.used() || attrType != "" ||
         attrText != "" || entryLineYes ) return true;

    return false;
}

Entry::Entry()
{
    clear();
}

Entry::~Entry()
{

}

void Entry::clear()
{
    day.clear();
    address.clear();
    multiMedia.clear();
    note.clear();
    source.clear();
    agency = "";
    age = 0;
    cause = "";
    place="";
    attrType = "";
    entryLineYes = false;
    attrText = "";
    type = (ENTRY)0;
    privacyPolicy = 0;
    clearEntryMoreShowFlags();
    indiId = 0;
    famiId = 0;
    //husbId = 0;
    //wifeId = 0;
}

void Entry::clearEntryMoreShowFlags()
{
    showAddEntryMore = false;
    showAddress0Line = false;
    showAddress1Line = false;
    showAddress2Line = false;
    showPostCityLine = false;
    showSourcePgQltyLine = false;
    showSourceTextLine = false;
    showstCnrPhoLine = false;
    showNotes = false;
}

