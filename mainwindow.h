#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtDebug> // ota pois kun debuggausta ei tarvita
#include <QMainWindow>
#include <QDate>
#include <QFile>
#include <QTranslator>
#include <QPixmap>
#include <QGraphicsScene>
#include <QStringListModel>
#include <QShortcut>
#include <QSignalMapper>
#include <QTextStream>
#include <QPlainTextEdit>

#include "ui/searchandhistory.h"
#include "ui_mainwindow.h"
#include "ui/box/current.h"
#include "ui/box/panel.h"
#include "ui/personlist/personlist.h"
#include "ui/personlist/childlist.h"
#include "ui/personlist/cousinlist.h"
#include "ui/personlist/siblinglist.h"

#include "io/savefile.h"
#include "io/loadfile.h"
#include "io/PrintSettings.h"

#include "generation.h"
#include "settings.h"
#include "dbview.h"
#include "problemfinder.h"
#include "linewidget.h"
#include "messagepanel.h"
#include "searchreplace.h"

#include "gui.h"

#define FEMALESTYLE             "QPushButton { background-color: rgb(255, 199, 199); }"
#define MALESTYLE               "QPushButton { background-color: rgb(199, 199, 255); }"
#define FEMALESTYLE_SELECTED    "QPushButton { background-color: rgb(255, 240, 240); }"
#define MALESTYLE_SELECTED      "QPushButton { background-color: rgb(240, 240, 255); }"
#define UNKNOWNSTYLE            "QPushButton { background-color: rgb(199, 199, 199); }"
#define UNKNOWNSTYLE_SELECTED   "QPushButton { background-color: rgb(240, 240, 240); }"
#define YELLOWSTYLE             "QPushButton { background-color: rgb(255, 255, 199); }"

namespace Ui
{
    class MainWindowClass;
}

class Load;
class LoadProgress;

class MainWindow : public QMainWindow, private Ui::MainWindowClass
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();
        Current         * panelCenter; // tätä tarvitaan ainakin dialogdbview:ssä
        QTranslator     * trans;

    private slots:
        void            slotPrint();
        void            settings();
        void            slotStatusBarMessage(QString);
        void            slotDatabases();
        void            slotLoadComplete();
        void            slotLoadFail(int);
        void            slotReportComplete(int);
        void            slotReportFail(int, int);
        void            problemFinder();
        void            slotStepChildSelected(bool);
        //void            slotStepSiblingsSelected(bool);
        void            slotRunGene();

        // menu slots
        void            slotShowHeader();
        void            slotSave();
        void            slotSaveAs();
        void            filenew();
        void            loadFile();
        int             debug_int(bool *newCol, int, QList<int> *rivit, QList<int> *henk, QString *output);
        void            debug1();
        void            debug2();
        void            debug3();
        void            debug4();
        void            debug5();
        void            debug6();

        void            langEnglish();
        void            langSuomi();        
        void            setRootFams();
        void            setRootIndi();
        void            slotSelectAncestors();
        void            slotSelectDescendants();
        void            slotSelectDescendantsAll();
        void            slotSelectAll();
        void            slotSelectNone();
        void            slotSelectThis();
        void            slotClose();
        void            slotCopy();
        void            slotPasteAsNewProject();
        void            slotPaste();
        void            setGd(int gdId);
        void            slotSources();
        void            slotLoadLastFile();
        void            slotSearchReplace();

        void            makeDefaultSetupValues();
        void            useDefaultSetupValues();

    public slots:                                
        void            slotLoadDialog(int header, int indi, int fam, int multi, int note, int repo, int sour, int subm, int prog);
                                            // Loadfile sends update data during load process
        void            update(quint16);
        void            update();


    private:
        QString         getFileEnd(QString fileName);
        bool            gdSpace(quint8 requiredSpace);
        MessagePanel    * messagePanel;
        QTextStream     * pLoadStream, * pPasteStream;
        QLabel          * uiTitle;
        QString         pasteString;
        QFile           file;
        QShortcut       * debug1key;
        QShortcut       * debug2key;
        QShortcut       * debug3key;
        QShortcut       * debug4key;
        QShortcut       * debug5key;
        QShortcut       * debug6key;
        QShortcut       * keyRunDemon_P;
        QShortcut       * keyUpdate_U;
        QShortcut       * keyCopy;
        QShortcut       * keyPaste;
        QShortcut       * keySelectAncestors;
        Generation      * generation;
        Panel           * panelGGGGp;
        Panel           * panelGGGp;
        Panel           * panelGGp;
        Panel           * panelGp;
        Panel           * panelFather;
        Panel           * panelMother;
        ChildList       * childList;
        PersonList      * siblingList;
        PersonList      * cousinList;
        SearchAndHistory * searchAndHistory;
        void            addChild(int relative);
        void            updateChildren(quint16 id);
        Load            * loadThread;
        Ui::MainWindowClass *ui;
        QList<quint16>      childIdList, siblingIdList, cousinIdList;
        QStringListModel childModel, mostClicked, lastClicked;
        void            resizeEvent(QResizeEvent *event);
        void            closeEvent(QCloseEvent *event);
        LoadProgress    *lp;

        void            updateProjectMenu();
        QSignalMapper   * projectSignalMapper;
        void            importStream(QTextStream * stream, bool activateAfterLoad, QString workName);
        PrintSettings   printSettings;        
        void            hideGgggps(bool hidden);
        void            hideGggps(bool hidden);
        void            hideGgps(bool hidden);
        void            hideGps(bool hidden);
};

#endif // MAINWINDOW_H
