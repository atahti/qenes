#ifndef DATE_H
#define DATE_H

#include "macro.h"
#include <QString>
#include <QDate>
#include <QDebug>

enum ACCURACY { NA, ABT, CAL, EST, BEF, AFT, EXA, FRO };

class Date
{
    private:
        QString dts(bool addPrefix);

    public:
        ACCURACY ac;
        Date();
        Date(QDate date1, ACCURACY acc);
        Date(QDate date1, QDate date2);
        QString         shortMonthName(int date);
        QString         month1ShortName();
        QString         month2ShortName();
        void            setAcc(ACCURACY acc);              // Sets the accuracy (and set known = true)
        ACCURACY        getAcc();
        void            setDates(Date date);               // Sets the day (and known = true)
        void            setDate1(QDate date);
        void            setDate2(QDate date);
        void            setDate1(int y, int m, int d);
        void            setDate2(int y, int m, int d);
        QDate           day1;                               // the actual day or the lower limit of the period, used always if day is known
        QDate           day2;                               // the upper limit of the period, used only if date ACC = FRO
        bool            known;                              // Knows, if the day is known or not
        QString         toString(bool addPrefix);    // Returns the report-format day, with prefix (e.g. =>01.01.2000, ~01.0.2000)
        QString         toString();
        QString         dayNameOfWeek();                 // Mon, tue, wed...
        void            clear();                            // Clears...
        bool            isLaterThan(Date anotherDay);
        bool            operator>(Date anotherDay);
        bool            operator<(Date anotherDay);
        bool            operator!=(Date anotherDay);
        Date            addDays(int days);
        QString         accuracyString();
        static QString  accuracyString(ACCURACY acc);
        static  bool    isOrder(Date date1, Date date2, Date date3);
        static  bool    isOrder(Date date1, Date date2, Date date3, Date date4);
        static  Date    getSmallestKnown(Date date1, Date date2, Date date3);
        static QString  accuracyGedCom(ACCURACY acc);
        QString         accuracyGedCom();
        static int      accuracyFromGedCom(QString gedcomacc);
        void            changeMonthsTo(int month);
        void            changeDaysTo(int date);
        PRIVACYMETHOD   whichAutoPrivacyMethodApplies();
        static QDate    roundInaccurateDate(Date date, bool toBeginIsFalseOrEndIsTrue);
        QString         day1explanation, day2explanation;
};

inline QDebug operator<<(QDebug s, const Date &date)
{
    s.nospace() << date.day1 << date.day1explanation << "..." << date.day2 << date.day2explanation << date.ac;
    return s.space();
}

#endif // DATE_H
