/*
 *
 *  qenes genealogy software
 *
 *  Copyright (C) 2009-2010 Ari T√§hti
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License veasion 2 as
 *  published by the Free Software Foundation.
 */

#include "data/genedata.h"
#include <QtDebug>
#include "macro.h"
#include <settings.h>
#include <QSettings>

extern GeneData        * geneData;
extern QSettings    sets;

void Famc::clear()
{
    tag = "";    
    value = 0;    
}

Famc::Famc()
{
    clear();
}

Famc::~Famc()
{

}

//----------------------------------------------------------------------------------------------

void Gedcom::clear()
{
    husb = "";
    wife = "";
    childs.clear();
    submitters.clear();
    subN.clear();
}

bool FamilyData::publishable()
{
    PRIVACYMETHOD pm = effectivePrivacyMethod();

    if ( pm == PRIVACY_2_HIDE_DETAILS || pm == PRIVACY_3_BLUR_NAME || pm == PRIVACY_4_HIDE ) return false;
    return true;
}

bool FamilyData::isLiving()
{
    // jos muija tai ukko el‰‰, niin "avioliitto el‰‰"
    if ( (INDI(husb).isLiving()) || INDI(wife).isLiving() ) return true;
    return false;
}

bool FamilyData::eventsToPrint()
{
    if (this->isLiving() && ( GENE.printPolicy == PRINTPOLICY_3_PUBLIC ) ) return false;

    if ( ( effectivePrivacyMethod() == PRIVACY_2_HIDE_DETAILS ) ||
         ( effectivePrivacyMethod() == PRIVACY_3_BLUR_NAME ) ||
         ( effectivePrivacyMethod() == PRIVACY_4_HIDE ) ) return false;

    for (int e=0 ; e<entry.size() ; e++) if ( entry.value(e).publishable() ) return true;

    return false;
    /*
     0 "Show"
     1 "Show Name, plur details, show genre"
     2 "Show name, no details"
     3 "Plur Name, hide details"
     4 "Hide"
          */
}

QString FamilyData::printNote()
{
    /*
     0 "Show"
     1 "Show Name, plur details, show genre"
     2 "Show name, no details"
     3 "Plur Name, hide details"
     4 "Hide"
          */

    if (note.text == "") return "";


    if (effectivePrivacyMethod() == PRIVACY_0_SHOW ) return note.text;
    if (effectivePrivacyMethod() == PRIVACY_1_BLUR_DETAILS && note.text != "") return "***";
    return "";
}

/*
QString FamilyData::printResn()
{
    if (GENE.printPolicy == PRINTPOLICY_1_EVERYTHING) return GeneData::resn(this->privacyPolicy);

    PRIVACYMETHOD pm = (PRIVACYMETHOD)s_privacyMethod(this->privacyPolicy);
    if ( pm == PRIVACY_0_SHOW ) return ""; else return GeneData::resn(1);
}
*/

PRIVACYMETHOD FamilyData::effectivePrivacyMethod()
{
    // t‰m‰ on aikas stub, ja t‰ss‰ siksi ett‰ samanlainen effective___ methodi on myˆs event ja indi luokille.
    // Katsotaan jos t‰h‰n tulisi joskus jotain

    PRIVACYMETHOD pm;

    if ( GENE.printPolicy == PRINTPOLICY_1_EVERYTHING ) return PRIVACY_0_SHOW;

    pm = (PRIVACYMETHOD)s_privacyMethod(this->privacyPolicy);
    //if (pm < this->effectivePrivacyMethod()) pm = this->effectivePrivacyMethod();
    return pm;
}

quint8 FamilyData::isFEntry(ENTRYf type)
{
    for (quint8 e=0 ; e<this->entry.size() ; e++) if (entry.at(e).type == type) return e;

    return -1;
}

