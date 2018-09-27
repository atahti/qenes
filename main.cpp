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

//#include <QtGui/
#include <QApplication>
//#include <QtGui/QApplication>
#include <QDialog>
//#include <QtGui/QDialog>
#include "mainwindow.h"
//#include "qmlmainwindow.h"

GeneData        * geneData = NULL;
bool            qmlGui;
bool            canExit = true;

void main(int argc, char *argv[])
{
    QTranslator trans;
    trans.load(":/lang/lang/suomi.qm");
    QApplication a(argc, argv);

    a.installTranslator(&trans);

    //if (QML_GUI) qmlGui = true; else qmlGui = false;

    qmlGui = false;
    if (!qmlGui)
    {
        MainWindow w;
        w.trans = &trans;
        w.show();
        a.exec();
    }
}
