#ifndef ENTRY_H
#define ENTRY_H

#include "data/date.h"
#include "data/multimedia.h"
#include "data/address.h"
#include "data/note.h"
#include "data/source.h"
#include "macro.h"

enum ENTRY { NOTYPE, BIRTH, CHRISTENING, CONFIRMATION, DEATH, EMIGRATION,
             IMMIGRATION, USEREVENT, ICENSUS, WILL, GRADUATION, BURIED, CREM,
             ADOPTATION, RETI, PROB, NATU, BLESSING, OCCUPATION, EDUCATION,
             NATIONALITY, TITLE, PROPERTY, RELIGION, RESIDENCE, SOCIAL_SECURITY_NRO,
             CAST, DSCR, NCHI, NMR, IDNO };
enum ENTRYf { NO_FAMILY_EVENT, MARRIAGE, DIVORCE, ANNULMENT, CENSUS, DIVORCE_FILED, ENGAGEMENT, MARRIAGE_BANN, MARRIAGE_CONTRACT, MARRIAGE_LICENSE, MARRIAGE_SETTLEMENT, EVEN };

class Entry
{
    public:
        Entry();
        ~Entry();
        //QString     printResn();
        int         type;
        void        clear();
        bool        used();
        Date        day;
        QString     place, agency, cause, attrType, attrText;
        bool        entryLineYes; // in gedcom file there may be a "y" denoting "yes" for that event
        int         age;
        bool        indi;
        ACCURACY    getAcc();
        QDate       date1();
        QDate       date2();
        bool        publishable();

        QString     printDay();
        QString     printWeekday();
        QString     printValue();
        QString     printPlace();
        QString     printNote();
//        QString     printAge();
        QString     printColor();
        MultiMedia  multiMedia;
        Address     address;
        Source      source;
        Note        note;
        int         privacyPolicy;
        QString     typeString();
        static QString     typeString(int type, bool indi);
        QString     typeGedCom();
        static QString      typeGedCom(int type, bool indi);
        static int         gedComToInt(QString type, bool indi);
        void        clearEntryMoreShowFlags();

        bool                showAddress0Line;
        bool                showAddress1Line;
        bool                showAddress2Line;
        bool                showPostCityLine;
        bool                showstCnrPhoLine;
        bool                showSourcePgQltyLine;
        bool                showSourceTextLine;
        bool                showAddEntryMore;
        bool                showNotes;

        // n‰it‰ k‰ytet‰‰n vain pritatessa, ja lasketaan ennen printtaamista. Ei saisi siis k‰ytt‰‰ muualla.
        int     indiId;
        int     famiId;

        PRIVACYMETHOD effectivePrivacyMethod();

private:
        void        addPrintLine(QString *line, QString add);
        void        addPrintLine(QString *result, QString comment, QString add);
        QString     addMmCitation();
        QString     addAddressCitation();
        QString     addSourceCitation();

};

#endif
