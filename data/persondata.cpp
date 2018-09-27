/*
 *
 *  qenes genealogy software
 *
 *  Copyright (C) 2009-2010 Ari Tähti
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Genl Public License veasion 2 as
 *  published by the Free Software Foundation.
 */

#include "data/genedata.h"
#include "data/setupvalues.h"
#include "macro.h"
#include <QStringList>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include "data/familydata.h"
#include "settings.h"

extern  GeneData        * geneData;

extern QSettings    sets;

#define FAMI2(famx)   parent->fami[famx]
#define INDI2(indix) parent->indi[indix]

// ongelma : tämä lukee aina geneData[currentGD].indi / fam etc...
// jos luetaan geneData[1].indi, miten hoidetaan että luetaan geneData[1].fam eikä geneData[currentGd].fam??

quint8 PersonData::howManyDescentantSteps()
{
    if (this->descentantSteps != 255) return this->descentantSteps;

    this->descentantSteps = 0;

    for (quint8 c=0 ; c<this->childIdList.size() ; c++) {
        descentantSteps = MAX(descentantSteps, INDI(this->childIdList.at(c)).howManyDescentantSteps()+1);
    }

    return descentantSteps;
}

quint16 PersonData::getNextAncestor()
{
    QList<quint16> chils;
    getChilds(&chils, false);

    if (!this->publishable()) return 0;

    for (quint8 c=0 ; c<chils.count() ; c++) if (INDI(chils.at(c)).isDirectAncestor) if (INDI(chils.at(c)).publishable()) return chils.at(c);
    return 0;
}

QString PersonData::getRootListName()
{
    QString name;
    if (this->nameFamily != "") name = this->nameFamily; else name = this->nameFirst;
    if (ientry(BIRTH).place != "") name += " (" + ientry(BIRTH).place + ")";
    return name;
}

bool PersonData::eventsToPrint()
{
    if ( (GENE.printPolicy == PRINTPOLICY_3_PUBLIC) && (this->isLiving()) )  return false;
    // julkisessa raportissa ei koskaan printata eventtejä jos hlö on elossa

    if ( (effectivePrivacyMethod() == PRIVACY_2_HIDE_DETAILS) ||
         (effectivePrivacyMethod() == PRIVACY_3_BLUR_NAME) ||
         (effectivePrivacyMethod() == PRIVACY_4_HIDE )) return false;

    if ( GET_PRINT_ESTIMATEDBIRTHS ) return true;

    for (int e=0 ; e<entry.size() ; e++)
        if ( this->entry.value(e).publishable() ) return true;

    return false;
   /*
     0 "Show"
     1 "Show Name, plur details, show genre"
     2 "Show name, no details"
     3 "Plur Name, hide details"
     4 "Hide"
          */
}

/*
QString PersonData::printResn()
{
    if (GENE.printPolicy == PRINTPOLICY_1_EVERYTHING) return GeneData::resn(this->privacyPolicy);

    PRIVACYMETHOD method  = (PRIVACYMETHOD)s_privacyMethod(this->privacyPolicy);
    PRIVACYMETHOD method2 = estimatedBirth.whichAutoPrivacyMethodApplies();
    PRIVACYMETHOD method3 = PRIVACY_0_SHOW;

    for (int f = 0 ; f<this->famSList.size() ; f++) method3 = MAX(method3, FAMI2(famSList.at(f)).effectivePrivacyMethod());

    PRIVACYMETHOD pm = MAX3(method, method2, method3);

    if (pm==PRIVACY_0_SHOW) return "";
    else return GeneData::resn(1);


}
*/

PRIVACYMETHOD PersonData::effectivePrivacyMethod()
{
    if ( GENE.printPolicy == PRINTPOLICY_1_EVERYTHING ) return PRIVACY_0_SHOW;

    PRIVACYMETHOD method  = (PRIVACYMETHOD)s_privacyMethod(this->privacyPolicy);

    PRIVACYMETHOD method2 = PRIVACY_0_SHOW;
    if (GENE.printPolicy == PRINTPOLICY_3_PUBLIC) method2 = estimatedBirth.whichAutoPrivacyMethodApplies();

    PRIVACYMETHOD method3 = PRIVACY_0_SHOW;
    int f;
    for (f = 0 ; f<this->famSList.size() ; f++) method3 = MAX(method3, FAMI2(famSList.at(f)).effectivePrivacyMethod());

    //qDebug() << method << method2 << method3;
    PRIVACYMETHOD pm = MAX3(method, method2, method3);

    // ***
    //for (f = 0 ; f<this->famSList.size() ; f++) if (FAMI2(famSList.value(f)).effectivePrivacyMethod() < pm) FAMI2(famSList.value(f)).effectivePrivacyMethod() = pm;

    return pm;

}

quint8 PersonData::isIEntry(ENTRY type)
{
    for (int e=0 ; e<this->entry.size() ; e++) if (entry.at(e).type == type) return e;

    return -1;
}

bool PersonData::publishable()
{
    PRIVACYMETHOD pm = effectivePrivacyMethod();

    if ( pm == PRIVACY_2_HIDE_DETAILS || pm == PRIVACY_3_BLUR_NAME || pm == PRIVACY_4_HIDE ) return false;
    return true;
}

void PersonData::arrangeEvents()
{
    // 0 = unlimited early day
    // 1 = birth.... 27
    // 28 = marriage
    // 39 = this day
    // 40 = unlimited max day
    // 41 = any living day event
    // 42 = buried (11) or chrematorized (12)

    int e, b;
    b=-1;

    // Järjestetään ensin niin, että päivämäärältään tunnetut tulevat ensin ja aikaisempi ensin.
    // Päivämäärältään tuntemattomat jää samalla jonon hännille

    for (e=0 ; e<entry.size()-1 ; e++) {        
        if ( entry.value(e).day > entry.value(e+1).day) {
            qDebug() << this->id << e<< entry.value(e).day << entry.value(e+1).day;
            entry.swap(e, e+1);
            e=0;b=0;
        }
        // etsitään samalla, mistä eventistä lähtien alkaa päivämäärältään tuntemattomat.
        // Tätä tietoa tarvitaan seuraavassa stepissä.
        if ( !entry.at(e).day.known && b==-1 ) b=e;
    }

    if (b==-1) b=entry.size();

    // Käydään lista uudelleen läpi. Nyt päivämäärältään tuntemattomat heitetään johonkin väliin

    // Käydään läpi ne eventit joilla ei ole päivämäärää. Katsotaan, mitä eventtiä ennen niiden pitää tapahtua.
    // Sen kertoo GET_I_ENTRY_MAX. Esim entrytyypin 2 (christening) entrymax arvoi voi olla 3 (confirmation),
    // eli jos listasta löytyy confirmation niin chr siirretään ennen confirmaatiota. Jos listassa ei ole chr,
    // etsitään jokin ...

    int max = entry.size();

    int e2;
    for (e=b ; e<max ; e++) {
        if (!entry.at(e).day.known) {
            // Löytyi eventti jonka päivää ei tiedetä. Siirretään tämä.

            for (e2 = 0 ; e2<max ; e2++) {

                if (GET_I_ENTRY_MIN(entry.at(e).type)==0) {

                    entry.move(e, 0);
                    e=b; b++; e2=max;
                } else
                if (GET_I_ENTRY_MAX(entry.at(e).type)==40) {

                    entry.move(e, max-1);
                    e=b; b++; max--; e2=max;
                } else
                if (GET_I_ENTRY_MAX(entry.at(e).type)==42 && (entry.at(e2).type == 11 || entry.at(e2).type == 12)) {

                    e2--;
                    if (e2<0) e2=0;
                    entry.move(e, e2);
                    e=b; b++; e2=max;
                } else
                if (Settings::indiEventMap[entry.at(e).type].indexOf(entry.at(e2).type) >
                    Settings::indiEventMap[entry.at(e).type].indexOf(entry.at(e).type)) {
                    // Eventing e2 tapahtumajärjestyksessä, tapahtuma e on myöhemmin kuin itse e2.

                    entry.move(e, e2);
                    e=b; b++; e2=max;
                }
            }
        }
    }
}

QString PersonData::printNote()
{
    if ( effectivePrivacyMethod() == PRIVACY_0_SHOW ) return this->note.text;

    if ( (GENE.printPolicy == PRINTPOLICY_3_PUBLIC) && this->isLiving() ) return "";
    // privacypolicy == PRINTPOLICY_2_GENE
    if ( effectivePrivacyMethod() == PRIVACY_0_SHOW ) return this->note.text;
    if ( effectivePrivacyMethod() == PRIVACY_1_BLUR_DETAILS ) {
        if (this->note.text != "") return "***"; else return "";
    }
    return "";
}

QString PersonData::nameInitials()
{
    QString returnStr = "";
    if (nameFirst != "" ) returnStr += nameFirst.left(1) + ".";
    if (name2nd != "" ) returnStr += name2nd.left(1) + ".";
    if (name3rd != "" ) returnStr += name3rd.left(1) + ".";
    if (nameFamily != "") returnStr += nameFamily.left(1) + ".";

    return returnStr;
}

QString PersonData::printName(QString name)
{
    /*
    0 "Show"
    1 "Show Name, plur details, show genre"
    2 "Show name, no details"
    3 "Plur Name, hide details",
    4 "Hide"
    */

    if ( GENE.printPolicy == PRINTPOLICY_1_EVERYTHING ) return name;

    PRIVACYMETHOD epm = effectivePrivacyMethod();

    if ( epm == PRIVACY_0_SHOW ) return name;
    if ( epm == PRIVACY_1_BLUR_DETAILS) return name;
    if ( epm == PRIVACY_2_HIDE_DETAILS ) return name;
    if ( epm == PRIVACY_3_BLUR_NAME ) return nameInitials();
    if ( epm == PRIVACY_4_HIDE ) return "***";

/*
    if ( ( GENE.printPolicy == PRINTPOLICY_3_PUBLIC ) && ( this->isLiving() ) ) {
        QString returnStr;
        returnStr = "#" + QString::number(id) + " " + nameInitials();

        return returnStr;
    }*/


    return "DOH! Error in PersonData::printName";
}

