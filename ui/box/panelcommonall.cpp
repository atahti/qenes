#include "mainwindow.h"
#include "panelcommonall.h"
#include "data/genedata.h"
#include "macro.h"

extern  GeneData        * geneData;


void PanelCommonAll::resizeEvent(QResizeEvent* event)
{

    Q_UNUSED( event );
    setPanelFontAutomatically();
}

void PanelCommonAll::setPanelFont(quint8 height)
{
    font.setPointSize(height);
    fontBold.setPointSize(height+2);
    uiName->setFont(fontBold);
    uiDay1->setFont(font);
    uiDay2->setFont(font);
    uiDay3->setFont(font);
    uiPlace1->setFont(font);
    uiPlace2->setFont(font);
    uiPlace3->setFont(font);
    uiRelation->setFont(font);
    uiTitle1->setFont(font);
    uiTitle2->setFont(font);
    uiValue1->setFont(font);
    uiValue2->setFont(font);
}

void PanelCommonAll::setPanelFontAutomatically()
{
    // 5 100
    // 3 50
    // 1 25
    //qDebug() << height() << this->heightId;
    if (this->height()>170) setPanelFont(11);
    else  if (this->height()>50) setPanelFont(7);
    else if (this->height()>10) setPanelFont(5);
    else setPanelFont(5);
}

PanelCommonAll::PanelCommonAll()
{
    id = 0;

    uiName          = new QLabel(this);
    uiDay1          = new QLabel(this);
    uiPlace1        = new QLabel(this);
    uiDay2          = new QLabel(this);
    uiPlace2        = new QLabel(this);
    uiDay3          = new QLabel(this);
    uiPlace3        = new QLabel(this);
    //uiCenter        = new QPushButton(this);
    uiRelation      = new QLabel(this);
    uiTitle1        = new QLabel(this);
    uiTitle2        = new QLabel(this);
    uiValue1        = new QLabel(this);
    uiValue2        = new QLabel(this);
    uiSpouseList    = new QComboBox(this);  // voi olla turhaan täällä
    uiAddSpouse     = new QPushButton(this);
    uiPhoto         = nullptr;

    font = QFont("Arial", 8);
    fontBold = QFont("Arial", 10, QFont::Bold);

    setPanelFontAutomatically();
   // setPanelFont(8);

    grid = new QGridLayout(this);
    grid->setContentsMargins(1, 1, 1, 1);
    grid->setHorizontalSpacing(1);
    grid->setVerticalSpacing(1);
    setLayout(grid);

}

void PanelCommonAll::setup(int hght, bool cb, bool ss)
{
    heightId = hght;
    centerButton = cb;
    spouseSelector = ss;

    uiSpouseList->setVisible(spouseSelector);
    uiAddSpouse->setVisible(spouseSelector);

    if (spouseSelector) {
        grid->addWidget(uiSpouseList, 0, 0, 1, 9, Qt::AlignLeft);
        uiSpouseList->setFixedHeight(30);
        uiSpouseList->show();
        uiSpouseList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        grid->addWidget(uiAddSpouse, 0, 10, 1, 1, Qt::AlignLeft);
        uiAddSpouse->show();
        uiAddSpouse->setStyleSheet("QPushButton { background-color: rgb(150, 150, 150); }" );
        uiAddSpouse->setFixedHeight(20);
        uiAddSpouse->setFixedWidth(20);
        grid->addWidget(uiDay1, 1, 0, 1, 5, Qt::AlignLeft);
        grid->addWidget(uiPlace1, 1, 6, 1, 5, Qt::AlignLeft);

        uiName->hide();
        uiRelation->hide();
        uiDay2->hide();
        uiDay3->hide();
        uiPlace2->hide();
        uiPlace3->hide();
        uiTitle1->hide();
        uiTitle2->hide();
        uiValue1->hide();
        uiValue2->hide();

        uiAddSpouse->setIcon(QIcon(":/icon/ui/benefits.png"));
        return;
    }

    switch (heightId) {
        case 1 : this->makeHeight1(); break;
        case 2 : this->makeHeight2(); break;
        case 3 : this->makeHeight3(); break;
        case 4 : this->makeHeight4(); break;
        case 5 : this->makeHeight5(); break;
    }

    if (id) updt();
}

