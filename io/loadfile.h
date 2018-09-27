#ifndef LOADFILE_H
#define LOADFILE_H

//#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include "data/genedata.h"
#include "data/submitter.h"
#include "data/repo.h"
#include "io/loadprogress.h"
#include <QThread>

enum CHARSET { NO_CHAR, ANSEL_CHAR, UNICODE_CHAR, ASCII_CHAR };
enum MONTHLANGUAGE { MO_FINNISH, MO_ENGLISH, MO_FRENCH };

class Load : public QThread
{
    Q_OBJECT

    public:
        Load(QTextStream *qts, QString projectName, bool activateAfterLoad, QObject *parent);
        Load(QObject *parent = 0);
        ~Load();
    protected:
        void run();
    private:
        int             cntErrorsDate, cntDates;
        QList<QString>  famList, repoList, indList, objeList, noteList, submList, sourList;
        QTextStream     * inputStream;
        Date            date;
        QTime           time;
        Entry           tmpEvent;
        Source          tmpSource;
        Address         tmpAddress;
        MultiMedia      tmpMultimedia;
        Submitter       tmpSubm;
        Note            tmpNote;
        //Entry           tmpAttribute;
        Repo            tmpRepo;
        Famc            tmpFamc;
        FamilyData      tmpFd;
        PersonData      tmpPd;
        QString         command[9], commandOld[9];
        bool            used;
        int             idHead;
        int             idIndi;
        int             idFam;
        int             idObje;
        int             idNote;
        int             idRepo;
        int             idSubm;
        int             idSour;
        QString         param, tmpParam;
        int             level;
        QString         cmd, tag, line;

        QString         fileName;
        bool            gedComLineAnalyze(QString* ln, int* level, QString* cmd, QString* param);
        void            gedComDayAnalyze(QString* day, Date* date);
        void            gedComTimeAnalyse(QString* line, QTime *time);
        bool            findGedComTag(QString str, QString* pTag);
        bool            isCommand(QString cmd[], QString cmd0, QString cmd1, QString cmd2);
        bool            isEntry(QString cmd[]);
        bool            hasRecordEnded(QString cmd, QString commandOld[], QString command[]);
        int             commandLevel(QString cmd[], QString cmd2);
        QString         familyEventRole(ENTRYf_ROLE fer);
        ENTRYf_ROLE     familyEventRole(QString fer);
        QUALITY         quality(QString str);
        void            cont(QString * data, QString add);
        void            conc(QString * data, QString add);

        void            noteRecordEnded();
        void            addrRecordEnded();
        void            objeRecordEnded();
        void            sourRecordEnded();
        void            famEventEnded();
        void            indiEventEnded();
        void            famcRecordEnded();
        void            headRecordEnded();

        void            analyseNoteRecord();
        void            analyseObjeRecord();
        void            analyseEvent();
        void            analyseRepoRecord();
        void            analyseAddrRecord();
        void            analyseSourRecord();
        void            analyseSubnRecord();
        void            analyseHeadRecord();
        void            analyseSubmRecord();
        void            analyseIndiRecord();
        void            analyseFamRecord();
        void            analyseFamcRecord();
        void            whichMonthUsed();

        QString         pedi(PEDI_TYPE);
        PEDI_TYPE       pedi(QString str);
        MULTIMEDIA_FORMAT mmFormat(QString str);

        bool            activateAfterLoad;

        MONTHLANGUAGE   monthLanguage; // tämä kertoo, millä kielellä kuukaudet on kirjattu



    signals:
        void    loadComplete();
        void    loadFail(int);
        void    loadDialog(int, int, int, int, int, int, int,int, int);
        void    sigMessage(QString);
        void    update(quint16);
};

#endif // LOADFILE_H
