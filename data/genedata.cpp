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
#include "macro.h"
#include <QSettings>
#include <QMessageBox>

extern QSettings    sets;
extern GeneData * geneData;
extern QList<QString>  patronyms;
extern QList<QString>  boyNames;
extern QList<QString>  girlNames;
extern QList<QString>  places;

quint8 GeneData::currentGd     = 0;
quint8 GeneData::gdLastUsed    = 0;
quint8 GeneData::gdCount       = 0;
QFont GeneData::panelFont = QFont("Arial", 10);
QFont GeneData::panelFontBold = QFont("Arial", 10, QFont::Bold);
//QWidget GeneData::mainWindow = NULL;// tehd‰‰n testitarkoituksiin, voi olla ettei tarvita

/*
QString GeneData::resn(int r)
{
    // t‰t‰ tarvitaa save, event, indi, fam:ssa
    switch ((int)r) {
        case 0 : return ""; break; //public
        case 1 : return "confidential"; break;
        case 2 : return "locked" ; break;
        case 3 : return "privacy" ; break;
    }
    return "";
}
*/

QString GeneData::test()
{

    QString fileName = QFileDialog::getOpenFileName(
            0,
            tr("Open Picture"),
            GET_DIRECTORY,
            tr("(*.*)"));

    return fileName;
}

void GeneData::arrangePersons(QList<quint16> *persons)
{
    // J‰rjest‰‰ personit syntym‰j‰rjestykseen
    for (int c = 0 ; c < persons->size()-1 ; c++) {
        if (
                (indi[persons->at(c)].ientry(BIRTH).used() && !indi[persons->at(c+1)].ientry(BIRTH).used() )
                || (indi[persons->at(c)].ientry(BIRTH).day > indi[persons->at(c+1)].ientry(BIRTH).day)
                ) {
            persons->swap(c, c+1);
            c= -1;
        }
    }
}

void GeneData::makeChildIdLists()
{
    for (quint16 i = 0; i<=this->indiLastUsed ; i++) {
        indi[i].getChilds(&indi[i].childIdList, true);
        GENE.arrangePersons(&indi[i].childIdList);
    }
}

void GeneData::arrangeEvents()
{
    quint16 x;
    for (x=0 ; x<=indiLastUsed ; x++) indi[x].arrangeEvents();
    for (x=0 ; x<=famiLastUsed ; x++) fami[x].arrangeEvents();
}

void GeneData::terminateAll()
{
    int g;
    for (g = 0 ; g<gdLastUsed ; g++) geneData[g].stop();

    delete geneData;
}

bool GeneData::isPatronym(QString name)
{
    int i;
    for (i = 0; i<patronyms.size() ; i++) {
        if (name.contains(patronyms.at(i))) {
            return true;
        }
    }
    return false;
}

void GeneData::checkNameStep(QString name, quint16 id)
{
    QList<QString> *names;
    if (indi[id].sex == MALE) names = &boyNames;
    if (indi[id].sex == FEMALE) names = &girlNames;

    if (indi[id].sex != UNKNOWN) {
        if (name != "" && !names->contains(name) && !isPatronym(name)) {
            Problem problem;
            problem.id = id;
            if (indi[id].sex == MALE) problem.type = PROBLEM_MALENAME;
            if (indi[id].sex == FEMALE) problem.type = PROBLEM_FEMALENAME;
            problem.attribute = name;
            problem.explanation = name + " (" + indi[id].nameFamily + " " + indi[id].nameFirst + ")";
            problem.indi = true;
            problems.append(problem);
        }
    } else {
        if ( name != "" && !boyNames.contains(name) && !girlNames.contains(name) ) {
            Problem problem;
            problem.id = id;
            problem.type = PROBLEM_NAME;
            problem.attribute = name;
            problem.explanation = indi[id].nameFamily + " " + indi[id].nameFirst;
            problem.indi = true;
            problems.append(problem);
        }
    }
}

void GeneData::checkSources()
{
    // t‰m‰ etsii, onko jokin l‰hde k‰ytt‰m‰tt‰. V‰h‰n huono idea, koska logiikka onkin v‰h‰n erilainen kuin muissa vireiss‰.
    // jotka aina viittaa tiettyyn henkilˆˆn. Ei k‰ytet‰ t‰t‰ ny.
/*

    int iFoo;
    bool bFoo;
    for (quint16 s=1 ; s<=GENE.sourLastUsed ; s++) {

        if (!GENE.sour[s].usedBy(&iFoo, &bFoo)) {
            //qDebug() << "problem";
            Problem problem;
            problem.id = s;
            problem.type = PROBLEM_SOURCE;
            problem.explanation = GENE.sour[s].title;
            problems.append(problem);
        }
    }
    */
}

void GeneData::checkName(QString name, quint16 id)
{
    QRegExp nameExp("(\\w*)?-?(\\w*)?-?(\\w*)?");

    nameExp.indexIn(name);
    checkNameStep(nameExp.cap(1), id);
    checkNameStep(nameExp.cap(2), id);
    checkNameStep(nameExp.cap(3), id);
}

QString GeneData::removePlaceComment(QString placeString)
{
    // from string "foo1 (foo2)" or "foo1", shall return "foo1"

    QRegExp sulutpois("(.[^\\)\\(]*)?");
    sulutpois.indexIn(placeString);
    return sulutpois.cap(1).trimmed();
}

void GeneData::checkAttr(bool indi, quint16 i, QString attr)
{
    if (places.contains(attr)) {
        Problem problem;
        problem.id = i;
        problem.type = PROBLEM_PLACE_AS_ATTRIBUTE;
        problem.indi = indi;
        problem.explanation = attr;
        problems.append(problem);
    }
}

void GeneData::checkPlace(bool indi, quint16 i, QString placeString, QString eventName)
{

    if (!places.contains(placeString) && placeString != "" ) {
        Problem problem;
        problem.id = i;
        problem.type = PROBLEM_PLACE;
        problem.indi = indi;
        problem.explanation = eventName + " " + placeString;
        problem.attribute = placeString;
        problems.append(problem);
    }
}

