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

#include "data/genedata.h"
#include "data/submitter.h"
#include "macro.h"

extern  GeneData        * geneData;

Submitter::Submitter()
{
    //this->id = GENE.submCount;
    clear();
    //GENE.submCount++;
}


Submitter::~Submitter()
{
    //GENE.submCount--;
}

void Submitter::clear()
{
    address.clear();
    tag = "";
    id = 0;
    language = "";
    name = "";
    multimedia.clear();
    rfn = "";
    rin = "";
}
