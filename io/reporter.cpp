/*
 *
 *  qenes genealogy software
 *
 *  Copyright (C) 2009-2010 Ari TÃ¤hti
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License veasion 2 as
 *  published by the Free Software Foundation.
 */


#include "reporter.h"
#include "data/setupvalues.h"
#include "data/genedata.h"
#include "io/savefile.h"
#include "macro.h"
#include <QTextDocument>
#include <QtPrintSupport/QPrinter>
#include <QUrl>
#include <QSettings>
#include <QResource>

#define COLOR_HTML_MALE "#ccffff"
#define COLOR_HTML_FEMALE "#ffcccc"

#define SHOW 0
#define DONT_SHOW 1
#define CHANGE_TO_TEXT 2

#define PINDI(indix) geneData[geneDataId].indi[indix]
#define PFAMI(famx) geneData[geneDataId].fami[famx]

extern GeneData        * geneData;
extern QSettings            sets;



QString Reporter::makeProblems()
{
    return "";

    // ongelmia ei tässä versiossa tehdä, korjaa joskus...
    /*
    if (!GENE.problems.size()) return "";

    QString out;
    quint16 problemId;

    out += "<br><big>Problem list></big><br><br>\n<table><tr><td>Id</td><td>Explanation</td><td>Type</td></td><td>Attribute</td>\n";
    for (problemId = 0 ; problemId < GENE.problems.size() ; problemId++) {
        QString line = "<tr><td>columnA</td><td>columnB</td><td>columnC</td></td><td>columnD</td>\n";
        line.replace("columnA", QString::number(problemId));
        line.replace("columnB", GENE.problems.value(problemId).explanation);
        line.replace("columnC", GENE.problems.value(problemId).typeToString());
        line.replace("columnD", GENE.problems.value(problemId).attribute);
        out += line;
    }
    out += "</table>";

    return out;
    */
}

bool Reporter::printableChilds(QList<int> *tables, QList<quint16> *childList)
{
    if ( !childList->size() ) return false;

    for (quint8 child = 0 ; child<childList->size() ; child++) if (!tables->contains(childList->at(child))) return false;

    return true;
}

QString Reporter::getFileEnd(QString fileName)
{
    if (fileName.contains("/") || fileName.contains("\\")) {
        // filename is in form "c;\directory\diipadaapa\file.name"
        // This will return "file.name"
        QRegExp qre("[.*\\\\|.*\\/]([^\\\\.|^\\/]+\\.\\S{3})");
        qre.indexIn(fileName);
        return qre.cap(1);
    }
    return fileName;
}

QString Reporter::addMultimedia(MultiMedia mm, QString dirName)
{
    QString result = "";

    if (mm.used()) {

        result = "<img src=\"FILENAME\" height = \"400\" width=\"400\">";
        result.replace("FILENAME", getFileEnd(mm.file));

        QFile::copy(GET_DIRECTORY + "/" + mm.file, dirName + "/" + getFileEnd(mm.file));

        QImage bmp;
        bmp.load(GET_DIRECTORY + "/" + mm.file);
        bmp = bmp.scaledToHeight(400, Qt::SmoothTransformation).scaledToWidth(400, Qt::SmoothTransformation);

        QUrl url;
        QString resourceName = getFileEnd(mm.file);
        url.setUrl(resourceName);

        pPdfText->addResource(
            QTextDocument::ImageResource,
            url,
            QVariant(bmp));
    }

    return result;
}


QString Reporter::makeIndex()
{
// lataa pohja

    QFile file(":/print/index.html");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString index = file.readAll();
    file.close();

    index.replace("DATE", QDate::currentDate().toString("dd.MM.yyyy"));
    index.replace("NAME", GENE.subm.name);
    index.replace("LANGUAGE", GENE.subm.language);
    index.replace("ADDRESS1", GENE.subm.address.line);
    index.replace("ADDRESS2", GENE.subm.address.line1);
    index.replace("ADDRESS3", GENE.subm.address.line2);
    index.replace("CITY", GENE.subm.address.city);
    index.replace("STATE", GENE.subm.address.state);
    index.replace("POSTOFFICE", GENE.subm.address.post);
    index.replace("COUNTRY", GENE.subm.address.country);
    index.replace("INDICOUNT", QString::number(GENE.indiLastUsed));
    index.replace("FAMCOUNT", QString::number(GENE.famiLastUsed));
    if (pPrintPdf) index.replace("PDF_LINK", "<a href=\"all.pdf\">Open a pdf report</a>");
    if (pPrintJava) index.replace("JAVA_LINK", "<a href=\"qenesBrowser.html\">Open in qenesBrowser Java Applet</a>");

    index += makeProblems();

    index.replace("LETTERLIST", letterList);

    return index;

}

int Reporter::descendantTableBuilder(bool *newCol, quint16 thisId, QList<int> *rivit, QList<int> *henk, QString *output)
{

    int childCount = 0;
    QList<quint16> childList;

    INDI(thisId).getChilds(&childList, false);

    if (*newCol) {
        *output += "<tr>\n";
        *newCol = false;
    }

    QString tag = QString("%1").arg(QString::number(thisId), 5, '0');
    *output += "<td valign=\"top\" rowspan = ROWS" + tag + " bgcolor=\"COLOR" + tag + "\"><small> NAME" + tag + "</small> </td>\n";

    for (int c=0 ; c<childList.size() ; c++) childCount += descendantTableBuilder(newCol, childList.at(c), rivit, henk, output);

    // tässähän lasketaan rivejä. Jos henkilöllä on 0 lasta, hän tarvii yhden rivin itseään varten, jos hänellä on
    // 1 tai useampi lasta, hän tarvii yhtä monta riviä kuin lapsia on
    if (childCount == 0) {
        childCount = 1;
        // tällä henkilöllä ei ollut enää lapsia, eli sarake loppuu
        *output += "</tr>\n";
        *newCol = true;
    }

    rivit->append(childCount);
    henk->append(thisId);

    return childCount;
}

QString Reporter::makeEventCalendar(QList<int> *tables)
{
    QList<QString> kk[12];

    int i, j, p;

    for (i=0 ; i<12 ; i++) { for (j=0 ; j<32 ; j++) kk[i].append(""); }

    for (i=1 ; i<tables->size() ; i++) {
        QString out = "";
        p = tables->at(i);

        for (j=0 ; j<PINDI(p).entry.size() ; j++) {
            Entry entry = PINDI(p).entry.at(j);

            if ( entry.effectivePrivacyMethod() == PRIVACY_0_SHOW ) {

                Date koe1 = PINDI(p).entry.at(j).day;
                if (koe1.getAcc() == EXA ) {
                    QString koe = QString::number(koe1.day1.year());
                    QString nimi = PINDI(p).nameFamily + " " + PINDI(p).nameFirst;
                    nimi = makeLink(nimi, p,tables);
                    out = nimi + PINDI(p).entry.value(j).typeString() + " " + koe;

                    QString old = kk[koe1.day1.month()-1].at(koe1.day1.day());
                    if (old != "") old += "<BR>";
                    old = old + "\n" + out;
                    kk[koe1.day1.month()-1][koe1.day1.day()] = old;
                }
            }
        }


        for (int k=0 ; k<=PINDI(p).famSList.size() ; k++) {
            PINDI(p).currentSpouse = k;

            int family = PINDI(p).getFamS();

            int husb = PFAMI(family).husb;
            int wife = PFAMI(family).wife;

            if ( (PINDI(p).sex == MALE && husb != 0) || (PINDI(p).sex == FEMALE && (husb == 0 && wife !=0 ) ) ) {

                for (j=0 ; j<PFAMI(family).entry.size() ; j++) {
                    Entry entry = PFAMI(family).entry.at(j);
                    if ( entry.effectivePrivacyMethod() == PRIVACY_0_SHOW ) {

                        Date koe1 = PFAMI(family).entry.at(j).day;
                        if (koe1.getAcc() == EXA) {
                            QString koe = QString::number(koe1.day1.year());
                            QString nameWife = PINDI(PFAMI(family).wife).nameFamily + " " + PINDI(PFAMI(family).wife).nameFirst;
                            QString nameHusb = PINDI(PFAMI(family).husb).nameFamily + " " + PINDI(PFAMI(family).husb).nameFirst;
                            nameWife = makeLink(nameWife, PFAMI(family).wife, tables);
                            nameHusb = makeLink(nameHusb, PFAMI(family).husb, tables);
                            out = nameWife + "&" + nameHusb + PFAMI(family).entry.value(j).typeString() + " " + koe;

                            QString old = kk[koe1.day1.month()-1].at(koe1.day1.day());
                            if (old != "") old += "<BR>";
                            old = old + out;
                            kk[koe1.day1.month()-1][koe1.day1.day()] = old;
                        }
                    }
                }
            }
            PINDI(p).currentSpouse = 0;
        }
    }

    QFile file;
    file.setFileName(":/print/calendar.html");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString cal= file.readAll();//.readLine();
    file.close();

    for (i=1 ; i<13 ; i++) {
        for (j=1 ; j<32 ; j++) {
            {
                QString x = QString("%1").arg(QString::number(i), 2, '0') + "-" + QString("%1").arg(QString::number(j), 2, '0');
                // x on muotoa kk-dd esim 02-01 tai 12-24

                cal.replace(x, kk[i-1].at(j));
            }
        }
    }
    return cal;
}

