/*
 *
 *  qenes genealogy software
 *
 *  Copyright (C) 2009-2010 Ari Tähti
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License veasion 2 as
 *  published by the Free Software Foundation.
 */


#include <QtDebug>
#include <QMetaType>
#include "problemfinder.h"
#include "data/persondata.h"
#include "data/familydata.h"
#include "ui/person.h"
#include "data/setupvalues.h"
#include "macro.h"

#define ITEM_AT(grid, x,y)          grid->itemAtPosition(y, x)->widget()

extern  GeneData        * geneData;

extern QList<QString>  patronyms;
extern QList<QString>  boyNames;
extern QList<QString>  girlNames;
extern QList<QString>  places;

ProblemThread::ProblemThread()
{

}

void ProblemThread::run()
{
    qRegisterMetaType<FIX>("FIX");
    int id;

    int size = GENE.problems.size();
    if (size>50) size = 50;

    for (int i=0 ; i<size ; i++) {
        if (!INDI(GENE.problems.at(i).id).deleted) {
            id = GENE.problems.at(i).id;
            QString errorStr = GENE.problems.value(i).typeToString();
            if (GENE.problems.at(i).type == PROBLEM_PLACE_AS_ATTRIBUTE) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_UNKNOWN_SEX && checkGenres ) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_UNLINKED) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_FEMALENAME && checkNames ) emit (problem(errorStr, id, ADD_FEMALE_NAME, GENE.problems.at(i).attribute, GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_MALENAME && checkNames ) emit (problem(errorStr, id, ADD_MALE_NAME, GENE.problems.at(i).attribute, GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_NAME && checkNames ) emit (problem(errorStr, id, NOFIX, GENE.problems.at(i).attribute, GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_EVENT_DAY_VIOLATION && checkDates ) emit(problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_DATE_BETWEEN_PERSONS && checkDates ) emit(problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_HOMOCOUPLE) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_PLACE && checkPlaces ) emit (problem(errorStr, id, ADD_PLACE, GENE.problems.at(i).attribute, GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_FATHER_DEATH && checkDates) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_MOTHER_DEATH && checkDates) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_TOO_OLD_MOTHER && checkDates) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_TOO_OLD_FATHER && checkDates) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_TOO_YOUNG_MOTHER && checkDates) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_TOO_YOUNG_FATHER && checkDates) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_EVENT_ORDER_VIOLATION && checkDates) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_MARRIAGE_CRIME && checkDates) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_TOO_YOUNG_SPOUSE && checkDates) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_TOO_OLD_SPOUSE && checkDates) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_SIBLINGS_TOO_CLOSE && checkDates) emit (problem(errorStr, id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_SOURCE ) emit (problem(errorStr, 0, NOFIX, "", GENE.problems.at(i).explanation));

            /*
            if (GENE.problems.at(i).type == PROBLEM_UNKNOWN_SEX && checkGenres ) emit (problem("Unknown sex for a male or female : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_UNLINKED) emit (problem("Unlinked : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_FEMALENAME && checkNames ) emit (problem("Female name : ", id, ADD_FEMALE_NAME, GENE.problems.at(i).attribute, GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_MALENAME && checkNames ) emit (problem("Male name : ", id, ADD_MALE_NAME, GENE.problems.at(i).attribute, GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_NAME && checkNames ) emit (problem("No gendre : ", id, NOFIX, GENE.problems.at(i).attribute, GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_EVENT_DAY_VIOLATION && checkDates ) emit(problem("Person too young or old for this event : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_DATE_BETWEEN_PERSONS && checkDates ) emit(problem("Birth day(s) of this or relatives : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_HOMOCOUPLE) emit (problem("Homo couple : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_PLACE && checkPlaces ) emit (problem("Place name : ", id, ADD_PLACE, GENE.problems.at(i).attribute, GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_FATHER_DEATH && checkDates) emit (problem("Father death before child birth : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_MOTHER_DEATH && checkDates) emit (problem("Mother death before child birth : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_TOO_OLD_MOTHER && checkDates) emit (problem("Too old mother : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_TOO_OLD_FATHER && checkDates) emit (problem("Too old father : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_TOO_YOUNG_MOTHER && checkDates) emit (problem("Too young mother : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_TOO_YOUNG_FATHER && checkDates) emit (problem("Too young father : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_EVENT_ORDER_VIOLATION && checkDates) emit (problem("Event before or after another event : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_MARRIAGE_CRIME && checkDates) emit (problem("Marriage crime : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_TOO_YOUNG_SPOUSE && checkDates) emit (problem("Too young spouse : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_TOO_OLD_SPOUSE && checkDates) emit (problem("Too old spouse : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_SIBLINGS_TOO_CLOSE && checkDates) emit (problem("Birth of siblings too close : ", id, NOFIX, "", GENE.problems.at(i).explanation));
            if (GENE.problems.at(i).type == PROBLEM_SOURCE ) emit (problem("Source not used : ", 0, NOFIX, "", GENE.problems.at(i).explanation));
            */
        }
    }


    this->exit(0);
}

