#ifndef SIBLINGLIST_H
#define SIBLINGLIST_H

#include "personlist.h"

class SiblingList : public PersonList
{
    Q_OBJECT


public:
    SiblingList(QList<quint16> *pL, QWidget *parent = 0);

private slots:
    void slotAdd();
};

#endif // SIBLINGLIST_H
