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

#include "header.h"
#include "ui_header.h"
#include "data/familydata.h"
#include "data/persondata.h"
#include "data/genedata.h"
#include "data/setupvalues.h"
#include "macro.h"
#include <QFile>
#include <QFileDialog>
#include <QSettings>

extern  GeneData        * geneData;
extern QSettings    sets;

void HeaderDialog::closeDialog()
{
    saveSubmitter();

    GENE.header.note = this->uiNotes->toPlainText();

    close();
}

void HeaderDialog::saveSubmitter()
{
    GENE.subm.address.line = uiSubmAddr->text();
    GENE.subm.address.line1 = uiSubmAddr1->text();
    GENE.subm.address.line2 = uiSubmAddr2->text();
    GENE.subm.address.city = uiSubmCity->text();
    GENE.subm.address.country = uiSubmCountry->text();
    GENE.subm.language = uiSubmLang->text();
    GENE.subm.name = uiSubmName->text();
    GENE.subm.address.post = uiSubmPost->text();
    GENE.subm.address.state = uiSubmState->text();
}

void HeaderDialog::submitter()
{
    uiSubmAddr->setText(GENE.subm.address.line);
    uiSubmAddr1->setText(GENE.subm.address.line1);
    uiSubmAddr2->setText(GENE.subm.address.line2);
    uiSubmCity->setText(GENE.subm.address.city);
    uiSubmCountry->setText(GENE.subm.address.country);
    uiSubmLang->setText(GENE.subm.language);
    uiSubmName->setText(GENE.subm.name);
    uiSubmPost->setText(GENE.subm.address.post);
    uiSubmState->setText(GENE.subm.address.state);
}

HeaderDialog::HeaderDialog(Header header, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    connect(uiExit, SIGNAL(clicked()), this, SLOT(closeDialog()));
    
    uiCharSet->setText(header.charset.name);    
    uiDate->setText(header.dataDate.toString("yyyy.MM.dd"));    
    uiFile->setText(header.file);
    uiGedComForm->setText(header.gedComForm);
    uiGedComVers->setText(header.gedComVersion);
    uiProductName->setText(header.source.productName);
    uiVersion->setText(header.source.version);
    uiNotes->setText(header.note);
    uiSubmAddr->setText(GENE.subm.address.line);
    uiSubmAddr1->setText(GENE.subm.address.line1);
    uiSubmAddr2->setText(GENE.subm.address.line2);
    uiSubmCity->setText(GENE.subm.address.city);
    uiSubmCountry->setText(GENE.subm.address.country);
    uiSubmLang->setText(GENE.subm.language);
    uiSubmName->setText(GENE.subm.name);
    uiSubmPost->setText(GENE.subm.address.post);
    uiSubmState->setText(GENE.subm.address.state);


}

HeaderDialog::~HeaderDialog()
{

}
