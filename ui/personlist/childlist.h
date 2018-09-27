#ifndef CHILDLIST_H
#define CHILDLIST_H

#include "personlist.h"

class ChildList : public PersonList
{
    Q_OBJECT

public:
    ChildList(QList<quint16> *pL, QWidget *parent = 0);

private slots:
    void    slotAdd();

};

#endif // CHILDLIST_H
