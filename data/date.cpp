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

#include "data/date.h"
#include "data/setupvalues.h"
#include "macro.h"
#include "entry.h"
#include <QSettings>
#include "data/genedata.h"

extern QSettings    sets;
extern GeneData        * geneData;

QString Date::shortMonthName(int date)
{
    switch (date) {
        case 0: return ""; break;
        case 1: return "JAN"; break;
        case 2: return "FEB"; break;
        case 3: return "MAR"; break;
        case 4: return "APR"; break;
        case 5: return "MAY"; break;
        case 6: return "JUN"; break;
        case 7: return "JUL"; break;
        case 8: return "AUG"; break;
        case 9: return "SEP"; break;
        case 10: return "OCT"; break;
        case 11: return "NOV"; break;
        case 12: return "DEC"; break;
        return "month-err";
    }
    return "month-err";
}

QString Date::month1ShortName()
{
    return this->shortMonthName(this->day1.month());
}

QString Date::month2ShortName()
{
    return this->shortMonthName(this->day2.month());
}

QDate Date::roundInaccurateDate(Date date, bool toBeginIsFalseOrEndIsTrue)
{
    // tätä tarvitaan ainakin kun lasketaan sallittuja eventtien min ja max aikoja.
    if (date.accuracyString() == "yyyy") {
        if (toBeginIsFalseOrEndIsTrue) {
            date.changeMonthsTo(12);
            date.changeDaysTo(31);
        } else {
            date.changeMonthsTo(1);
            date.changeDaysTo(1);
        }
    }

    if (date.accuracyString() == "MM.yyyy") {
        if (toBeginIsFalseOrEndIsTrue) {
            date.changeDaysTo(31);
        } else {
            date.changeDaysTo(1);
        }
    }

    if (toBeginIsFalseOrEndIsTrue) return date.day2; else return date.day1;
}


void Date::changeMonthsTo(int month)
{    
    if (!day1.isNull()) day1.setDate(day1.year(), month, day1.day());
    if (!day2.isNull()) day2.setDate(day2.year(), month, day2.day());
}

void Date::changeDaysTo(int date)
{
    if (!day1.isNull()) day1.setDate(day1.year(), day1.month(), date);
    if (!day2.isNull()) day2.setDate(day2.year(), day2.month(), date);
}

Date Date::getSmallestKnown(Date date1, Date date2, Date date3)
{
    if (date1.known && date2.known && date3.known) {
        if ( (date1 < date2) && (date1 < date3) ) return date1;
        if ( (date2 < date1) && (date2 < date3) ) return date2;
        if ( (date3 < date1) && (date3 < date2) ) return date3;
    }
    if (date1.known && date2.known && !date3.known) {
        if (date1 < date2) return date1; else return date2;
    }
    if (date1.known && !date2.known && date3.known) {
        if (date1 < date3) return date1; else return date3;
    }
    if (!date1.known && date2.known && date3.known) {
        if (date2 < date3) return date2; else return date3;
    }

    if (date1.known) return date1;
    if (date2.known) return date2;
    if (date3.known) return date3;

    return Date();
}

bool Date::isOrder(Date date1, Date date2, Date date3, Date date4)
{
    if (!date4.known) return isOrder(date1, date2, date3);
    else
        if (!isOrder(date1, date2, date3)) return false;
    else
        return (date3 < date4);
    return false;
}

bool Date::isOrder(Date date1, Date date2, Date date3)
{
    if (date1.known && date2.known && date3.known) {

        return (date1 < date2) && (date2 < date3);
    }
    else
        if (date1.known && date2.known) return (date1 < date2);
    else
        if (date1.known && date3.known) return (date1 < date3);
    else
        if (date2.known && date3.known) return (date2 < date3);
    return false;
}

Date Date::addDays(int days)
{
    Date output;
    if (!this->day1.isNull()) output.day1 = this->day1.addDays(days);
    if (!this->day2.isNull()) output.day2 = this->day2.addDays(days);

    return output;
}

QString Date::accuracyString()
{
    return accuracyString(this->ac);
}