void PanelCommonAll::makeHeight5()
{    
    //setFixedSize(160, 105);
    setMinimumSize(160, 105);

    uiRelation->show();
    uiRelation->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiRelation->setStyleSheet( "QLabel { background: transparent; } ");
    grid->addWidget(uiRelation, 0, 0, 1, 2, Qt::AlignLeft);

    uiName->show();
    uiName->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiName->setStyleSheet( "QLabel { background: transparent; } " );
    grid->addWidget(uiName, 1, 0, 1, 2, Qt::AlignLeft);

    uiDay1->show();
    uiDay1->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiDay1->setStyleSheet( " QLabel { background: transparent; } " );
    grid->addWidget(uiDay1, 2, 0, 1, 1, Qt::AlignLeft);

    uiPlace1->show();
    uiPlace1->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiPlace1->setStyleSheet( "QLabel { background: transparent; } " );
    grid->addWidget(uiPlace1, 2, 1, 1, 1, Qt::AlignLeft);

    uiDay2->show();
    uiDay2->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiDay2->setStyleSheet( "QLabel { background: transparent; } " );
    grid->addWidget(uiDay2, 3, 0, 1, 1, Qt::AlignLeft);

    uiPlace2->show();
    uiPlace2->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiPlace2->setStyleSheet( "QLabel { background: transparent; } " );
    grid->addWidget(uiPlace2, 3, 1, 1, 1, Qt::AlignLeft);

    uiTitle1->show();
    uiTitle1->setText("");
    uiTitle1->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiTitle1->setStyleSheet( "QLabel { background: transparent; } " );
    grid->addWidget(uiTitle1, 4, 0, 1, 2, Qt::AlignLeft);

    uiTitle2->show();
    uiTitle2->setText("");
    uiTitle2->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiTitle2->setStyleSheet( "QLabel { background: transparent; } " );
    grid->addWidget(uiTitle2, 5, 0, 1, 2, Qt::AlignLeft);
}

void PanelCommonAll::makeHeight4()
{
    //setFixedSize(160, 60);
    setMinimumSize(160, 60);

    uiRelation->show();
    uiRelation->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiRelation->setStyleSheet( "QLabel { background: transparent; } ");
    grid->addWidget(uiRelation, 0, 0, 1, 2, Qt::AlignLeft);

    uiName->show();
    uiName->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiName->setStyleSheet( "QLabel { background: transparent; } " );
    grid->addWidget(uiName, 1, 0, 1, 2, Qt::AlignLeft);

    uiDay1->show();
    uiDay1->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiDay1->setStyleSheet( " QLabel { background: transparent; } " );
    grid->addWidget(uiDay1, 2, 0, 1, 1, Qt::AlignLeft);

    uiPlace1->show();
    uiPlace1->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiPlace1->setStyleSheet( "QLabel { background: transparent; } " );
    grid->addWidget(uiPlace1, 2, 1, 1, 1, Qt::AlignLeft);

    uiDay2->show();
    uiDay2->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiDay2->setStyleSheet( "QLabel { background: transparent; } " );
    grid->addWidget(uiDay2, 3, 0, 1, 1, Qt::AlignLeft);

    uiPlace2->show();
    uiPlace2->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiPlace2->setStyleSheet( "QLabel { background: transparent; } " );
    grid->addWidget(uiPlace2, 3, 1, 1, 1, Qt::AlignLeft);

    uiTitle1->hide();
    uiTitle2->hide();
    uiValue1->hide();
    uiValue2->hide();
//qDebug() << "make4";
    //setPanelFontAutomatically();
}

void PanelCommonAll::makeHeight3()
{
    setMinimumSize(160, 50);

    uiRelation->show();
    uiRelation->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiRelation->setStyleSheet( "background: transparent;" );
    grid->addWidget(uiRelation, 0, 0, 1, 2, Qt::AlignLeft);

    uiName->show();
    uiName->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiName->setStyleSheet( "background: transparent;" );
    grid->addWidget(uiName, 1, 0, 1, 2, Qt::AlignLeft);

    uiDay1->show();
    uiDay1->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiDay1->setStyleSheet( "background: transparent;" );
    grid->addWidget(uiDay1, 2, 0, 1, 1, Qt::AlignLeft);

    uiDay2->show();
    uiDay2->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiDay2->setStyleSheet( "background: transparent;" );
    grid->addWidget(uiDay2, 2, 1, 1, 1, Qt::AlignLeft);

    uiPlace1->setVisible(false);
    uiPlace2->setVisible(false);

    uiTitle1->hide();
    uiTitle2->hide();
    uiValue1->hide();
    uiValue2->hide();
}