QString PersonData::printName1()
{
    QString name = printName(nameFirst);
    if (name.left(1) == "#") name ="";
    return name;
}

QString PersonData::printName2()
{
    QString name = printName(name2nd);
    if (name.left(1) == "#") name ="";
    return name;
}

QString PersonData::printName3()
{
    QString name = printName(name3rd);
    if (name.left(1) == "#") name ="";
    return name;
}

QString PersonData::printNameFamily()
{
    return printName(nameFamily);
}

QString PersonData::printName123Family()
{
    return printName(nameFamily + " " + nameFirst + " " + name2nd + " " + name3rd);
}

QString PersonData::printNameFamily1()
{
    return printName(nameFamily + " " + nameFirst);
}

QString PersonData::getPrintAge(Date when)
{
    if ( (GENE.printPolicy == PRINTPOLICY_3_PUBLIC) && isLiving()) return "";

    return getAge(when);
}

QString PersonData::getAge(Date when)
{
    QString ageString;    

    if (when.getAcc() == NA) return "";

    if (estimatedBirth.getAcc() == NA) return "";

    int age;

    if (estimatedBirth.getAcc() != FRO) {
        age = estimatedBirth.day1.daysTo(when.day1);

        QString prefix;
        if (estimatedBirth.getAcc() == EXA) prefix = "";
        if (estimatedBirth.getAcc() == ABT) prefix = "~";
        if (estimatedBirth.getAcc() == EST) prefix = "~";
        if (estimatedBirth.getAcc() == BEF) prefix = "<";
        if (estimatedBirth.getAcc() == AFT) prefix = ">";
        ageString = prefix + QString::number(age/365);
        if (privacyPolicy && GENE.printPolicy ) return "";
        return ageString;
    }
    if (estimatedBirth.getAcc() == FRO) {
        int age2;

        QDate when1, when2;

        if (when.getAcc() == FRO) {
            when1 = when.day1;
            when2 = when.day2;
        } else {
            when1 = when.day1;
            when2 = when.day1;
        }

        age  = estimatedBirth.day2.daysTo(when.day1);
        age2 = estimatedBirth.day1.daysTo(when.day2);

        QString ageString;

        if (age2/365 > GET_I_MAX(4)) {
            // Henkilö on syntymäajan perusteella auttamattoman kuollut, mikään ei enää auta!
            ageString = "+ < " + QString::number(estimatedBirth.day2.year()+ GET_I_MAX(4));
        } else if ( (age/365 > GET_I_MAX(4)) && (age2/365 < GET_I_MAX(4)) ) {
            // Hlö voi syntymäajan puolesta olla hengissä, mutta hädintuskin
            ageString = QString::number(age/365) + "...+<" + QString::number(estimatedBirth.day2.year()+ GET_I_MAX(4));

        }

        if (privacyPolicy && GENE.printPolicy) ageString = "";
        return ageString;
    }
    return "";
}

QString PersonData::getAge()
{
    Date now(QDate::currentDate(), EXA);
    return getAge(now);
}



void PersonData::eventPeriod(Date *answer, quint8 type, bool indi)
{
    QDate minDate, maxDate;
    QString minExp, maxExp;
    eventMinDay(type, indi, &minDate, &minExp);
    eventMaxDay(type, indi, &maxDate, &maxExp);

    answer->day1 = minDate;
    answer->day2 = maxDate;
    answer->day1explanation = minExp;
    answer->day2explanation = maxExp;

    if (answer->day1 != answer->day2) answer->setAcc(FRO); else answer->setAcc(EXA);
}

bool PersonData::knownDateIterator(int *type, QDate *date, bool roundInAccurateDays, QString *explanation)
{
    if (*type !=0 && *type != 40 && *type != 41 && *type != 42) {

        // tämä tapahtuma on joku normaali tapahtuma, eikä mikään erikoistype

        bool indi;
        if (*type>27) {
            *type -= 27;
            indi = false;
        } else indi = true;

        Entry entry;

        if (indi)  entry = ientry((ENTRY)*type);
        if (!indi) entry = fentry((ENTRYf)*type);

        if (entry.day.known) {
            if (!indi) *type += 27;
            *date = Date::roundInaccurateDate(entry.day, roundInAccurateDays);
            * explanation = entry.typeString();
            return true;
        }

        if (indi && *type == 1) {
            *date = Date::roundInaccurateDate(estimatedBirth, roundInAccurateDays);
            *explanation = "Birth (2)";
            return true;
        }
    } else {

        // tämä on jokin erikoistyyppi

        if (*type == 0 ) {
            *date = GET_ALLMINDAY;
            *explanation = "System min day (3)";
            return true;
        }

        if (*type == 40) {
            // unlimited max
            *date = GET_ALLMAXDAY;
            *explanation = "System max day (4)";
            return true;
        }

        if (*type == 41) {
            // While living
            // dead 4
            // buried 11
            // crem 12

            for (int e=0 ; e<this->entry.size() ; e++)
                if ( (entry.at(e).type != 4) && (entry.at(e).type != 11) && (entry.at(e).type != 12) ) if (!entry.at(e).day.day1.isNull()) {
                    if (entry.at(e).day.day1 > *date) {
                        *type = entry.at(e).type;
                        *date = Date::roundInaccurateDate(entry.at(e).day, roundInAccurateDays);
                        *explanation = entry.value(e).typeString() + " is known (5)";
                    }
                }

            for (int m=0 ; m<famSList.size() ; m++) for (int e=0 ; e<FAM(famSList.at(m)).entry.size() ; e++)
                if (!FAM(famSList.at(m)).entry.at(e).day.day1.isNull()) {
                    if (FAM(famSList.at(m)).entry.at(e).day.day1 > *date) {
                        *type = FAM(famSList.at(m)).entry.at(e).type + 27;
                        *date = Date::roundInaccurateDate(FAM(famSList.at(m)).entry.at(e).day, roundInAccurateDays);
                        *explanation = FAM(famSList.at(m)).entry.value(e).typeString() + " is known (6)";// + FAM(famSList.at(m)).entry.value(e).day.toString(true);
                    }
                }

            QList<quint16> childs;
            getChilds(&childs, false);
            for (int c=0 ; c<childs.size() ; c++) {
                if (sex == FEMALE) if (INDI(childs.at(c)).estimatedBirth.day2 > *date) {
                    *date = INDI(childs.at(c)).estimatedBirth.day2;
                    *explanation = "Child " + INDI(childs.at(c)).nameFirst + " birth (latest estimation) " + date->toString("dd.MM.yyyy");
                }
                if (sex == MALE) if (INDI(childs.at(c)).estimatedBirth.day2.addDays(-PREGNANCY_PERIOD) > *date) {
                    *date = INDI(childs.at(c)).estimatedBirth.day2.addDays(-PREGNANCY_PERIOD);
                    *explanation = "Child " + INDI(childs.at(c)).nameFirst + " birth (latest estimation) " + INDI(childs.at(c)).estimatedBirth.day2.toString("dd.MM.yyyy") + " - pregnancy period " + QString::number(PREGNANCY_PERIOD);
                }
            }
            return true;
        }

        if (*type == 42) {
            Date buried = this->ientry(BURIED).day;
            Date chrema = this->ientry(CREM).day;
            if (!buried.known && chrema.known) {
                *type = 12;
                *date = Date::roundInaccurateDate(chrema, roundInAccurateDays);
                *explanation = "Crema is known (7)";
                return true;
            }

            if (buried.known && !chrema.known ) {
                *type = 11;
                *date = Date::roundInaccurateDate(buried, roundInAccurateDays);
                *explanation = "Buried is known (8)";
                return true;
            }

            if ( buried.known && chrema.known ) {
                if (buried > chrema) {
                    *date = Date::roundInaccurateDate(buried, roundInAccurateDays);
                    *explanation = "Buried is known (9)";
                    *type = 11;
                    return true;
                } else {
                    *date = Date::roundInaccurateDate(chrema, roundInAccurateDays);
                    *type = 12;
                    *explanation = "Crema is known (10)";
                    return true;
                }
            }

            // ei hautuuta eikä krematoriosaatiota, eiköhän anneta tämä päivämäärä...?

            *date = QDate::currentDate();
            *type = 39;
            *explanation = "Current day (11)";
            return true;
        }
   }

    return false;

}

void PersonData::knownMinDate(int *type, QDate *date, QString *explanation)
{
    bool indi = false;

    // type on tässä formaattia
    // 0 palauttaa ALLMINDAY
    // 40 palauttaa ALLMAXDAY
    // 41 eloaikainen tapahtuma (eli ei kuolo, buried eikä crem)
    // 42 buried tai krematorisaatio
    // 1... 26 = indi tapahtumat
    // 27... marriage tapahtumat (eli 27=marriage)
    // knownDateIterator palauttaa true, jos eventin "type" päivä on tiedossa, ja se pivä palautetaan pointterissa "date"

    if (knownDateIterator(type, date, false, explanation)) return;

    // eventtiä "type" ei tidetä. Tutkitaan, mikä on mahdollinen tätä edeltävä eventti.
    if (*type>27) {
        type -=27;
        indi = false;
    } else indi = true;

    if ( indi) *type = GET_I_ENTRY_MIN(*type);
    if (!indi) *type = GET_F_ENTRY_MIN(*type);

    knownMinDate(type, date, explanation);
}