void Reporter::printStep(quint16 id, QList<int> *tables)
{
    QList<quint16> childID;
    int j, i;

    if (!tables->contains(id)) tables->append(id);

    for (i=0 ; i<PINDI(id).famSList.size() ; i++) {
        PINDI(id).currentSpouse = i;

        if ( PINDI(PINDI(id).getSpouse()).selectedSystem && !tables->contains(PINDI(id).getSpouse()))
            tables->append(PINDI(id).getSpouse());

        /* Jos ollaan valittu "include all cousins" ja "include both parents", valitaan tÃ¤ssÃ¤ kÃ¤ytÃ¤nnÃ¶ssÃ¤ se serkusten toinen
           vanhempi joka ei ole suoraan sukua.
           */

        if ( ( GET_PRINT_BOTHPARENTS ) && !PINDI(PINDI(id).getSpouse()).selectedSystem && !tables->contains(PINDI(id).getSpouse()) )
            tables->append(PINDI(id).getSpouse());

        PINDI(id).getChilds(&childID, false);
        GENE.arrangePersons(&childID);

        for (j=0 ; j<childID.size() ; j++) {
            if ( ( GET_PRINT_ANCDEC ) ||
                 ( !GET_PRINT_ANCDEC && PINDI(childID.at(j)).selectedSystem ) ) printStep(childID.at(j), tables);
        }
    }

    PINDI(id).currentSpouse = 0;
}

QString Reporter::fileName(int i)
{
    return QString("%1").arg(QString::number(i), 5, '0') + ".html";
}

/*
QString Reporter::makeReferenceLink(QString name, quint16 id, QList<int> *tables)
{
    if (tables->contains(id)) return "<a href=\"#" + QString("%1").arg(QString::number(id), 5, '0') + "\">" + name + "</a>";
    return QString();
}
*/

QString Reporter::makeLink(QString name, quint16 id, QList<int> *tables)
{
    // Linkki voi olla jompaakumpaa muotoa riippuen siitä onko 1*html tai n*html
    //      <a href="00135.html">Wallenius Anna Camilla </a>
    //      <a href="#00135"Wallenius Anna Camilla </a>
    // Tehdään tällainen linkki
    //      <a href="LINK_A000135LINKB
    // Vaihdetaan tapauskohtaisesti
    //      LINK_A joko # tai ""
    //      LINK_B joko "" tai .html
    if (tables->contains(id)) return "<a href=\"LINK_A" + QString("%1").arg(QString::number(id), 5, '0') + "LINK_B\">"+ name + "</a>"; else return name;

}

void Reporter::wwwReplacer(QString str, quint16 id, QString *tree, QList<int> *tables)
{

    QString name = str + "-NAME";
    QString birth = str + "-BIRTH";
    QString death = str + "-DEATH";
    QString marriage = str + "-MARRIAGE";

    if (id>0) {
        QString output;

        tree->replace(name, makeLink(PINDI(id).printName(PINDI(id).printNameFamily()) + " " + PINDI(id).printName1(), id, tables));

        output = PINDI(id).ientry(BIRTH).printDay();

        if (output != "") tree->replace(birth, "&#9734;" + output); else tree->replace(birth, "");

        output = PINDI(id).ientry(DEATH).printDay();
        if (output != "") tree->replace(death, "&dagger;" + output); else tree->replace(death, "");

        output = PINDI(id).fentry(MARRIAGE).printDay();
        if (output != "") tree->replace(marriage, "&hearts;" + output); else tree->replace(marriage, "");
    }
    else  /// id == 0, eli tätä henkilöä ei ole, eli usein isoisoiso...vanhemman
    {
        tree->replace(name, "(?)");
        tree->replace(birth, "");
        tree->replace(death, "");
        tree->replace(marriage, "(?)");
    }
}


QString Reporter::tableId(int p, QList<int> *tables)
{
    QString indexTxt;
    if (tables->indexOf(p) != -1) indexTxt = QString::number(tables->indexOf(p)); else indexTxt = "NA";

    return indexTxt + " (" + QString::number(p) + ")";
    return "";
}

QString Reporter::makeHtmlFamilyTreeSmall(quint16 p)
{
    QString tree = "";

    genTree0 = familytree12_0gp;
    genTree1 = familytree12_1gp;

    int f = PINDI(p).getFather();
    int m = PINDI(p).getMother();

    if (f || m) {
        wwwReplacer("F", f, &genTree0, &tables);
        wwwReplacer("M", m, &genTree0, &tables);

        tree += genTree0;

        int ff = PINDI(f).getFather();
        int fm = PINDI(f).getMother();
        int mf = PINDI(m).getFather();
        int mm = PINDI(m).getMother();

        if (ff || fm || mf || mm) {
            wwwReplacer("FF", ff, &genTree1, &tables);
            wwwReplacer("FM", fm, &genTree1, &tables);
            wwwReplacer("MF", mf, &genTree1, &tables);
            wwwReplacer("MM", mm, &genTree1, &tables);

            tree += genTree1;

        }
    }
    return tree;
}

