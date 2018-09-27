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

#include "repo.h"
#include "data/genedata.h"
#include "macro.h"

extern  GeneData        * geneData;

void Repo::clear()
{
    name = "";
    refn = "";
    refnType = "";
    rin = "";
    id = 0;
    note.clear();
    address.clear();
}

Repo::Repo()
{
    this->id = GENE.repoCount;
    GENE.repoCount++;
}

Repo::~Repo()
{
    GENE.repoCount--;
}
