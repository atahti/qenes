#ifndef PERSONDATA_H
#define PERSONDATA_H

#include <QDate>
#include <QtDebug> // ota pois kun debuggausta ei tarvita
#include "data/source.h"
#include "data/entry.h"
#include "macro.h"

enum SEX { UNKNOWN, MALE, FEMALE };
enum ADOPTED_BY { NO_ADOP, HUSB_ADOP, WIFE_ADOP, BOTH_ADOP, YES_ADOP };
enum PEDI_TYPE { NO_PEDI, ADOPTED_PEDI, BIRTH_PEDI, FOSTER_PEDI, SEALING_PEDI };

class Famc
{
    // Child-family class.
    public:
        Famc();
        ~Famc();        
        QString     tag;
        quint16     value;
        void        clear();        
};

class GeneData;

class PersonData
{
    public:
        PersonData();
        PersonData(GeneData *parent);
        ~PersonData();
        bool        publishable();

        GeneData    * parent;
        bool        deleted;       // is set true, this person wont be saved => will actually be deleted.
        bool        selected;
        bool        selectedSystem;
        bool        eventsToPrint();
        QString      printName1();
        QString      printName2();
        QString      printName3();
        QString      printNameFamily();
        QString     printName123Family();
        QString     printNameFamily1();
        QString      printNote();
        void        eventPeriod(Date *answer, quint8 type, bool indi);
        QString     nameFirst, nameFamily, name2nd, name3rd;
        QString     familyRelation;
        Source      nameSource;
        Note        nameNote;
        quint16     currentSpouse;     // the number of currently selected spouse family order-id
        Note        note;
        SEX         sex;
        Famc        famc, famcSecondary;    // famcSecondary will be used in load, if - for some reason - the person has two child
                                        // families. That may happen if another family is e.g. adoption family. The load proceudre
                                        // will however combine the families in just one so that famcSecondary wont be used after
                                        // the load.
        PEDI_TYPE pedi;             // *!* ei käytössä
        QString     rfn, afn, refn, refnType;
        Source      source;
        QList<MultiMedia> multiMedia;        
        QDateTime   changed;
        ADOPTED_BY  adoptedBy;           // *!* this should go into adopted event...
        int         anci, desi, rin;
        quint16     getFamS(), getFather(), getMother(), getWife(), getHusb(); // functions to get persons or values
        quint16     getSpouse();
        quint16     getSpouse(quint16 id);
        QString     getAge();
        QString     getAge(Date when);
        QString     getPrintAge(Date when);
        void        setRin();
        bool        isParent(quint16 adult), isChild(quint16 child);
        void        setMother(quint16 mother), setFather(quint16 father), addFamS(quint16 fams), setChild (quint16 child);
        Date        getEstimatedBirth(bool *possiblyIncomplete);
        Date        getEstimatedBirth();
        QDate       getEstimatedDeath();
        QDate       getCalBirth();
        //QDate       getCalBirth(int deltaYear);
        bool        isLiving();
        Date        getEntryDay(bool indi, quint8 entryId);           // gives the entry day, calculate it if accuracy = ACC
        Date        getEntryDaybyType(bool indi, quint8 type);        // find the entry of type 'type', and do the 'entryDay'
        Date        estimatedBirth, estimatedDeath;

        Entry       ientry(ENTRY type);
        quint8      isIEntry(ENTRY type);
        Entry       fentry(ENTRYf type);

        void        getChilds(QList<quint16> *childs, bool currentSpouseOnly);
        void        getSiblings(QList<quint16> *childs, bool currentSpouseOnly);
        void        getCousins(QList<quint16> *childs, bool currentSpouseOnly);
        void        makeNameList(QStringList *nameList, QList<quint16> *list);
        void        spouses(QList<quint16> *spo);
        int         getMotherInLaw();

        //void siblings(QList<int> *siblingList);           // tällä hetkellä ei tarvita
        void        deleteThis();

        void        setHusb(quint16 husb);
        void        setWife(quint16 wife);

        quint16     id;
        void        clear(GeneData *parent);

        void        assignFather(quint16 father);
        void        assignMother(quint16 mother);
        void        assignChildFromTo(quint16 child, quint16 oldFamc, quint16 newFamc);

        QList<quint16>  submitters;
        QList<quint16>  famSList;
        QList<quint16>  childIdList;

        void        selectAncestors();
        void        selectAncestorsAll();
        void        selectDescendants();
        void        selectNone();
        void        selectAll();

        QList<Entry> entry;
        QString     getShortFamilyRelation();
        QString     nameGedCom;
        QList<QString> submTags;
        QList<QString> famsTags;
        QString     anciTag, desiTag;
        QString     getFamsTag(quint16 famsID);
        quint16     getFamS(quint16 famsId);
        void        descendantStep();
        quint8      generation;
        bool        incompleteBirthEvaluation;
        quint8      privacyPolicy;
        PRIVACYMETHOD effectivePrivacyMethod();
        void        getLivingEventDays(QDate * minDayTmp, QDate * maxDayTmp);
        void        arrangeEvents();
        QString     printName(QString name);
        bool        isDirectAncestor;
        quint16     getNextAncestor();
        QString     getRootListName();
        bool        usedInRootList;
        quint8      howManyDescentantSteps();
        quint8      descentantSteps;

private:

        void        ancestorStep();
        //Date        limitDayTest(int limitType);
        void        eventMaxDay(quint8 type, bool indi, QDate * returnDate, QString * returnExplanation);
        void        eventMinDay(quint8 type, bool indi, QDate * returnDate, QString * returnExplanation);
        bool        knownDateIterator(int *type, QDate *date, bool roundInAccurateDays, QString *explanation);
        void        knownMaxDate(int *type, QDate *date, QString *explanation);
        void        knownMinDate(int *type, QDate *date, QString *explanation);
        QString     nameInitials();
};

#endif // PERSONDATA_H


