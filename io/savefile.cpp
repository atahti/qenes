/*
 *
 *  qenes genealogy software
 *
 *  Copyright (C) 2009-2010 Ari T‰hti
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License veasion 2 as
 *  published by the Free Software Foundation.
 */


#include "savefile.h"
#include "data/genedata.h"
#include "macro.h"

extern  GeneData        * geneData;

QList<int> Save::sourceList;

#define WRITE_MM    true
#define FAM_EVENT   true
#define ADDRESS     true
#define NOTE        true
#define SOURCE      true
#define INDI_EVENT  true
#define INDI_ATTRIBUTES true
#define CHANGED     true
#define RIN         true
#define SUBMITTER   true

int Save::quayToInt(QUALITY q)
{
    if (q == QUAY0) return 0;
    if (q == QUAY1) return 1;
    if (q == QUAY2) return 2;
    if (q == QUAY3) return 3;
    return 0;    
}

QString Save::mmFormatToString(QString file)
{
    if (file.right(3) == "BMP" || file.right(3) == "bmp") return "bmp";
    if (file.right(3) == "GIF" || file.right(3) == "gif") return "gif";
    if (file.right(3) == "jpeg" || file.right(3) == "JPEG" || file.right(3) == "jpg" || file.right(3) == "JPG" ) return "bmp";
    if (file.right(3) == "OLE" || file.right(3) == "ole") return "ole";
    if (file.right(3) == "PCX" || file.right(3) == "pcx") return "pcx";
    if (file.right(3) == "WAV" || file.right(3) == "wav") return "wav";
    if (file.right(3) == "TIFF" || file.right(3) == "tiff" || file.right(3) == "TIF" || file.right(3) == "tif" ) return "tif";
    return "";
}

QString Save::accToString(ACCURACY acc)
{
    if ((int)acc == 0) return "";
    if ((int)acc == 6) return ""; // exactly
    if ((int)acc < 8 ) return Date::accuracyGedCom(acc) + " ";

    return "";
}

QString Save::multiDecInt(int i, int decs)
{
    return QString("%1").arg(QString::number(i), decs, '0');

}

QString Save::makeTime(QTime time)
{
    return multiDecInt(time.hour(), 2) + ":" +
           multiDecInt(time.minute(), 2) + ":" +
           multiDecInt(time.second(), 2);
}

QString Save::makeDay(Date date)
{
    QString s;
    s = accToString(date.getAcc());
    QString date1month = date.month1ShortName();// QDate::shortMonthName(date.day1.month(), QDate::DateFormat); date1month = date1month.toUpper();
    QString date2month = date.month2ShortName(); //QDate::shortMonthName(date.day2.month(), QDate::DateFormat); date2month = date2month.toUpper();

// despite of accuracy, lest file always day, month, year (lets not loose possible day and month data...)
    if (s != "FROM ") return s + QString::number(date.day1.day()) + " " + date1month + " " + multiDecInt(date.day1.year(),4);
    else return s + QString::number(date.day1.day()) + " " + date1month + " " + multiDecInt(date.day1.year(),4)
            + " TO " + QString::number(date.day2.day()) + " " + date2month + " " + multiDecInt(date.day2.year(),4);
}

QString Save::makeTag(QString type, int i)
{    
    return "@" + type + QString::number(i) + "@";
    //return "@" + type + multiDecInt(i,5) + "@";
}