void ProblemFinder::slotFemaleNameFix(QString name)
{
    if (!girlNames.contains(name)) {
        girlNames.append(name);
        Settings::changed = true;
        //Settings::saveFromRamToDisk();
    }
    geneData->demonRun.errors = true;
    geneData->start();
    geneData->wait(10000);
    this->update();
}

void ProblemFinder::slotBrowseTo(int id)
{
    done(id);    
}

void ProblemFinder::slotMaleNameFix(QString name)
{
    if (!boyNames.contains(name)) {
        boyNames.append(name);
        Settings::changed = true;
        //Settings::saveFromRamToDisk();
    }
    geneData->demonRun.errors = true;
    if (RUN_CHECK) {
        geneData->start(QThread::NormalPriority);
        geneData->wait(10000);
        this->update();
    }
}

void ProblemFinder::slotPlaceFix(QString name)
{
    if (!places.contains(name)) {
        places.append(name);
        Settings::changed = true;
        //Settings::saveFromRamToDisk();
    }
    geneData->demonRun.errors = true;
    if (RUN_CHECK) {
        geneData->start(QThread::NormalPriority);
        geneData->wait(10000);
        this->update();
    }
}

void ProblemFinder::addProblem(QString text, int id, FIX fix, QString name, QString explanation)
{
    QLayoutItem *item = grid->itemAtPosition(line, 0);

    if (line <50 && item == NULL ) {
        label   = new QLabel(uiErrors);
        label2  = new QLabel(uiErrors);        

        label->setText("# " + QString::number(id) + " " + text);
        label2->setText(explanation);

        if (id) {
            bt1     = new QPushButton(uiErrors);
            bt1->setText("show");
            bt1->setFixedWidth(60);
            browse  = new QPushButton(uiErrors);
            browse->setText("Browse to");
            fixBt   = new QPushButton(uiErrors);

            switch (fix) {
                case NOFIX : fixBt->setText("no fix"); break;
                case ADD_FEMALE_NAME : {
                        fixBt->setText("Add name to list");
                        connect(fixBt, SIGNAL(clicked()), fixFemaleNameMapper, SLOT(map()));
                        break;
                }
                case ADD_MALE_NAME : {
                        fixBt->setText("Add name to list");
                        connect(fixBt, SIGNAL(clicked()), fixMaleNameMapper, SLOT(map()));
                        break;
                }
                case ADD_PLACE : {
                        fixBt->setText("Add place to list");
                        connect(fixBt, SIGNAL(clicked()), fixPlaceMapper, SLOT(map()));
                        break;
                }
            }
            connect(bt1, SIGNAL(clicked()), signalMapper, SLOT(map()));
            connect(browse, SIGNAL(clicked()), browseToMapper, SLOT(map()));

            signalMapper->setMapping(bt1, id);
            browseToMapper->setMapping(browse, id);
            fixFemaleNameMapper->setMapping(fixBt, name);
            fixMaleNameMapper->setMapping(fixBt, name);
            fixPlaceMapper->setMapping(fixBt, name);

            grid->addWidget(bt1,    line, 2, 0);
            grid->addWidget(fixBt,  line, 3, 0);
            grid->addWidget(browse, line, 4, 0);
        }

        grid->addWidget(label,  line, 0, 0);
        grid->addWidget(label2, line, 1, 0);
    };

    if (line == 100) {
        label   = new QLabel(uiErrors);
        label->setText("Only 100 first shown.");
        grid->addWidget(label, line, 1, 0);
    }

    line++;
}