QString Date::accuracyString(ACCURACY acc)
{
    if (acc == ABT) return GET_ACC_ABT;
    if (acc == CAL) return GET_ACC_CAL;
    if (acc == EST) return GET_ACC_EST;
    if (acc == BEF) return GET_ACC_BEF;
    if (acc == AFT) return GET_ACC_AFT;
    if (acc == EXA) return GET_ACC_EXA;
    if (acc == FRO) return GET_ACC_FRO;
    return "dd.MM.yyyy";
}

QString Date::accuracyGedCom()
{
    return this->accuracyGedCom(this->ac);
}

QString Date::accuracyGedCom(ACCURACY acc)
{
    switch ((int)acc) {
        case 0 : return "NA"; break;
        case 1 : return "ABT"; break;
        case 2 : return "CAL"; break;
        case 3 : return "EST"; break;
        case 4 : return "BEF"; break;
        case 5 : return "AFT"; break;
        case 6 : return "EXA"; break;
        case 7 : return "FROM"; break;
    }
    return "";
}
/*
ACCURACY Date::accuracyFromInt(int acc)
{
    switch (acc) {
        case 0 : return NA; break;
        case 1 : return ABT; break;
        case 2 : return CAL; break;
        case 3 : return EST; break;
        case 4 : return BEF; break;
        case 5 : return AFT; break;
        case 6 : return EXA; break;
        case 7 : return FRO; break;
    }
    return NA;
}
*/
int Date::accuracyFromGedCom(QString gedcomacc)
{
    if (gedcomacc == "NA") return 0;
    if (gedcomacc == "ABT") return 1;
    if (gedcomacc == "CAL") return 2;
    if (gedcomacc == "EST") return 3;
    if (gedcomacc == "BEF") return 4;
    if (gedcomacc == "AFT") return 5;
    if (gedcomacc == "EXA") return 6;
    if (gedcomacc == "FROM") return 7;
    return -1;
}

/*
 this  = 2000 ... 2003
 ad    = 2001 (.. 2001) this > ad = true, because 2003 > 2001.

 this  = 2001 (.. 2001)
 ad    = 2001 ... 2003  this > ad = true, because 2003 > 2001.

 this  = 1891
 ad    = 1888 ... 1945  this > ad = true, because 1945 > 1891

 */

bool Date::operator >(Date anotDate)
{

    /* palauttaa tosi jos anotherDay on todistettavasti myï¿½hemmin kuin tï¿½mï¿½ pï¿½ivï¿½.
       Eli jos tï¿½mï¿½ on ABT 2000 ja anotherDay on ABT2000, palautetaan false
       Tï¿½mï¿½ = 1.1.2000, anotherDay ABT 2000, palautetaan false
       */


    // -----------------------------------------------------------------------
    // thisDate    <===============>
    // anotDate                            <=============> anotDate on myöhemmin kuin thisDate

    // -----------------------------------------------------------------------
    // thisDate    <===============>
    // anotDate              <=============> anotDate ei ole myöhemmin kuin thisDate

    Date thisDate = *this;

    if (!anotDate.known) return false;

    quint8 compAcc = 0;
    if (( thisDate.accuracyString() == "yyyy") || (anotDate.accuracyString() == "yyyy")) compAcc = 1;
    if (( thisDate.accuracyString() == "MM.yyyy") || (anotDate.accuracyString() == "MM.yyyy")) compAcc = 2;
    if (!compAcc) compAcc = 3;

    if (thisDate.day2.isNull()) thisDate.day2 = thisDate.day1;
    if (anotDate.day2.isNull()) anotDate.day2 = anotDate.day1;

    if (compAcc == 1) {
        if (anotDate.day2.year() < thisDate.day1.year()) return true;
        return false;
    }

    if (compAcc == 2) {
        // esim 1912-01 vs. 1908-03
        if (anotDate.day2.year() < thisDate.day1.year()) return true;
        if (anotDate.day2.year() > thisDate.day1.year()) return false;
        if (anotDate.day2.month() < thisDate.day1.month()) return true;
        return false;
    }

    if (compAcc == 3) {
        if (anotDate.day2 < thisDate.day1) return true;
        return false;
    }

    qDebug() << "date comparison (>) problem (2) with " << thisDate << anotDate;
    return false;
/*
    if (thisDate.accuracyString() == "yyyy" || anotDate.accuracyString() == "yyyy") {
        if (thisDate.day2.year() > anotDate.day1.year()) return true;

        return false;
    }

    if (anotDate.day2 < thisDate.day1) return true;

    return false;
*/
}