void PersonData::knownMaxDate(int *type, QDate *date, QString *explanation)
{
    // kts sepostusta knwonMinDate
    bool indi = false;

    if (knownDateIterator(type, date, true, explanation)) return;

    if (*type>27) {
        *type -=27;
        indi = false;
    } else indi = true;

    if ( indi) *type = GET_I_ENTRY_MAX(*type);
    if (!indi) *type = GET_F_ENTRY_MAX(*type);

    knownMaxDate(type, date, explanation);
}

void PersonData::eventMinDay(quint8 thisType, bool thisIndi, QDate * returnDate, QString * returnExplanation)
{

    /*  Called by makeUi and updateDates.
        Lets count the min day for the event.
        If the event has no type, return nothing.
        If the event type is birth, then count the birth minimum day in its own routhine birthMinMax
        (that's a bit complicated to fit in this method). Then, check if birthday has been entered for
        this individual. If there's no birthday, then simply return the general min/max values for the
        event. This might be a bit more sophisticated, maybe in the future.
        If the birthday has been entereded, then first calculate the simple minimun day by using the
        min-age value for that event. After that, go trhough the "order" values, and check that
        for example death will not be before birth etc...

        */

    if (GET_EVENT_TIMELIMITDISABLED) {
        * returnDate = GET_ALLMINDAY;
        * returnExplanation = "System minday";
        return;
    }

    int type = thisType;
    bool indi = thisIndi;

    if (type > 0) {

        if ((ENTRY)type == BIRTH && indi) {
            * returnExplanation = estimatedBirth.day1explanation;
            * returnDate = estimatedBirth.day1;
            return;
        }

        else {

            if (indi) type = GET_I_ENTRY_MIN(type);
            else type = GET_F_ENTRY_MIN(type);

            // GET_F_ENTRY_MIN antaa siis vastauksen mikä on rajoittava tapahtuma,
            // mutta koska kaikki eventit on oltava samassa kombolistassa peräkkäin, on myös
            // vastaukset numeroitava peräkkäin. Eli 0-27 on indi tapahtumat, 28 on marriage jne.
            // Kuitenkin funktiolle menevä parametri on "perintinen" eli GET_F_ENTRY_MIN(1) antaa
            // marriage:a edeltävän tapahtuman.
            // 0 = unlimited early day
            // 1 = birth.... 27
            // 28 = marriage
            // 39 = this day
            // 40 = unlimited max day
            // 41 = any living day event
            // 42 = buried (11) or chrematorized (12)

            // Tämä on rekursiivinen alkoritmi, joka etsii sen tapahtuman päivämäärän ja tyypin, joka rajoittaa tapahtuman entryTypeLimitingMin tyyppisen
            // eventin päivämäärää. Rajoittavan tapahtuman tyyppi tulee "type" muuttujassa ja päivämäärä day muuttujassa.

            knownMinDate(&type, returnDate, returnExplanation);

            if (type>27 || type < 39) {
                type -= 27;
                indi = false;
            } else indi = true;

            // -----------------------------------------------------------------------------------------------------

            // Varmistetaan, ettÃ¤ Ã¤idin kuolema tai sen jÃ¤lkeiset tapahtumat ei tapahdu lapsen syntymÃ¤n jÃ¤lkeen,
            // ts. ettÃ¤ Ã¤iti on hengissÃ¤ sinÃ¤ hetkenÃ¤ kun lapsi syntyy. Ã?idin kuolema voi tapahtua yleensä vain
            // lapsen syntymÃ¤n jÃ¤lkeen.

            if (indi && ( (type == 4) || (type == 11) || (type == 12) ) ) {
                // 4 death
                // 11 crem
                // 12 buried

                QList<quint16> childs;
                QDate childBirth;
                getChilds(&childs, false);
                QString tmpExp;

                for (int c = 0 ; c<childs.size() ; c++) {
                    QDate birth = INDI2(childs.at(c)).estimatedBirth.day1;
                    if (childBirth.isNull()) {
                        childBirth = birth;
                        tmpExp = "This person birth";
                    }
                    else if (birth > childBirth) {
                        childBirth = birth;
                        tmpExp = "Child " + INDI2(childs.at(c)).nameFirst + " birth";
                    } //                        childBirth = MAX(childBirth, birth);
                }

                // childBirthiin on haettu nyt myöhäisin lapsen syntymä

                if (!childBirth.isNull() && this->sex == MALE) childBirth = childBirth.addDays(-280);
                if (childBirth > * returnDate) {
                    * returnDate = childBirth;
                    * returnExplanation = tmpExp;
                } //  day = MAX(day, childBirth);

            }

            // lasketaan aikaisin sallittu päivä, jona tämä tapahtuma saa tapahtua laskien tapahtuman oma sallittu minimi-ikä

            Date birth;
            QDate compAge;

            if ( ientry(BIRTH).day.known ) birth = ientry(BIRTH).day; else {
                birth = this->estimatedBirth;
            }

            quint8 age;
            if (thisIndi) age = GET_I_MIN(thisType); else age = GET_F_MIN(thisType);

            compAge = birth.day1.addYears(age);

            if (compAge > * returnDate) {

                * returnDate = compAge;
                * returnExplanation = " earliest allowed age for this event is " + QString::number(age) + " years.";
            }


            return;
        }
    }

    return;
}

void PersonData::eventMaxDay(quint8 thisType, bool thisIndi, QDate * returnDate, QString * returnExplanation)
{

    /*  Called by makeUi and updateDates.
        Lets count the min day for the event.
        If the event has no type, return nothing.
        If the event type is birth, then count the birth minimum day in its own routhine birthMinMax
        (that's a bit complicated to fit in this method). Then, check if birthday has been entered for
        this individual. If there's no birthday, then simply return the general min/max values for the
        event. This might be a bit more sophisticated, maybe in the future.
        If the birthday has been entereded, then first calculate the simple minimun day by using the
        min-age value for that event. After that, go trhough the "order" values, and check that
        for example death will not be before birth etc...

        */

    if (GET_EVENT_TIMELIMITDISABLED) {
        * returnDate = GET_ALLMAXDAY;
        * returnExplanation = "system maxday";
        return;
    }

    int type = thisType;
    bool indi = thisIndi;

    QDate dayMax;
    QString explanationMax;

    if (type > 0) {
        if ((ENTRY)type == BIRTH && indi) {
            * returnExplanation = estimatedBirth.day2explanation;
            * returnDate = estimatedBirth.day2;
            return;
        }

        else {
            //int limitType;
            if (indi) type = GET_I_ENTRY_MAX(type);
                 else type = GET_F_ENTRY_MAX(type);

            // GET_F_ENTRY_MIN antaa siis vastauksen mikä on rajoittava tapahtuma,
            // mutta koska kaikki eventit on oltava samassa kombolistassa peräkkäin, on myös
            // vastaukset numeroitava peräkkäin. Eli 0-27 on indi tapahtumat, 28 on marriage jne.
            // Kuitenkin funktiolle menevä parametri on "perintinen" eli GET_F_ENTRY_MIN(1) antaa
            // marriage:a edeltävän tapahtuman.
            // 0 = unlimited early day
            // 1 = birth.... 27
            // 28 = marriage
            // 39 = this day
            // 40 = unlimited max day
            // 41 = any living day event
            // 42 = buried (11) or chrematorized (12)

            // Tämä on rekursiivinen alkoritmi, joka etsii sen tapahtuman päivämäärän ja tyypin, joka rajoittaa tapahtuman entryTypeLimitingMin tyyppisen
            // eventin päivämäärää. Rajoittavan tapahtuman tyyppi tulee "type" muuttujassa ja päivämäärä day muuttujassa.

            knownMaxDate(&type, &dayMax, &explanationMax);
            //if (this->id == 227) qDebug() << "aaa" << day << type;
            if (type>27 || type<39) {
                // mitä näillä tehtäisiin?
                type -= 27;
                indi = false;
            } else indi = true;

            // -----------------------------------------------------------------------------------------------------

            // lasketaan aikaisin sallittu päivä, jona tämä tapahtuma saa tapahtua laskien tapahtuman oma sallittu minimi-ikä

            Date birth;
            QDate compAge;

            if ( ientry(BIRTH).day.known ) birth = ientry(BIRTH).day; else {
                explanationMax = "This person birth";
                birth = estimatedBirth;
            }

            QString typeString;
            if (thisIndi) {
                compAge = birth.day1.addYears(GET_I_MAX(thisType));
                typeString = "Event " + Entry::typeString(thisType, true) + " latest allowed day " + QString::number(GET_I_MAX(thisType)) + " years after birth.";
                //qDebug() << "true" << GET_I_MAX(thisType) << thisType;
            } else {
                compAge = birth.day1.addYears(GET_F_MAX(thisType));
                typeString = "Event " + Entry::typeString(thisType, false) + " latest allowed day " + QString::number(GET_I_MAX(thisType)) + " years after birth.";
                //typeString = Entry::typeString(thisType, false);
                //qDebug() << "false" << GET_F_MAX(thisType) << thisType;
            }

            if (compAge < dayMax) {
            if (this->id == 1) qDebug() << "bbb" << typeString << compAge;
                explanationMax = typeString;
                dayMax = compAge;
            }

            //if (this->id == 227) qDebug() << "ccc" << day << birth;
            // lasketaan aikaisin sallittu päivä perustuen löydettyyn rajoittavaan päivämäärään

            * returnExplanation = explanationMax;
            * returnDate = dayMax;

            if (* returnDate > QDate::currentDate()) {
                * returnDate = QDate::currentDate();
                * returnExplanation = "This date";
            }

            return;
        }
    }

    return;
}

