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

#include "data/note.h"
#include "data/genedata.h"
#include "macro.h"
#include <QSettings>

extern QSettings    sets;
extern  GeneData        * geneData;

bool Note::used()
{
    if (tag != "" || text != "" || rin != "" || refn != "" ) return true;
    return false;
}

void Note::clear()
{
    tag = "";
    refn = "";
    refnType = "";
    rin = "";
    text = "";
    id = 0;
}

Note::Note()
{
    clear();
}

Note::~Note()
{

}

void NoteRecord::copyDataFrom(Note n)
{
    this->tag = n.tag;
    this->text = n.text;
    this->refn = n.refn;
    this->refnType = n.refnType;
    this->rin = n.rin;
    //this->id = n.id;
    this->change = n.change;
}

NoteRecord::NoteRecord()
{
    this->id = GENE.noteCount;
    GENE.noteCount++;
}

NoteRecord::~NoteRecord()
{
    GENE.noteCount--;
}


