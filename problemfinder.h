#ifndef PROBLEMFINDER_H
#define PROBLEMFINDER_H

#include <QDialog>
#include <QThread>
#include "ui_problemFinder.h"
#include <QGridLayout>
#include <QLabel>
#include <QSignalMapper>
#include "settings.h"

enum FIX { NOFIX, ADD_FEMALE_NAME, ADD_MALE_NAME, ADD_PLACE };

class ProblemThread;

class ProblemFinder : public QDialog, private Ui::problemFinder
{
    Q_OBJECT

    private:
        QGridLayout     *   grid;
        QSpacerItem     *   verticalSpacer;
        QLabel          *   label;
        QLabel          *   label2;
        QLabel          *   reason;
        QPushButton     *   bt1;        
        QPushButton     *   fixBt;        
        QPushButton     *   browse;
        int                 line;        
        QSignalMapper   *   signalMapper;
        QSignalMapper   *   fixFemaleNameMapper;
        QSignalMapper   *   fixMaleNameMapper;
        QSignalMapper   *   fixPlaceMapper;
        QSignalMapper   *   browseToMapper;
        ProblemThread   *   problemThread;
        void                deleteUi(int x, int y);
        //Settings            settings;***

    public:
        ProblemFinder(QWidget *parent = 0);                

    private slots:
        void showProblemPerson(int);
        void slotFemaleNameFix(QString);
        void slotMaleNameFix(QString);
        void slotPlaceFix(QString name);
        void slotBrowseTo(int);
        void addProblem(QString text, int id, FIX fix, QString name, QString explanation);
        void update();
        void slotClose();

};

class ProblemThread : public QThread
{
    Q_OBJECT

    public:
        ProblemThread();
        void run();
        ProblemFinder  *pf;
        bool    checkGenres;
        bool    checkNames;
        bool    checkPlaces;
        bool    checkDates;
    signals:
        void    problem(QString, int, FIX, QString, QString);
        void    delRow(int row);
};

#endif // PROBLEMFINDER_H