void GeneData::checkPlaces(bool indi, quint16 id, QString placeString, QString eventName)
{
    if (!placeString.contains("http://")) {
        //QRegExp nameExp("(.[^\\/]*)?/?([^\\/].[^\\/]*)?/?([^\\/].[^\\/]*)?");

        //QRegExp nameExp("(.[^\\/\\,]*)?/?([^\\/\\,].[^\\/\\,]*)?/?([^\\/\\,].[^\\/\\,]*)?");

        // T‰m‰ hirmuinen h‰rveli hakee esim
        // paikka1 / paikka2 / paikka3 => paikka_ arvot
        // paikka1, paikka2, paikka3 => paikka_ arvot
        QRegExp nameExp("(.[^\\/\\,]*)?[\\/\\,]?([^\\/\\,].[^\\/\\,]*)?[\\/\\,]?([^\\/\\,].[^\\/\\,]*)?");
        //QRegExp nameExp("(\\w[^\\/\\,]*)?[\\/\\,]?([^\\/\\,]\\w[^\\/\\,]*)?[\\/\\,]?([^\\/\\,]\\w[^\\/\\,]*)?");

        QString place1, place2, place3, place4;

        nameExp.indexIn(placeString);

        // t‰ss‰ haetaan esim
        // eura (diipadaapa) => eura

        place1 = removePlaceComment( nameExp.cap(1).trimmed() );
        place2 = removePlaceComment( nameExp.cap(2).trimmed() );
        place3 = removePlaceComment( nameExp.cap(3).trimmed() );
        place4 = removePlaceComment( nameExp.cap(4).trimmed() );

        //qDebug() << placeString << place1 << place2 << place3 << place4;

        checkPlace(indi, id, place1, eventName);
        checkPlace(indi, id, place2, eventName);
        checkPlace(indi, id, place3, eventName);
        checkPlace(indi, id, place4, eventName);
    }
}

void GeneData::addFamRel(quint16 relativeId, quint16 thisId, QString relation)
{
    if ( indi[thisId].sex == UNKNOWN ) {
        if ( thisId == currentId || indi[thisId].getMother() == currentId || indi[thisId].getFather() == currentId ) relation = "#";
        else relation ="?";
    }

    if (relation != "" && indi[thisId].familyRelation == "" ) {
        indi[thisId].familyRelation = indi[relativeId].familyRelation + relation;
        if (indi[thisId].getFather() && indi[indi[thisId].getFather()].familyRelation == "" ) this->addFamRel(thisId, indi[thisId].getFather(), "f");
        if (indi[thisId].getMother() && indi[indi[thisId].getMother()].familyRelation == "" ) this->addFamRel(thisId, indi[thisId].getMother(), "m");
    }
}

