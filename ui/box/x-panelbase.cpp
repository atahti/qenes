#include "Panelbase.h"
#include "data/genedata.h"
#include "mainwindow.h"
#include "ui/person.h"
#include <QSettings>
#include <QDebug>

extern  GeneData        * geneData;
extern  QSettings       sets;

void PanelBase::slotPersonAssigned(int assignedId)
{
    if (assignedId) {
        if (this->boxKind == FEMALEBOX) INDI(child).assignMother(assignedId);
        if (this->boxKind ==   MALEBOX) INDI(child).assignFather(assignedId);
        emit (sigUpdate(0));
    }
}

QPoint PanelBase::leftPos(QPoint global)
{
    QPoint posMain = this->mapToGlobal(QPoint(0, 0));
    QPoint returnValue;

    returnValue.setX(  posMain.x() - global.x());
    returnValue.setY( (posMain.y() - global.y()) + this->height()/2 );

    return returnValue;

}

QPoint PanelBase::rightPos(QPoint global)
{
    QPoint posMain = this->mapToGlobal(QPoint(0, 0));
    QPoint returnValue;

    returnValue.setX(  posMain.x() - global.x()  + this->width() );
    returnValue.setY( (posMain.y() - global.y()) + this->height()/2  );

    return returnValue;
}

void PanelBase::setStyle(QString style)
{
    setStyleSheet(style);
    uiCenter->setStyleSheet(UNKNOWNSTYLE);
}

void PanelBase::showPerson()
{
    bool showMale = false;
    bool showFemale = false;

    if (GENE.locked) {
        QMessageBox qmb;
        qmb.setText("Not editable during output operation");
        qmb.exec();
        return;
    }

    GENE.browseHistory.removeAll(id);
    if (!INDI(id).deleted && id != 0 ) GENE.browseHistory.prepend(id);

    /* Oletetaan, ett‰ k‰ytt‰j‰ haluaa pit‰‰ muutokset jotka h‰n tekee, ja poikkeustilanteessa
       painetaan "close without saving". Siksi muutokset tehd‰‰n suoraan tietokantaan (person.cpp:ss‰)
       sit‰mukaa kun muutoksia tehd‰‰n. Jos muutokset perutaan, pit‰‰ palauttaa varmuuskopio muutoksia
       edelt‰neest‰ tilanteesta. Alla otetaan varmuuskopiot. */

    GENE.copyToEditUndo(id);
    GENE.copyToEditUndo(child);

    if (id == 0) {
        // Uusi henkilˆ
        qDebug() << "uusi " << GeneData::currentGd << GENE.indiLastUsed;
        id = GENE.indiLastUsed+1;
        GENE.editAddIndi = id;

        GENE.indiSpace(id);    // t‰m‰n j‰lkeen gd.indiLastUsed = id;

        if (INDI(id).deleted) INDI(id).clear(&GENE);
        // lets reuse if this has been deleted. This may have been deleted if user has been staring to
        // enter a new person, but pressed cancel.

        QDate allowedDateMin, allowedDateMax;
        if ( boxKind == FEMALEBOX ) {
            showMale = false;
            showFemale = true;
            INDI(id).sex = FEMALE;
            allowedDateMin.setDate(INDI(child).estimatedBirth.day1.year() - GET_AGE_MOTHERCHILD_MAX,
                                 INDI(child).estimatedBirth.day1.month(),
                                 INDI(child).estimatedBirth.day1.day());
            allowedDateMax.setDate(INDI(child).estimatedBirth.day1.year() - GET_AGE_MOTHERCHILD_MIN,
                                 INDI(child).estimatedBirth.day1.month(),
                                 INDI(child).estimatedBirth.day1.day());
        }
        if ( boxKind == MALEBOX ) {
            showMale = true;
            showFemale = false;
            INDI(id).sex = MALE;
            allowedDateMin.setDate(INDI(child).estimatedBirth.day1.year() - GET_AGE_FATHERCHILD_MAX,
                                 INDI(child).estimatedBirth.day1.month(),
                                 INDI(child).estimatedBirth.day1.day());
            allowedDateMax.setDate(INDI(child).estimatedBirth.day1.year() - GET_AGE_FATHERCHILD_MIN,
                                 INDI(child).estimatedBirth.day1.month(),
                                 INDI(child).estimatedBirth.day1.day());
        }

        INDI(id).estimatedBirth.setDate1(allowedDateMin);
        INDI(id).estimatedBirth.setDate2(allowedDateMax);

        if (INDI(child).famc.value == 0) {
            // famc on siis lapsen lapsiperhe-id. Jos t‰m‰ famc
            // on 0, ei siis t‰m‰n henkilˆn lapsella ole kumpaakaan vanhempaa ja pit‰‰ luoda uusi
            // familyID, joka sijoitetaan t‰m‰n henkilˆn spouse-family arvoon ja lapse child-family
            // arvoon.

            GENE.editAddFam = GENE.famLastUsed + 1;
            GENE.famSpace(GENE.editAddFam);
            INDI(child).famc.value = GENE.editAddFam; // (*1)
            INDI(id).setChild(child);

            if ( INDI(id).sex == MALE ) FAM(GENE.editAddFam).husb = id; // *2
            else FAM(GENE.editAddFam).wife = id; // myˆs jos sex = unknown


        } else {

            // T‰ss‰ tapauksessa em. famc arvo on != 0, eli t‰m‰n nyt annettavan henkilˆn lapsella
            // on jo lapsiperhe ID, ts. t‰ll‰kin henkilˆll‰ on jo puoliso. K‰ytet‰‰n setSpouse funktiota
            // asettamaan famS, wife ja husb arvot.

            INDI(id).setChild(child);

            if (INDI(id).sex == FEMALE) INDI(id).setSpouse(INDI(child).getFather());
            else INDI(id).setSpouse(INDI(child).getMother());

        }

        INDI(id).setRin();

    }

    Person ps(showMale, showFemale, &id, &child);
    qDebug() << "PanelBase::showPerson";
    connect(&ps, SIGNAL(sigUpdate(int)), this, SIGNAL(sigUpdate(int)));
    connect(&ps, SIGNAL(sigPersonAssigned(int)), this, SLOT(slotPersonAssigned(int)));

    int result = ps.exec();

    if (!result) {
        // Painettiin exit without save. Palautetaan 0:n tallennettu backup copy.
        GENE.stop();
        GENE.pasteFromEditUndo();
        if (GENE.wasRunningWhenStopped) GENE.start(QThread::NormalPriority);
    } else {
        GENE.stop();
        GENE.demonRun.dateAccuracyTest = true;
        GENE.demonRun.familyRelations = true;
        GENE.demonRun.estimatedBirths = true;
        GENE.demonRun.errors = true;
        GENE.start(QThread::NormalPriority);
    }

    emit(sigUpdate(0));

    disconnect(&ps, SIGNAL(sigUpdate(int)), this, SIGNAL(sigUpdate(int)));
    disconnect(&ps, SIGNAL(sigPersonAssigned(int)), this, SLOT(slotPersonAssigned(int)));
}

