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


#include "io/loadprogress.h"
#include <QtDebug> // ota pois kun debuggausta ei tarvita

void LoadProgress::setMax(int max)
{
    uiProgressBar->setMaximum(max);
}

void LoadProgress::set(int header, int indi, int fam, int multi, int note, int repo, int sour, int subm, int prog)
{
    if ( (prog != 0) && (maxim == 0) ) {
        maxim = header + indi + fam + multi + note + repo + sour + subm;
        uiProgressBar->setMaximum(maxim);
        qDebug() << "loadprogress" << maxim << prog;
    }
    uiHeader->setText(QString::number(header));
    uiIndi->setText(QString::number(indi));
    uiFam->setText(QString::number(fam));
    uiMm->setText(QString::number(multi));
    uiNotes->setText(QString::number(note));
    uiRepo->setText(QString::number(repo));
    uiSour->setText(QString::number(sour));
    uiSubm->setText(QString::number(subm));    
    uiProgressBar->setValue(prog);
}

LoadProgress::LoadProgress()
{
    maxim = 0;
    setupUi(this);
}