void GeneData::addFamRelAdd(quint16 id)
{
    if (indi[id].familyRelation == "") {

        bool mother = false;
        bool father = false;

        if (indi[id].getMother() && indi[indi[id].getMother()].familyRelation != "" ) mother = true;
        if (indi[id].getFather() && indi[indi[id].getFather()].familyRelation != "" ) father = true;

        if (indi[indi[id].getMother()].familyRelation.contains("s")) if (father) mother = false;

        if (indi[indi[id].getFather()].familyRelation.contains("s")) if (mother) father = false;

        if (mother) indi[id].familyRelation = indi[indi[id].getMother()].familyRelation + "c";
        else if (father) indi[id].familyRelation = indi[indi[id].getFather()].familyRelation + "c";
        else for (int i = 0 ; i<indi[id].famSList.size() ; i++) {
            indi[id].currentSpouse = i;
            if (indi[id].getSpouse() >0 && indi[indi[id].getSpouse()].familyRelation != "") {

                indi[id].familyRelation = indi[indi[id].getSpouse()].familyRelation + "s";
                if (indi[id].getMother()) addFamRel(id, indi[id].getMother(), "m");
                if (indi[id].getFather()) addFamRel(id, indi[id].getFather(), "f");
            }
        }
        indi[id].currentSpouse = 0;
    }
    indi[id].familyRelation.replace("fs", "m");
    indi[id].familyRelation.replace("ms", "f");
}
/*
 *
 * miksi t‰t‰ tarvittiin, sotkee vain p‰iv‰m‰‰r‰t
void GeneData::dayAccuracyTest()
{
    demonRun.dateAccuracyTest = true;
    quint16 i;
    //if (demonRun.dateAccuracyTest)
    {
        demonRun.dateAccuracyTest = false;

        for (i = 0 ; i<=indiLastUsed ; i++) {
            for (int e = 0 ; e<indi[i].entry.size() ; e++) {                
                if (indi[i].entry[e].day.getAcc() == ABT) {
                    QDate date = indi[i].entry[e].day.day1;
                    date.setDate(date.year(), 1, 1);
                    indi[i].entry[e].day.setDate1(date);
                }

                if (indi[i].entry[e].day.getAcc() == EST) {
                    QDate date = indi[i].entry[e].day.day1;
                    if (date.month() == 2) date.setDate(date.year(), 2, 28);

                    if (date.month() == 1 || date.month() == 3 || date.month() == 5 ||
                        date.month() == 7 || date.month() == 8 || date.month() == 10 ||
                        date.month() == 12) date.setDate(date.year(), date.month(), 31);

                    if (date.month() == 4 || date.month() == 6 || date.month() == 9 ||
                        date.month() == 11) date.setDate(date.year(), 12, 30);

                    indi[i].entry[e].day.setDate1(date);
                }

            }
        }
        // fam eventit
    }
    // t‰ss‰ ei p‰ivitet‰ n‰yttˆ‰
    demonRun.dateAccuracyTest = false;
}
*/
void GeneData::estimatedDateTest()
{
    demonRun.estimatedBirths = true;
    quint16 i;
    bool cont;
    //if (demonRun.estimatedBirths)
    {
        demonRun.estimatedBirths = false;
        for (i=1 ; i<=indiLastUsed ; i++) {
            indi[i].estimatedBirth.day1 = QDate();
            indi[i].estimatedBirth.day2 = QDate();
            indi[i].estimatedBirth.setAcc(NA);
            indi[i].estimatedBirth.known = false;
        }

        // Pistet‰‰n EXA arvioitu birth, jos birth on annettu
        for (i=1 ; i<=indiLastUsed ; i++) {
            if (!indi[i].estimatedBirth.known) { // t‰‰ lienee turha jos edell‰ nollattiin
                if (indi[i].ientry(BIRTH).day.known && indi[i].ientry(BIRTH).day.getAcc() != CAL) {
                    // t‰h‰n pit‰‰ lis‰t‰ metodej ajoissa birth lasketaan esim henkilˆn DEATH:st‰ tai muusta tapahtumasta
                    indi[i].estimatedBirth.setDate1(indi[i].ientry(BIRTH).date1());
                    indi[i].estimatedBirth.setDate2(indi[i].ientry(BIRTH).date2()); // t‰m‰ oli tarkoituksella date1?? *!*
                    indi[i].estimatedBirth.setAcc(indi[i].ientry(BIRTH).day.getAcc());

                    indi[i].estimatedBirth.day1explanation = "birth is known";
                    indi[i].estimatedBirth.day2explanation = "birth is known";

                    indi[i].incompleteBirthEvaluation = false;
                }
            } else indi[i].incompleteBirthEvaluation = true;
        }

        //for (i=1 ; i<=indiLastUsed ; i++) indi[i].incompleteBirthEvaluation = false;

        int c = 0;
        bool result;
        do {
            c++;
//qDebug() << "---";
            cont = false;
            for (i=1 ; i<=indiLastUsed ; i++)
                if (!indi[i].deleted) {
                    if (!indi[i].estimatedBirth.known || indi[i].incompleteBirthEvaluation) {
                    result = false;
                    Date estimation = indi[i].getEstimatedBirth(&result);
                    if (result) cont = true;
                    indi[i].estimatedBirth = estimation;
                }
            }
            //qDebug() << cont << c;
        } while (cont && (c < 100));

        // Jos estimated birthi‰ ei ole voitu laskea, tai jos laskutoimituksen tulos on pienempi kuin ALLMINDAY
        // (se voi tarkoittaa sit‰, ett‰ ko henkilˆ‰ ei ole linkattu yhteenk‰‰ henkilˆˆn eli h‰n on ainoa
        // tietokannassa tai irrallaan muista), asetetaan allminday ja allmaxday arvot
        for (i=1 ; i<=indiLastUsed ; i++) {
            if (!indi[i].estimatedBirth.known || (indi[i].estimatedBirth.day1 < GET_ALLMINDAY) ) {
                indi[i].estimatedBirth.setDate1(GET_ALLMINDAY);
                indi[i].estimatedBirth.day1explanation = "min day (1)";
                indi[i].estimatedBirth.setDate2(GET_ALLMAXDAY);
                indi[i].estimatedBirth.day2explanation = "max day (1)";
            }
        }
    }

    emit(sigStatusBarMessage("debugC"));
    demonRun.estimatedBirths = false;
}