QString Reporter::makeHtmlFamilyTreeMore(quint16 p)
{
    QString tree;

    genTree2 = familytree12_2gp;
    genTree3 = familytree12_3gp;
    genTree4 = familytree12_4gp;
    genTree5 = familytree12_5gp;

    int f = PINDI(p).getFather();
    int m = PINDI(p).getMother();
    int ff = PINDI(f).getFather();
    int fm = PINDI(f).getMother();
    int mf = PINDI(m).getFather();
    int mm = PINDI(m).getMother();

    int fff = PINDI(ff).getFather();
    int ffm = PINDI(ff).getMother();
    int fmf = PINDI(fm).getFather();
    int fmm = PINDI(fm).getMother();
    int mff = PINDI(mf).getFather();
    int mfm = PINDI(mf).getMother();
    int mmf = PINDI(mm).getFather();
    int mmm = PINDI(mm).getMother();

    if (fff || ffm || fmf || fmm || mff || mfm || mmf || mmm) {
        wwwReplacer("FFF", fff, &genTree2, &tables);
        wwwReplacer("FFM", ffm, &genTree2, &tables);
        wwwReplacer("FMF", fmf, &genTree2, &tables);
        wwwReplacer("FMM", fmm, &genTree2, &tables);
        wwwReplacer("MFF", mff, &genTree2, &tables);
        wwwReplacer("MFM", mfm, &genTree2, &tables);
        wwwReplacer("MMF", mmf, &genTree2, &tables);
        wwwReplacer("MMM", mmm, &genTree2, &tables);

        tree += genTree2;

        int ffff = PINDI(fff).getFather();
        int fffm = PINDI(fff).getMother();
        int ffmf = PINDI(ffm).getFather();
        int ffmm = PINDI(ffm).getMother();
        int fmff = PINDI(fmf).getFather();
        int fmfm = PINDI(fmf).getMother();
        int fmmf = PINDI(fmm).getFather();
        int fmmm = PINDI(fmm).getMother();
        int mfff = PINDI(mff).getFather();
        int mffm = PINDI(mff).getMother();
        int mfmf = PINDI(mfm).getFather();
        int mfmm = PINDI(mfm).getMother();
        int mmff = PINDI(mmf).getFather();
        int mmfm = PINDI(mmf).getMother();
        int mmmf = PINDI(mmm).getFather();
        int mmmm = PINDI(mmm).getMother();

        if (ffff || fffm || ffmf || ffmm || fmff || fmfm || fmmf || fmmm || mfff || mffm || mfmf || mfmm || mmff || mmfm || mmmf || mmmm) {
            wwwReplacer("FFFF", ffff, &genTree3, &tables);
            wwwReplacer("FFFM", fffm, &genTree3, &tables);
            wwwReplacer("FFMF", ffmf, &genTree3, &tables);
            wwwReplacer("FFMM", ffmm, &genTree3, &tables);
            wwwReplacer("FMFF", fmff, &genTree3, &tables);
            wwwReplacer("FMFM", fmfm, &genTree3, &tables);
            wwwReplacer("FMMF", fmmf, &genTree3, &tables);
            wwwReplacer("FMMM", fmmm, &genTree3, &tables);
            wwwReplacer("MFFF", mfff, &genTree3, &tables);
            wwwReplacer("MFFM", mffm, &genTree3, &tables);
            wwwReplacer("MFMF", mfmf, &genTree3, &tables);
            wwwReplacer("MFMM", mfmm, &genTree3, &tables);
            wwwReplacer("MMFF", mmff, &genTree3, &tables);
            wwwReplacer("MMFM", mmfm, &genTree3, &tables);
            wwwReplacer("MMMF", mmmf, &genTree3, &tables);
            wwwReplacer("MMMM", mmmm, &genTree3, &tables);

            tree += genTree3;

            int fffff = PINDI(ffff).getFather();
            int ffffm = PINDI(ffff).getMother();
            int fffmf = PINDI(fffm).getFather();
            int fffmm = PINDI(fffm).getMother();
            int ffmff = PINDI(ffmf).getFather();
            int ffmfm = PINDI(ffmf).getMother();
            int ffmmf = PINDI(ffmm).getFather();
            int ffmmm = PINDI(ffmm).getMother();
            int fmfff = PINDI(fmff).getFather();
            int fmffm = PINDI(fmff).getMother();
            int fmfmf = PINDI(fmfm).getFather();
            int fmfmm = PINDI(fmfm).getMother();
            int fmmff = PINDI(fmmf).getFather();
            int fmmfm = PINDI(fmmf).getMother();
            int fmmmf = PINDI(fmmm).getFather();
            int fmmmm = PINDI(fmmm).getMother();
            int mffff = PINDI(mfff).getFather();
            int mfffm = PINDI(mfff).getMother();
            int mffmf = PINDI(mffm).getFather();
            int mffmm = PINDI(mffm).getMother();
            int mfmff = PINDI(mfmf).getFather();
            int mfmfm = PINDI(mfmf).getMother();
            int mfmmf = PINDI(mfmm).getFather();
            int mfmmm = PINDI(mfmm).getMother();
            int mmfff = PINDI(mmff).getFather();
            int mmffm = PINDI(mmff).getMother();
            int mmfmf = PINDI(mmfm).getFather();
            int mmfmm = PINDI(mmfm).getMother();
            int mmmff = PINDI(mmmf).getFather();
            int mmmfm = PINDI(mmmf).getMother();
            int mmmmf = PINDI(mmmm).getFather();
            int mmmmm = PINDI(mmmm).getMother();

            if (fffff || ffffm || fffmf || fffmm || ffmff || ffmfm || ffmmf || ffmmm || fmfff || fmffm || fmfmf || fmfmm || fmmff || fmmfm || fmmmf || fmmmm || mffff || mfffm || mffmf || mffmm || mfmff || mfmfm || mfmmf || mfmmm || mmfff || mmffm || mmfmf || mmfmm || mmmff || mmmfm || mmmmf || mmmmm) {
                wwwReplacer("FFFFF", fffff, &genTree4, &tables);
                wwwReplacer("FFFFM", ffffm, &genTree4, &tables);
                wwwReplacer("FFFMF", fffmf, &genTree4, &tables);
                wwwReplacer("FFFMM", fffmm, &genTree4, &tables);
                wwwReplacer("FFMFF", ffmff, &genTree4, &tables);
                wwwReplacer("FFMFM", ffmfm, &genTree4, &tables);
                wwwReplacer("FFMMF", ffmmf, &genTree4, &tables);
                wwwReplacer("FFMMM", ffmmm, &genTree4, &tables);
                wwwReplacer("FMFFF", fmfff, &genTree4, &tables);
                wwwReplacer("FMFFM", fmffm, &genTree4, &tables);
                wwwReplacer("FMFMF", fmfmf, &genTree4, &tables);
                wwwReplacer("FMFMM", fmfmm, &genTree4, &tables);
                wwwReplacer("FMMFF", fmmff, &genTree4, &tables);
                wwwReplacer("FMMFM", fmmfm, &genTree4, &tables);
                wwwReplacer("FMMMF", fmmmf, &genTree4, &tables);
                wwwReplacer("FMMMM", fmmmm, &genTree4, &tables);
                wwwReplacer("MFFFF", mffff, &genTree4, &tables);
                wwwReplacer("MFFFM", mfffm, &genTree4, &tables);
                wwwReplacer("MFFMF", mffmf, &genTree4, &tables);
                wwwReplacer("MFFMM", mffmm, &genTree4, &tables);
                wwwReplacer("MFMFF", mfmff, &genTree4, &tables);
                wwwReplacer("MFMFM", mfmfm, &genTree4, &tables);
                wwwReplacer("MFMMF", mfmmf, &genTree4, &tables);
                wwwReplacer("MFMMM", mfmmm, &genTree4, &tables);
                wwwReplacer("MMFFF", mmfff, &genTree4, &tables);
                wwwReplacer("MMFFM", mmffm, &genTree4, &tables);
                wwwReplacer("MMFMF", mmfmf, &genTree4, &tables);
                wwwReplacer("MMFMM", mmfmm, &genTree4, &tables);
                wwwReplacer("MMMFF", mmmff, &genTree4, &tables);
                wwwReplacer("MMMFM", mmmfm, &genTree4, &tables);
                wwwReplacer("MMMMF", mmmmf, &genTree4, &tables);
                wwwReplacer("MMMMM", mmmmm, &genTree4, &tables);

                tree += genTree4;

                int ffffff = PINDI(fffff).getFather();
                int fffffm = PINDI(fffff).getMother();
                int ffffmf = PINDI(ffffm).getFather();
                int ffffmm = PINDI(ffffm).getMother();
                int fffmff = PINDI(fffmf).getFather();
                int fffmfm = PINDI(fffmf).getMother();
                int fffmmf = PINDI(fffmm).getFather();
                int fffmmm = PINDI(fffmm).getMother();
                int ffmfff = PINDI(ffmff).getFather();
                int ffmffm = PINDI(ffmff).getMother();
                int ffmfmf = PINDI(ffmfm).getFather();
                int ffmfmm = PINDI(ffmfm).getMother();
                int ffmmff = PINDI(ffmmf).getFather();
                int ffmmfm = PINDI(ffmmf).getMother();
                int ffmmmf = PINDI(ffmmm).getFather();
                int ffmmmm = PINDI(ffmmm).getMother();
                int fmffff = PINDI(fmfff).getFather();
                int fmfffm = PINDI(fmfff).getMother();
                int fmffmf = PINDI(fmffm).getFather();
                int fmffmm = PINDI(fmffm).getMother();
                int fmfmff = PINDI(fmfmf).getFather();
                int fmfmfm = PINDI(fmfmf).getMother();
                int fmfmmf = PINDI(fmfmm).getFather();
                int fmfmmm = PINDI(fmfmm).getMother();
                int fmmfff = PINDI(fmmff).getFather();
                int fmmffm = PINDI(fmmff).getMother();
                int fmmfmf = PINDI(fmmfm).getFather();
                int fmmfmm = PINDI(fmmfm).getMother();
                int fmmmff = PINDI(fmmmf).getFather();
                int fmmmfm = PINDI(fmmmf).getMother();
                int fmmmmf = PINDI(fmmmm).getFather();
                int fmmmmm = PINDI(fmmmm).getMother();
                int mfffff = PINDI(mffff).getFather();
                int mffffm = PINDI(mffff).getMother();
                int mfffmf = PINDI(mfffm).getFather();
                int mfffmm = PINDI(mfffm).getMother();
                int mffmff = PINDI(mffmf).getFather();
                int mffmfm = PINDI(mffmf).getMother();
                int mffmmf = PINDI(mffmm).getFather();
                int mffmmm = PINDI(mffmm).getMother();
                int mfmfff = PINDI(mfmff).getFather();
                int mfmffm = PINDI(mfmff).getMother();
                int mfmfmf = PINDI(mfmfm).getFather();
                int mfmfmm = PINDI(mfmfm).getMother();
                int mfmmff = PINDI(mfmmf).getFather();
                int mfmmfm = PINDI(mfmmf).getMother();
                int mfmmmf = PINDI(mfmmm).getFather();
                int mfmmmm = PINDI(mfmmm).getMother();
                int mmffff = PINDI(mmfff).getFather();
                int mmfffm = PINDI(mmfff).getMother();
                int mmffmf = PINDI(mmffm).getFather();
                int mmffmm = PINDI(mmffm).getMother();
                int mmfmff = PINDI(mmfmf).getFather();
                int mmfmfm = PINDI(mmfmf).getMother();
                int mmfmmf = PINDI(mmfmm).getFather();
                int mmfmmm = PINDI(mmfmm).getMother();
                int mmmfff = PINDI(mmmff).getFather();
                int mmmffm = PINDI(mmmff).getMother();
                int mmmfmf = PINDI(mmmfm).getFather();
                int mmmfmm = PINDI(mmmfm).getMother();
                int mmmmff = PINDI(mmmmf).getFather();
                int mmmmfm = PINDI(mmmmf).getMother();
                int mmmmmf = PINDI(mmmmm).getFather();
                int mmmmmm = PINDI(mmmmm).getMother();

                if (ffffff || fffffm || ffffmf || ffffmm || fffmff || fffmfm || fffmmf || fffmmm || ffmfff || ffmffm || ffmfmf || ffmfmm || ffmmff || ffmmfm || ffmmmf || ffmmmm || fmffff || fmfffm || fmffmf || fmffmm || fmfmff || fmfmfm || fmfmmf || fmfmmm || fmmfff || fmmffm || fmmfmf || fmmfmm || fmmmff || fmmmfm || fmmmmf ||fmmmmm || mfffff || mffffm || mfffmf || mfffmm || mffmff || mffmfm || mffmmf || mffmmm || mfmfff || mfmffm || mfmfmf || mfmfmm || mfmmff || mfmmfm || mfmmmf || mfmmmm || mmffff || mmfffm || mmffmf || mmffmm || mmfmff || mmfmfm || mmfmmf || mmfmmm || mmmfff || mmmffm || mmmfmf || mmmfmm || mmmmff || mmmmfm || mmmmmf || mmmmmm) {
                    wwwReplacer("FFFFFF", ffffff, &genTree5, &tables);
                    wwwReplacer("FFFFFM", fffffm, &genTree5, &tables);
                    wwwReplacer("FFFFMF", ffffmf, &genTree5, &tables);
                    wwwReplacer("FFFFMM", ffffmm, &genTree5, &tables);
                    wwwReplacer("FFFMFF", fffmff, &genTree5, &tables);
                    wwwReplacer("FFFMFM", fffmfm, &genTree5, &tables);
                    wwwReplacer("FFFMMF", fffmmf, &genTree5, &tables);
                    wwwReplacer("FFFMMM", fffmmm, &genTree5, &tables);
                    wwwReplacer("FFMFFF", ffmfff, &genTree5, &tables);
                    wwwReplacer("FFMFFM", ffmffm, &genTree5, &tables);
                    wwwReplacer("FFMFMF", ffmfmf, &genTree5, &tables);
                    wwwReplacer("FFMFMM", ffmfmm, &genTree5, &tables);
                    wwwReplacer("FFMMFF", ffmmff, &genTree5, &tables);
                    wwwReplacer("FFMMFM", ffmmfm, &genTree5, &tables);
                    wwwReplacer("FFMMMF", ffmmmf, &genTree5, &tables);
                    wwwReplacer("FFMMMM", ffmmmm, &genTree5, &tables);
                    wwwReplacer("FMFFFF", fmffff, &genTree5, &tables);
                    wwwReplacer("FMFFFM", fmfffm, &genTree5, &tables);
                    wwwReplacer("FMFFMF", fmffmf, &genTree5, &tables);
                    wwwReplacer("FMFFMM", fmffmm, &genTree5, &tables);
                    wwwReplacer("FMFMFF", fmfmff, &genTree5, &tables);
                    wwwReplacer("FMFMFM", fmfmfm, &genTree5, &tables);
                    wwwReplacer("FMFMMF", fmfmmf, &genTree5, &tables);
                    wwwReplacer("FMFMMM", fmfmmm, &genTree5, &tables);
                    wwwReplacer("FMMFFF", fmmfff, &genTree5, &tables);
                    wwwReplacer("FMMFFM", fmmffm, &genTree5, &tables);
                    wwwReplacer("FMMFMF", fmmfmf, &genTree5, &tables);
                    wwwReplacer("FMMFMM", fmmfmm, &genTree5, &tables);
                    wwwReplacer("FMMMFF", fmmmff, &genTree5, &tables);
                    wwwReplacer("FMMMFM", fmmmfm, &genTree5, &tables);
                    wwwReplacer("FMMMMF", fmmmmf, &genTree5, &tables);
                    wwwReplacer("FMMMMM", fmmmmm, &genTree5, &tables);
                    wwwReplacer("MFFFFF", mfffff, &genTree5, &tables);
                    wwwReplacer("MFFFFM", mffffm, &genTree5, &tables);
                    wwwReplacer("MFFFMF", mfffmf, &genTree5, &tables);
                    wwwReplacer("MFFFMM", mfffmm, &genTree5, &tables);
                    wwwReplacer("MFFMFF", mffmff, &genTree5, &tables);
                    wwwReplacer("MFFMFM", mffmfm, &genTree5, &tables);
                    wwwReplacer("MFFMMF", mffmmf, &genTree5, &tables);
                    wwwReplacer("MFFMMM", mffmmm, &genTree5, &tables);
                    wwwReplacer("MFMFFF", mfmfff, &genTree5, &tables);
                    wwwReplacer("MFMFFM", mfmffm, &genTree5, &tables);
                    wwwReplacer("MFMFMF", mfmfmf, &genTree5, &tables);
                    wwwReplacer("MFMFMM", mfmfmm, &genTree5, &tables);
                    wwwReplacer("MFMMFF", mfmmff, &genTree5, &tables);
                    wwwReplacer("MFMMFM", mfmmfm, &genTree5, &tables);
                    wwwReplacer("MFMMMF", mfmmmf, &genTree5, &tables);
                    wwwReplacer("MFMMMM", mfmmmm, &genTree5, &tables);
                    wwwReplacer("MMFFFF", mmffff, &genTree5, &tables);
                    wwwReplacer("MMFFFM", mmfffm, &genTree5, &tables);
                    wwwReplacer("MMFFMF", mmffmf, &genTree5, &tables);
                    wwwReplacer("MMFFMM", mmffmm, &genTree5, &tables);
                    wwwReplacer("MMFMFF", mmfmff, &genTree5, &tables);
                    wwwReplacer("MMFMFM", mmfmfm, &genTree5, &tables);
                    wwwReplacer("MMFMMF", mmfmmf, &genTree5, &tables);
                    wwwReplacer("MMFMMM", mmfmmm, &genTree5, &tables);
                    wwwReplacer("MMMFFF", mmmfff, &genTree5, &tables);
                    wwwReplacer("MMMFFM", mmmffm, &genTree5, &tables);
                    wwwReplacer("MMMFMF", mmmfmf, &genTree5, &tables);
                    wwwReplacer("MMMFMM", mmmfmm, &genTree5, &tables);
                    wwwReplacer("MMMMFF", mmmmff, &genTree5, &tables);
                    wwwReplacer("MMMMFM", mmmmfm, &genTree5, &tables);
                    wwwReplacer("MMMMMF", mmmmmf, &genTree5, &tables);
                    wwwReplacer("MMMMMM", mmmmmm, &genTree5, &tables);

                    tree += genTree5;
                }
            }
        }
    }

    return tree;
}