void PersonData::getLivingEventDays(QDate *minDay, QDate *maxDay)
{
    int type, e, f;

    *minDay = QDate();
    *maxDay = QDate();

    for (e=0 ; e<this->entry.size() ; e++) {
        Date date = this->entry.at(e).day;
        type = this->entry.value(e).type;
        if (type != 11 && type != 12) {
            // 4 = death
            // 11 buried
            // 12 Chremat.
            if (minDay->isNull() && !date.day1.isNull()) *minDay = date.day1; else *minDay = MIN(*minDay, date.day1);
            if (maxDay->isNull() && !date.day2.isNull()) *maxDay = date.day2; else *maxDay = MAX(*maxDay, date.day2);
        }
    }

    for (f = 0 ; f<this->famSList.size() ; f++ ) {
        for (e = 0 ; e<FAMI2(this->famSList.at(f)).entry.size() ; e++ ) {
            Date date = FAMI2(this->famSList.at(f)).entry.value(e).day;
            if (minDay->isNull() && !date.day1.isNull()) *minDay = date.day1; else *minDay = MIN(*minDay, date.day1);
            if (maxDay->isNull() && !date.day2.isNull()) *maxDay = date.day2; else *maxDay = MAX(*maxDay, date.day2);
        }
    }
}

bool PersonData::isLiving()
{
    Date death;

    death = this->ientry(DEATH).day;
    if (death.known) return false;

    QDate estDeath = getCalBirth().addYears(GET_I_MAX(DEATH));
    if ( estDeath < QDate::currentDate() ) return false;

    return true;
}

QString PersonData::getShortFamilyRelation()
{    
    QString koe = this->familyRelation;

    if (GET_RELATION_COUSIN3_USE)
    {
        koe.replace("mmmmcccc", GET_RELATION_COUSIN3);
        koe.replace("mmmfcccc", GET_RELATION_COUSIN3);
        koe.replace("mmfmcccc", GET_RELATION_COUSIN3);
        koe.replace("mmffcccc", GET_RELATION_COUSIN3);
        koe.replace("mfmmcccc", GET_RELATION_COUSIN3);
        koe.replace("mfmfcccc", GET_RELATION_COUSIN3);
        koe.replace("mffmcccc", GET_RELATION_COUSIN3);
        koe.replace("mfffcccc", GET_RELATION_COUSIN3);
        koe.replace("fmmmcccc", GET_RELATION_COUSIN3);
        koe.replace("fmmfcccc", GET_RELATION_COUSIN3);
        koe.replace("fmfmcccc", GET_RELATION_COUSIN3);
        koe.replace("fmffcccc", GET_RELATION_COUSIN3);
        koe.replace("ffmmcccc", GET_RELATION_COUSIN3);
        koe.replace("ffmfcccc", GET_RELATION_COUSIN3);
        koe.replace("fffmcccc", GET_RELATION_COUSIN3);
        koe.replace("ffffcccc", GET_RELATION_COUSIN3);
    }

    if (GET_RELATION_COUSIN2_USE)
    {
        koe.replace("mmmccc", GET_RELATION_COUSIN2);
        koe.replace("mmfccc", GET_RELATION_COUSIN2);
        koe.replace("mfmccc", GET_RELATION_COUSIN2);
        koe.replace("mffccc", GET_RELATION_COUSIN2);
        koe.replace("fmmccc", GET_RELATION_COUSIN2);
        koe.replace("fmfccc", GET_RELATION_COUSIN2);
        koe.replace("ffmccc", GET_RELATION_COUSIN2);
        koe.replace("fffccc", GET_RELATION_COUSIN2);
    }

    if (GET_RELATION_COUSIN_USE)
    {
        koe.replace("mmcc", GET_RELATION_COUSIN);
        koe.replace("mfcc", GET_RELATION_COUSIN);
        koe.replace("fmcc", GET_RELATION_COUSIN);
        koe.replace("ffcc", GET_RELATION_COUSIN);
    }

    if (GET_RELATION_FUNCLE_USE)
        if (this->sex == MALE) {
            if (this->familyRelation.right(1) != "s") {
                koe.replace("ffc", GET_RELATION_FUNCLE);
                koe.replace("fmc", GET_RELATION_FUNCLE);
            } else {
                koe.replace("ffc", GET_RELATION_AUNT);
                koe.replace("fmc", GET_RELATION_AUNT);
            }
    }

    if (GET_RELATION_MUNCLE_USE)
        if (this->sex == MALE) {
            if (this->familyRelation.right(1) != "s") {
                koe.replace("mfc", GET_RELATION_MUNCLE);
                koe.replace("mmc", GET_RELATION_MUNCLE);
            } else {
                koe.replace("mfc", GET_RELATION_AUNT);
                koe.replace("mmc", GET_RELATION_AUNT);
            }

    }


    if (GET_RELATION_AUNT_USE)
        if (this->sex == FEMALE) {
            if (this->familyRelation.right(1) != "s") {
                koe.replace("mfc", GET_RELATION_AUNT);
                koe.replace("mmc", GET_RELATION_AUNT);
                koe.replace("ffc", GET_RELATION_AUNT);
                koe.replace("fmc", GET_RELATION_AUNT);
            } else {
                koe.replace("mfc", GET_RELATION_MUNCLE);
                koe.replace("mmc", GET_RELATION_MUNCLE);
                koe.replace("ffc", GET_RELATION_MUNCLE);
                koe.replace("fmc", GET_RELATION_MUNCLE);
            }
    }

    if (GET_RELATION_SIBLING_USE)
        {
        koe.replace("mc", GET_RELATION_SIBLING);
        koe.replace("fc", GET_RELATION_SIBLING);
    }

    koe.replace("cm", "s");
    koe.replace("cf", "s");

    koe.replace("cf", "h");
    koe.replace("cm", "w");

    koe.replace("mmmmmmmmmmm", "11" + GET_RELATION_MOTHER);
    koe.replace("mmmmmmmmmm", "10" + GET_RELATION_MOTHER);
    koe.replace("mmmmmmmmm", "9" + GET_RELATION_MOTHER);
    koe.replace("mmmmmmmm", "8" + GET_RELATION_MOTHER);
    koe.replace("mmmmmmm", "7" + GET_RELATION_MOTHER);
    koe.replace("mmmmmm", "6" + GET_RELATION_MOTHER);
    koe.replace("mmmmm", "5" + GET_RELATION_MOTHER);
    koe.replace("mmmm", "4" + GET_RELATION_MOTHER);
    koe.replace("mmm", "3" + GET_RELATION_MOTHER);
    koe.replace("mm", "2" + GET_RELATION_MOTHER);
    koe.replace("m", GET_RELATION_MOTHER);

    koe.replace("fffffffffff", "11" + GET_RELATION_FATHER);
    koe.replace("ffffffffff", "10" + GET_RELATION_FATHER);
    koe.replace("fffffffff", "9" + GET_RELATION_FATHER);
    koe.replace("ffffffff", "8" + GET_RELATION_FATHER);
    koe.replace("fffffff", "7" + GET_RELATION_FATHER);
    koe.replace("ffffff", "6" + GET_RELATION_FATHER);
    koe.replace("fffff", "5" + GET_RELATION_FATHER);
    koe.replace("ffff", "4" + GET_RELATION_FATHER);
    koe.replace("fff", "3" + GET_RELATION_FATHER);
    koe.replace("ff", "2" + GET_RELATION_FATHER);
    koe.replace("f", GET_RELATION_FATHER);

    koe.replace("ccccccccccc", "11" + GET_RELATION_CHILD);
    koe.replace("cccccccccc", "10" + GET_RELATION_CHILD);
    koe.replace("ccccccccc", "9" + GET_RELATION_CHILD);
    koe.replace("cccccccc", "8" + GET_RELATION_CHILD);
    koe.replace("ccccccc", "7" + GET_RELATION_CHILD);
    koe.replace("cccccc", "6" + GET_RELATION_CHILD);
    koe.replace("ccccc", "5" + GET_RELATION_CHILD);
    koe.replace("cccc", "4" + GET_RELATION_CHILD);
    koe.replace("ccc", "3" + GET_RELATION_CHILD);
    koe.replace("cc", "2" + GET_RELATION_CHILD);
    koe.replace("c", GET_RELATION_CHILD);

    return koe;
 //   return (koe.right(koe.length()-1)); // there is always mark # in the beginning, this removes that
}

void PersonData::clear(GeneData *p)
{
    nameFirst = "";    
    name2nd = "";
    name3rd = "";
    nameFamily = "";
    nameGedCom = "";
    familyRelation = "";
    nameSource.clear();
    nameNote.clear();
    currentSpouse = 0;
    note.clear();
    sex = UNKNOWN;    
    famc.clear();
    famcSecondary.clear();
    pedi = NO_PEDI; // ei käytössä
    rfn = "";
    afn = "";
    refn = "";
    refnType = "";
    rin = 0;
    source.clear();
    multiMedia.clear();
    adoptedBy = NO_ADOP;
    anciTag = "";
    desiTag = "";
    anci = 0;
    desi = 0;    
    deleted = false;    
    changed.setDate(QDate());

    submTags.clear();
    submitters.clear();
    entry.clear();
    famsTags.clear();
    famSList.clear();
    selected = false;
    incompleteBirthEvaluation = true;
    generation = 0;
    privacyPolicy = 0;
    parent = p;    
    descentantSteps = 255;
}

quint16 PersonData::getFamS() { return famSList.value(currentSpouse); }