void GeneData::problemFinding()
{
    demonRun.errors = true;
    quint16 i;

    problems.clear();
    twinAlfa.clear();
    twinBeta.clear();

    // tarkastetaan ensin onko henkilˆill‰ tuntemattomia sukupuolia tai onko henkilˆit‰ joita ei ole
    // linkattu kehenk‰‰n muuhun.

    emit sigStatusBarMessage("Looking for person errors...");
    for (i=1 ; i<=indiLastUsed ; i++) {
        if (!stopDemonNow) {
            if (indi[i].sex == UNKNOWN && (boyNames.contains(indi[i].nameFirst) || girlNames.contains(indi[i].nameFirst) ) && !indi[i].deleted) {
                Problem problem;
                problem.id = i;
                problem.indi = true;
                problem.explanation = indi[i].nameFirst;
                problem.type = PROBLEM_UNKNOWN_SEX;
                problems.append(problem);
            }
            if ( indi[i].famc.value == 0 && indi[i].famSList.size() == 0 && !indi[i].deleted) {
                Problem problem;
                problem.id = i;
                problem.indi = true;
                problem.type = PROBLEM_UNLINKED;
                problems.append(problem);
            }

            // tarkastetaan onko nimet rekistreˆity nimitietokantaan.
            checkName(indi[i].nameFirst, i);
            checkName(indi[i].name2nd, i);
            checkName(indi[i].name3rd, i);

            // t‰t‰ ei voi pist‰‰ eventPeriod:n, koska sisarusten syntym‰aikaa tarkastelemalla ei saada selkeit‰
            // min...max arvoja syntym‰lle. Antaa t‰n olla t‰s‰!
            for (int c = 0 ; c<indi[i].childIdList.size()-1 ; c++) {
                if ( indi[indi[i].childIdList.at(c)].ientry(BIRTH).day.known && indi[indi[i].childIdList.at(c+1)].ientry(BIRTH).day.known) {
                    if ( indi[indi[i].childIdList.at(c)].ientry(BIRTH).day.getAcc() == EXA && indi[indi[i].childIdList.at(c+1)].ientry(BIRTH).day.getAcc() == EXA ) {
                        if (indi[i].sex == MALE || !indi[indi[i].childIdList.at(c)].getFather()) {
                            int ageDiff = indi[indi[i].childIdList.at(c)].ientry(BIRTH).day.day1.daysTo( indi[indi[i].childIdList.at(c+1)].ientry(BIRTH).day.day1);
                            if ( ageDiff >1 && ageDiff < PREGNANCY_PERIOD ) {
                                // jotta samaa virhett‰ ei raportoida kahteen kertaa (eli kun k‰yd‰‰n l‰pi sek‰ is‰n ett‰ ‰idin lapsia)
                                // raportoidaan virhe vain silloin kun k‰yd‰‰n is‰n tietoja, tai silloin kun lapsella ei ole is‰‰
                                    Problem problem;
                                    problem.id = i;
                                    problem.indi = true;
                                    problem.type = PROBLEM_SIBLINGS_TOO_CLOSE;
                                    problem.explanation = indi[indi[i].childIdList.at(c)].nameFamily + " " + indi[indi[i].childIdList.at(c)].nameFirst + " and " + indi[indi[i].childIdList.at(c+1)].nameFamily + " " + indi[indi[i].childIdList.at(c+1)].nameFirst;
                                    problems.append(problem);
                            }
                            if ( ageDiff <1 ) {
                                GENE.twinAlfa.append(indi[i].childIdList.at(c));
                                GENE.twinBeta.append(indi[i].childIdList.at(c+1));
                            }
                        }
                    }
                }
            }
        }


        // ------------------------------------------------------------------------------------------------

        // Onko lapsi syntynyt aikaisemmin kuin 300 pv ennen is‰n kuolemaa
        // !! voiko pist‰‰ eventPeriod:n
        /*
        if (indi[i].getFather() && indi[i].getEntryDaybyType(true, BIRTH).known && indi[indi[i].getFather()].getEntryDaybyType(true, DEATH).known) {
            Date limitDate = indi[i].getEntryDaybyType(true, BIRTH).addDays(-PREGNANCY_PERIOD);
            if ( indi[indi[i].getFather()].getEntryDaybyType(true, DEATH) < limitDate) {
                if (!stopDemonNow) {
                    Problem problem;
                    problem.id = i;
                    problem.type = PROBLEM_FATHER_DEATH;
                    problem.explanation = indi[indi[i].getFather()].nameFamily + " " + indi[indi[i].getFather()].nameFirst;
                    problem.indi = true;
                    problems.append(problem);
                }
            }
        }
        */

        // Onko lapsi syntynyt ennen ‰idin kuolemaa
        // !! voiko pist‰‰ eventPeriod:n
        if (indi[i].getMother() && indi[i].getEntryDaybyType(true, BIRTH).known && indi[indi[i].getMother()].getEntryDaybyType(true, DEATH).known) {
            if ( indi[i].getEntryDaybyType(true, BIRTH) > indi[indi[i].getMother()].getEntryDaybyType(true, DEATH) ) {
                if (!stopDemonNow) {
                    Problem problem;
                    problem.id = i;
                    problem.type = PROBLEM_MOTHER_DEATH;
                    problem.explanation = indi[indi[i].getMother()].nameFamily + " " + indi[indi[i].getMother()].nameFirst;
                    problem.indi = true;
                    problems.append(problem);
                }
            }
        }

        // onko is‰ liian vanha
        // !! voiko pist‰‰ eventPeriod:n
        if (indi[i].getFather() && indi[i].getEntryDaybyType(true, BIRTH).known ) {
            Date limitDate = indi[i].getEntryDaybyType(true, BIRTH).addDays(-1*GET_AGE_FATHERCHILD_MAX*365);
            if ( indi[indi[i].getFather()].estimatedBirth < limitDate ) {
                if (!stopDemonNow) {
                    Problem problem;
                    problem.id = i;
                    problem.type = PROBLEM_TOO_OLD_FATHER;
                    problem.explanation = indi[indi[i].getFather()].nameFamily + " " + indi[indi[i].getFather()].nameFirst;
                    problem.indi = true;
                    problems.append(problem);

                }
            }
        }


        // onko ‰iti liian vanha
        // !! voiko pist‰‰ eventPeriod:n
        if (indi[i].getMother() && indi[i].getEntryDaybyType(true, BIRTH).known ) {
            Date limitDate = indi[i].getEntryDaybyType(true, BIRTH).addDays(-1*GET_AGE_MOTHERCHILD_MAX*365);
            if ( indi[indi[i].getMother()].estimatedBirth < limitDate) {
                if (!stopDemonNow) {                    
                    Problem problem;
                    problem.id = i;
                    problem.type = PROBLEM_TOO_OLD_MOTHER;
                    problem.explanation = indi[indi[i].getMother()].nameFamily + " " + indi[indi[i].getMother()].nameFirst;
                    problem.indi = true;
                    problems.append(problem);
                }
            }
        }

        // onko is‰ liian nuori
        // !! voiko pist‰‰ eventPeriod:n
        if (indi[i].getFather() && indi[i].getEntryDaybyType(true, BIRTH).known ) {
            Date limitDate = indi[i].getEntryDaybyType(true, BIRTH).addDays(-1*GET_AGE_FATHERCHILD_MIN*365);
            if ( indi[indi[i].getFather()].estimatedBirth > limitDate) {
                if (!stopDemonNow) {
                    Problem problem;
                    problem.id = i;
                    problem.type = PROBLEM_TOO_YOUNG_FATHER;
                    problem.explanation = indi[indi[i].getFather()].nameFamily + " " + indi[indi[i].getFather()].nameFirst;
                    problem.indi = true;
                    problems.append(problem);
                }
            }
        }

        // onko ‰iti liian nuori
        // !! voiko pist‰‰ eventPeriod:n
        if (indi[i].getMother() && indi[i].getEntryDaybyType(true, BIRTH).known ) {
            Date limitDate = indi[i].getEntryDaybyType(true, BIRTH).addDays(-1*GET_AGE_MOTHERCHILD_MIN*365);
            if ( indi[indi[i].getMother()].estimatedBirth > limitDate) {
                if (!stopDemonNow) {
                    Problem problem;
                    problem.id = i;
                    problem.type = PROBLEM_TOO_YOUNG_MOTHER;
                    problem.explanation = indi[indi[i].getMother()].nameFamily + " " + indi[indi[i].getMother()].nameFirst;
                    problem.indi = true;
                    problems.append(problem);
                }
            }
        }

        // ----------------- onko arvioitu syntym‰ajan max arvo aikaisemmin kuin min arvo -----------------
        // jos on niin vika on joko t‰ss‰ henkilˆss‰ tai jossain l‰hisukulaisessa.
        if (indi[i].estimatedBirth.day2 < indi[i].estimatedBirth.day1) {
            if (!stopDemonNow) {
               Problem problem;
               problem.id = i;
               problem.type = PROBLEM_DATE_BETWEEN_PERSONS;
               problem.explanation = "Birth between " + QString::number(indi[i].estimatedBirth.day1.year()) + " and " + QString::number(indi[i].estimatedBirth.day2.year()) + "?!";
               problem.indi = true;
               problems.append(problem);
           }
       }


       // tarkastetaan muut kuin BIRTH eventit
        // !! voiko pist‰‰ eventPeriod:n
       for (int j=0 ; j<indi[i].entry.size() ; j++) {
           if (!stopDemonNow) {
               checkAttr(true, i, indi[i].entry.at(j).attrText);
               Date entryDate = indi[i].entry.at(j).day;
               if (entryDate.day2.isNull()) entryDate.day2 = entryDate.day1;

               if (entryDate.known && indi[i].entry.at(j).type != BIRTH ) {
                   Date compDate;
                   compDate.setDate1( indi[i].estimatedBirth.day1.addYears(GET_I_MIN(indi[i].entry.at(j).type)) );
                   compDate.setDate2( indi[i].estimatedBirth.day1.addYears(GET_I_MAX(indi[i].entry.at(j).type)) );
                   if ( entryDate != compDate ) {
                       Problem problem;
                       problem.id = i;
                       problem.indi = true;
                       problem.type = PROBLEM_EVENT_DAY_VIOLATION;
                       problem.explanation = indi[i].entry.value(j).typeString() + " (" + indi[i].entry.value(j).day.day1.toString("dd.MM.yyyy") + ")";
                       problems.append(problem);
                   }
               }
           }

           checkPlaces(true, i, indi[i].entry.at(j).place, indi[i].entry.value(j).typeString());
       }
// ----------------------------------------------------------------------------------

       int e;

       for (e=0 ; e<INDI(i).entry.size() ; e++) {

           Date comp;
           Date day;
           INDI(i).eventPeriod(&comp, INDI(i).entry.at(e).type, true); //

           comp.setAcc(FRO);

           day = INDI(i).entry.at(e).day;
           if ( comp != day ) {

               Problem problem;
               problem.id = i;
               problem.indi = true;
               if (comp < day)
                   problem.explanation = indi[i].entry.value(e).typeString() + " of " + indi[i].nameFirst + " " + day.day1.toString("dd.MM.yyyy")+ " should be earlier than " + comp.day2.toString("dd.MM.yyyy") + " (" + comp.day2explanation + ")";
               else
                   problem.explanation = indi[i].entry.value(e).typeString() + " of " + indi[i].nameFirst + " " + day.day1.toString("dd.MM.yyyy") + " should be later than " + comp.day1.toString("dd.MM.yyyy") + " (" + comp.day1explanation + ")";
               problem.type = PROBLEM_EVENT_ORDER_VIOLATION;
               problems.append(problem);
           }
       }

// ----------------------------------------------------------------------------------

       for (int f=0 ; f< indi[i].famSList.size() ; f++) {
           int s1;
           s1 = indi[i].getSpouse(f);

           if (s1) {
               // tarkastetaan ett‰ t‰m‰ henkilˆ ei ole mennyt naimisiin tai eronnut t‰m‰n avioliiton aikana toisen henkilˆn kanssa
               if (indi[i].fentry(MARRIAGE).day.known && indi[i].fentry(DIVORCE).day.known) {
                   int s2;
                   for (int f2 = 0 ; f2<indi[i].famSList.size() ; f2++) {
                        s2 = indi[i].getSpouse(f2);

                        if (s1 != s2) {
                            // avioliiton oletetaan alkaneen silloin kuin engagement, marriage tapahtumat on toteutuneet
                            // avioliitto p‰‰ttyy silloin kuin divorce tai annulment
                            Date marr1 = Date::getSmallestKnown(INDI(s1).fentry(ENGAGEMENT).day, INDI(s1).fentry(MARRIAGE).day,  Date() );
                            Date marr2 = Date::getSmallestKnown(INDI(s2).fentry(ENGAGEMENT).day, INDI(s2).fentry(MARRIAGE).day,  Date() );
                            Date divo1 = Date::getSmallestKnown(INDI(s1).fentry(DIVORCE).day,    INDI(s1).fentry(ANNULMENT).day, Date() );
                            Date divo2 = Date::getSmallestKnown(INDI(s2).fentry(DIVORCE).day,    INDI(s2).fentry(ANNULMENT).day, Date() );

                            bool crime = false;
                            if (marr1.known && marr2.known && divo1.known && Date::isOrder(marr1, marr2, divo1) ) crime = true;
                            if (marr1.known && divo2.known && divo1.known && Date::isOrder(marr1, divo2, divo1) ) crime = true;
                            if (marr2.known && marr1.known && divo2.known && Date::isOrder(marr2, marr1, divo2) ) crime = true;
                            if (marr2.known && divo2.known && divo2.known && Date::isOrder(marr2, divo2, divo2) ) crime = true;

                            if (crime) {
                                Problem problem;
                                problem.id = i;
                                problem.indi = true;
                                problem.explanation = indi[s1].nameFirst + " and " + indi[s2].nameFirst;
                                problem.type = PROBLEM_MARRIAGE_CRIME;
                                problems.append(problem);
                            }
                        }
                   }
               }

               // Onko puoliso sopivan ik‰inen?

               Date limitDate;
               limitDate.day1 = indi[i].estimatedBirth.day1.addDays(-1*GET_AGE_SPOUSES_MAX*365);
               limitDate.day2 = indi[i].estimatedBirth.day2.addDays(   GET_AGE_SPOUSES_MAX*365);


               if (indi[s1].estimatedBirth.day1 > limitDate.day2 ) {
                   Problem problem;
                   problem.id = i;
                   problem.indi = true;
                   problem.explanation = indi[s1].nameFamily + " " + indi[s1].nameFirst;
                   problem.type = PROBLEM_TOO_YOUNG_SPOUSE;
                   problems.append(problem);
               }


               if (indi[s1].estimatedBirth.day2 < limitDate.day1 ) {
                   Problem problem;
                   problem.id = i;
                   problem.indi = true;
                   problem.explanation = indi[s1].nameFamily + " " + indi[s1].nameFirst;
                   problem.type = PROBLEM_TOO_OLD_SPOUSE;
                   problems.append(problem);
               }

               // e = event

               for (e=0 ; e<fami[indi[i].famSList.at(f)].entry.size() ; e++ ) {
                   Entry fentry = fami[indi[i].famSList.at(f)].entry.at(e);
                   Date comp;
                   Date day;
                   INDI(i).eventPeriod(&comp, fentry.type, false); //
                   comp.setAcc(FRO);

                   day = fentry.day;
                   if ( comp != day ) {
                       Problem problem;
                       problem.id = i;
                       problem.indi = false;
                       if (comp < day) problem.explanation = fentry.typeString() + " too late (" + day.day1.toString("dd.MM.yyyy") + ")";
                       else problem.explanation = fentry.typeString() + " too early (" + day.day1.toString("dd.MM.yyyy") + ")";
                       problem.type = PROBLEM_EVENT_ORDER_VIOLATION;
                       problems.append(problem);
                   }
               }
            }
        }
    }

// --------------------- person errors check completed ------------------------------

    emit sigStatusBarMessage("Looking for family errors...");
    for (quint16 f=1 ; f<= famiLastUsed ; f++) {
        if ( ( indi[fami[f].husb].sex == indi[indi[fami[f].husb].getSpouse()].sex ) && indi[fami[f].husb].getSpouse() ) {
            if (!stopDemonNow) {
                Problem problem;
                problem.id = fami[f].husb;
                problem.type = PROBLEM_HOMOCOUPLE;
                problem.indi = false;
                problem.explanation = indi[fami[f].husb].nameFirst + " & " + indi[indi[fami[f].husb].getSpouse()].nameFirst;
                problems.append(problem);
            }
        }

        for (int e=0 ; e<fami[f].entry.size() ; e++) {
            if (!stopDemonNow) {
                int corrId;
                if (fami[f].husb != 0) corrId = fami[f].husb;
                else corrId = fami[f].wife;

                checkPlaces(false, corrId, fami[f].entry.at(e).place, fami[f].entry.value(e).typeString());
            }
        }
    }

    checkSources();

    emit(sigStatusBarMessage("Found " + QString::number(problems.size()) + " errors"));
    demonRun.errors = false;
}

