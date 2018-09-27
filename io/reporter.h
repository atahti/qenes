#ifndef REPORTER_H
#define REPORTER_H

#include <QList>
#include <QStringList>
#include <QFile>
#include <QThread>
#include <QFileDialog>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
//#include <QPrintDialog>
#include "data/familydata.h"
#include "ui_printsettings.h"
#include "ui/header.h"
#include "io/PrintSettings.h"

enum PRINT_FAMILYDATA   { FAMPRINT_NEITHER, FAMPRINT_FATHER, FAMPRINT_MOTHER, FAMPRINT_BOTH };
enum PRINT_WHAT         { PRINT_ALL, PRINT_ANCESTORS, PRINT_DESCENDATS };


class ReporterDialog : public QDialog, private Ui::uiPrintSettings
{
    Q_OBJECT

    private:
        PrintSettings * pPrintSettings;
        int             id;
        void            saveSetup();
        void            startReport();

    private slots:
        void            printtoHtml();
        void            cousinCheck();
        void            whatToPrintChanged(int index);
        void            closeWithoutPrinting();
        void            showPrivacySettings();

    public:
        ReporterDialog(PrintSettings *pSettings, QWidget *parent = 0);
};

class Reporter : public QThread
{
    Q_OBJECT

    private:
        QString     makeProblems();
        void        makeReport();
        QString     getFileEnd(QString fileName);
        QString     letterList;
        QString     makeIndex();
        bool        * pPrintPdf;
        bool        * pPrintJava;
        QString     * pDirName;
        QString     tableId(int p, QList<int> *tables);
        void        printStep(quint16 id, QList<int> *tables);
        QString     addMultimedia(MultiMedia mm, QString dirName);
        QString     fileName(int i);
        QString     makeLink(QString name, quint16 id, QList<int> *tables);
        void        wwwReplacer(QString str, quint16 id, QString *tree, QList<int> *tables);
        QString     makeEventCalendar(QList<int> *tables);
        int         geneDataId;
        int         descendantTableBuilder(bool *newCol, quint16 child, QList<int> *rivit, QList<int> *henk, QString *output);
        bool        printableChilds(QList<int> *tables, QList<quint16> *childList);
        QString style00;
        QString header00;
        QString table01;
        QString indinotes03;
        QString indieventstitle04;
        QString indieventsline05;
        QString marriagespouse06;
        QString marriageeventsnote07;
        QString marriageeventsheader08;
        QString marriageeventsline09;
        QString marriagechildstitle10;
        QString marriagechildsline11;
        QString familytree12_0gp;
        QString familytree12_1gp;
        QString familytree12_2gp;
        QString familytree12_3gp;
        QString familytree12_4gp;
        QString familytree12_5gp;
        QString familytree12_footer;
        QString footer13, javabrowser15;
        QString     makeHtml(quint16 indiNro);
        QString     makeHtmlFamilyTreeSmall(quint16 p);
        QString     makeHtmlFamilyTreeMore(quint16 p);
        QString     makeHtmlDescentantTable(quint16 i);
        QString     makeHtmlFamily(quint16 p);
        QString     makeHtmlFamilyEventChildren(quint16 p, quint16 f);
        //QString maleColor, femaleColor, eventLineColor, eventLineColor2;
        QTextDocument * pPdfText;
        QList<int>  tables;
        QString genTree0;
        QString genTree1;
        QString genTree2;
        QString genTree3;
        QString genTree4;
        QString genTree5;
        quint16 rows, rows2;

    public:
        Reporter(int geneDataId, PrintSettings * pSettings, QObject *parent=0);
        void        run();
    signals:
        void        reportComplete(int);
        void        reportFail(int, int);
        void        sigStatusBarMessage(QString);
};

#endif // REPORTER_H
