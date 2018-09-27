#ifndef COUSINLIST_H
#define COUSINLIST_H

#include "personlist.h"

class CousinList : public PersonList
{

public:
    CousinList(QList<quint16> *pL, QWidget *parent = 0);
};

#endif // COUSINLIST_H