quint16 PersonData::getFamS(quint16 x) { return famSList.value(x); }

quint16 PersonData::getFather() { return FAMI2(this->famc.value).husb; }

quint16 PersonData::getMother() { return FAMI2(this->famc.value).wife; }

quint16 PersonData::getWife() { return FAMI2(this->getFamS()).wife; }

quint16 PersonData::getHusb() { return FAMI2(this->getFamS()).husb; }

quint16 PersonData::getSpouse(quint16 id)
{
    quint16 spouse = FAMI2( famSList.value(id) ).husb;
    if ( ( spouse==0 ) || ( spouse==this->id )) spouse=FAMI2( famSList.value(id) ).wife;
    if (spouse != this->id) return spouse; else return 0;
}

quint16 PersonData::getSpouse() {
    return getSpouse(currentSpouse);
}

bool PersonData::isParent(quint16 adult)
{
    if ( (FAMI2(this->famc.value).wife == adult) || (FAMI2(this->famc.value).husb == adult ) ) return true;
    else return false;
}

void PersonData::setMother(quint16 mother) {
    FAMI2(this->famc.value).wife = mother;
}

void PersonData::setFather(quint16 father) {
    FAMI2(this->famc.value).husb = father;
}

void PersonData::addFamS(quint16 fams) { this->famSList.append(fams); }

// asettaa child:n tämän henkilön lapseksi, kun child:n famc arvo on laskettu.
void PersonData::setChild (quint16 child)
{
    famSList.append(INDI2(child).famc.value);
    if (!FAMI2(INDI2(child).famc.value).childs.contains(child)) FAMI2(INDI2(child).famc.value).childs.append(child);
}

QString PersonData::getFamsTag(quint16 famsID) { return this->famsTags.value(famsID); }

void PersonData::setHusb(quint16 husb)
{
    FAMI2(this->getFamS()).husb = husb;
}

void PersonData::setWife(quint16 wife) {
    FAMI2(this->getFamS()).wife = wife;
}

bool PersonData::isChild(quint16 child)
{
    int i;
    for (i=0 ; i<= this->famSList.size() ; i++)
        if ( (this->famSList.value(i) == INDI2(child).famc.value) && this->famSList.value(i) != 0 ) return true;
    return false;
}

void PersonData::getCousins(QList<quint16> *cousins, bool currentSpouseOnly)
{
    quint16 i, ff, fm, mf, mm, mother, father;
    QList<int> myGrandParents;
    cousins->clear();

    mother = this->getMother();
    father = this->getFather();

    myGrandParents.append(INDI2(getFather()).getFather());
    myGrandParents.append(INDI2(getFather()).getMother());
    myGrandParents.append(INDI2(getMother()).getFather());
    myGrandParents.append(INDI2(getMother()).getMother());

    for (i=1 ; i<=parent->indiLastUsed ; i++) {
        ff = INDI2(INDI2(i).getFather()).getFather();
        fm = INDI2(INDI2(i).getFather()).getMother();
        mf = INDI2(INDI2(i).getMother()).getFather();
        mm = INDI2(INDI2(i).getMother()).getMother();

        if ((INDI2(i).getFather() != father && INDI2(i).getMother() != mother)) {
            if (   ((ff != 0) && myGrandParents.contains(ff) )
                || ((fm != 0) && myGrandParents.contains(fm) )
                || ((mf != 0) && myGrandParents.contains(mf) )
                || ((mm != 0) && myGrandParents.contains(mm) ) ) {

                if ( (currentSpouseOnly
                    && ( (myGrandParents.contains(ff) && myGrandParents.contains(fm))
                         || (myGrandParents.contains(mf) && myGrandParents.contains(mf)) ))
                    || !currentSpouseOnly )
                    if (!INDI2(i).deleted) cousins->append(i);
            }
        }
    }
}

void PersonData::getChilds(QList<quint16> *childs, bool currentSpouseOnly)
{
    quint16 i;
    childs->clear();
    for (i=1 ; i<=parent->indiLastUsed ; i++) {
        if (isChild(i) && ( !currentSpouseOnly || ( currentSpouseOnly && this->getFamS() == INDI2(i).famc.value ) ) )
        {
            if (!INDI2(i).deleted) childs->append(i);
        }
    }
}

void PersonData::getSiblings(QList<quint16> *siblings, bool currentSpouseOnly)
{    
    quint16 i;
    siblings->clear();

    for (i=1 ; i <= parent->indiLastUsed ; i++) {
        if (currentSpouseOnly) {
            if ( (INDI2(i).famc.value == this->famc.value) && this->famc.value != 0) siblings->append(i);
        } else {
            if (   ( ( INDI2(i).getFather() == this->getFather()) && this->getFather() != 0 )
                || ( ( INDI2(i).getMother() == this->getMother()) && this->getMother() != 0 ) )
                if (!INDI2(i).deleted) siblings->append(i);

        }
    }    
}

int PersonData::getMotherInLaw()
{
    int mil;

    if (INDI2(id).sex == MALE)
        mil = INDI2(FAMI2(INDI2(id).getFamS()).wife).getMother();
    else mil = INDI2(FAMI2(INDI2(id).getFamS()).husb).getMother();

    return mil;
}

void PersonData::makeNameList(QStringList *nameList, QList<quint16> *list)
{
    QString name;
    int i;
    for (i=0 ; i<list->size() ; i++) {
        if (list->at(i) == 0 ) name = "(unknown)";
        else {
            name = INDI2(list->at(i)).nameFamily + " " + INDI2(list->at(i)).nameFirst + " " + INDI2(list->at(i)).name2nd + " " + INDI2(list->at(i)).name3rd;
            name = name.left(23); // liian pitkä nimi ui-boxissa voi kaataa ohjelman
        }
        *nameList << name;
    }
}

void PersonData::spouses(QList<quint16> *spo)
{
    spo->clear();

    for (int i = 0 ; i<this->famSList.size() ; i++) {
        if (this->sex == MALE) {
            spo->append(FAMI2(this->famSList.at(i)).wife);
        } else {
            spo->append(FAMI2(this->famSList.at(i)).husb);
        }
    }
}

void PersonData::assignChildFromTo(quint16 child, quint16 oldFamc, quint16 newFamc)
{
    if (FAMI2(oldFamc).childs.contains(child))
        FAMI2(oldFamc).childs.removeAll(child);

    if (!FAMI2(newFamc).childs.contains(child))
         FAMI2(newFamc).childs.append(child);
}

void PersonData::assignFather(quint16 father)
{
    //int spouse      = INDI2(father).getSpouse();
    quint16 oldFamc     = famc.value;
    quint16 oldFather   = FAMI2(famc.value).husb;
    quint16 familyID    = 0;

    // onko fd:tä jossa on samat isä ja äiti
    for (quint16 f=1 ; f<=parent->famiLastUsed ; f++)
        if ( ( FAMI2(f).wife && ( FAMI2(f).wife == this->getMother() && FAMI2(f).husb == father) )
          || ( FAMI2(f).wife && ( FAMI2(f).wife == this->getMother() && FAMI2(f).husb == 0 && famc.value == f ) ) ) //??
           {
        familyID = f;
        FAMI2(f).husb = father;
        famc.value = f; //?

        break;
    }

    // Ollaanko isi asettamassa sellaiseen perheeseen jossa isiä ei ennen ole. Jos ollaan, ei tarvi luoda uutta
    // fd:tä

    // ei ole sellaista fd:tä, jolla olisi samat isä ja äiti. Luodaan uusi fd.

    if (familyID == 0) {
        parent->famiSpace(parent->famiLastUsed+1);
        familyID = parent->famiLastUsed;
        FAMI2(familyID).husb = father;
        FAMI2(familyID).wife = FAMI2(famc.value).wife;
        famc.value = familyID;

    }

    // Lisätään childs listaan tämä henkilö


    this->assignChildFromTo(id, oldFamc, familyID);

    if (!INDI2(father).famSList.contains(familyID))
         INDI2(father).famSList.append(familyID);

    if (!INDI2(FAMI2(famc.value).wife).famSList.contains(familyID))
         INDI2(FAMI2(famc.value).wife).famSList.append(familyID);

    /*
      mikä tämä on?
    int i;
    for (i=0 ; i<INDI2(spouse).famSList.size() ; i++) {
        if ( spouse && FAMI2(INDI2(spouse).famSList.value(i)).husb == 0 ) {
            INDI2(spouse).famSList.removeAt(i);
            i=-1;
        }
    }
    */

    famc.value = familyID;

    // jäikö vanha fd paikka käyttämättä? Jos sinne jäi vain äiti eikä lapsia, otetaan pois. Isähän deletoitiin jo

    if (FAMI2(oldFamc).childs.size() == 0 ) {
        FAMI2(oldFamc).wife = 0;
        FAMI2(oldFamc).deleted = true;

        if (INDI2(oldFather).famSList.contains(oldFamc))
            INDI2(oldFather).famSList.removeAll(oldFamc);

        if (INDI2(FAMI2(famc.value).wife).famSList.contains(oldFamc))
            INDI2(FAMI2(famc.value).wife).famSList.removeAll(oldFamc);
    }

    // tämä toteutuu jos ollaan poistamassa henkilöltä kumpaakin vanhempaa, ja tässä vaiheessa
    // kumpikin on juuri poistettu. Silloin familyID jää käyttämättä, se näkyy siitä että husb ja wife
    // on nolla. Tällöin myös famc arvo on asetettava nollaan ja fd merkattava tarpeettomaksi.
    if ( FAMI2(familyID).husb == 0 && FAMI2(familyID).wife == 0 ) {
        FAMI2(familyID).deleted = true;
        this->famc.value = 0;
    }
}

