#ifndef GENEDATA_H
#define GENEDATA_H

#include <QWidget>
#include <QThread>
#include "ui/header.h"
#include "data/familydata.h"
#include "data/persondata.h"
#include "data/repo.h"
#include "data/submitter.h"
#include "data/problem.h"
#include "io/reporter.h"
#include "generation.h"

class DemonRun
{
    public:
        bool familyRelations;
        bool errors;
        bool estimatedBirths;
        bool dateAccuracyTest;
};

class GeneData : public QThread
{
Q_OBJECT

public:
    GeneData(QObject *parent = 0);
    ~GeneData();    
              QString test();
    static QFont    panelFont, panelFontBold;
    Reporter        * reporter;
    DemonRun        demonRun;
    Header          header;     // copy ok
    QMap<quint8, GenerationData> generationData; // copy ok
    Submitter       subm;
    Repo            * repo; // copy ok
    MultiMediaRecord* obje; // copy ok
    NoteRecord      * note; // copy ok
    SourceRecord    * sour; // copy ok
    FamilyData      * fami; // copy ok
    PersonData      * indi; // copy ok
    QList<Problem>  problems; // copy ok
    quint16         currentId;
    quint16         rootFams;
    quint16         rootId;
    QString         fileName;

    void            indiSpace(quint16 j);   // functions that make space for records
    void            famiSpace(quint16 j);
    void            objeSpace(quint16 j);
    void            sourSpace(quint16 j);
    void            noteSpace(quint16 j);
    void            repoSpace(quint16 j);
    static bool     gdSpace(quint8 j);
    static quint8   gdCount, gdLastUsed, currentGd;

    quint16         repoCount, objeCount, noteCount, sourCount, famiCount, indiCount;
    quint16         repoLastUsed, objeLastUsed, noteLastUsed, sourLastUsed, famiLastUsed, indiLastUsed;
    void            clear();
    void            format();
    QList<quint16>  browseHistory;
    bool            canBeSaved;
    bool            isSaved;
    bool            locked;
    void            copyTo(GeneData *gd);
    QList<PersonData> editIndiUndo;
    QList<FamilyData> editFamUndo;
    quint16         editUndoIndiCount;
    quint16         editUndoFamiCount;
    void            copyToEditUndo(int indi);
    void            pasteFromEditUndo();
    void            clearEditUndo();
    quint16         editUndoFamLastUsed;
    quint16         editUndoIndiLastUsed;
    quint16         editAddIndi, editAddFam;
    quint16         editUndoCurrentId;
    void            arrangeEvents();
    void            run();
    quint16         id;
    bool            wasRunningWhenStopped;
    void            stop();
    bool            userSelections();
    static void     terminateAll();
    QStringList     usrEvents, famEvents;
    bool            stopDemonNow;
    void            makeChildIdLists();
    void            arrangePersons(QList<quint16> *personList);
    QList<quint16>  twinAlfa, twinBeta;
    PPOLICY         printPolicy;

private:
    bool            isPatronym(QString name);
    void            addFamRel(quint16 relativeId, quint16 thisId, QString relation);
    void            addFamRelAdd(quint16 id);
    void            checkName(QString name, quint16 id);
    void            checkNameStep(QString name, quint16 id);

    void            checkPlaces(bool indi, quint16 id, QString placeString, QString eventName);
    void            checkPlace(bool indi, quint16 id, QString placeString, QString eventName);
    void            checkAttr(bool indi, quint16 i, QString attr);
    void            checkSources();
    QString         removePlaceComment(QString placeString);
    //void            dayAccuracyTest();
    void            estimatedDateTest();
    void            problemFinding();
    void            familyRelationTest();
signals:
    void            sigUpdate(quint16);
    void            sigUpdate();
    void            demonOk();
    void            sigStatusBarMessage(QString);
};

#endif // GENEDATA_H