void FamilyData::arrangeEvents()
{
    // 0 = unlimited early day
    // 1 = birth.... 27
    // 28 = marriage
    // 39 = this day
    // 40 = unlimited max day
    // 41 = any living day event
    // 42 = buried (11) or chrematorized (12)

    int e, b;
    b=-1;

    // J‰rjestet‰‰n ensin niin, ett‰ p‰iv‰m‰‰r‰lt‰‰n tunnetut tulevat ensin ja aikaisempi ensin.
    // P‰iv‰m‰‰r‰lt‰‰n tuntemattomat j‰‰ samalla jonon h‰nnille
    for (e=0 ; e<entry.size()-1 ; e++) {
        if (    (!entry.at(e).day.known && entry.at(e+1).day.known)
                || ( (entry.at(e).day.known && entry.at(e+1).day.known) && (entry.value(e).day > entry.value(e+1).day))
                ) {
            entry.swap(e, e+1);
            e=0;b=0;
        }

        // etsit‰‰n samalla, mist‰ eventist‰ l‰htien alkaa p‰iv‰m‰‰r‰lt‰‰n tuntemattomat.
        // T‰t‰ tietoa tarvitaan seuraavassa stepiss‰.
        if ( !entry.at(e).day.known && b==-1 ) b=e;
    }

    if (b==-1) b=entry.size();

    // K‰yd‰‰n lista uudelleen l‰pi. Nyt p‰iv‰m‰‰r‰lt‰‰n tuntemattomat heitet‰‰n johonkin v‰liin

    // K‰yd‰‰n l‰pi ne eventit joilla ei ole p‰iv‰m‰‰r‰‰. Katsotaan, mit‰ eventti‰ ennen niiden pit‰‰ tapahtua.
    // Sen kertoo GET_I_ENTRY_MAX. Esim entrytyypin 2 (christening) entrymax arvoi voi olla 3 (confirmation),
    // eli jos listasta lˆytyy confirmation niin chr siirret‰‰n ennen confirmaatiota. Jos listassa ei ole chr,
    // etsit‰‰n jokin ...

//    qDebug() << "haku alkaa " << b;

    int max = entry.size();

    int e2;
    for (e=b ; e<max ; e++) {
        if (!entry.at(e).day.known) {
            // Lˆytyi eventti jonka p‰iv‰‰ ei tiedet‰. Siirret‰‰n t‰m‰.
//            qDebug() << "siirret‰‰n eventti‰ " << e;
            for (e2 = 0 ; e2<max ; e2++) {

                if (GET_F_ENTRY_MIN(entry.at(e).type)==0) {
//                    qDebug() << "case0" << e;
                    entry.move(e, 0);
                    e=b; b++; e2=max;
                } else
                if (GET_F_ENTRY_MAX(entry.at(e).type)==40) {
//                    qDebug() << "case1" << e << max;
                    entry.move(e, max-1);
                    e=b; b++; max--; e2=max;
                } else
                if (GET_F_ENTRY_MAX(entry.at(e).type)==42 && (entry.at(e2).type == 11 || entry.at(e2).type == 12)) {
//                    qDebug() << "hˆps";
                    e2--;
                    if (e2<0) e2=0;
                    entry.move(e, e2);
                    e=b; b++; e2=max;
                } else
                if (Settings::famiEventMap[entry.at(e).type].indexOf(entry.at(e2).type) >
                    Settings::famiEventMap[entry.at(e).type].indexOf(entry.at(e).type)) {
                    // Eventing e2 tapahtumaj‰rjestyksess‰, tapahtuma e on myˆhemmin kuin itse e2.
/*                    qDebug() << "vaihdetaan " << e << e2 << entry.at(e).type << entry.at(e2).type <<
                                Settings::indiEventMap[entry.at(e).type].indexOf(entry.at(e).type) <<
                                Settings::indiEventMap[entry.at(e).type].indexOf(entry.at(e2).type);
*/
                    entry.move(e, e2);
                    e=b; b++; e2=max;
                }
            }
        }
    }
}

FamilyData::FamilyData()
{    
    this->id = GENE.famiCount;
    GENE.famiCount++;
    clear(this->parent);
}

FamilyData::~FamilyData()
{        
    //qDebug() << "fam dest";
    GENE.famiCount--;
}

void FamilyData::clear(GeneData *p)
{
    husb = 0;
    wife = 0;
    nchi = 0;
    sourCitations.clear();
    entry.clear();
    childs.clear();
    gedComTag.clear();
    subM.clear();
    subN.clear();
    multiMedia.clear();
    refn = "";
    refnType = "";
    rin = "";
    note.clear();
    deleted = false;
    privacyPolicy = 0;
    this->parent = p;
}

Entry FamilyData::fentry(ENTRYf type)
{
    int i;
    for (i = 0; i<this->entry.size() ; i++)
        if (this->entry.at(i).type == type) return this->entry.at(i);
    return Entry();
}