void PanelCommonAll::makeHeight2()
{
    //this->setMinimumSize(160, 35);

    uiRelation->show();

    uiName->show();
    uiName->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiName->setStyleSheet( "background: transparent;" );
    grid->addWidget(uiName, 0, 0, 1, 2, Qt::AlignLeft);

    uiDay1->show();
    uiDay1->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiDay1->setStyleSheet( "background: transparent;" );
    grid->addWidget(uiDay1, 1, 0, 1, 1, Qt::AlignLeft);

    uiDay2->show();
    uiDay2->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiDay2->setStyleSheet( "background: transparent;" );
    grid->addWidget(uiDay2, 1, 1, 1, 1, Qt::AlignLeft);

    uiPlace1->hide();
    uiPlace2->hide();
    uiPlace3->hide();
    uiDay3->hide();

    uiTitle1->hide();
    uiTitle2->hide();
    uiValue1->hide();
    uiValue2->hide();
}

int PanelCommonAll::suggestHeight(int spaceAvailable)
{
    if (spaceAvailable < 20) return 0;
    if (spaceAvailable < 35) return 1;
    if (spaceAvailable < 50) return 2;
    if (spaceAvailable < 60) return 3;
    if (spaceAvailable < 90) return 4;
    return 5;
}

void PanelCommonAll::makeHeight1()
{
    //setFixedSize(160, 20);
    setMinimumSize(160,20);
    //this->setSizePolicy(5);

    uiName->show();
    uiName->setAttribute(Qt::WA_TransparentForMouseEvents);
    grid->addWidget(uiName, 0, 0, 1, 1, Qt::AlignLeft);
    uiName->setStyleSheet( "background: transparent;" );

    uiDay1->hide();
    uiDay2->hide();
    uiDay3->hide();
    uiPlace1->hide();
    uiPlace2->hide();
    uiPlace3->hide();
    //uiCenter->hide();
    uiRelation->hide();
    uiTitle1->hide();
    uiTitle2->hide();
    uiValue1->hide();
    uiValue2->hide();

}

void PanelCommonAll::setStyle(QString style)
{
    setStyleSheet(style);
    //if (centerButton) uiCenter->setStyleSheet(UNKNOWNSTYLE);
}

void PanelCommonAll::clear()
{    
    uiName->setText("");
    uiRelation->setText("");

    if (uiPhoto) uiPhoto->setVisible(false);
    if (uiPlace1) uiPlace1->setText("");
    if (uiPlace2) uiPlace2->setText("");
    if (uiDay2) uiDay2->setText("");
    if (uiDay1) uiDay1->setText("");
    if (uiTitle1) uiTitle1->setText("");
    if (uiTitle2) uiTitle2->setText("");

    if ( panelType == PANEL_MALE || panelType == PANEL_FEMALE || panelType == PANEL_SPOUSE ) {
        if (pChildId) {
            if (*pChildId && this->panelType == PANEL_MALE) setStyle(MALESTYLE);
            if (*pChildId && this->panelType == PANEL_FEMALE) setStyle(FEMALESTYLE);
            if (!(*pChildId) ) setStyle(UNKNOWNSTYLE);
        }
    }
    if ( panelType == PANEL_SPOUSE ) setStyle(UNKNOWNSTYLE);
    if ( panelType == PANEL_CURRENT) setStyle(YELLOWSTYLE);
}

QString PanelCommonAll::place(QString input)
{
    QRegExp nameExp("(.[^\\/]*)?/?([^\\/].[^\\/]*)?/?([^\\/].[^\\/]*)?");
    nameExp.indexIn(input);
    return nameExp.cap(1);
}

void PanelCommonAll::updt(quint16 c)
{
    this->id = c;
    updt();
}