bool Date::operator <(Date anotDate)
{
    if (!anotDate.known) return false;

    Date thisDate = *this;
//qDebug() << "<" << thisDate << anotDate;
    quint8 compAcc = 0;
    if (( thisDate.accuracyString() == "yyyy") || (anotDate.accuracyString() == "yyyy")) compAcc = 1;
    if (( thisDate.accuracyString() == "MM.yyyy") || (anotDate.accuracyString() == "MM.yyyy")) compAcc = 2;
    if (!compAcc) compAcc = 3;

    if (thisDate.day2.isNull()) thisDate.day2 = thisDate.day1;
    if (anotDate.day2.isNull()) anotDate.day2 = anotDate.day1;

    if (compAcc == 1) {
        if (thisDate.day2.year() < anotDate.day1.year()) return true;
        return false;
    }

    if (compAcc == 2) {
        // esim 1912-01 vs. 1908-03
        if (thisDate.day2.year() < anotDate.day1.year()) return true;
        if (thisDate.day2.year() > anotDate.day1.year()) return false;
        if (thisDate.day2.month() < anotDate.day1.month()) return true;
        return false;
    }

    if (compAcc == 3) {
        if (thisDate.day2 < anotDate.day1) return true;
        return false;
    }

    qDebug() << "date comparison (<) problem (2) with " << thisDate << anotDate;
    return false;

/*


    if (thisDate.accuracyString() == "yyyy" || anotDate.accuracyString() == "yyyy") {
        if (thisDate.day1.year() < anotDate.day2.year()) return true;

        return false;
    }

    if (thisDate.day2 < anotDate.day1) return true;

    return false;
*/
}

bool Date::operator !=(Date anotDate)
{
    Date thisDate = *this;

    if (!thisDate.known || !anotDate.known) return false;

    if (thisDate.day2.isNull()) thisDate.day2 = thisDate.day1;
    if (anotDate.day2.isNull()) anotDate.day2 = anotDate.day1;

    if (thisDate.accuracyString() == "yyyy" || anotDate.accuracyString() == "yyyy") {
        if (thisDate.day1.year() == anotDate.day2.year()) return false;
        if (thisDate.day1.year() == anotDate.day1.year()) return false;
        if (thisDate.day2.year() == anotDate.day2.year()) return false;
    }

    if ( (thisDate.day1 > anotDate.day2) || (thisDate.day2 < anotDate.day1) ) return true;

    return false;
}

void Date::setAcc(ACCURACY acc)
{
    if (acc != NA) known = true;
    ac = acc;
}

ACCURACY Date::getAcc()
{
    return this->ac;
}