void GeneData::familyRelationTest()
{
    demonRun.familyRelations = true;
//    if (indiLastUsed == 1) return;

    if (rootFams) {

        demonRun.familyRelations = false;

        for (quint16 i=1 ; i<=indiLastUsed ; i++) indi[i].familyRelation = "";
        this->generationData.clear();

        // lasketaan familyrelationit t‰ss‰
        for (int i=0 ; i<fami[rootFams].childs.size() ; i++) {
            int rootId = fami[rootFams].childs.at(i);
            addFamRel(rootId, rootId, "#");
        }

        int emptys, emptysOld;

        emptys = emptysOld = 0;

        do {
            emptysOld = emptys;
            emptys = 0;
            for (quint16 i = 1 ; i<=indiLastUsed ; i++) {
                if (indi[i].familyRelation == "") {
                    addFamRelAdd(i); // t‰m‰ kaataa
                    emptys++;
                }
            }
        } while (emptys != emptysOld && !stopDemonNow);

        for (quint16 i=1 ; i<=indiLastUsed ; i++) {
            if (!stopDemonNow) {
                indi[i].generation =
                        indi[i].familyRelation.count("f") +
                        indi[i].familyRelation.count("m") -
                        indi[i].familyRelation.count("c");

                if ((indi[i].familyRelation.count("c") == 0) && (indi[i].familyRelation.count("s") == 0) ) indi[i].isDirectAncestor = true; else indi[i].isDirectAncestor = false;

                GenerationData ld = generationData.value(indi[i].generation);

                ld.count++;

                QDate birth = indi[i].ientry(BIRTH).day.day1;

                if (ld.min.isNull() || (!birth.isNull() && birth < ld.min) ) ld.min = birth;
                if (ld.min.isNull() || (!birth.isNull() && birth > ld.max) ) ld.max = birth;
                if (ld.estMin.isNull() || (ld.estMin > indi[i].estimatedBirth.day1) ) ld.estMin = indi[i].estimatedBirth.day1;
                if (ld.estMax.isNull() || (ld.estMax < indi[i].estimatedBirth.day2) ) ld.estMax = indi[i].estimatedBirth.day2;

                generationData.insert(indi[i].generation, ld);
            }
        }        
    }

    for (int i=0 ; i<=geneData->indiLastUsed ; i++) INDI(i).howManyDescentantSteps();

    emit sigStatusBarMessage("Checking estimated dates and fixing them...");
}

