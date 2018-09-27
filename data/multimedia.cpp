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

#include "data/multimedia.h"
#include "data/genedata.h"
#include "macro.h"

extern  GeneData        * geneData;

bool MultiMedia::used()
{
    if (file != "" || format != NO_MM || tag != "" || title != "" || note.used() ) return true;
    return false;
}

void MultiMedia::clear()
{
    file = "";
    format = NO_MM;
    refn = "";
    refnType = "";
    rin = "";
    title = "";
    tag = "";
    id = 0;
    note.clear();
}

MultiMedia::MultiMedia()
{
    clear();
}

MultiMedia::~MultiMedia()
{

}

MultiMediaRecord::MultiMediaRecord()
{
    this->id = GENE.objeCount;
    GENE.objeCount++;
}

MultiMediaRecord::~MultiMediaRecord()
{

}

void MultiMediaRecord::copyDataFrom(MultiMedia m)
{
    this->format = m.format;
    this->title = m.title;
    this->file = m.file;
    this->refn = m.refn;
    this->refnType = m.refnType;
    this->rin = m.rin;
    this->tag = m.tag;
    this->note = m.note;
    this->change = m.change;
}
