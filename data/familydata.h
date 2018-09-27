#ifndef FAMILYDATA_H
#define FAMILYDATA_H

#include <QDate>
#include <QList>
#include <QDateTime>
#include "data/persondata.h"
#include "data/source.h"
#include "macro.h"

class Gedcom
{
    public:
        QString husb, wife;
        QList<QString> childs, submitters, subN;
        void clear();
};

class FamilyData
{   
    public:        
        FamilyData();
        ~FamilyData();

        //QString         printResn();
        bool            isLiving();
        GeneData        * parent;
        QString         printNote();
        bool            publishable();
        bool            eventsToPrint();
        QList<Entry>    entry;              // marriages, divorces etc...
        Gedcom          gedComTag;        
        quint16         husb, wife, nchi;   // husband, wife ID's and number of children
        QList<quint16>  childs;             // list of chils ID's
        QList<quint16>  subM;               // submitters
        QList<quint16>  subN;
        QList<Source>   sourCitations;        // sourccitations
        QDateTime       changed;
        QList<MultiMedia>  multiMedia;
        QString         refn, refnType, rin;
        Source          source;
        Note            note;
        void            clear(GeneData *parent);
        bool            deleted;
        Entry           fentry(ENTRYf type);
        quint16         id;
        quint8          privacyPolicy;
        void            arrangeEvents();
        quint8          isFEntry(ENTRYf type);
        PRIVACYMETHOD   effectivePrivacyMethod();
};


#endif // FAMILYDATA_H