void GeneData::run()
{
    if (!rootFams) {
        // root perhearvoa ei ole asetettu. Varmaan uusi tietokanta tai avattu uusi tiedosto jossa
        // sit‰ ei ole m‰‰ritelty. Arvotaan joku arvo, k‰ytt‰j‰ voi sen korjata niin halutessaan.

        if (FAM(INDI(currentId).getFamS()).childs.size()) {
            rootFams = INDI(currentId).getFamS();
            rootId = currentId;
            GENE.demonRun.familyRelations = true;
            emit(sigStatusBarMessage("debugA"));
        } else if (INDI(currentId).famc.value) {
            rootFams = INDI(currentId).famc.value;
            rootId = currentId;
            GENE.demonRun.familyRelations = true;
            emit(sigStatusBarMessage("debugB"));
        }
    }

    wasRunningWhenStopped = false;
    emit(sigStatusBarMessage("Calculating relationships..."));
    makeChildIdLists();
    if ( demonRun.familyRelations && !stopDemonNow ) this->familyRelationTest();
    if ( demonRun.estimatedBirths && !stopDemonNow ) this->estimatedDateTest();
    //if ( demonRun.dateAccuracyTest && !stopDemonNow ) this->dayAccuracyTest(); // toimii
    if ( demonRun.errors && !stopDemonNow ) this->problemFinding();
    if (stopDemonNow) {
        wasRunningWhenStopped = true;
        problems.clear();
        emit sigStatusBarMessage("Database Analyze Suspended");
    }
    emit (sigStatusBarMessage(QString::number(GENE.twinAlfa.size()) + " twins found."));

    emit (sigUpdate()); // *** muutettu sigUpdate() st‰
    //emit (sigUpdate(GENE.rootId)); // *** muutettu sigUpdate() st‰

}


