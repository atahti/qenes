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

#include "data/source.h"
#include "data/genedata.h"
#include <QSettings>
#include "macro.h"

extern  GeneData        * geneData;

extern QSettings    sets;

void SourceRecord::copyDataFrom(Source s)
{
    this->quay = s.quay;
    this->multiMedia = s.multiMedia;
    this->note = s.note;
    this->address = s.address;
    this->event = s.event;
    this->tag = s.tag;
    this->page = s.page;
    this->author = s.author;
    this->title = s.title;
    this->name = s.name;
    this->copyright = s.copyright;
    this->text = s.text;
    this->version = s.version;
    this->approvedSystemID = s.approvedSystemID;
    this->corporation = s.corporation;
    this->productName = s.productName;
    this->phone = s.phone;
    this->abbreviation = s.abbreviation;
    this->publication = s.publication;
    this->gedComRepoID = s.gedComRepoID;
    this->refn = s.refn;
    this->refnType = s.refnType;
    this->rin = s.rin;
    this->deleted = s.deleted;
    this->aboutToBeDeleted = s.aboutToBeDeleted;
    this->changed = s.changed;
}

SourceRecord::SourceRecord()
{
    this->id = GENE.sourCount;
    GENE.sourCount++;    
}

SourceRecord::~SourceRecord()
{
    GENE.sourCount--;
}

QString Source::printQuality()
{
    if ( quay == QUAY0) return " (Data quality 0/3 : Uncertain info)";
    if ( quay == QUAY1) return " (Data quality 1/3 : Questionable info)";
    if ( quay == QUAY2) return " (Data quality 2/3 : Secondary evidence info)";
    if ( quay == QUAY3) return " (Data quality 3/3 : Primary evidence info)";

    return "Data quality NA";
}

bool Source::used()
{
    if ( id != 0 || page != "" || event.type != "" || event.role != NO_FAMILY_EVENT_ROLE || text != "" || title != "" ) return true;
    return false;
}

void Source::clear()
{
    abbreviation = "";
    address.clear();
    approvedSystemID = "";
    author = "";
    corporation = "";
    copyright = "";
    name = "";
    text = "";
    event.type = "";
    event.agency = "";
    event.place = "";
    event.role = NO_FAMILY_EVENT_ROLE;
    gedComRepoID = "";
    multiMedia.clear();
    note.clear();
    page = "";
    phone = "";
    productName = "";
    publication = "";
    quay = NO_QUAY;
    tag = "";
    title = "";
    version = "";
    id = 0;
    deleted = false;
    aboutToBeDeleted = false;
}

bool Source::usedBy(int *x, bool *indi)
{
    for (quint16 i=1 ; i<=GENE.indiLastUsed ; i++) {
        if (INDI(i).source.id == this->id) {
            *x = i;
            *indi = true;
            return true;
        }

        for (int e=0 ; e<INDI(i).entry.size() ; e++) {
            if ( INDI(i).entry.value(e).source.id == this->id) {
                *x = i;
                *indi = true;
                return true;
            }
        }
    }

    for (quint16 f=1 ; f<=GENE.famiLastUsed ; f++) {
        if (FAM(f).source.id == this->id) {
            *x = f;
            *indi = false;
            return true;
        }

        for (int e=0 ; e<FAM(f).entry.size() ; e++) {
            if ( FAM(f).entry.value(e).source.id == this->id) {
                *x = f;
                *indi = false;
                return true;
            }
        }
    }
    return false;
}

Source::Source()
{
    clear();
}

Source::~Source()
{

}