void Save::writeLine(int level, QString command, QString param, QString *gedCom)
{
    if (param != "") {
        int i, l2, charAt; // int j
        int alku, loppu, pituus, lastchar;
        QString koe, line;
        command = command + " ";
        //j=0;

        // If param is empty, the row wont be written. If param is "-", the level and command will be written
        // without param, e.g. "2 SOUR"
        if (param == "-") {
            line = QString::number(level) + " " + command + "\n";
            *gedCom += line; //   pFile->write(line.toLatin1());
        }
        else {

            koe = param;

            i = koe.size();
            do {
                i--;
                charAt = int(koe.at(i).toLatin1());
            } while ( (i!=0) && ( (charAt == 32) || ( charAt == 10) ) );
            koe = koe.left(i+1);

            alku = 0;
            do {
                loppu = koe.size()-1;
                if ((loppu-alku) >245 ) loppu = alku + 245;

                while (int(koe.at(loppu).toLatin1()) == 32) loppu--;

                pituus = loppu - alku;
                charAt = koe.mid(alku, pituus).indexOf('\n');

                if (charAt != -1) loppu = alku + charAt;

                lastchar = int(koe.at(loppu).toLatin1());

                if (lastchar != 10) loppu++;

                pituus = loppu - alku;

                if (( command == "CONT " ) || ( command == "CONC " ) ) l2 = level + 1; else l2 = level;
                line = QString::number(l2) + " " + command + koe.mid(alku, pituus) + "\n";
                *gedCom += line;//pFile->write(line.toLatin1());

                if (lastchar == 10) {
                    command = "CONT ";
                    alku = loppu + 1;
                }
                else {
                    command = "CONC ";
                    alku = loppu;
                }
            } while (alku < koe.size());
        }
    }
}

void Save::writeFamEvent(Entry event, QString *gedCom)
{
    if (event.publishable()) {
        QString line;
        line = "1 " + Entry::typeGedCom((int)event.type, false) + " Y\n";
        *gedCom += line; //pFile->write(line.toLatin1());
        writeLine(2, "TYPE", event.attrType, gedCom);                            // n  TYPE <EVENT_DESCRIPTOR>  {0:1}
        if (event.day.known) writeLine(2, "DATE", makeDay(event.day), gedCom);  // n  DATE <DATE_VALUE>  {0:1}
        writeLine(2, "PLAC", event.place, gedCom);                               //  n  <<PLACE_STRUCTURE>>  {0:1}
        writeAddress(2, event.address, gedCom);                                  //n  <<ADDRESS_STRUCTURE>>  {0:1}
        if (event.age != 0) writeLine(2, "AGE", QString::number(event.age), gedCom); // n  AGE <AGE_AT_EVENT>  {0:1}
        writeLine(2, "AGNC", event.agency, gedCom);                              //n  AGNC <RESPONSIBLE_AGENCY>  {0:1}
        writeLine(2, "CAUS", event.cause, gedCom);                               // n  CAUS <CAUSE_OF_EVENT>  {0:1}
        writeSourceCitation(2, event.source, gedCom);                            //n  <<SOURCE_CITATION>>  {0:M}
        writeMmLink(2, event.multiMedia, gedCom);                                //  n  <<MULTIMEDIA_LINK>>  {0:M}
        writeNoteStructure(2, event.note, gedCom);                               //n <<NOTE_STRUCTURE>>  {0:M}
        //writeLine(2, "RESN", event.printResn(), gedCom);
        writeLine(2, "RESN", resn(event.privacyPolicy), gedCom);
        //if (event.privacyPolicy != 0) writeLine(2, "RESN", GeneData::resn(event.privacyPolicy), gedCom); //    +1 RESN <RESTRICTION_NOTICE>  {0:1}
    }
}

void Save::writeAddress(int level, Address address, QString *gedCom)
{    
    if (address.used() && ADDRESS) {

        if (address.line == "") {
            QString line1;
            line1 = QString::number(level) + " ADDR\n";
            *gedCom += line1;//pFile->write(line1.toLatin1());
        } else {
            //pFile->write(QString::number(level).toLatin1() + " ADDR\n");
            writeLine(level, "ADDR", address.line, gedCom);
            writeLine(level+1, "CONT", "-", gedCom);
        }
        writeLine(level+1, "ADR1", address.line1, gedCom);
        writeLine(level+1, "ADR2", address.line2, gedCom);
        writeLine(level+1, "CITY", address.city, gedCom);
        writeLine(level+1, "CTRY", address.country, gedCom);
        writeLine(level+1, "POST", address.post, gedCom);
        writeLine(level+1, "STAE", address.state, gedCom);
        writeLine(level+1, "EMAI", address.email, gedCom);
    }

}