void GeneData::stop()
{
    if (this->isRunning()) {
        stopDemonNow = true;
        wait(5000);
        if (this->isRunning()) this->terminate();
    }
}

// --------------------------------------------------------------------------------------
void GeneData::pasteFromEditUndo()
{
    int x;

    for (x=0 ; x<editIndiUndo.size() ; x++) {
        indi[editIndiUndo.at(x).id] = editIndiUndo.at(x);
    }
    for (x=0 ; x<editFamUndo.size()  ; x++) {
        fami[editFamUndo.at(x).id] = editFamUndo.at(x);
    }

    // ollaan ehk‰ oltu luomassa uutta perhett‰, mutta luovuttu aikeesta.
    if (editUndoFamLastUsed < famiLastUsed) fami[famiLastUsed].clear(this);


    famiLastUsed = editUndoFamLastUsed;
    indiLastUsed = editUndoIndiLastUsed;
    currentId = editUndoCurrentId;

    indi[editAddIndi].clear(this);
    fami[editAddFam].clear(this);

    famiCount = editUndoFamiCount;
    indiCount = editUndoIndiCount;
}

void GeneData::copyToEditUndo(int id)
{
    if (id >= 0) {
        editUndoFamiCount = famiCount;
        editUndoIndiCount = indiCount;

        editIndiUndo.append(indi[id]);
        for (int f=0 ; f<indi[id].famSList.size() ; f++) {
            editFamUndo.append(fami[indi[id].famSList.at(f)]);
        }

        editFamUndo.append(fami[indi[id].famc.value]);
        editUndoFamLastUsed = famiLastUsed;
        editUndoIndiLastUsed = indiLastUsed;
        editUndoCurrentId = currentId;
        editAddIndi = 0;
        editAddFam = 0;
    }    
}

void GeneData::clearEditUndo()
{
    editIndiUndo.clear();
    editFamUndo.clear();

    editUndoFamLastUsed     = 0;
    editUndoIndiLastUsed    = 0;
    editUndoCurrentId       = 0;
    editAddIndi             = 0;
    editAddFam              = 0;

    famiCount               = editUndoFamiCount;//       = 0;
    indiCount               = editUndoIndiCount;//       = 0;
}

void GeneData::indiSpace(quint16 j)
{
    this->demonRun.familyRelations = true;
    if (j > indiLastUsed) indiLastUsed = j;

    int add = 500;

    if ((int)( indiCount - j ) < 1 ) {
        quint16 x;
        quint16 oldSize = indiLastUsed; indiCount = 0;
        quint16 newSize = MAX(j, (oldSize + add));

        PersonData* tmp = new PersonData[newSize];
        if (indi && oldSize) {
            for (x=0 ; x<oldSize ; x++) tmp[x] = indi[x];

            delete [] indi;
        }

        indi = tmp;

        indiCount = newSize;

        for (x = 0 ; x <= indiCount ; x++) indi[x].parent = this;
    }

}

void GeneData::famiSpace(quint16 j)
{
    if (j > famiLastUsed ) famiLastUsed = j;

    int add = 200;

    if ((int)( famiCount - j) < 1) {
        quint16 x;
        int oldSize = famiLastUsed ; famiCount = 0;
        int newSize = MAX(j, (oldSize + add));

        FamilyData* tmp = new FamilyData[newSize];

        if (fami && oldSize) {
            int i ; for (i = 0 ; i<oldSize ; i++) tmp[i] = fami[i];
            delete [] fami;
        }
        fami = tmp;
        famiCount = newSize;
        for (x = 0 ; x < famiCount ; x++) fami[x].parent = this;

    }
}

void GeneData::objeSpace(quint16 id)
{
    if (id > objeLastUsed ) objeLastUsed = id;

    int add = MAX(id, 5);

    if ((int)( objeCount  - id) < 1) {
        int oldSize = objeLastUsed ; objeCount = 0;
        MultiMediaRecord* tmp = new MultiMediaRecord[oldSize + add];
        if (obje && oldSize) {
            int i ; for (i = 0 ; i<oldSize ; i++) tmp[i] = obje[i];
            delete [] obje;
        }
        obje = tmp;
        objeCount = oldSize + add;
    }
}

void GeneData::noteSpace(quint16 id)
{
    if (id > noteLastUsed) noteLastUsed = id;

    int add = MAX(5, id);

    if ((int)( noteCount - id) < 1) {
        int oldSize = noteLastUsed ; noteCount = 0;
        NoteRecord* tmp = new NoteRecord[oldSize + add];
        if (note && oldSize) {
            int i ; for (i = 0 ; i<oldSize ; i++) tmp[i] = note[i];
            delete [] note;
        }
        note = tmp;
        noteCount = oldSize + add;
    }
}

void GeneData::repoSpace(quint16 id)
{
    if (id > repoLastUsed ) repoLastUsed = id;

    int add = MAX(id, 5);

    if ((int)( repoCount - id) < 1) {
        int oldSize = repoLastUsed ; repoCount = 0;
        Repo* tmp = new Repo[oldSize + add];
        if (repo && oldSize) {
            int i ; for (i = 0 ; i<oldSize ; i++) tmp[i] = repo[i];
            delete [] repo;
        }
        repo = tmp;
        repoCount = oldSize + add;
    }
}