QString Reporter::makeHtmlFamilyEventChildren(quint16 p, quint16 f)
{
    QList<quint16> childID;
    bool        rowBool = false;
    quint16     k;
    QString     currentPerson;

    childID.clear();

    PINDI(p).currentSpouse = f;
    PINDI(p).getChilds(&childID, true);
    GENE.arrangePersons(&childID);
    quint16 familyId = PINDI(p).famSList.at(f);

    // ----------------------------------------------------------
    if ( PFAMI(familyId).note.text != "" && PFAMI(familyId).effectivePrivacyMethod() == PRIVACY_0_SHOW ) {
        // PFAMI note kentässä saattaa olla jotain printattavaa.

        QString eventTitle = marriageeventsnote07;

        //if (rowBool) eventTitle.replace("CELLCOLOR", eventLineColor); else eventTitle.replace("CELLCOLOR", eventLineColor2);

        rowBool = !rowBool;

        currentPerson += eventTitle; rows2++;
        currentPerson.replace("NOTE", PFAMI(familyId).printNote());
    }

    // Täytetään avioliittotiedot. Tarkastetaan ensin, onko tämä sellainen henkilö jonka kohdalla
    // on ylipäätään tarkoitus printata

    if (PFAMI(familyId).eventsToPrint()) {
        // Family eventtejä on, printataan jokainen eventti vuorotellen
        // Ensin otsikkorivi.
        currentPerson += marriageeventsheader08; rows2++;
        for (k=0 ; k<PFAMI(familyId).entry.size() ; k++ ) {
            Entry entry = PFAMI(familyId).entry.at(k);

            if (entry.publishable()) {
                // Tulostetaan tämä avioliittotapahtuma

                currentPerson += marriageeventsline09; rows2++;
                //currentPerson.replace("CELLCOLOR", entry.printColor());

                currentPerson.replace("EVENT", Entry::typeString(entry.type, false));
                currentPerson.replace("DATE", entry.printDay());
                currentPerson.replace("WEEKDAY", entry.printWeekday());
                currentPerson.replace("PLACE", entry.printPlace());
                currentPerson.replace("VALUE", entry.printValue());
                currentPerson.replace("HAG", PINDI(PFAMI(familyId).husb).getPrintAge(entry.day));
                currentPerson.replace("WAG", PINDI(PFAMI(familyId).wife).getPrintAge(entry.day));
                currentPerson.replace("NOTES", entry.printNote());
            } // entry
        } // entry loop
    } // family events

    // ------------------------------lapset------------------------------

    // tässä pitää virittää funktio printableChilds, joka käy childId:n läpi ja varmistaa että tablessa on kaikki id:t
    if (printableChilds(&tables, &childID)) { ////
    //if (childID.size() >0 ) {
        // on lapsia, printataan otsikkorivi
        currentPerson += marriagechildstitle10; rows2++;

        // printataan jokainen lapsi omalle rivilleen
        for (k=0 ; k<childID.size() ; k++) {

            currentPerson += marriagechildsline11; rows2++;

            //if (rowBool) currentPerson.replace("CELLCOLOR", eventLineColor); else currentPerson.replace("CELLCOLOR", eventLineColor2);
            rowBool = !rowBool;

            int child = childID.at(k);

            if (tables.contains(child)) {

                // child value will have policy that is the greatest of:
                // 1) child id policy, child birth policy or family policy

                QString childName;

                if (PINDI(child).sex == MALE) childName = "<b>&#9794;</b> ";
                else if (PINDI(child).sex == FEMALE) childName = "<b>&#9792;</b> ";
                else currentPerson.replace("GENDER", "? ");

                // child id number, gender
                childName +="#" + tableId(child, &tables);
                currentPerson.replace("CHILD-ID", childName);
                childName = "";

                // child name

                childName = PINDI(child).printName123Family();
                childName = makeLink(childName, child, &tables);
                currentPerson.replace("CHILD", childName);

                if (PINDI(child).publishable()) {
                    currentPerson.replace("BIRTH", PINDI(child).ientry(BIRTH).printDay());
                    currentPerson.replace("WEEKDAY", PINDI(child).ientry(BIRTH).printWeekday());
                    currentPerson.replace("PLACE", INDI(child).ientry(BIRTH).printPlace());
                    currentPerson.replace("CHIL-NOTE", PINDI(child).printNote());

                    if (PINDI(child).effectivePrivacyMethod() == PRIVACY_0_SHOW) {
                        currentPerson.replace("FAG", PINDI(PINDI(child).getFather()).getPrintAge(PINDI(child).ientry(BIRTH).day));
                        currentPerson.replace("MAG", PINDI(PINDI(child).getMother()).getPrintAge(PINDI(child).ientry(BIRTH).day));
                    }
                    else
                    {
                        currentPerson.replace("FAG", "");
                        currentPerson.replace("MAG", "");
                    }

                    QString deathStr = "";
                    if (PINDI(child).effectivePrivacyMethod() == PRIVACY_0_SHOW) {
                        deathStr = PINDI(child).ientry(DEATH).printDay();

                        Date death = PINDI(child).getEntryDaybyType(true, DEATH);
                        Date birth = PINDI(child).getEntryDaybyType(true, BIRTH);

                        if (death.known && birth.known && PINDI(child).ientry(DEATH).effectivePrivacyMethod() == PRIVACY_0_SHOW ) {
                            deathStr = " (" + QString::number(birth.day1.daysTo(death.day1) / 365) + ")";
                        }
                    }

                    currentPerson.replace("DEATH", deathStr);
                } else {
                    currentPerson.replace("BIRTH", "");
                    currentPerson.replace("WEEKDAY", "");
                    currentPerson.replace("FAG", "");
                    currentPerson.replace("MAG", "");
                    currentPerson.replace("PLACE", "");
                    currentPerson.replace("CHIL-NOTE", "");
                    currentPerson.replace("DEATH", "");
                }

                // death column

            } // child was pritable (indluded in tables)
        } // next child
    } // are there marriage events to be printed
    return currentPerson;
}