void Save::writeNoteStructure(int level, Note note, QString *gedCom)
{
    if (NOTE) {
        if (note.id != 0) writeLine(level, "NOTE", makeTag("note", note.id), gedCom);
        else writeLine(level, "NOTE", note.text, gedCom);

        if (!note.change.isNull()) {
            writeLine(level+1, "CHAN", "", gedCom);
            Date tmp;
            tmp.setDate1(note.change.date());
            writeLine(level+2, "DATE", makeDay(tmp), gedCom);
            writeLine(level+2, "TIME", makeTime(note.change.time()), gedCom); // *!* pit‰‰ testata
        }
    }
}

void Save::writeMmLink(int level, MultiMedia mm, QString *gedCom)
{    
    if (WRITE_MM) {
        if (mm.id != 0) writeLine(level, "OBJE", makeTag("obje", mm.id), gedCom);
        else if (mm.used()){
            writeLine(level, "OBJE", "-", gedCom);
            //writeLine(level+1, "FORM", mmFormatToString(mm.format), pFile);
            writeLine(level+1, "FORM", mmFormatToString(mm.file), gedCom);
            writeLine(level+1, "TITL", mm.title, gedCom);
            writeLine(level+1, "FILE", mm.file, gedCom);
            writeNoteStructure(level+1, mm.note, gedCom);
        }
    }
}

void Save::writeSourceCitation(int level, Source source, QString *gedCom)
{    
    if (SOURCE) {
        if (source.id == 0 && ( (source.text != "") || source.note.used() )) {
            writeLine(level, "SOUR", "-", gedCom);  // n SOUR <SOURCE_DESCRIPTION>  {1:1}
            writeLine(level+1, "TEXT", source.text, gedCom);
            writeNoteStructure(level+1, source.note, gedCom);
        } else if (source.used()) {
            sourceList.append(source.id);
            writeLine(level, "SOUR", makeTag("sour", source.id), gedCom);
            writeLine(level+1, "PAGE", source.page, gedCom);
            writeLine(level+1, "EVEN", source.event.type, gedCom);
            //writeLine(level+2, "ROLE", source.evenRole, pFile);
            if ( source.text != "" ) {
                writeLine(level+1, "DATA", "-", gedCom);
                //if (source.date.known) writeLine(level+2, "DATE", makeDay(source.date), pFile);
                writeLine(level+2, "TEXT", source.text, gedCom);
            }
            if (source.quay != NO_QUAY) {
                if (quayToInt(source.quay) != 0) writeLine(level+1, "QUAY", QString::number(quayToInt(source.quay)), gedCom);
            }
            writeMmLink(level+1, source.multiMedia, gedCom);

            writeNoteStructure(level+1, source.note, gedCom);
        }
    }
}

