#ifndef SAVEFILE_H
#define SAVEFILE_H

#include "mainwindow.h"
#include "ui/header.h"
#include <QFile>
#include <QFileDialog>

class Save
{
    private:
        static void        writeLine(int level, QString command, QString param, QString *gedCom);
        static void        writeAddress(int level, Address address, QString *gedCom);
        static void        writeSourceCitation(int level, Source source, QString *gedCom);
        static void        writeNoteStructure(int level, Note note, QString *gedCom);
        static void        writeMmLink(int level, MultiMedia mm, QString *gedCom);
        static QString     makeTag(QString type, int i);
        static QString     makeDay(Date date);
        static QString     makeTime(QTime time);
        static QString     multiDecInt(int i, int decs);
        static void        writeIndiEvent(Entry entry, QString *gedCom, int adoption);
        static void        writeIndiAttribute(Entry attribute, QString *gedCom);
        static void        writeFamEvent(Entry event, QString *gedCom); //void writeFamEvent(Entry event, QFile *pFile);
        static QString     accToString(ACCURACY acc);
        static QString     mmFormatToString(QString file);
        static int         quayToInt(QUALITY q);        
        static QList<int>  sourceList;
        static QString     resn(int restriction);
        static PPOLICY     printPolicy;

    public:
        static void file(QString file);
        static void file();
        static void gedComCoder(QString *gedCom);
        static void        save(QString fileName);
};

#endif // SAVEFILE_H
