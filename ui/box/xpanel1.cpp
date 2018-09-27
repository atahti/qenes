#include "panel1.h"


Panel1::Panel1()
{
    setFixedSize(160, 60);

    uiRelation = new QLabel(this);
    uiRelation->show();
    uiRelation->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiRelation->setGeometry(4, 2, 46, 13);    
    uiRelation->setStyleSheet( "QLabel { background: transparent; } ");

    uiName  = new QLabel(this);
    uiName->show();
    uiName->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiName->setGeometry(4, 16, 130, 13);
    uiName->setStyleSheet( "QLabel { background: transparent; } " );

    uiBirthDay = new QLabel(this);
    uiBirthDay->show();
    uiBirthDay->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiBirthDay->setGeometry(4, 30, 73, 13);
    uiBirthDay->setStyleSheet( " QLabel { background: transparent; } " );

    uiBirthPlace = new QLabel(this);
    uiBirthPlace->show();
    uiBirthPlace->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiBirthPlace->setGeometry(78, 30, 75, 13);
    uiBirthPlace->setStyleSheet( "QLabel { background: transparent; } " );

    uiDeathDay = new QLabel(this);
    uiDeathDay->show();
    uiDeathDay->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiDeathDay->setGeometry(4, 45, 73, 13);
    uiDeathDay->setStyleSheet( "QLabel { background: transparent; } " );

    uiDeathPlace = new QLabel(this);
    uiDeathPlace->show();
    uiDeathPlace->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiDeathPlace->setGeometry(82, 45, 75, 13);
    uiDeathPlace->setStyleSheet( "QLabel { background: transparent; } " );

    uiPhoto         = NULL;

    uiCenter = new QPushButton(this);
    uiCenter->setGeometry(140, 0, 20, 20);
    uiCenter->show();

    connect(uiCenter, SIGNAL(pressed()), this, SLOT(center()));
    connect(this, SIGNAL(pressed()), this, SLOT(showPerson()));
}