void Save::writeIndiEvent(Entry entry, QString *gedCom, int adoption)
{    
    if (entry.publishable() && entry.used() && INDI_EVENT) {
        QString line;        
        line = "1 " + Entry::typeGedCom((int)entry.type, true);

        if (entry.type == ADOPTATION) {
            switch (adoption) {
                case 1 : line += " HUSB"; break;
                case 2 : line += " WIFE"; break;
                case 3 : line += " BOTH"; break;
                case 4 : line += " Y"; break;
            }
        }
        else line += " Y";
            // if (entry.entryLineYes) line += " Y";

        line += "\n";

        *gedCom += line;
        if (entry.type == USEREVENT) writeLine(2, "TYPE", entry.attrType, gedCom);
        if (entry.day.known) writeLine(2, "DATE", makeDay(entry.day), gedCom);
        writeAddress(2, entry.address, gedCom);
        if (entry.age != 0) writeLine(2, "AGE", QString::number(entry.age), gedCom);
        writeLine(2, "AGNC", entry.agency, gedCom);
        writeLine(2, "CAUS", entry.cause, gedCom);
        writeSourceCitation(2, entry.source, gedCom);
        writeMmLink(2, entry.multiMedia, gedCom);
        writeNoteStructure(2, entry.note, gedCom);
        writeLine(2, "PLAC", entry.place, gedCom);
        //if (entry.privacyPolicy != 0) writeLine(2, "RESN", restrictionTxt(entry.privacyPolicy), gedCom); //    +1 RESN <RESTRICTION_NOTICE>  {0:1}

        //writeLine(2, "RESN", entry.printResn(), gedCom);
        writeLine(2, "RESN", resn(entry.privacyPolicy), gedCom);
        /*
        if (GENE.printPolicy == PRINTPOLICY_1_EVERYTHING) {
            if (entry.privacyPolicy != 0) writeLine(2, "RESN", GeneData::resn(entry.privacyPolicy), gedCom);
        } else writeLine(2, "RESN", entry.printResn(), gedCom);
*/
    }
}

void Save::writeIndiAttribute(Entry attribute, QString *gedCom)
{
    if (attribute.publishable() && attribute.used() && INDI_ATTRIBUTES) {
        // t‰m‰ on melkein sama kuin eventiss‰, voisiko k‰yt‰‰ uudelleen?
        QString line;
        line = "1 " + Entry::typeGedCom((int)attribute.type, true) + " " + attribute.attrText + "\n";
        *gedCom += line;
        if (attribute.day.known) writeLine(2, "DATE", makeDay(attribute.day), gedCom);
        writeAddress(2, attribute.address, gedCom);
        if (attribute.age != 0) writeLine(2, "AGE", QString::number(attribute.age), gedCom);
        writeLine(2, "AGNC", attribute.agency, gedCom);
        writeLine(2, "CAUS", attribute.cause, gedCom);
        writeSourceCitation(2, attribute.source, gedCom);
        writeMmLink(2, attribute.multiMedia, gedCom);
        writeNoteStructure(2, attribute.note, gedCom);
        writeLine(2, "PLAC", attribute.place, gedCom);
        writeLine(2, "RESN", resn(attribute.privacyPolicy), gedCom);
    }
}

void Save::file(QString filename)
{
    GENE.header.file = filename;
    save(filename);
}

void Save::file()
{
    if (GENE.header.file != "") save(GENE.header.file);
}


QString Save::resn(int r)
{
    switch ((int)r) {
        case 0 : return ""; break; //public
        case 1 : return "confidential"; break;
        case 2 : return "locked" ; break;
        case 3 : return "privacy" ; break;
    }
    return "";
}

