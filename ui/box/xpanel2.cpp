#include "panel2.h"

Panel2::Panel2()
{
    setFixedSize(160, 50);

    uiRelation = new QLabel(this);
    uiRelation->show();
    uiRelation->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiRelation->setGeometry(4, 2, 46, 13);
    uiRelation->setStyleSheet( "background: transparent;" );

    uiName  = new QLabel(this);
    uiName->show();
    uiName->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiName->setGeometry(4, 18, 260, 13);
    uiName->setStyleSheet( "background: transparent;" );

    uiBirthDay = new QLabel(this);
    uiBirthDay->show();
    uiBirthDay->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiBirthDay->setGeometry(4, 34, 73, 13);
    uiBirthDay->setStyleSheet( "background: transparent;" );

    uiDeathDay = new QLabel(this);
    uiDeathDay->show();
    uiDeathDay->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiDeathDay->setGeometry(82, 34, 73, 13);
    uiDeathDay->setStyleSheet( "background: transparent;" );

    uiBirthPlace    = NULL;
    uiDeathPlace    = NULL;
    uiPhoto         = NULL;

    uiCenter = new QPushButton(this);
    uiCenter->setGeometry(140, 0, 20, 20);
    uiCenter->show();

    connect(uiCenter, SIGNAL(pressed()), this, SLOT(center()));
    connect(this, SIGNAL(pressed()), this, SLOT(showPerson()));
}