void ProblemFinder::deleteUi(int x, int y)
{
    QLayoutItem *item = grid->itemAtPosition(y, x);
    if (item != NULL)
    {
        QWidget *wdg = (QWidget*)ITEM_AT(grid, x, y);
        grid->removeWidget(wdg);
        wdg->deleteLater();
    }
}

void ProblemFinder::update()
{    
    for (int y=0 ; y<grid->rowCount() ; y++) {
        deleteUi(0, y);
        deleteUi(1, y);
        deleteUi(2, y);
        deleteUi(3, y);
        deleteUi(4, y);
    }

    line = 0;

    qDebug() << grid->rowCount();

    problemThread->checkDates   = this->uiDates->isChecked();
    problemThread->checkGenres  = this->uiGenres->isChecked();
    problemThread->checkNames   = this->uiNames->isChecked();
    problemThread->checkPlaces  = this->uiPlaces->isChecked();

    problemThread->start();

}

void ProblemFinder::showProblemPerson(int id)
{
    //quint16 rel=0;
    quint16 id16 = id;
    Person p(false, false, &id16); //###
    //Person p(false, false, &id16, &rel); //###
    p.exec();
}

void ProblemFinder::slotClose()
{
    reject();
}

ProblemFinder::ProblemFinder(QWidget * parent) : QDialog(parent)
{
        setupUi(this);

        line = 0;

        grid = new QGridLayout(uiErrors);
        grid->setVerticalSpacing(2);
        grid->setHorizontalSpacing(2);
        grid->setContentsMargins(0, 0, 0, 0);

        setFont(GeneData::panelFont);

        signalMapper        = new QSignalMapper(this);
        browseToMapper      = new QSignalMapper(this);
        fixFemaleNameMapper = new QSignalMapper(this);
        fixMaleNameMapper   = new QSignalMapper(this);        
        fixPlaceMapper      = new QSignalMapper(this);

        verticalSpacer = new QSpacerItem(50, 60, QSizePolicy::Minimum, QSizePolicy::Expanding);

        grid->addItem(verticalSpacer, 30, 1, 1, 1);

        connect(uiClose, SIGNAL(clicked(bool)), this, SLOT(slotClose()));
        connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(showProblemPerson(int)));
        connect(fixFemaleNameMapper, SIGNAL(mapped(QString)), this, SLOT(slotFemaleNameFix(QString)));
        connect(fixMaleNameMapper, SIGNAL(mapped(QString)), this, SLOT(slotMaleNameFix(QString)));
        connect(browseToMapper, SIGNAL(mapped(int)), this, SLOT(slotBrowseTo(int)));
        connect(fixPlaceMapper, SIGNAL(mapped(QString)), this, SLOT(slotPlaceFix(QString)));
        connect(uiDates, SIGNAL(clicked()), this, SLOT(update()));
        connect(uiGenres, SIGNAL(clicked()), this, SLOT(update()));
        connect(uiNames, SIGNAL(clicked()), this, SLOT(update()));
        connect(uiPlaces, SIGNAL(clicked()), this, SLOT(update()));

        problemThread = new ProblemThread();
        connect(problemThread, SIGNAL(problem(QString, int, FIX, QString, QString)), this, SLOT(addProblem(QString, int, FIX, QString, QString)));

        problemThread->pf = this;

        this->update();
}

