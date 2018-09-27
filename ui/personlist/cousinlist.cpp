#include "cousinlist.h"

CousinList::CousinList(QList<quint16> *pL, QWidget *parent)
{
    this->setup(pL, tr("Cousins"), false, tr("No stepcousins"), parent);
}