void PanelBase::center()
{
    emit (this->sigUpdate(id));
}

void PanelBase::clear()
{
    uiName->setText("");
    if (uiRelation) uiRelation->setText("");

    if (uiPhoto) uiPhoto->setVisible(false);
    if (uiBirthPlace) uiBirthPlace->setText("");
    if (uiDeathPlace) uiDeathPlace->setText("");
    if (uiDeathDay) uiDeathDay->setText("");
    if (uiBirthDay) uiBirthDay->setText("");
}

QString PanelBase::place(QString input)
{
    QRegExp nameExp("(.[^\\/]*)?/?([^\\/].[^\\/]*)?/?([^\\/].[^\\/]*)?");
    nameExp.indexIn(input);
    return nameExp.cap(1);
}

PanelBase::PanelBase()
{

}

void PanelBase::updt(int c, int rel)
{
    QString birth, death;
    id = c;
    child = rel;

    setIcon(QIcon());

    for (int i = 0 ; i<GENE.problems.size() ; i++ ) {
        if ( GENE.problems.at(i).id == id ) {
            QIcon icon;
            icon.addFile(QString::fromUtf8(":/icons/error.jpg"), QSize(), QIcon::Normal, QIcon::Off);
            setIcon(icon);
        }
    }

    if ( (id == 0) && ( child == 0 ) ) {
        // the person is not known for this box.
        // Persondata cannot be added to this box.
        //if (uiPhoto) uiPhoto->setVisible(false);
        setText(""); //   uiBox->setTitle("");
        clear();
        setDisabled(true);
        setText("");
        if (uiCenter) uiCenter->setDisabled(true);
        setStyle(UNKNOWNSTYLE);

    } else {
        if ( (child != 0) && (id==0) ) {
            // T‰ss‰ boxissa olevaa henkilˆ‰ ei ole syˆtetty aiemmin, mutta boxilla on sukulainen
            // (eli puoliso tai lapsi) joten t‰h‰n boxiin voidaan syˆtt‰‰ henkilˆn tiedot.
            //if (uiPhoto) uiPhoto->setVisible(false);

            QIcon icon;
            icon.addFile(QString::fromUtf8(":/icons/benefits.png"), QSize(), QIcon::Normal, QIcon::Off);
            setIcon(icon);

            setDisabled(false);

            if (uiCenter) uiCenter->setDisabled(false);

            if ( boxKind == FEMALEBOX ) setStyle(FEMALESTYLE);
            else if ( boxKind == MALEBOX ) setStyle(MALESTYLE);
            else setStyle(YELLOWSTYLE);

            this->setText(""); //setTitle("");
            clear();
        } else { // henkilˆtiedot on annettu aiemmin ja niit‰ voidaan muuttaa
            birth = this->birthString(id);
            death = this->deathString(id);

            if (uiRelation) uiRelation->setText(INDI(id).getShortFamilyRelation());
            uiName->setText(INDI(id).nameFamily + " " + INDI(id).nameFirst );

            if (uiBirthDay) uiBirthDay->setText(birth);
            if (uiDeathDay) uiDeathDay->setText(death);
//qDebug() << 1;
            if (uiBirthPlace && uiDeathPlace) {
                uiBirthPlace->setText(place(INDI(id).ievent(BIRTH).place));
                uiDeathPlace->setText(place(INDI(id).ievent(DEATH).place));
            }

            setText(""); // open
            setDisabled(false);
            if (uiCenter) uiCenter->setDisabled(false);
            if ( INDI(id).sex == MALE) {
                if (INDI(id).selected) setStyleSheet(MALESTYLE_SELECTED);
                else setStyleSheet(MALESTYLE);
            }
            if ( INDI(id).sex == FEMALE) {
                if (INDI(id).selected) setStyleSheet(FEMALESTYLE_SELECTED);
                else setStyleSheet(FEMALESTYLE);
            }
            if ( INDI(id).sex == UNKNOWN) {
                if (INDI(id).selected) setStyleSheet(UNKNOWNSTYLE_SELECTED);
                else setStyleSheet(UNKNOWNSTYLE);
            }
        }
    }
}
