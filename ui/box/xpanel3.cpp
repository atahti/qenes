#include "Panel3.h"

Panel3::Panel3()
{
    setFixedSize(160, 20);

    uiName  = new QLabel(this);
    uiName->show();
    uiName->setAttribute(Qt::WA_TransparentForMouseEvents);
    uiName->setGeometry(0, 0, 100, 20);
    uiName->setStyleSheet( "background: transparent;" );

    uiRelation      = NULL;
    uiBirthDay      = NULL;
    uiDeathDay      = NULL;
    uiBirthPlace    = NULL;
    uiDeathPlace    = NULL;
    uiPhoto         = NULL;

    uiCenter = new QPushButton(this);
    uiCenter->setGeometry(140, 0, 20, 20);
    uiCenter->show();

    connect(uiCenter, SIGNAL(pressed()), this, SLOT(center()));
    connect(this, SIGNAL(pressed()), this, SLOT(showPerson()));
}