void Save::gedComCoder(QString * gedCom)
{
    QString tmpStr;
    QList<int> famUsed;

    GENE.header.source.productName = "qenes";
    GENE.header.source.version = "0.1";

    QString line;
    quint16 i;
    int y;

    *gedCom += "0 HEAD\n";//file.write("0 HEAD\n");
    *gedCom += "1 SOUR " + GENE.header.source.approvedSystemID.toLatin1() + "\n";
    //writeLine(1, "SOUR", gd.header.source.approvedSystemID, &file);
    // headers source-data is not similar to a header-record or header citation, so lets write it in its own routine

    writeLine(2, "VERS", GENE.header.source.version, gedCom);
    writeLine(2, "NAME", GENE.header.source.productName, gedCom);
    if ( GENE.header.source.corporation != "") {
        writeLine(2, "CORP", GENE.header.source.corporation, gedCom);
        writeAddress(3, GENE.header.source.address, gedCom);
        writeLine(2, "PHON", GENE.header.source.phone, gedCom);
    }
    if ( GENE.header.source.name != "") {
        writeLine(2, "DATA", GENE.header.source.name, gedCom);
        //if (header.source.date.known) writeLine(3, "DATE", makeDay(header.source.date), &file);
        writeLine(3, "COPR", GENE.header.source.copyright, gedCom);
    }
    *gedCom += "1 CHAR ASCII\n";

    writeLine(1, "DEST", GENE.header.receivingSystemName, gedCom);
    Date now ; now.setDate1(QDate::currentDate());
    writeLine(1, "DATE", makeDay(now), gedCom);
    writeLine(2, "TIME", makeTime(QTime::currentTime()), gedCom);
    writeLine(1, "SUBM", makeTag("subm", 0), gedCom); // header submitter == 0

    //header.subN;

    //writeLine(1, "FILE", fileName, gedCom);

    *gedCom += "1 GEDC\n";
    *gedCom += "2 VERS 5.5\n";
    *gedCom += "2 FORM LINEAGE-LINKED\n";

    writeLine(1, "COPR", GENE.header.copyright, gedCom);
    writeLine(1, "LANG", GENE.header.language, gedCom);
    writeLine(1, "PLAC", GENE.header.plac, gedCom);
    writeLine(2, "FORM", GENE.header.placForm, gedCom);
    writeLine(1, "NOTE", GENE.header.note, gedCom);
    writeLine(1, "SUBN", GENE.header.subnTag, gedCom); // only one subn, tag can be used

    // -------------------------------------------------------------------------------------------
    for ( i = 1 ; i<=GENE.indiLastUsed ; i++) {
        if (!INDI(i).deleted && INDI(i).selected) {
            line = "0 "+makeTag("i", i) + " INDI\n";
            *gedCom += line;
            tmpStr = "";

            if (INDI(i).publishable()) {
                if (INDI(i).nameFirst != "") tmpStr = INDI(i).printName1();
                if (INDI(i).name2nd != "") tmpStr = tmpStr + " " + INDI(i).printName2();
                if (INDI(i).name3rd != "") tmpStr = tmpStr + " " + INDI(i).printName3();
                if (INDI(i).nameFamily != "") tmpStr = tmpStr + " /" + INDI(i).printNameFamily() + "/";
            } else tmpStr = " /" + INDI(i).printName123Family() + "/";
            writeLine(1, "NAME", tmpStr, gedCom);
// enum PPOLICY { PRINTPOLICY_1_EVERYTHING, PRINTPOLICY_2_GENE, PRINTPOLICY_3_PUBLIC, PRINTPOLICY_4_ALL };
            // siell‰ miss‰ tallennetaan menusta, printPolicy pit‰‰ asentaa 1_everything:n

            //writeLine(1, "RESN", INDI(i).printResn(), gedCom);
            writeLine(1, "RESN", resn(INDI(i).privacyPolicy), gedCom);
            /*
            if (GENE.printPolicy == PRINTPOLICY_1_EVERYTHING) {
                if (INDI(i).privacyPolicy != 0) writeLine(1, "RESN", GeneData::resn(INDI(i).privacyPolicy), gedCom); //    +1 RESN <RESTRICTION_NOTICE>  {0:1}
            } else writeLine(1, "RESN", INDI(i).printResn(), gedCom);
            */
            if (INDI(i).sex == FEMALE) writeLine(1, "SEX", "F", gedCom);
            if (INDI(i).sex == MALE) writeLine(1, "SEX", "M", gedCom);

            for (y=0 ; y<INDI(i).entry.size() ; y++) {
                if (INDI(i).entry.at(y).type < 18) writeIndiEvent(INDI(i).entry.at(y), gedCom, (int)INDI(i).adoptedBy);
                else writeIndiAttribute(INDI(i).entry.at(y), gedCom);
            }

         //    +1 <<INDIVIDUAL_EVENT_STRUCTURE>>  {0:M}
//    +1 <<INDIVIDUAL_ATTRIBUTE_STRUCTURE>>  {0:M}
            if (INDI(i).famc.value != 0) {
                famUsed.append(INDI(i).famc.value);
                writeLine(1, "FAMC", makeTag("f", INDI(i).famc.value), gedCom);
            }
            if (INDI(i).famSList.count() > 0)
                for (y=0 ; y<INDI(i).famSList.count() ; y++) {
                famUsed.append(INDI(i).famSList.value(y));
                writeLine(1, "FAMS", makeTag("f", INDI(i).famSList.value(y)), gedCom);
            }
            if (INDI(i).submitters.count() > 0) for (y=0 ; y<INDI(i).submitters.count() ; y++) writeLine(1, "SUBM", makeTag("i", INDI(i).submitters.value(y)), gedCom);

//    +1 <<ASSOCIATION_STRUCTURE>>  {0:M}
//    +1 ALIA @<XREF:INDI>@  {0:M}
//    +1 ANCI @<XREF:SUBM>@  {0:M}
//    +1 DESI @<XREF:SUBM>@  {0:M}

            writeSourceCitation(1, INDI(i).source, gedCom); //    +1 <<SOURCE_CITATION>>  {0:M}

            //qDebug() << "in";
            //qDebug() << i << gd.rootId;
            if (i == GENE.rootId) *gedCom += "1 NOTE QENES ROOT\n";//writeLine(1, "NOTE", "QENES ROOT", &file);
            //qDebug() << "out";

            if (INDI(i).publishable()) {
                for (y = 0 ; y<INDI(i).multiMedia.count(); y++)
                    writeMmLink(1, INDI(i).multiMedia.value(y), gedCom); //    +1 <<MULTIMEDIA_LINK>>  {0:M}
                writeNoteStructure(1, INDI(i).note, gedCom); //    +1 <<NOTE_STRUCTURE>>  {0:M}
                                                                                // +1 RFN <PERMANENT_RECORD_FILE_NUMBER>  {0:1}
                                                                                // +1 AFN <ANCESTRAL_FILE_NUMBER>  {0:1}
                                                                                // +1 REFN <USER_REFERENCE_NUMBER>  {0:M}
                                                                                // +2 TYPE <USER_REFERENCE_TYPE>  {0:1}
            }
            if (RIN) writeLine(1, "RIN", QString::number(INDI(i).rin), gedCom); // +1 RIN <AUTOMATED_RECORD_ID>  {0:1}

            if (!INDI(i).changed.date().isNull() && CHANGED) {
                line = "1 CHAN\n"; *gedCom += line;
                Date date; date.setDate1(INDI(i).changed.date());
                writeLine(2, "DATE", makeDay(date), gedCom);
                writeLine(3, "TIME", makeTime(INDI(i).changed.time()), gedCom);
            }                                                                   // +1 <<CHANGE_DATE>>  {0:1}

//----------------------------------------------------------------------------------------

        }
    }

    for ( i = 1 ; i<=GENE.famiLastUsed ; i++) {
//        qDebug() << "in1" << i;
        if (!FAM(i).deleted) {
//            qDebug() << "in2" << i;
                if ( famUsed.indexOf(i) != -1) {
//                qDebug() << "out " << i;

                line = "0 "+makeTag("f", i) + " FAM\n";
                *gedCom += line;

                for (y=0 ; y<FAM(i).entry.size() ; y++) writeFamEvent(FAM(i).entry.value(y), gedCom);   //    +1 <<FAMILY_EVENT_STRUCTURE>>  {0:M}

                if ( (FAM(i).husb != 0 ) && ( INDI(FAM(i).husb).selected ) ) writeLine(1, "HUSB", makeTag("i", FAM(i).husb), gedCom); //    +1 HUSB @<XREF:INDI>@  {0:1}
                if ( (FAM(i).wife != 0 ) && ( INDI(FAM(i).wife).selected ) ) writeLine(1, "WIFE", makeTag("i", FAM(i).wife), gedCom); //    +1 WIFE @<XREF:INDI>@  {0:1}

                writeLine(1, "RESN", resn(FAM(i).privacyPolicy), gedCom);
                //writeLine(1, "RESN", FAM(i).printResn(), gedCom);

                //if (FAM(i).privacyPolicy) writeLine(1, "RESN", GeneData::resn(FAM(i).privacyPolicy), gedCom);

                // tulostetaan lapset, lasketaan lapset samalla.
                {
                    int childCount = 0;

                    for (y=0 ; y<FAM(i).childs.count() ; y++) {
                        if ( INDI(FAM(i).childs.value(y)).selected) {
                            childCount++;
                            writeLine(1, "CHIL", makeTag("i", FAM(i).childs.value(y)), gedCom); //    +1 CHIL @<XREF:INDI>@  {0:M}
                        }
                    }

                    writeLine(1, "NCHI", QString::number(childCount), gedCom); // +1 NCHI <COUNT_OF_CHILDREN>  {0:1}
                }

                for (y=0 ; y<FAM(i).subM.count() ; y++)
                    //writeLine(1, "SUBM", makeTag("subm", FAM(i).subM.value(y)), &file);
                    writeLine(1, "SUBM", makeTag("subm", y), gedCom);
                                                                                // +1 SUBM @<XREF:SUBM>@  {0:M}

                for (y=0 ; y<FAM(i).sourCitations.count() ; y++)
                    writeSourceCitation(1, FAM(i).sourCitations.value(y), gedCom);

                for (y=0 ; y<FAM(i).multiMedia.size() ; y++)
                    writeMmLink(1, FAM(i).multiMedia.at(y), gedCom);

                writeNoteStructure(1, FAM(i).note, gedCom);
                                                                                // +1 REFN <USER_REFERENCE_NUMBER>  {0:M}
                                                                                // +2 TYPE <USER_REFERENCE_TYPE>  {0:1}
                if (RIN) writeLine(1, "RIN", QString::number(i), gedCom);       // +1 RIN <AUTOMATED_RECORD_ID>  {0:1}
                                                                                // +1 <<CHANGE_DATE>>  {0:1}
                if (i == GENE.rootFams) writeLine(1, "NOTE", "QENES ROOT", gedCom);

                writeSourceCitation(1, FAM(i).source, gedCom);
                /*
                    n SOUR @<XREF:SOUR>@ {0:1}
                    +1 PAGE <WHERE_WITHIN_SOURCE> {0:1}
                    +1 EVEN <EVENT_TYPE_CITED_FROM> {0:1}
                    +2 ROLE <ROLE_IN_EVENT> {0:1}
                    +1 DATA {0:M}
                    +2 DATE <ENTRY_RECORDING_DATE> {0:M}
                    +2 TEXT <TEXT_FROM_SOURCE> {0:M}
                    +3 [CONC|CONT] <TEXT_FROM_SOURCE> {0:M}
                    +1 <<MULTIMEDIA_LINK>> {0:1}
                    +1 <<NOTE_STRUCTURE>>
                    +1 QUAY <CERTAINTY_ASSESSMENT>

                  */
            }
        }
    }

    if (SOURCE) {
        for (i = 1 ; i <= GENE.sourLastUsed ; i++) {
            //if (sourceList.contains(i) && !GENE.sour[i].data.deleted) {
            if ( !GENE.sour[i].deleted ) {
                line = "0 " + makeTag("sour", i) + " SOUR\n";
                *gedCom += line; // n  @<XREF:SOUR>@ SOUR  {1:1}

                if (GENE.sour[i].event.type != "" || GENE.sour[i].event.place != "" || GENE.sour[i].event.agency != "") {
                    writeLine(1, "DATA", "-", gedCom);
                    writeLine(2, "EVEN", GENE.sour[i].event.type, gedCom); //      +2 EVEN <EVENTS_RECORDED>  {0:M}
                    if (GENE.sour[i].event.date.known) writeLine(3, "DATE", makeDay(GENE.sour[i].event.date), gedCom); //        +3 DATE <DATE_PERIOD>  {0:1}
                    writeLine(3, "PLAC", GENE.sour[i].event.place, gedCom) ;//        +3 PLAC <SOURCE_JURISDICTION_PLACE>  {0:1}
                    writeLine(2, "AGNC", GENE.sour[i].event.agency, gedCom);//      +2 AGNC <RESPONSIBLE_AGENCY>  {0:1}
                    writeNoteStructure(2, GENE.sour[i].note, gedCom);      // +2 <<NOTE_STRUCTURE>>  {0:M}
                }

                writeLine(1, "AUTH", GENE.sour[i].author, gedCom);         // +1 AUTH <SOURCE_ORIGINATOR>  {0:1}
                writeLine(1, "TITL", GENE.sour[i].title, gedCom);          // +1 TITL <SOURCE_DESCRIPTIVE_TITLE>  {0:1}
                writeLine(1, "ABBR", GENE.sour[i].abbreviation, gedCom);   // +1 ABBR <SOURCE_FILED_BY_ENTRY>  {0:1}
                writeLine(1, "PUBL", GENE.sour[i].publication, gedCom);    // +1 PUBL <SOURCE_PUBLICATION_FACTS>  {0:1}
                writeLine(1, "TEXT", GENE.sour[i].text, gedCom);           // +1 TEXT <TEXT_FROM_SOURCE>  {0:1}
                                                                                // +1 <<SOURCE_REPOSITORY_CITATION>>  {0:1}
                writeMmLink(1, GENE.sour[i].multiMedia, gedCom);           // +1 <<MULTIMEDIA_LINK>>  {0:M}
                writeNoteStructure(1, GENE.sour[i].note, gedCom);          // +2 <<NOTE_STRUCTURE>>  {0:M} //    +1 <<NOTE_STRUCTURE>>  {0:M}
                                                                                // +1 REFN <USER_REFERENCE_NUMBER>  {0:M}
                                                                                // +2 TYPE <USER_REFERENCE_TYPE>  {0:1}
                if (RIN) writeLine(1, "RIN", QString::number(i), gedCom);       // +1 RIN <AUTOMATED_RECORD_ID>  {0:1}
                                                                                // +1 <<CHANGE_DATE>>  {0:1}
            }
        }
    }

    if (SUBMITTER) {
        line = "0 " + makeTag("subm", 0) + " SUBM\n";
        *gedCom += line;                                                        //  n  @<XREF:SUBM>@   SUBM {1:1}
        writeLine(1, "NAME", GENE.subm.name, gedCom);                           // +1 NAME <SUBMITTER_NAME>  {1:1}
        writeAddress(1, GENE.subm.address, gedCom);                             // +1 <<ADDRESS_STRUCTURE>>  {0:1}
        writeMmLink(1, GENE.subm.multimedia, gedCom);                           // +1 <<MULTIMEDIA_LINK>>  {0:M}
        writeLine(1, "LANG", GENE.subm.language, gedCom);                       // +1 LANG <LANGUAGE_PREFERENCE>  {0:3}
        writeLine(1, "RFN", GENE.subm.rfn, gedCom);                             // +1 RFN <SUBMITTER_REGISTERED_RFN>  {0:1}
        if (RIN) writeLine(1, "RIN", GENE.subm.rin, gedCom);                    // +1 RIN <AUTOMATED_RECORD_ID>  {0:1}
                                                                                // +1 <<CHANGE_DATE>>  {0:1}
    }

    writeLine(0, "TRLR\n", "-", gedCom);
}

void Save::save(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
        INDI(0).selectAll();
        QString saveString;
        gedComCoder(&saveString);
        file.write(saveString.toLatin1()); // tallentaa iso8851-1, t‰t‰ kannattaa k‰ytt‰‰

        INDI(0).selectNone();
    }

    file.close();
}