QString Reporter::makeHtmlFamily(quint16 p)
{
    QString currentPerson;
    QStringList spouseNames;
    QList<quint16> spouseID;
    quint16 j;

    PINDI(p).spouses(&spouseID);
    PINDI(p).makeNameList(&spouseNames, &spouseID);

    rows2 = 0;

    for (j=0 ; j<PINDI(p).famSList.size() ; j++) {

        currentPerson += marriagespouse06; rows++;

        QString spouseTxt;

        PINDI(p).currentSpouse = j;
        // printataan puolison nimi
        if ( tables.indexOf(PINDI(p).getSpouse()) != -1) {
            if (PINDI(p).getSpouse() != 0) spouseTxt = "#" + tableId(PINDI(p).getSpouse(), &tables);
            if (PINDI(p).getSpouse() == 0) spouseTxt = "unknown";
        } else spouseTxt = " (not in this report).";

        currentPerson.replace("SPOUSEID", spouseTxt);
        //QString spouseName = PINDI(PINDI(p).getSpouse()).printNameFamily() + " " + PINDI(PINDI(p).getSpouse()).printName1() + " " + PINDI(PINDI(p).getSpouse()).printName2() + " " + PINDI(PINDI(p).getSpouse()).printName3();
        QString spouseName = PINDI(PINDI(p).getSpouse()).printName123Family();
        spouseName = makeLink(spouseName, PINDI(p).getSpouse(), &tables);

        currentPerson.replace("SPOUSEFAMILY", spouseName);

        currentPerson += "INSERT_CHILDS_HERE" + QString::number(j);

    } // next spouse PFAMI

    PINDI(p).currentSpouse = 0;

    return currentPerson;
}

QString Reporter::makeHtmlDescentantTable(quint16 i)
{
    QString output = "";

    QList<int> rivit;
    QList<int> hlot;

    bool newCol = true;

    descendantTableBuilder(&newCol, i, &rivit, &hlot, &output);
// tästä otin <body> :n pois
    QString add = "<table style=\"border: medium none ; background: #ffffcc ; border-collapse: collapse;\" cellpadding=\"0\" cellspacing=\"0\" border=\"1\" width=\"100%\"><tbody>\n";
    output.prepend(add);

    // tässä pistetään jälkipolvitaulukon nimet paikalleen.
    for (i = 0 ; i<rivit.size() ; i++) {
        QString tag = QString("%1").arg(QString::number(hlot.at(i)), 5, '0');
        QString row = "ROWS" + tag;
        QString color = "COLOR" + tag;
        QString name = "NAME" + tag;

        output.replace(row, QString::number(rivit.at(i)));

        output.replace(name, makeLink(PINDI(hlot.at(i)).printNameFamily1(), hlot.at(i), &tables));

        if (INDI(hlot.at(i)).effectivePrivacyMethod() == PRIVACY_4_HIDE) output.replace(color, "#cccccc"); else {
            if (INDI(hlot.at(i)).sex == MALE) output.replace(color, "#ccffff");
            if (INDI(hlot.at(i)).sex == FEMALE) output.replace(color, "#ffcccc");
            if (INDI(hlot.at(i)).sex == UNKNOWN) output.replace(color, "#cccccc");
        }
    }

    output += "</table><br><br>\n";
    return output;

}

QString Reporter::makeHtml(quint16 p)
{
    QString     multiMediaFooter;

    int         imageHeight = 0;
    int         imageWidth = 0;
    int         k;

    QImage      bmp;
    QImage      bmp4pdf;

    rows = 0;
    multiMediaFooter = "";
    QString currentPerson = table01;
    QString tmp = "\n<a name=\"" + QString("%1").arg(QString::number(p), 5, '0') + "\"></a>\n";
    currentPerson.replace("LINKTOTHISPERSON", tmp);

    rows++;

    QString mmCitations = ""; // taulukon perään tulevat kuvat

    emit (sigStatusBarMessage(PINDI(p).nameFamily + " " + PINDI(p).nameFirst));

    // The basic info : ID, name, gender...
    currentPerson.replace("TABLEID", tableId(p, &tables));

    currentPerson.replace("FAMILYNAME", PINDI(p).printName123Family());
    currentPerson.replace("NAME1", "");//PINDI(p).printName1());
    currentPerson.replace("NAME2", "");//PINDI(p).printName2());
    currentPerson.replace("NAME3", "");//PINDI(p).printName3());

    if (PINDI(p).effectivePrivacyMethod() == PRIVACY_4_HIDE) currentPerson.replace("GENDER", ""); else {
        if (PINDI(p).sex == MALE) currentPerson.replace("GENDER", "<b>&#9794;</b>");
        if (PINDI(p).sex == FEMALE) currentPerson.replace("GENDER", "<b>&#9792;</b>");
        if (PINDI(p).sex == UNKNOWN) currentPerson.replace("GENDER", "(gender ?)");
    }


    currentPerson.replace("RELA", PINDI(p).getShortFamilyRelation());

    currentPerson.replace("CHANGED", "Changed:" + PINDI(p).changed.toString("dd.MM.yyyy"));

// -----------------------------------------------------------------------------------------------
// Persondata:n NOTE kenttä
// -----------------------------------------------------------------------------------------------

    QString note = PINDI(p).printNote();
    if ( note != "" ) {
        currentPerson += indinotes03; rows++;        
        //currentPerson.replace("CELLCOLOR", eventLineColor2);
        currentPerson.replace("NOTES", PINDI(p).printNote());
    }

// -----------------------------------------------------------------------------------------------
// Onko henkilöllä Eventtejä tai attribuutteja, siis syntymä, ammatti yms yms.
// Jos on niin printataan ne, jos luoja suo.
// -----------------------------------------------------------------------------------------------

    if ( PINDI(p).eventsToPrint() ) {
        currentPerson += indieventstitle04; rows++;

        if ( (GET_PRINT_ESTIMATEDBIRTHS) && !PINDI(p).ientry(BIRTH).used() ) {
            currentPerson += indieventsline05; rows++;
            currentPerson.replace("DATE", PINDI(p).estimatedBirth.toString(true));
            currentPerson.replace("VALUE", "");
            currentPerson.replace("EVENT", tr("Est. Birth"));
            currentPerson.replace("NOTES", PINDI(p).estimatedBirth.day1explanation + "<br>" + PINDI(p).estimatedBirth.day2explanation);
            currentPerson.replace("AGE", "");
            currentPerson.replace("PLACE", "");
            currentPerson.replace("WEEKDAY", "");
        }

        for (k=0 ; k<PINDI(p).entry.size() ; k++) {
            Entry entry = PINDI(p).entry.at(k);

            if ( entry.publishable() ) {
                QString indiTitle = indieventsline05;

                currentPerson += indiTitle; rows++;
                int type = entry.type;

                QString typeText;
                if (type == 7) typeText = entry.attrType; else typeText = Entry::typeString(type, true);
                currentPerson.replace("EVENT", typeText);
                currentPerson.replace("DATE", entry.printDay());
                currentPerson.replace("WEEKDAY", entry.printWeekday());
                currentPerson.replace("PLACE", entry.printPlace());
                currentPerson.replace("VALUE", entry.printValue());
                currentPerson.replace("AGE", PINDI(p).getPrintAge(entry.day));// entry.printAge());
                currentPerson.replace("NOTES", entry.printNote());
                mmCitations += addMultimedia(entry.multiMedia, *pDirName);
            } // entry
        } // entry loop
    } // if events

// -----------------------------------------------------------------------------------------------
// Käydään läpi kaikki avioliitot, tulostetaan avioliittotiedot ml. lapset
// -----------------------------------------------------------------------------------------------

    currentPerson += makeHtmlFamily(p); //   "INSERT_FAMILY_HERE";

// -----------------------------------------------------------------------------------------------
// Tulostetaan henkilön kuva. Jos kuvia ei ole, tai jos kuvien prittauspolicy (privacyMethod)
// tämän henkilön policy arvolla on 0 (0==älä printtaa kuvia), korvataan kuva tyhjällä
// -----------------------------------------------------------------------------------------------


    if ( ( PINDI(p).multiMedia.size() == 0 ) ||
         ( PINDI(p).effectivePrivacyMethod() != PRIVACY_0_SHOW ) ||
         ( (GENE.printPolicy == PRINTPOLICY_3_PUBLIC) && (PINDI(p).isLiving()) ) )
    {
        currentPerson.replace("FILENAME", "");
    } // ei kuvia
    else { // Kuvia on
        QString imagetag;

        bmp.load(GET_DIRECTORY + "/" +PINDI(p).multiMedia.value(0).file);

        bmp4pdf = bmp;

        // Tulostetaan kuva leveydellä 80 jos pdf printtaus, muuten leveydellä 150
        //if (*pPrintPdf) imageWidth = 80; else imageWidth = 150;

        imageWidth = 150 ; imageHeight = (imageWidth*bmp.height())/bmp.width(); bmp = bmp.scaled(imageWidth, imageHeight, Qt::IgnoreAspectRatio, Qt::FastTransformation);
        imageWidth = 80 ;  imageHeight = (imageWidth*bmp.height())/bmp.width(); bmp4pdf = bmp4pdf.scaled(imageWidth, imageHeight, Qt::IgnoreAspectRatio, Qt::FastTransformation);

        imagetag = "<img src=\"FILENAME\" alt=\"FILENAME\">";
        imagetag.replace("FILENAME", getFileEnd(PINDI(p).multiMedia.value(0).file));
        imagetag.replace("HEIGHT", "height=" + QString::number(imageHeight));
        imagetag.replace("WIDTH", "width=" + QString::number(imageWidth));
        currentPerson.replace("FILENAME", imagetag);

        for(k=0 ; k<PINDI(p).multiMedia.size() ; k++) {

            {
                // Siirretään / tallennetaan kuva raporttidirikkaan.
                // Jos kuva on pääkuva eli k=0, tallennetaan edellä tehty zoomattu kuva.
                // Jos kuva on jokin muu, tallennetaan se sellaisenaan zoomaamattomana.
                if (k==0) {
                    bmp.save(*pDirName + "/" + getFileEnd(PINDI(p).multiMedia.value(k).file));

                } else QFile::copy(
                        GET_DIRECTORY + "/" + PINDI(p).multiMedia.value(k).file,
                        *pDirName + "/" + getFileEnd(PINDI(p).multiMedia.value(k).file));
            }

            // photo 0 is above with all other data. The loops starts from 0 because
            // the photo 0 needs to be added to pdfText resources below.
            if (k != 0) {
                QString line = "<img src=\"FILENAME\" height=200 width=\"200\"><br>NOTE<br>";
                line.replace("FILENAME", getFileEnd(PINDI(p).multiMedia.value(k).file));
                line.replace("NOTE", PINDI(p).multiMedia.value(k).note.text);
                multiMediaFooter += line;
            }

            if (*pPrintPdf) {
                QString resourceName = getFileEnd(PINDI(p).multiMedia.value(k).file);

                QUrl url;
                url.setUrl(resourceName);
/*
                bmp.load(GET_DIRECTORY + "/" +PINDI(p).multiMedia.value(k).file);

                if ( k==0 ) bmp = bmp.scaled(imageWidth, imageHeight, Qt::KeepAspectRatio, Qt::FastTransformation);
*/
                pPdfText->addResource(
                        QTextDocument::ImageResource,
                        url,
                        QVariant(bmp4pdf));

            } // printpdf
        } // loop for going trhough all photos
    } // yes we have pictures


// -----------------------------------------------------------------------------------------------
// Esipolvitaulu, jos ollaan tekemässä www raporttia. Muun tyyppisissä raporteissa tämä veisi ihan
// hillittömästi tilaa dokumentissä
// -----------------------------------------------------------------------------------------------

    currentPerson += "INSERT_SMALL_TREE_HERE";
    currentPerson += "INSERT_MORE_TREE_HERE"; // nämä lisätään myöhemmin
    currentPerson += "INSERT_TABLE_FOOTER_HERE";
    currentPerson += "INSERT_DESCENTANT_TABLE_HERE";

    return currentPerson;
}