void PanelCommonAll::updt()
{
    QString birth, death;

    setIcon(QIcon());

    for (int i = 0 ; i<GENE.problems.size() ; i++ )
        if ( GENE.problems.at(i).id == id ) {
            //qDebug() << "vika " << i << " / " << id;
            setIcon(QIcon(":/icon/ui/error.jpg"));
        }

    if ( (id == 0) && ( *pChildId == 0 ) ) {
        // the person is not known for this box.
        // Persondata cannot be added to this box.
        //if (uiPhoto) uiPhoto->setVisible(false);
        clear();
        setDisabled(true);

        //if (uiCenter) uiCenter->setDisabled(true);
        //setStyle(UNKNOWNSTYLE);

    } else {        
        if ( pChildId ) if ( ( *pChildId && id==0) ) {
            // Tässä boxissa olevaa henkilöä ei ole syötetty aiemmin, mutta boxilla on sukulainen
            // (eli puoliso tai lapsi) joten tähän boxiin voidaan syöttää henkilön tiedot.
            //if (uiPhoto) uiPhoto->setVisible(false);

            setIcon(QIcon(":/icon/ui/benefits.png"));

            setDisabled(false);

            //if (uiCenter) uiCenter->setDisabled(false);

            if ( panelType == PANEL_FEMALE ) setStyle(FEMALESTYLE);
            else if ( panelType == PANEL_MALE ) setStyle(MALESTYLE);
            else setStyle(YELLOWSTYLE);

            clear();
        }
          if ( id ) {
        // henkilötiedot on annettu aiemmin ja niitä voidaan muuttaa
            birth = birthString(id);
            death = deathString(id);

            if (uiRelation->isVisible()) uiRelation->setText(INDI(id).getShortFamilyRelation());
            uiName->setText(INDI(id).nameFamily + " " + INDI(id).nameFirst );

            if (uiDay1) uiDay1->setText(birth);
            if (uiDay2) uiDay2->setText(death);

            if (uiPlace1 && uiPlace2) {
                uiPlace1->setText(place(INDI(id).ientry(BIRTH).place));
                uiPlace2->setText(place(INDI(id).ientry(DEATH).place));
            }

            if (INDI(id).ientry(EDUCATION).attrText != "" ) this->uiTitle2->setText(INDI(id).ientry(EDUCATION).attrText); else this->uiTitle2->setText("");
            if (INDI(id).ientry(OCCUPATION).attrText != "" ) this->uiTitle1->setText(INDI(id).ientry(OCCUPATION).attrText); else this->uiTitle1->setText("");

            setText(""); // open
            setDisabled(false);

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

void PanelCommonAll::updtAll(quint16 c)
{
    updt(c);
    if (this->pFather ) this->pFather->updtAll(INDI(id).getFather());
    if (this->pMother ) this->pMother->updtAll(INDI(id).getMother());
}

QString PanelCommonAll::birthString(quint16 id)
{
    QString birth;
    if (INDI(id).ientry(BIRTH).day.getAcc() == NA) {
        if (INDI(id).estimatedBirth.known) birth = "*(" + QString::number(INDI(id).estimatedBirth.day1.year())
            + "-" + QString::number(INDI(id).estimatedBirth.day2.year())+")";
        else birth = "(*) ?";
    }

    if (INDI(id).ientry(BIRTH).day.known) birth = "*" + INDI(id).getEntryDaybyType(true, BIRTH).toString(true);

    return birth;
}

QString PanelCommonAll::deathString(quint16 id)
{
    /* a: (+) 12.34.1234 paikka
       b: (+ < 1234 ) paikka
       c: (+ < 1234 )
       d: (+) alive
       */

    QString death;

    // Mitään tietoa kuolemasta ei ole annettu. Voi olla joko (c) tai (d)
    if (!INDI(id).ientry(DEATH).used() ) {
        Date estDeath;
        INDI(id).eventPeriod(&estDeath, 4, true);
        //QDate estDeath = INDI(id).eventMaxDay(4, true);
        //qDebug() << id << estDeath;
        if (estDeath.day2 < QDate::currentDate() ) death = "(+ < " + estDeath.day2.toString("yyyy") + ")";
        else death = "age : " + INDI(id).getAge();
    }
    else
    // jotain tietoa (päivä tai paikka) on annettu, hlö on siis kuollut varmasti
    {
        Date deathDay = INDI(id).getEntryDaybyType(true, DEATH);

        if (deathDay.known) death = "+" + INDI(id).getEntryDaybyType(true, DEATH).toString(true);
        else {
            QDate estDeath = MIN(INDI(id).getEstimatedDeath(), QDate::currentDate());
            death = "(+ < " + estDeath.toString("yyyy") + " ) ";
        }
    }

    return death;
}