void PersonData::assignMother(quint16 mother)
{
    //int spouse    = INDI2(mother).getSpouse();
    quint16 oldFamc   = this->famc.value;
    quint16 oldMother = FAMI2(oldFamc).wife;

    quint16 familyID = 0;

    // Haetaan perhettä jonka isä ja äiti on täsmälleen samat kuin "assignoimisen" jälkeen.
    // Jos löytyy, käytetään tätä familyID numeroa.
    // Myös, jos löytyy perheID jossa äiti on 0 (muutoksen jälkeen siis äiti => mother) ja tämän henkilön lapsi famc... plää plää
    for (quint16 f=1 ; f<=parent->famiLastUsed ; f++) {
        if (
                ( FAMI2(f).husb && ( FAMI2(f).husb == this->getFather() && FAMI2(f).wife == mother ) )
             || ( FAMI2(f).husb && ( FAMI2(f).husb == this->getFather() && FAMI2(f).wife == 0 && this->famc.value == f ) )
                ) {
            familyID = f;
            FAMI2(f).wife = mother;
            this->famc.value = f;

            break;
        }
    }
    // *!* tässä voi käydä niin että assignoimisen seurauksena on fd record jota kukaan ei käytä.
    // Sellaista ei pitäisi tallentaa.

    // Jos ei ole perhettä jonka isä ja äiti on täsmälleen samat kuin "assignoimisen" jälkeen,
    // pitää luoda uusi perheID.
    if (familyID == 0) {
        parent->famiSpace(parent->famiLastUsed+1);
        familyID = parent->famiLastUsed;
        FAMI2(familyID).wife = mother;
        FAMI2(familyID).husb = FAMI2(this->famc.value).husb;
        this->famc.value = familyID;
    }

   assignChildFromTo(id, oldFamc, familyID);

    if (!INDI2(mother).famSList.contains(familyID))
         INDI2(mother).famSList.append(familyID);

    if (!INDI2(FAMI2(famc.value).husb).famSList.contains(familyID))
         INDI2(FAMI2(famc.value).husb).famSList.append(familyID);

    famc.value = familyID;

    // jäikö vanha fd paikka käyttämättä? Jos sinne jäi vain isä eikä lapsia, otetaan pois. Vaimohan deletoitiin jo

    if (FAMI2(oldFamc).childs.size() == 0 ) {
        FAMI2(oldFamc).husb = 0;
        FAMI2(oldFamc).deleted = true;

        if (INDI2(oldMother).famSList.contains(oldFamc))
            INDI2(oldMother).famSList.removeAll(oldFamc);

        if (INDI2(FAMI2(famc.value).husb).famSList.contains(oldFamc))
            INDI2(FAMI2(famc.value).husb).famSList.removeAll(oldFamc);
    }

    if ( FAMI2(familyID).husb == 0 && FAMI2(familyID).wife == 0 ) {
        FAMI2(familyID).deleted = true;
        this->famc.value = 0;
    }
}

void PersonData::deleteThis()
{
    deleted = true; // tämän perusteella savefile ei tallennta tätä henkilöä

    // Käydään kaikki perhetietokannat läpi, ja etsitään ne tiedostot jossa tämä henkilö on joko
    // mies tai vaimo. Kun tällainen löytyy, asetetaan arvoksi 0.
    for (quint16 f=0 ; f<=parent->famiLastUsed ; f++) {
        if (FAMI2(f).wife == this->id) FAMI2(f).wife = 0;
        if (FAMI2(f).husb == this->id) FAMI2(f).husb = 0;

        // Jos löydetyn perheen sekä mies että vaimo on nolla, ei tätä perhettä silloin enää ole ja siksi tämä
        // perhe merkataan deletoiduksi. Deletoitu perhe jää vielä muistikantaan kummittelemaan, mutta sitä ei
        // tallenneta.
        if (FAMI2(f).wife == 0 && FAMI2(f).husb == 0) {
            //if (f == parent->famiLastUsed && parent->famiLastUsed > 0) parent->famiLastUsed--;
            FAMI2(f).deleted = true;

            // Käydään kaikki henkilöt tietokannassa läpi. Jos jonkun henkilön lapsuusperhe osoittaa tähän
            // perheeseen, joka juuri tuhottiin ja jota siis ei ole, merkataan tämä lapsuusperhearvo nollaksi
            // jotta ei siis viitata perheeseen jota ei ole.
            for (quint16 i=0 ; i<=parent->indiLastUsed ; i++) if ( INDI2(i).famc.value == f ) INDI2(i).famc.value = 0;
        }

        FAMI2(f).childs.removeAll(id);
    }

    famc.value = 0;    
    famSList.clear();
    clear(this->parent);

    nameFamily  = "[deleted]"; // näkyy taulukossa deletoituna
    nameFirst   = "[deleted]";
    name2nd     = "[deleted]";
    name3rd     = "[deleted]";
    deleted = true; // ei tallenneta tätä tyhjäksi jäävää muistipaikkaa

    parent->browseHistory.removeAll(this->id);

}

void PersonData::ancestorStep()
{
    selected = true;
    if (getMother() && !INDI2(getMother()).selected) INDI2(getMother()).ancestorStep();
    if (getFather() && !INDI2(getFather()).selected) INDI2(getFather()).ancestorStep();
}

void PersonData::selectAll()
{
    for (quint16 i=0 ; i<=parent->indiLastUsed ; i++) INDI2(i).selected = true;
}

void PersonData::selectAncestorsAll()
{
    quint16 i, f;

    INDI2(parent->currentId).selectAncestors();

    for (i = 1 ; i<=parent->indiLastUsed ; i++) { // käydään kaikki henkilöt läpi
        if (!INDI2(i).selected) {           // etsitään niitä joita ei ole selectoitu
            if (   (INDI2(INDI2(i).getMother()).selected)
                || (INDI2(INDI2(i).getFather()).selected) ) {
                if (   ( INDI2(i).getMother() != parent->currentId )
                    && ( INDI2(i).getFather() != parent->currentId ) ) INDI2(i).selectDescendants();
            }

            for (int s = 0 ; s<INDI2(i).famSList.size() ; s++) {  // valitsemattomien hlö:n puolisto
                f = INDI2(i).getFamS(s);
                if (    (INDI2(FAMI2(f).wife).selected)
                    ||  (INDI2(FAMI2(f).husb).selected) ) {

                    if (   ( FAMI2(f).wife != parent->currentId )
                        && ( FAMI2(f).husb != parent->currentId ) ) {

                        INDI2(i).selectAncestors();
                    }
                }
            }
        }
    }
}

void PersonData::selectAncestors()
{
    ancestorStep();
}

void PersonData::selectNone()
{
    for (quint16 i=0 ; i<=parent->indiLastUsed ; i++) INDI2(i).selected = false;
}

void PersonData::descendantStep()
{
    QList<quint16> childID;

    getChilds(&childID, false);
    this->selected = true;

    for (int j = 0 ; j<childID.size() ; j++) {
        if (!INDI2(childID.at(j)).selected) INDI2(childID.at(j)).descendantStep();
    }
}

void PersonData::selectDescendants()
{
    descendantStep();
}

Date PersonData::getEntryDaybyType(bool indi, quint8 type)
{
    int i;
    if (indi) {
        for (i=0 ; i<this->entry.size() ; i++) if (entry.at(i).type == type) return getEntryDay(indi, i);
    }
    if (!indi) {
        for (i=0 ; i<FAMI2(this->getFamS()).entry.size() ; i++) if (FAMI2(this->getFamS()).entry.at(i).type == type) return getEntryDay(indi, i);
    }

    Date date;
    date.setDate1(QDate());
    date.known = false;
    return date;
}

Date PersonData::getEntryDay(bool indi, quint8 entryId)
{
    if (indi)  if (entry[entryId].getAcc() != CAL) return entry[entryId].day;
    if (!indi) if (FAMI2(this->getFamS()).entry[entryId].getAcc() != CAL ) return FAMI2(this->getFamS()).entry[entryId].day;
    if (indi) {
        if (entry[entryId].type != BIRTH) {
            Date date;
            int deltaYear = GET_I_CAL(entry[entryId].type);            
            date.setDate1(getCalBirth().addYears(deltaYear));
            date.setAcc(CAL);
            return date;
        }
        else
        {            
            Date date;
            date.setDate1(getCalBirth());
            date.setAcc(CAL);
            return date;
        }
    }

    if (!indi) {
        Date date;
        QDate cDay;
        QDate birth = this->getEntryDaybyType(true, BIRTH).day1;
        cDay = birth.addYears(GET_F_CAL(entryId));
        date.setDate1(cDay);
        return date;
    }
    return Date();
}

Entry PersonData::ientry(ENTRY type)
{
    for (int i=0 ; i<this->entry.size() ; i++) {
        if ( entry.at(i).type == type ) {
            return entry.at(i);
            break;
        }
    }
    return Entry();
}

Entry PersonData::fentry(ENTRYf type)
{
    for (int i=0 ; i<FAMI2(this->getFamS()).entry.size() ; i++)
        if (FAMI2(this->getFamS()).entry.at(i).type == type) return FAMI2(this->getFamS()).entry.at(i);
    return Entry();
}

void PersonData::setRin()
{
    this->rin = INDI2(parent->indiLastUsed-1).rin + 1 ;
}

Date PersonData::getEstimatedBirth()
{
    bool foo = false;
    Date out;
    do {
        foo = false;
        out = getEstimatedBirth(&foo);
    } while (foo);
    return out;
}