void Reporter::run()
{
    if (GENE.printPolicy != PRINTPOLICY_4_ALL) makeReport();
    else {
        QString baseDir = *pDirName;
        GENE.printPolicy = PRINTPOLICY_1_EVERYTHING;
        *pDirName = baseDir + "/1";
        if (!QDir(*pDirName).exists()) QDir().mkdir(*pDirName);
        makeReport();

        GENE.printPolicy = PRINTPOLICY_2_GENE;
        *pDirName = baseDir + "/2";
        if (!QDir(*pDirName).exists()) QDir().mkdir(*pDirName);
        makeReport();

        GENE.printPolicy = PRINTPOLICY_3_PUBLIC;
        *pDirName = baseDir + "/3";
        if (!QDir(*pDirName).exists()) QDir().mkdir(*pDirName);
        makeReport();
    }
    emit(reportComplete(geneDataId));
}

void Reporter::makeReport()
{
    int i, p;
    QList<quint16>  rootID;

    QString     currentPerson;
    QString     output;

    QString     personlist;
    pPdfText = new QTextDocument();

    /*
    if (GET_PRINT_COLORS == 0) { // white
        maleColor = "#FFFFFF";
        femaleColor ="#FFFFFF";
        eventLineColor = "#FFFFFF";
        eventLineColor2 = "#FFFFFF";
    }

    if (GET_PRINT_COLORS == 1) { // light yellow
        maleColor = COLOR_PRINTYELLOW;
        femaleColor = COLOR_PRINTYELLOW;
        eventLineColor = COLOR_PRINTYELLOW;
        eventLineColor2 = COLOR_PRINTYELLOW;
    }

    if (GET_PRINT_COLORS == 2) {
        maleColor = HTML_MALE_COLOR;
        femaleColor = HTML_FEMALE_COLOR;        
        eventLineColor = COLOR_PRINTYELLOW;
        eventLineColor2 = COLOR_PRINTYELLOW;
    }
    */

    // prf printtauksessa jokatoinen rivi on vähän vaaleampi. html printtauksessa sitä ei tarvita koska html taulukossa näkyy
    // solujen reunat
    //if (!*pPrintPdf) eventLineColor2 = eventLineColor;
    // -----------------------------------------------------------------------------------------------
    // Lets mark the persons to be printed into pd/selected, if only ancestors of current persons will be
    // printed. If all or descentants will be printed, lets mark 'true' to all pdCheck's.
    // -----------------------------------------------------------------------------------------------

    PINDI(GENE.currentId).selectNone();
    if ( GET_PRINT_WHAT == PRINT_ANCESTORS ) PINDI(GENE.currentId).selectAncestors();
    if ( GET_PRINT_WHAT == PRINT_ALL ) PINDI(GENE.currentId).selectAll();
    if ( GET_PRINT_WHAT == PRINT_DESCENDATS ) PINDI(GENE.currentId).selectDescendants();

    // ei käytetä indi.selected tietoa, koska se näkyisi valittujen henkilöiden high-lightaamisena.
    for (quint16 i = 0 ; i<=geneData[this->geneDataId].indiLastUsed ; i++) PINDI(i).selectedSystem = PINDI(i).selected;
    PINDI(GENE.currentId).selectNone();

    // lets fill the first table entry so that the first actual people in the talbe will be @ #1 position
    tables.append(0);

    // -----------------------------------------------------------------------------------------------
    // Step 1 : lets find the "root" persons who will be scanned upwards. For all or ancestors, we dont
    // yet know who could be the "root" persons but if we print "descentants" for a current person, then
    // the current person is the only root person.
    // -----------------------------------------------------------------------------------------------
    if ( GET_PRINT_WHAT == PRINT_ALL || GET_PRINT_WHAT == PRINT_ANCESTORS ) {
        for (quint16 i=1 ; i<=GENE.indiLastUsed ; i++) {
            if (PINDI(i).selectedSystem) {
                if (PINDI(i).getFather() == 0 &&
                    PINDI(i).getMother() == 0 &&
                    PINDI(PINDI(i).getSpouse()).getFather() == 0 &&
                    PINDI(PINDI(i).getSpouse()).getMother() == 0 &&
                    !rootID.contains(PINDI(i).getSpouse()))
                {
                    rootID.append(i);
                }
            }
        }
    } else if ( GET_PRINT_WHAT == PRINT_DESCENDATS ) rootID.append(GENE.currentId); // only current person is a "root" person

    // -----------------------------------------------------------------------------------------------
    // Step 2 : an recursive algorithm for scanning all the chilnden, their childrens etc... from the
    // root persons
    // -----------------------------------------------------------------------------------------------

    for (i=0 ; i<rootID.size() ; i++) printStep(rootID.at(i), &tables);

    // Step2.1 : lasketaan kaikille indi ja fami eventeille vähän apuarvoja. Näitä apuarvoja käytetään vain
    // printatessa

    for (quint16 i = 1; i<=GENE.indiLastUsed ; i++) {
        INDI(i).usedInRootList = false;
        for (quint8 e=0 ; e<INDI(i).entry.size() ; e++) {
            INDI(i).entry[e].indiId = i;
            INDI(i).entry[e].famiId = 0;
        }
    }

    for (quint16 f = 0 ; f<=GENE.famiLastUsed ; f++) {
        for (quint8 e=0 ; e<FAM(f).entry.size() ; e++) {
            FAM(f).entry[e].indiId = 0;
            FAM(f).entry[e].famiId = f;
        }
    }

    // ------------------------------------------------------------------------------------------------
    // Step 2.9 : Load output model files
    QFile file;

    file.setFileName(":/print/00header.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    header00 = file.readAll();
    file.close();    

    file.setFileName(":/print/00css-style.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    style00 = file.readAll();
    file.close();

    file.setFileName(":/print/01tableheader.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    table01 = file.readAll();
    file.close();

    file.setFileName(":/print/03indi-notes.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    indinotes03 = file.readAll();
    file.close();

    file.setFileName(":/print/04indi-events-title.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    indieventstitle04 = file.readAll();
    file.close();

    file.setFileName(":/print/05indi-events-line.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    indieventsline05 = file.readAll();
    file.close();

    file.setFileName(":/print/06marriage-spouse.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    marriagespouse06 = file.readAll();
    file.close();

    file.setFileName(":/print/07marriage-events-note.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    marriageeventsnote07 = file.readAll();
    file.close();

    file.setFileName(":/print/08marriage-events-header.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    marriageeventsheader08 = file.readAll();
    file.close();

    file.setFileName(":/print/09marriage-events-line.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    marriageeventsline09 = file.readAll();
    file.close();

    file.setFileName(":/print/10marriage-childs-title.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    marriagechildstitle10 = file.readAll();
    file.close();

    file.setFileName(":/print/11marriage-childs-line.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    marriagechildsline11 = file.readAll();
    file.close();

    file.setFileName(":/print/12-0gp-familytree.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    familytree12_0gp = file.readAll();
    file.close();

    file.setFileName(":/print/12-1gp-familytree.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    familytree12_1gp = file.readAll();
    file.close();

    file.setFileName(":/print/12-2gp-familytree.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    familytree12_2gp = file.readAll();
    file.close();

    file.setFileName(":/print/12-3gp-familytree.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    familytree12_3gp = file.readAll();
    file.close();

    file.setFileName(":/print/12-4gp-familytree.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    familytree12_4gp = file.readAll();
    file.close();

    file.setFileName(":/print/12-5gp-familytree.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    familytree12_5gp = file.readAll();
    file.close();

    file.setFileName(":/print/12-footer-familytree.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    familytree12_footer = file.readAll();
    file.close();

    file.setFileName(":/print/13footer.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    footer13 = file.readAll();
    file.close();

    file.setFileName(":/print/letterlist.html");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    letterList = file.readAll();
    file.close();

    file.setFileName(":/print/personlist.html");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    personlist = file.readAll();
    file.close();

    QFile::copy(":/print/style.css", *pDirName + "/style.css");

    // -----------------------------------------------------------------------------------------------
    // Step 2.9. Kansilehti. Joko omaan tiedostoonsa index.html jos tehdään www raporttia. Muuten
    // raportin alkuun.
    // -----------------------------------------------------------------------------------------------

    output = "";

    {
        QString fileName = *pDirName + "/index.html";
        QFile file(fileName);
        file.open(QIODevice::WriteOnly|QIODevice::Text);

        file.write(header00.toUtf8() + makeIndex().toUtf8() + footer13.toUtf8());

        QString out = "Root persons: <BR><BR>";

        for (quint16 x = 0 ; x<rootID.size() ; x++) {
            int tmpId = rootID.at(x);

            if (    !PINDI(tmpId).familyRelation.contains(GET_RELATION_SIBLING) &&
                    !PINDI(tmpId).familyRelation.contains(GET_RELATION_CHILD) &&
                    !PINDI(tmpId).familyRelation.isEmpty() )
            {
                QString name;// = PINDI(rootID.at(x)).nameFamily + " " + PINDI(rootID.at(x)).nameFirst;

                QList<QString> places;

                quint16 x2 = tmpId;
                do {
                    QString n = PINDI(x2).getRootListName();
                    if (PINDI(x2).usedInRootList) name += "..."; else {
                        PINDI(x2).usedInRootList = true;
                        if (!places.contains(n)) {
                            places.append(n);
                            if (name != "") name += " => ";
                            name += n;
                        }
                        x2 = PINDI(x2).getNextAncestor();
                    }
                } while (x2 && !PINDI(x2).usedInRootList);

                out += makeLink(name, rootID.at(x), &tables)  + "<br><br>" ;

            }
        }
        out.replace("LINK_A", "");
        out.replace("LINK_B", ".html");
        file.write(out.toUtf8());
        file.close();
    }

    output += makeIndex().toUtf8();
    output += "\n<br><br><br>\n";

    // -------------------------------------------------------------------------------------
    // Step 3 : output
    // Lets go through all individuals, by order spesifyed in "tables" (e.g. a root person first,
    // then his/her childrend, their childs etc...
    // -----------------------------------------------------------------------------------------------

    for (i=1 ; i<tables.size() ; i++) {
    /*
    rows             =  how many rows in the table, to be used to extend the photo cell
    multiMediaFooter =  html code after the table holding photos
    output           =  if the report is not WWW, the output file
    currentPerson    =  the html of this person, to be either saved in file (if report is WWW) or added
                        in output
                        */


        p = tables.at(i);

        currentPerson = makeHtml(p);

        QString fileName = *pDirName + "/" + QString("%1").arg(QString::number(tables.at(i)), 5, '0') + ".html";

        // tulostetaan tämä henkilö omalle tiedostolleen, ja lisätään vähän tietoa kun tilaa on

        QList<QString> childString;
        for (int ff = 0 ; ff<PINDI(p).famSList.size() ; ff++) childString.append(makeHtmlFamilyEventChildren(p,ff));
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
            QString thisPerson = header00 + currentPerson;
            thisPerson.replace("INSERT_SMALL_TREE_HERE", makeHtmlFamilyTreeSmall(p));
            thisPerson.replace("INSERT_MORE_TREE_HERE", makeHtmlFamilyTreeMore(p));
            thisPerson.replace("INSERT_TABLE_FOOTER_HERE", familytree12_footer);
            thisPerson.replace("INSERT_DESCENTANT_TABLE_HERE", makeHtmlDescentantTable(p));
            for (int ff=0 ; ff<PINDI(p).famSList.size() ; ff++) {
                QString tag = "INSERT_CHILDS_HERE" + QString::number(ff);
                thisPerson.replace(tag, childString.at(ff));
            }
            //thisPerson.replace("INSERT_STYLE_HERE", style00);
            thisPerson.replace("INSERT_BORDER_HERE", "");
            thisPerson.replace("SPOUSENOTE", "");
            //thisPerson +=  "<br>" + letterList + footer13;
            thisPerson.replace("ROWCOUNT", QString::number(rows + rows2));
            thisPerson.replace("LINK_A", "");
            thisPerson.replace("LINK_B", ".html");
            thisPerson.replace("CELLCOLOR", COLOR_PRINTYELLOW);

            //      LINK_A joko # tai ""
            //      LINK_B joko "" tai .html

            thisPerson += "<br><br><small><small>&copy " + GENE.subm.name + "</small></small>" + footer13;
            file.write(thisPerson.toUtf8());
            file.close();
        }

        // lisätään tämä henkilö output muuttujaan, johon tulee siis kaikki henkilöt. Siksi ihan kaikkea ei voi output
        // muuttujaan lisätä.

        currentPerson.replace("INSERT_SMALL_TREE_HERE", makeHtmlFamilyTreeSmall(p));
        currentPerson.replace("INSERT_MORE_TREE_HERE", "");
        currentPerson.replace("INSERT_TABLE_FOOTER_HERE", familytree12_footer);
        currentPerson.replace("INSERT_DESCENTANT_TABLE_HERE", "");

        if (INDI(p).sex == FEMALE) {
            for (int ff=0 ; ff<PINDI(p).famSList.size() ; ff++) {
                QString tag = "INSERT_CHILDS_HERE" + QString::number(ff);
                currentPerson.replace(tag, childString.at(ff));
            }
            currentPerson.replace("SPOUSENOTE", "");
            currentPerson.replace("ROWCOUNT", QString::number(rows + rows2));
        }
        else {
            // tämä on siis ukko. Jos jostain syystä ukon puolisosta ei hajua, snakataan perhetiedot ukolle
            // jos miehellä on yksi puoliso, se oli se sama puoliso jonka kohdalle perhetiedot yllä printattiin.
            // Jos miehen puoliso ei ole tiedossa eli 0, tai jos puolisoita on enemmän kuin yksi, printataan perhetiedot

            if ( INDI(p).famSList.size() != 1 ) {
                for (int ff=0 ; ff<PINDI(p).famSList.size() ; ff++) {
                    QString tag = "INSERT_CHILDS_HERE" + QString::number(ff);
                    currentPerson.replace(tag, childString.at(ff));
                }

                currentPerson.replace("SPOUSENOTE", "" );
                currentPerson.replace("ROWCOUNT", QString::number(rows + rows2));
            }
            else {

                for (int ff=0 ; ff<PINDI(p).famSList.size() ; ff++) {
                    QString tag = "INSERT_CHILDS_HERE" + QString::number(ff);
                    currentPerson.replace(tag, "");
                }

//                currentPerson.replace("INSERT_CHILDS_HERE", "");
                currentPerson.replace("SPOUSENOTE", " See wife for family data");
                currentPerson.replace("ROWCOUNT", QString::number(rows));
            }
        }

        output+=currentPerson;

    }


    QString events = makeEventCalendar(&tables);

    events.replace("LINK_A", "");
    events.replace("LINK_B", ".html");

    output += events;

    QString fileName = *pDirName + "/" + "events.html";
    file.setFileName(fileName);
    file.open(QIODevice::WriteOnly|QIODevice::Text);
    file.write(header00.toUtf8() + events.toUtf8() + footer13.toUtf8());

    file.close();

    {
        QList<int>  list[30];
        QString test1, test2;
        int j;

        for (quint16 i=1 ; i<GENE.indiLastUsed ; i++) {
            if ((PINDI(i).nameFamily.length() > 0) && PINDI(i).publishable()) {

                j = PINDI(i).nameFamily.at(0).toUpper().toLatin1() - 65;

                if ( j==-124 )      j = 26;   // Ã¥
                if ( j==-125 )      j = 27;   // Ã¤
                if ( j == -107 )    j = 28; // Ã¶
                if ( j == -2 )      j = 29; // ?

                test1 = PINDI(i).printName123Family();
                //test1 = PINDI(i).printNameFamily() + PINDI(i).printName1() + PINDI(i).printName2() + PINDI(i).printName3();

                if ((test1.left(1) != "#") && (test1 != "")) {
                    if (list[j].size() == 0) list[j].append(i);
                    else {
                        int k = list[j].size();

                        do {
                            k--;
                            test2 = PINDI(list[j].at(k)).nameFamily + PINDI(list[j].at(k)).nameFirst + PINDI(list[j].at(k)).name2nd + PINDI(list[j].at(k)).name3rd;
                        } while ( ( test2 > test1 ) && ( k>0 ) );

                        test2 = PINDI(list[j].at(0)).nameFamily + PINDI(list[j].at(0)).nameFirst + PINDI(list[j].at(0)).name2nd + PINDI(list[j].at(0)).name3rd;
                        if (k==0 && ( test2 > test1 ) ) list[j].insert(0, i);
                        else list[j].insert(k+1, i);
                    }
                }
            }
        }

        for (int j=65 ; j<91 ; j++) {
            int i;
            int l = j-65;
            QString persons, output2;

            for (i=0 ; i<list[l].size() ; i++) {
                if (tables.contains(list[l].at(i))) {
                    QString name = tableId(list[l].at(i), &tables) + "  " + PINDI(list[l].at(i)).nameFamily + " " + PINDI(list[l].at(i)).nameFirst + " " + PINDI(list[l].at(i)).name2nd + " " + PINDI(list[l].at(i)).name3rd + " " + "<br>\n"; //
                    persons += makeLink(name, list[l].at(i), &tables);
                }
            }

            output2 = personlist;
            output2.replace("PERSONS", persons);
            output2.replace("XX", (QString)j);
            QString output3 = output2;

            output += output2;

            QString fileName = *pDirName + "/" + (QString)j + ".html";
            QFile file(fileName);
            file.open(QIODevice::WriteOnly|QIODevice::Text);
            output3.replace("LINK_A", "");
            output3.replace("LINK_B", ".html");
            output3 += "<br><br><small><small>&copy " + GENE.subm.name;
            //output3 += footer13;
            file.write( header00.toUtf8() + output3.toUtf8() + footer13.toUtf8());

            file.close();

        }
    }

    output.replace("LINK_A", "#");
    output.replace("LINK_B", "");

    output += "<br><br><small><small>&copy " + GENE.subm.name + "</small></small>" + footer13;


    GENE.locked = false;

    {
        //emit (sigStatusBarMessage("Creating report. You can edit the database now. I'll tell you when the report is ready. Don't close qenes before that."));
        QFile file2(*pDirName + "/all.html");        
        file2.open(QIODevice::WriteOnly|QIODevice::Text);
        QString output2 = header00 + output;
        //output2.replace("INSERT_STYLE_HERE", style00);
        output2.replace("INSERT_BORDER_HERE", "");
        output2.replace("CELLCOLOR", COLOR_PRINTYELLOW);
        file2.write(output2.toUtf8());
        file2.close();
        emit (sigStatusBarMessage("Html file done."));
    }


    if (*pPrintPdf) {
        //QTextDocument pdfText; // tämä on tuplamääritetty
        emit (sigStatusBarMessage("Creating pdf report. You can edit the database now. Don't close qenes before you'll be noted that report is ready."));
        QPrinter printer(QPrinter::ScreenResolution);

        printer.setPaperSize(QPrinter::A4);
        printer.setPageMargins(1, 1, 1, 1, QPrinter::Millimeter);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(*pDirName + "/all.pdf");
        printer.newPage();

        //output.replace("INSERT_STYLE_HERE", "");
        output.replace("INSERT_BORDER_HERE", "border = 1");
        output.replace("CELLCOLOR", "#FFFFFF");

        pPdfText->setDefaultFont(QFont("Arial", 7));

        pPdfText->setHtml(output);
        pPdfText->print(&printer);

        emit (sigStatusBarMessage("pdf file done."));

    }

    if (*pPrintJava) {
        QFile::copy(":/print/15qenesbrowser.txt", *pDirName + "/qenesBrowser.html");
        QFile::copy(":/print/qenes.jar", *pDirName + "/qenes.jar");
        Save::save(*pDirName + "/qenes.ged");
    }

    PINDI(0).selectNone();
}

Reporter::Reporter(int id, PrintSettings * pSettings, QObject *parent) : QThread(parent)
{
    this->geneDataId    = id;
    this->pPrintPdf     = &pSettings->printPdf;
    this->pPrintJava    = &pSettings->printJava;
    this->pDirName      = &pSettings->dirName;
}

void ReporterDialog::showPrivacySettings()
{
    Settings set(3);

    set.exec();
}

void ReporterDialog::cousinCheck()
{
    if (this->uiDescs4Ancs->isChecked()) this->uiBothParentofDec4anc->setEnabled(false);
    else this->uiBothParentofDec4anc->setEnabled(true);
    saveSetup();
}

void ReporterDialog::printtoHtml()
{
    saveSetup();
    startReport();
}

void ReporterDialog::saveSetup()
{
    sets.setValue("print/print", this->uiPrint->currentIndex());
    sets.setValue("print/inclBothParents", this->uiBothParentofDec4anc->isChecked());
    sets.setValue("print/inclAncDec", this->uiDescs4Ancs->isChecked());
    sets.setValue("print/inclPhoto", this->uiPrintPhoto->isChecked());
    //sets.setValue("print/inclEventCalendar", this->uiEventCalendar->isChecked());
    //sets.setValue("print/colors", this->uiBackGround->currentIndex());
    sets.setValue("print/privacy", this->uiPrivacy->currentIndex());
    sets.setValue("print/estimatedBirth", this->uiShowCalculatedBirth->isChecked());
    sets.setValue("print/pdf", this->uiAddPdf->isChecked());
}

void ReporterDialog::whatToPrintChanged(int index)
{
    if (index == 0) {
        uiDescs4Ancs->setDisabled(true);
        uiBothParentofDec4anc->setDisabled(true);
    }

    if (index == 1) {
        uiDescs4Ancs->setDisabled(false);
        uiBothParentofDec4anc->setDisabled(false);
    }

    if (index == 2) {
        uiDescs4Ancs->setDisabled(true);
        uiBothParentofDec4anc->setDisabled(false);
    }
}

void ReporterDialog::startReport()
{
    GENE.printPolicy = (PPOLICY)set_printPolicy;
    pPrintSettings->printPdf = uiAddPdf->isChecked();
    pPrintSettings->printJava = uiJava->isChecked();
    pPrintSettings->dirName = QFileDialog::getExistingDirectory(this, tr("Create report"), GET_DIRECTORY, 0);

    this->accept();
}

void ReporterDialog::closeWithoutPrinting()
{
    this->saveSetup();
    this->reject();
}

ReporterDialog::ReporterDialog(PrintSettings * pSettings, QWidget * parent) : QDialog(parent)
{    
    setupUi(this);

    id = GENE.currentId;
    pPrintSettings = pSettings;

    this->uiPrint->setCurrentIndex(GET_PRINT_WHAT);
    this->uiBothParentofDec4anc->setChecked(GET_PRINT_BOTHPARENTS);
    this->uiDescs4Ancs->setChecked(GET_PRINT_ANCDEC);
    this->uiPrintPhoto->setChecked(GET_PRINT_PHOTOS);
    this->uiPrivacy->setCurrentIndex(set_printPolicy);
    this->uiShowCalculatedBirth->setChecked(GET_PRINT_ESTIMATEDBIRTHS);
    this->uiAddPdf->setChecked("print/pdf");

    this->setFont(GeneData::panelFont);

    connect(uiToHtml, SIGNAL(clicked()), this, SLOT(printtoHtml()));

    connect(uiPrint, SIGNAL(currentIndexChanged(int)), this, SLOT(whatToPrintChanged(int)));

    connect(uiDescs4Ancs, SIGNAL(pressed()), this, SLOT(cousinCheck()));
    connect(uiCancel, SIGNAL(pressed()), this, SLOT(closeWithoutPrinting()));
    connect(uiPrivacyShowSettings, SIGNAL(pressed()), this, SLOT(showPrivacySettings()));

    whatToPrintChanged(uiPrint->currentIndex());
}