void GeneData::sourSpace(quint16 id)
{
    if (id > sourLastUsed) sourLastUsed = id;

    int add = MAX(id, 5);

    if ((int)( sourCount - id) < 1) {
        int oldSize = sourLastUsed ; sourCount = 0;
        SourceRecord* tmp = new SourceRecord[oldSize + add];
        if (sour && oldSize) {
            int i ; for (i = 0 ; i<oldSize ; i++) tmp[i] = sour[i];
            delete [] sour;            
        }
        sour = tmp;
        sourCount = oldSize + add;
    }
}

void GeneData::format()
{
    //qDebug() << "formatting A " << this->id << GeneData::currentGd;
    this->clear();
    //qDebug() << "formatting B " << this->id << GeneData::currentGd;
    this->indiSpace(2000); this->indiLastUsed = 0;
    this->famiSpace(400); this->famiLastUsed = 0;
    this->noteSpace(20); this->noteLastUsed = 0;
    this->objeSpace(20); this->objeLastUsed = 0;
    this->repoSpace(20) ; this->repoLastUsed = 0;
    this->sourSpace(20); this->sourLastUsed = 0;
    this->sour[0].title = "no source";
}

void GeneData::clear()
{
    this->fileName      = "";

    this->canBeSaved = false;
    this->isSaved   = false;
    this->locked = false;
    this->wasRunningWhenStopped = false;

    this->currentId = 0;
    this->browseHistory.clear();

    demonRun.errors          = true;
    demonRun.estimatedBirths = true;
    demonRun.familyRelations = true;
    demonRun.dateAccuracyTest = true;
    clearEditUndo();

    this->stopDemonNow = false;

    this->indiCount     = 0;
    this->famiCount     = 0;
    this->noteCount     = 0;
    this->objeCount     = 0;
    this->repoCount     = 0;
    this->sourCount     = 0;
    this->rootId        = 0;
//    this->currentGd     = 0; t‰t‰ ei nollata koska t‰m‰ on staattinen muuttuja joka kertoo mik‰ gd on n‰kyvill‰
    this->currentId     = 0;

}

GeneData::GeneData(QObject *parent) : QThread(parent)
{    
    this->id = GeneData::gdCount;
    indi = NULL; this->indiCount = 0; this->indiLastUsed = 0;
    fami = NULL; this->famiCount = 0; this->famiLastUsed = 0;
    repo = NULL; this->repoCount = 0; this->repoLastUsed = 0;
    obje = NULL; this->objeCount = 0; this->objeLastUsed = 0;
    note = NULL; this->noteCount = 0; this->noteLastUsed = 0;
    sour = NULL; this->sourCount = 0; this->sourLastUsed = 0;
    generationData.clear();
    reporter = NULL;
    currentId = 0;
    GeneData::gdCount++;    
}

bool GeneData::userSelections()
{
    for (quint16 i=0 ; i<=this->indiLastUsed ; i++) if (this->indi[i].selected) return true;
    return false;
}

bool GeneData::gdSpace(quint8 id)
{
    // t‰t‰ kutsutaan mainWindowsista ainoastaan, ja vain silloin kuin varmasti tiedet‰‰n ett‰
    // gd:aa pit‰‰ kasvattaa

    quint8 add = id - GeneData::gdCount;
    quint8 currentGdTmp = GeneData::currentGd;
    if (add==0) add = 1;
    int i ;
    int oldSize = GeneData::gdCount;
    GeneData::gdCount = 0; // automaattinen muuttuja joka kertoo miten iso GeneData taulukko on

    GeneData * tmp = new GeneData[oldSize + add];

    //if ( geneData && oldSize)
        for (i = 0 ; i<oldSize ; i++) {
            geneData[i].copyTo(&tmp[i]);
        }

    delete [] geneData;
    geneData = tmp;
    geneData->currentGd = currentGdTmp;
    GeneData::gdCount = oldSize + add;

    return true;
}

GeneData::~GeneData()
{
    terminate();
    clear();
}

void GeneData::copyTo(GeneData *gd)
{
    quint16 x;

    gd->fileName = "";
    gd->canBeSaved = false;
    gd->locked = false;
    gd->wasRunningWhenStopped = false;

    gd->header = this->header;
    gd->generationData = this->generationData;
    gd->subm = this->subm;
    gd->repoCount = this->repoCount ; gd->repoLastUsed = this->repoLastUsed; gd->repo = this->repo;
    gd->objeCount = this->objeCount ; gd->objeLastUsed = this->objeLastUsed; gd->obje = this->obje;
    gd->noteCount = this->noteCount ; gd->noteLastUsed = this->noteLastUsed; gd->note = this->note;
    gd->sourCount = this->sourCount ; gd->sourLastUsed = this->sourLastUsed; gd->sour = this->sour;
    gd->indiCount = this->indiCount ; gd->indiLastUsed = this->indiLastUsed; gd->indi = this->indi;
    gd->famiCount = this->famiCount ; gd->famiLastUsed = this->famiLastUsed; gd->fami = this->fami;

    for (x=0 ; x<=this->indiLastUsed ; x++) {
        gd->indi[x].parent = gd;
    }
    for (x=0 ; x<=this->famiLastUsed ; x++) gd->fami[x].parent = gd;

    gd->problems = this->problems;
    gd->currentId = this->currentId;
    gd->rootFams = this->rootFams;
    gd->rootId = this->rootId;
    gd->fileName = this->fileName;

    gd->indiLastUsed = this->indiLastUsed;
    gd->browseHistory = this->browseHistory;
    gd->canBeSaved = this->canBeSaved;
    gd->locked = this->locked;
    gd->id = this->id;
    gd->stopDemonNow = false;
    gd->wasRunningWhenStopped = false;
}