Date PersonData::getEstimatedBirth(bool *possiblyIncomplete)
{
    // lasketaan BIRTH lapsen estimatedbirth arvosta.

    if (this->estimatedBirth.known && !this->incompleteBirthEvaluation) return this->estimatedBirth;
    int j;
    QList<quint16> childs;
    QList<quint16> spouses;
    Date minMax;

    Date out;
    getChilds(&childs, false);
    *possiblyIncomplete = false;

    minMax.day1 = GET_ALLMINDAY; minMax.day1explanation = "earliest allowed day";
    minMax.day2 = QDate::currentDate(); minMax.day2explanation = "latest allowed day";
    // -------------------------------------------------------------------------------------------------
    // Lasketaan mahdollisten lasten syntymäajasta tämän henkilön arvioitu syntymäaika.
    // -------------------------------------------------------------------------------------------------
    Date childBirth;

    quint16 foo=0; // jos haluaa tarkastella miten estimated birth lasketaan, nakkaa tähän id numero niin stdoutista löytää tietoo
    int parentChildMin = 0;
    int parentChildMax = 0;


if (this->id == foo) qDebug() << "--------------------" << INDI(id).nameFamily << INDI(id).nameFirst;
if (this->id == foo) qDebug() << "getestimatedbirth   " << this->id << minMax.day1 << minMax.day2;

    for (j=0 ; j<childs.size() ; j++) {
        if (INDI2(childs.at(j)).estimatedBirth.known) {
            Date d;

            if (INDI2(id).sex == FEMALE) {
                parentChildMin = GET_AGE_MOTHERCHILD_MIN;
                parentChildMax = GET_AGE_MOTHERCHILD_MAX;
            }

            if (INDI2(id).sex == MALE) {
                parentChildMin = GET_AGE_FATHERCHILD_MIN;
                parentChildMax = GET_AGE_FATHERCHILD_MAX;
            }

            // etsitään henkilön id lasten syntymäpäivien pienin ja suurin arvo
            if ( (childBirth.day1 > INDI2(childs.at(j)).estimatedBirth.day1) || d.day1.isNull() ) {
                d.day1 = INDI2(childs.at(j)).estimatedBirth.day1;
                d.day1explanation = "Child " + INDI2(childs.at(j)).nameFirst + " birth";
            }
            if ( (childBirth.day2 < INDI2(childs.at(j)).estimatedBirth.day2) || d.day2.isNull() ) {
                d.day2 = INDI2(childs.at(j)).estimatedBirth.day2;
                d.day2explanation = "Child " + INDI2(childs.at(j)).nameFirst + " birth";
            }
            if (childBirth.day1.isNull()) {
                childBirth.day1 = d.day1;
                childBirth.day1explanation = d.day1explanation;
            } else if (childBirth.day1 > d.day1) {
                childBirth.day1 = d.day1;
                childBirth.day1explanation = d.day1explanation;
            }
            //    childBirth.day1 = MIN(childBirth.day1, d.day1);

            if (childBirth.day2.isNull()) {
                childBirth.day2 = d.day2;
                childBirth.day2explanation = d.day2explanation;
            } else if (childBirth.day2 < d.day2) {
                childBirth.day2 = d.day2;
                childBirth.day2explanation = d.day2explanation;
            }
             //   else childBirth.day2 = MAX(childBirth.day2, d.day2);

            if (this->id == foo) qDebug() << childBirth.day1 << childBirth.day2;
            childBirth.day1 = childBirth.day1.addYears(-parentChildMax);
            childBirth.day2 = childBirth.day2.addYears(-parentChildMin);
            if (this->id == foo) qDebug() << parentChildMin << parentChildMax;
            if (this->id == foo) qDebug() << "child " << childs.at(j) << INDI2(childs.at(j)).estimatedBirth.day1 << INDI2(childs.at(j)).estimatedBirth.day2 << childBirth.day1 << childBirth.day2;
            // lapsen estimatedBirth ei tiedetä
        } else *possiblyIncomplete = true;

        if (!*possiblyIncomplete) {
            if (!childBirth.day1.isNull()) if (childBirth.day1 > minMax.day1) {
                minMax.day1 = childBirth.day1;
                minMax.day1explanation = childBirth.day1explanation + " oldest age to become parent (" + QString::number(parentChildMax) +" years)";
                //minMax.day1 = MAX(minMax.day1, childBirth.day1);
            }
            if (!childBirth.day2.isNull()) if (childBirth.day2 < minMax.day2) {
                minMax.day2 = childBirth.day2;
                minMax.day2explanation = childBirth.day2explanation + " youngest age to become parent (" + QString::number(parentChildMin) +" years)";
                //minMax.day2 = MIN(minMax.day2, childBirth.day2);
            }
        }
    }

if (this->id == foo) qDebug() << "childs " << minMax.day1 << minMax.day2 << *possiblyIncomplete;

    // -------------------------------------------------------------------------------------------------
    // jos henkilöllä on puoliso(ita), lasketaan puolisoiden syntymäajan perusteella lisärajoituksia
    // tämän henkilön mahdolliseksi syntymäajaksi
    // -------------------------------------------------------------------------------------------------

    Date spouseBirth;

    this->spouses(&spouses);
    for (j=0 ; j<spouses.size() ; j++) {
        if (INDI2(spouses.at(j)).estimatedBirth.known && spouses.at(j)) {
            Date d;

            if (spouseBirth.day1 > INDI2(spouses.at(j)).estimatedBirth.day1 || d.day1.isNull()) {
                d.day1 = INDI2(spouses.at(j)).estimatedBirth.day1;
                d.day1explanation = "Spouse " + INDI2(spouses.at(j)).nameFirst + " birth (earliest estimation)";
            }
            if (spouseBirth.day2 < INDI2(spouses.at(j)).estimatedBirth.day2 || d.day2.isNull()) {
                d.day2 = INDI2(spouses.at(j)).estimatedBirth.day2;
                d.day2explanation = "Spouse " + INDI2(spouses.at(j)).nameFirst + " birth (latest estimation)";

            }

            if (spouseBirth.day1.isNull() || (d.day1 < spouseBirth.day1)) {
                spouseBirth.day1 = d.day1;
                spouseBirth.day1explanation = d.day1explanation;
            }

            if (spouseBirth.day2.isNull() || (d.day2 < spouseBirth.day2) ) {
                spouseBirth.day2 = d.day2;
                spouseBirth.day2explanation = d.day2explanation;
            }
        } else {
            if (spouses.at(j)) *possiblyIncomplete = true; // joskus puolisotieto voi olla indi(0), niistä ei välitetä

        }
    } // pitää varmaan etsiä max ja min arvot kaikista puolisoista
if (this->id == foo) qDebug() << "spouse " << spouseBirth.day1 << spouseBirth.day2;
    if (!spouseBirth.day1.isNull()) {
        spouseBirth.day1 = spouseBirth.day1.addYears(- GET_AGE_SPOUSES_MAX);
        if (!spouseBirth.day1.isNull()) if (spouseBirth.day1 > minMax.day1) {
            minMax.day1 = spouseBirth.day1;
            minMax.day1explanation = spouseBirth.day1explanation + " - max spouse age difference (" + QString::number(GET_AGE_SPOUSES_MAX) + ")";
        }
    }
    if (!spouseBirth.day2.isNull()) {
        spouseBirth.day2 = spouseBirth.day2.addYears(GET_AGE_SPOUSES_MAX);
        if (!spouseBirth.day2.isNull()) if (spouseBirth.day2 < minMax.day2) {
            minMax.day2 = spouseBirth.day2;
            minMax.day2explanation = spouseBirth.day2explanation + " + max spouse age difference (" + QString::number(GET_AGE_SPOUSES_MAX) + ")";
        }
    }

    // -------------------------------------------------------------------------------------------------
    // katsellaan isin ja äitin syntymäaikaa vastaavasti
    // -------------------------------------------------------------------------------------------------
    if (this->getMother() != 0 && INDI2(this->getMother()).estimatedBirth.known) {
        QDate motherMin = INDI2(this->getMother()).estimatedBirth.day1.addYears(GET_AGE_MOTHERCHILD_MIN);
        QDate motherMax = INDI2(this->getMother()).estimatedBirth.day2.addYears(GET_AGE_MOTHERCHILD_MAX);

        if (motherMin > minMax.day1) {
            minMax.day1 = motherMin;
            minMax.day1explanation = "Mother " + INDI2(this->getMother()).nameFirst + " birth (earliest estimation)";
        } // minMax.day1 = MAX(minMax.day1, motherMin );

        if (motherMax < minMax.day2) {
            minMax.day2 = motherMax;
            minMax.day2explanation = "Mother " + INDI2(this->getMother()).nameFirst + " birth (latest estimation)";
        } //  minMax.day2 = MIN(minMax.day2, motherMax );
    }
    if (this->getMother() != 0 && !INDI2(this->getMother()).estimatedBirth.known) *possiblyIncomplete = true;

    if (this->getFather() != 0 && INDI2(this->getFather()).estimatedBirth.known) {
        QDate fatherMin = INDI2(this->getFather()).estimatedBirth.day1.addYears(GET_AGE_FATHERCHILD_MIN);
        QDate fatherMax = INDI2(this->getFather()).estimatedBirth.day2.addYears(GET_AGE_FATHERCHILD_MAX);

        if (fatherMin > minMax.day1) {
            minMax.day1 = fatherMin;
            minMax.day1explanation = "Father " + INDI2(this->getFather()).nameFirst + " birth (earliest estimation)";
        } //minMax.day1 = MAX(minMax.day1, fatherMin );

        if (fatherMax < minMax.day2) {
            minMax.day2 = fatherMax;
            minMax.day2explanation = "Father " + INDI2(this->getFather()).nameFirst + " birth (latest estimation)";
        } //minMax.day2 = MIN(minMax.day2, fatherMax );
    }
    if (this->getFather() != 0 && !INDI2(this->getFather()).estimatedBirth.known) *possiblyIncomplete = true;

if (this->id == foo) qDebug() << "m & f birth         " << minMax.day1 << minMax.day2 << *possiblyIncomplete;
    // -------------------------------------------------------------------------------------------------
    // Tutkitaan tämän henkilön muut eventit joille on annettu explisiittinen päiväys
    // -------------------------------------------------------------------------------------------------

    Date entryDate;

    for (int i=0 ; i<this->entry.size() ; i++) {
        Entry entry = this->entry.at(i);

        if ( !entry.day.day1.isNull() && entry.type != BIRTH && ( entry.day.getAcc() != NA && entry.day.getAcc() != CAL) ) {            

            QDate dateMin = entry.day.day1.addYears(- GET_I_MAX( entry.type ) );
            QDate dateMax = entry.day.day2.addYears(- GET_I_MIN( entry.type ) );

            if (entryDate.day1 <= dateMin || entryDate.day1.isNull() ) {
                entryDate.day1 = dateMin;
                entryDate.day1explanation = "This person event : " + entry.typeString() + " (earliest estimation)";
            }
            if (entryDate.day2 >= dateMax || entryDate.day2.isNull() ) {
                entryDate.day2 = dateMax;
                entryDate.day2explanation =" This person event : " + entry.typeString() + " (latest estimation)";
            }
        }
    }

    if (!entryDate.day1.isNull()) if (entryDate.day1 > minMax.day1) {
        minMax.day1 = entryDate.day1;
        minMax.day1explanation = entryDate.day1explanation;
    } // minMax.day1 = MAX(minMax.day1, entryDate.day1);
    if (!entryDate.day2.isNull()) if (entryDate.day2 < minMax.day2) {
        minMax.day2 = entryDate.day2;
        minMax.day2explanation = entryDate.day2explanation;
    } // minMax.day2 = MIN(minMax.day2, entryDate.day2);
if (this->id == foo) qDebug() << "other events        " << minMax.day1 << minMax.day2 << *possiblyIncomplete;
    // -------------------------------------------------------------------------------------------------
    // isän ja äidin kuolema rajoittaa syntymää
    // -------------------------------------------------------------------------------------------------

    QDate motherDeath = QDate();
    QDate fatherDeath = QDate();

    if (this->getMother()) motherDeath = INDI2(this->getMother()).getEstimatedDeath();

    if (this->getFather()) if (!INDI2(this->getFather()).getEstimatedDeath().isNull())
            fatherDeath = INDI2(this->getFather()).getEstimatedDeath().addDays(PREGNANCY_PERIOD);

    if (!motherDeath.isNull()) if (motherDeath < minMax.day2) {
        minMax.day2 = motherDeath;
        minMax.day2explanation = "Mother " + INDI2(this->getMother()).nameFirst + "death";
    }
    if (!fatherDeath.isNull()) if (fatherDeath < minMax.day2) {
        minMax.day2 = fatherDeath;
        minMax.day2explanation = "Father " + INDI2(this->getFather()).nameFirst + "death";
    }
if (this->id == foo) qDebug() << "m and f death2      " << minMax.day1 << minMax.day2 << *possiblyIncomplete << motherDeath << fatherDeath;

//qDebug() << "qwer" << minMax.day1 << minMax.day1explanation << minMax.day2 << minMax.day2explanation;

    {
        if (this->descentantSteps == 255) this->descentantSteps = this->howManyDescentantSteps();
        QDate descStepDate = QDate(
                    QDate::currentDate().year() - this->descentantSteps * GET_AGE_FATHERCHILD_MIN,
                    QDate::currentDate().month(),
                    QDate::currentDate().day());

        if (descStepDate < minMax.day2) {
            //qDebug() << "abc " << descStepDate << minMax.day2 << descentantSteps;
            minMax.day2 = descStepDate;
            minMax.day2explanation = QString::number(descentantSteps) + " generation steps of descendant";
        }
    }

    // -------------------------------------------------------------------------------------------------
    // Käydään läpi avioliittojen tapahtumat ja niistä mahdollisesti laskettavissa oleva syntymäarvio
    // -------------------------------------------------------------------------------------------------

    for ( int marr = 0 ; marr < famSList.size() ; marr++ ) {
        for ( int e = 0 ; e < FAMI2( famSList.at(marr)).entry.size() ; e++ ) {
            int type;
            Date marrDate;

            Date date = FAMI2(famSList.at(marr)).entry.at(e).day;
            type = FAMI2(famSList.at(marr)).entry.at(e).type;

            marrDate.day1 = date.day1.addYears(- GET_F_MAX(type) );
            marrDate.day2 = date.day1.addYears(- GET_F_MIN(type) );

            if (!marrDate.day1.isNull()) if (marrDate.day1 > minMax.day1) {
                minMax.day1 = marrDate.day1;
                minMax.day1explanation = "Marriage event XXX";
            }
            if (!marrDate.day2.isNull())  if (marrDate.day2 < minMax.day2) {
                minMax.day2 = marrDate.day2;
                minMax.day2explanation = "Marriage event XXX";
            }

        }
    }

    // -------------------------------------------------------------------------------------------------
    // Tarkastetaan vielä, onko birth arvo oikeasti annettu. Jos oikea birth arvo onkin lasketun ulkopuolelle
    // muutetaan laskettuja birth arvoja niin että oikea mahtuu lasketun sisään
    // -------------------------------------------------------------------------------------------------

    Date birth = this->ientry(BIRTH).day;

    if (birth.known) {
        if (birth.day1 < minMax.day1) {
            minMax.day1 = birth.day1;
            minMax.day1explanation = "Own birth";
        }
        if ( birth.day2 > minMax.day2) {
            minMax.day2 = birth.day2;
            minMax.day2explanation = "Own birth";
        }
    }

if (this->id == foo) qDebug() << "real birth conflict " << minMax.day1 << minMax.day1explanation << minMax.day2 << minMax.day2explanation << *possiblyIncomplete;

    if (minMax.day2.isNull()) {
        minMax.day2 = minMax.day1;
        minMax.day2explanation = minMax.day1explanation;
    }
    out.setDate1(minMax.day1); out.day1explanation = minMax.day1explanation;
    out.setDate2(minMax.day2); out.day2explanation = minMax.day2explanation;

if (this->id == foo) qDebug() << "viimeistely         " << minMax.day1 << minMax.day2 << *possiblyIncomplete;
    if (!minMax.day1.isNull() && !minMax.day2.isNull()) {
        out.setAcc(FRO);
        out.known = true;
    } else {
        out.known = false;
    }

    if (out.day1.isNull() && out.day2.isNull()) {
        out.day1 = GET_ALLMINDAY;
        out.day2 = GET_ALLMAXDAY;
        out.day1explanation = "min";
        out.day2explanation = "max";
    }

    // tässä vaiheessa allMinDay arvoja käytettiin vain referenssiarvoina jotta voidaan tehdä
    // järkeviä vertauksia. Jos seuraava ehto on tosi, ei tämän henkilön syntymäaikaa voitu
    // arvioida isän, äidin, lasten eikä puolisoiden syntymäajasta. On mahdollista, että jos
    // otetaan (demonissa) uusi iterointikierros, joku edellämainituista saa arviosyntymäajan
    // jolloin tämäkin voidaan arvioida. Asetetaan siksi known arvo falseksi, jotta demon tietää
    // että iteroimista pitää jatkaa. Iterointikierroksen päätteeksi demon asettaa sitten
    // allminmax arvot niille henkilöille, joiden syntymäaikaa ei voitu arvioida.
    if (minMax.day1 == GET_ALLMINDAY && minMax.day2 == GET_ALLMAXDAY) {
        out.known = false;
        *possiblyIncomplete = true;
    }

    incompleteBirthEvaluation = *possiblyIncomplete;

    if (this->id == foo) qDebug() << out.day1 << out.day2 << *possiblyIncomplete;

    return out;
}