PRIVACYMETHOD Date::whichAutoPrivacyMethodApplies()
{
    // Jos käyttäjä on asettanut, ettei halua autopolicyä käyttää, niin palautetaan privacdy-show
    // Autopolicy on siis se, että iän perusteella päätetään mitä tehdään
    if ( GENE.printPolicy == PRINTPOLICY_1_EVERYTHING ) return PRIVACY_0_SHOW;
    if ( GENE.printPolicy == PRINTPOLICY_2_GENE) return PRIVACY_0_SHOW;

    // autoprivacymethodia käytetään vain PRIVACYPOLICY_3_PUBLIC:ssa

    // tämä palauttaa siis policyn, eli numeron 0...3, ei metodia. 0 tarkoittaa, että mitään policyä ei tarvi noudattaa

    int age1, age2, age3;

    QDate date;
    if (this->day2.isNull()) date = this->day1; else date = this->day2;

    quint16 ageOfDate = QDate::currentDate().year() - date.year(); // tapahtuman ikä vuosissa

    // 100  3
    // 50   2
    // 20   1

    // ikä = 10
    // 100- 10 > 0
    // 50 - 10 > 0
    // 20 - 10 > 0 => show
    age1 = s_privacyMethodAutoAge(1) - ageOfDate;
    age2 = s_privacyMethodAutoAge(2) - ageOfDate;
    age3 = s_privacyMethodAutoAge(3) - ageOfDate;

    int value = -1;

    if ((age1<=0) && (age2<=0) && (age3<=0)) return PRIVACY_0_SHOW;
    if ((age1>=0) && (age2<=0) && (age3<=0)) value = 1;
    if ((age1>=0) && (age2>=0) && (age3<=0)) value = 2;
    if ((age1>=0) && (age2>=0) && (age3>=0)) value = 3;

    //qDebug() << date << ageOfDate << age1 << age2 << age3 << value;


    if (value != -1) return (PRIVACYMETHOD)s_privacyMethodAuto(value);

    qDebug() << "virhe Date::whichAutMethodPolicyApplies." << date << ageOfDate << age1 << age2 << age3;
    return PRIVACY_0_SHOW;
}

QString Date::dayNameOfWeek()
{
    if (this->getAcc() == EXA) return QDate::shortDayName(day1.dayOfWeek());
    return "";
}

QString Date::toString()
{
    return toString(false);
}

QString Date::toString(bool addPrefix)
{
    return dts(addPrefix);
}

QString Date::dts(bool addPrefix)
{

    if (ac == NA || !known )        return "";
    if (ac == ABT && addPrefix)     return "~" +    day1.toString(GET_ACC_ABT);
    if (ac == ABT && !addPrefix)    return          day1.toString(GET_ACC_ABT);
    if (ac == CAL )                 return "#" +    day1.toString(GET_ACC_CAL);
    if (ac == EST && addPrefix)     return "~" +    day1.toString(GET_ACC_EST);
    if (ac == EST && !addPrefix)    return          day1.toString(GET_ACC_EST);
    if (ac == BEF && addPrefix)     return "=>" +   day1.toString(GET_ACC_BEF);
    if (ac == BEF && !addPrefix)    return          day1.toString(GET_ACC_BEF);
    if (ac == AFT && addPrefix)     return          day1.toString(GET_ACC_AFT) + "=>";
    if (ac == AFT && !addPrefix)    return          day1.toString(GET_ACC_AFT);
    if (ac == EXA) return day1.toString(GET_ACC_EXA);
    if (ac == FRO) return this->day1.toString("yyyy") + "=>" + this->day2.toString("yyyy");
    //if (ac == FRO) return this->day1.toString("dd.MM.yyyy") + "=>" + this->day2.toString("dd.MM.yyyy");
    return "";
}

void Date::clear()
{
    day1 = QDate();
    day2 = QDate();
    ac = NA;
    known = false;
}

Date::Date()
{    
    clear();
}

void Date::setDate1(QDate date)
{
    day1.setDate(date.year(), date.month(), date.day());
    known = true;
}

void Date::setDate2(QDate date)
{
    this->setDate2(date.year(), date.month(), date.day());
    known = true;
}

void Date::setDate1(int y, int m, int d)
{

    day1.setDate(y, m, d);
    known = true;
}

void Date::setDate2(int y, int m, int d)
{
    day2.setDate(y, m, d);
    known = true;
}

void Date::setDates(Date date)
{
    day1.setDate(date.day1.year(), date.day1.month(), date.day1.day());
    day2.setDate(date.day2.year(), date.day2.month(), date.day2.day());
    ac = date.getAcc();
    known = true;
}

Date::Date(QDate date1, ACCURACY acc)
{
    setDate1(date1);
    setDate2(date1);
    setAcc(acc);
}

Date::Date(QDate date1, QDate date2)
{
    setDate1(date1);
    setDate2(date2);
    setAcc(FRO);
}
