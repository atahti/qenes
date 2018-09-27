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

#include "data/address.h"
#include "data/genedata.h"
#include "macro.h"
#include <QSettings>

extern QSettings    sets;

void Address::clear()
{
    city = "";
    country = "";
    line = "";
    line1 = "";
    line2 = "";
    post = "";
    state = "";
    email = "";
}

Address::Address()
{
    clear();
}

Address::~Address()
{

}

bool Address::used()
{
    if ( (city != "") || (country != "") || (line != "") || (line1 != "") || (line2 != "")
         || (post != "") || (state != "") || ( email != "" )) return true;

    return false;
}