QDate PersonData::getCalBirth()
{
    QDate min, max;
    min = this->estimatedBirth.day1;
    max = this->estimatedBirth.day2;
    return min.addDays(min.daysTo(max)/2);
}

QDate PersonData::getEstimatedDeath()
{
    // tämä pitää tehdä käyttämättä eventPerdiod metodia, koska siitä seuraa päättymätön loop
    QDate death = this->ientry(DEATH).date1();
    QDate buried = this->ientry(BURIED).date1();
    QDate crem = this->ientry(CREM).date1();

    QDate estDeath;

    if (!death.isNull()) estDeath = death;
    if (!buried.isNull()) estDeath = buried;
    if (!crem.isNull()) estDeath = crem;

    if (estDeath.isNull() && (estimatedBirth.day1 == GET_ALLMINDAY) ) estDeath = GET_ALLMAXDAY;
    else
    if (estDeath.isNull() && !estimatedBirth.day1.isNull()) {
        estDeath = estimatedBirth.day2.addYears(GET_I_MAX(DEATH));
    }

    return estDeath;
}

PersonData::PersonData()
{    
    clear(this->parent);
    this->id = GENE.indiCount;
    GENE.indiCount++;
    //qDebug() << "pd const" << GENE.indiCount << GeneData::currentGd;
}

PersonData::~PersonData()
{
    //qDebug() << "pd des";
    parent->indiCount--;
    famSList.clear();
    famsTags.clear();
    submitters.clear();
    entry.clear();
    submTags.clear();
    multiMedia.clear();
}

