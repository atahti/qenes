#include "panel.h"
#include "data/genedata.h"
#include "ui/person.h"
#include <QSettings>
#include <QDebug>

extern  GeneData        * geneData;
extern  QSettings       sets;


void Panel::slotPersonAssigned(quint16 assignedId)
{
    if (assignedId) {
        if (this->panelType == PANEL_FEMALE) INDI(*pChildId).assignMother(assignedId);
        if (this->panelType ==   PANEL_MALE) INDI(*pChildId).assignFather(assignedId);
        emit (sigUpdate(0));
    }
}

void Panel::showPerson()
{
    bool showMale = false;
    bool showFemale = false;

    idWhenOpened = id;

    if (GENE.locked) {
        QMessageBox qmb;
        qmb.setText("Not editable during output operation");
        qmb.exec();
        return;
    }

    GENE.browseHistory.removeAll(id);
    if (!INDI(id).deleted && id != 0 ) GENE.browseHistory.prepend(id);

    /* Oletetaan, ett� k�ytt�j� haluaa pit�� muutokset jotka h�n tekee, ja poikkeustilanteessa
       painetaan "close without saving". Siksi muutokset tehd��n suoraan tietokantaan (person.cpp:ss�)
       sit�mukaa kun muutoksia tehd��n. Jos muutokset perutaan, pit�� palauttaa varmuuskopio muutoksia
       edelt�neest� tilanteesta. Alla otetaan varmuuskopiot. */

    GENE.copyToEditUndo(id);
    GENE.copyToEditUndo(*pChildId);

    // Kopsataan viel� puoliso
    if (panelType == PANEL_FEMALE) { // mother
        GENE.copyToEditUndo(INDI(*pChildId).getFather());
    } else {
        GENE.copyToEditUndo(INDI(*pChildId).getMother());
    }

    if (id == 0) {
        //// Uusi henkil�

        id = GENE.indiLastUsed+1;
        GENE.editAddIndi = id;

        GENE.indiSpace(id);    // t�m�n j�lkeen gd.indiLastUsed = id;

        if (INDI(id).deleted) INDI(id).clear(&GENE);
        // lets reuse if this has been deleted. This may have been deleted if user has been staring to
        // enter a new person, but pressed cancel.

        if ( panelType == PANEL_FEMALE ) {
            showMale = false;
            showFemale = true;
            INDI(id).sex = FEMALE;
        }
        if ( panelType == PANEL_MALE ) {
            showMale = true;
            showFemale = false;
            INDI(id).sex = MALE;
        }

        if (INDI(*pChildId).famc.value == 0) {
            // famc on siis lapsen lapsiperhe-id. Jos t�m� famc
            // on 0, ei siis t�m�n henkil�n lapsella ole kumpaakaan vanhempaa ja pit�� luoda uusi
            // familyID, joka sijoitetaan t�m�n henkil�n spouse-family arvoon ja lapse child-family
            // arvoon.

            GENE.editAddFam = GENE.famiLastUsed + 1;
            GENE.famiSpace(GENE.editAddFam);
            INDI(*pChildId).famc.value = GENE.editAddFam; // (*1)
            INDI(id).setChild(*pChildId);

            if ( INDI(id).sex == MALE ) FAM(GENE.editAddFam).husb = id; // *2
            else FAM(GENE.editAddFam).wife = id; // my�s jos sex = unknown

        } else {

            // T�ss� tapauksessa em. famc arvo on != 0, eli t�m�n nyt annettavan henkil�n lapsella
            // on jo lapsiperhe ID, ts. t�ll�kin henkil�ll� on jo puoliso.

            // Eli t�nne eksyt��n, kun ollaan lis��m�ss� uutta mummoa tai vaaria, jolla on olemassa jo puoliso
            quint16 fam = INDI(*pChildId).famc.value;

            INDI(id).famSList.append(fam);
            // T�h�n kohtaan eksyt��n vain silloin kun annetaan esi-is��. Eli t�m�n uuden henkil�n famS voi olla
            // vain 0... t�ss� oli : //INDI(id).currentSpouse = INDI(id).famSList.size();
            INDI(id).currentSpouse = 0;


            if (INDI(id).sex == MALE) {
                FAM(fam).husb = id;
                FAM(fam).wife = INDI(*pChildId).getMother();

            } else {
                FAM(fam).wife = id;
                FAM(fam).husb = INDI(*pChildId).getFather();

            }            
        }

        INDI(id).setRin();

    }

    INDI(id).estimatedBirth = INDI(id).getEstimatedBirth();
    Person ps(showMale, showFemale, &id);//###
    //Person ps(showMale, showFemale, &id, pChildId);//###

    connect(&ps, SIGNAL(sigUpdate(quint16)), this, SIGNAL(sigUpdate(quint16)));
    connect(&ps, SIGNAL(sigPersonAssigned(quint16)), this, SLOT(slotPersonAssigned(quint16)));

    int result = ps.exec();

    if (!result) {
        // Painettiin exit without save. Palautetaan 0:n tallennettu backup copy.

        id = idWhenOpened;
        GENE.stop();
        GENE.pasteFromEditUndo();
        if (GENE.wasRunningWhenStopped && RUN_CHECK ) GENE.start(QThread::NormalPriority);
    } else {
        GENE.stop();
        GENE.demonRun.dateAccuracyTest = true;
        GENE.demonRun.familyRelations = true;
        GENE.demonRun.estimatedBirths = true;
        GENE.demonRun.errors = true;
        if (RUN_CHECK) GENE.start(QThread::NormalPriority);
    }

    GENE.clearEditUndo();
//qDebug() << "debug 000";
    emit(sigUpdate(0));

    disconnect(&ps, SIGNAL(sigUpdate(quint16)), this, SIGNAL(sigUpdate(quint16)));
    disconnect(&ps, SIGNAL(sigPersonAssigned(quint16)), this, SLOT(slotPersonAssigned(quint16)));
}

void Panel::center()
{
    qDebug() << "Panel::center";
    emit (this->sigUpdate(id));
}

void Panel::panelPressed()
{
    timer->start(200);
}

void Panel::panelReleased()
{
    if (ignoreNextRelease) ignoreNextRelease = false;

    else {
        timer->stop();
        showPerson();
    }
}

void Panel::timerTimeOut()
{
    ignoreNextRelease = true;
    timer->stop();
    center();
}

Panel::Panel()
{
    ignoreNextRelease = false;
    timer = new QTimer(this);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(this, SIGNAL(pressed()), this, SLOT(panelPressed()));
    connect(this, SIGNAL(released()), this, SLOT(panelReleased()));
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeOut()));

}


