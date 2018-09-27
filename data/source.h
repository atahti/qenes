#ifndef SOURCE_H
#define SOURCE_H

#include <QString>
#include "data/multimedia.h"
#include "data/address.h"
#include "data/date.h"

enum ENTRYf_ROLE { NO_FAMILY_EVENT_ROLE, CHIL_ROLE, HUSB_ROLE, WIFE_ROLE, MOTH_ROLE, FATH_ROLE, SPOU_ROLE };
enum QUALITY { NO_QUAY, QUAY0, QUAY1, QUAY2, QUAY3 };

class SourceEvent
{
    public:
        QString type;
        ENTRYf_ROLE role;
        Date date;
        QString place;
        QString agency;
};

// this record is for source_record of gedcom
class Source
{
    public:
        Source();
        ~Source();

        QString         printQuality();
        int             id;
        QUALITY         quay;
        MultiMedia      multiMedia;
        Note            note;
        Address         address;
        SourceEvent     event;
        QString         tag;
        QString         page;
        QString         author;
        QString         title;
        QString         name;
        QString         copyright;
        QString         text;
        QString         version;
        QString         approvedSystemID;
        QString         corporation;
        QString         productName;
        QString         phone;
        QString         abbreviation;
        QString         publication;
        QString         gedComRepoID;
        QString         refn;
        QString         refnType;
        QString         rin;
        void            clear();
        bool            used();
        bool            deleted;
        bool            aboutToBeDeleted;
        QDateTime       changed;
        bool            usedBy(int *x, bool *indi);

        /*
        QString         getPage(int restriction);
        QString         getAuthor(int restriction);
        QString         getTitle(int restriction);
        QString         getName(int restriction);
        QString         getCopyright(int restriction);
        QString         getText(int restriction);
        QString         getVersion(int restriction);
        //QString         approvedSystemID;
        QString         getCorporation(int restriction);
        //QString         productName;
        QString         getPhone(int restriction);
        QString         getAbbreviation(int restriction);
        QString         getPublication(int restriction);
        */
};

// SourceRecord is a class for Gedcom's source_record, and Source is a class for holding source_records data
// AND for individual source citations. SourceRecord has a counter for counting source_records, so its not
// possible to combine SourceRecord and Source because use of citations would mess the record counter.
class SourceRecord : public Source
{
    public:
        SourceRecord();
        ~SourceRecord();
        void copyDataFrom(Source s);
};

#endif // SOURCE_H
