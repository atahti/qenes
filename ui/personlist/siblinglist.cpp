#include "siblinglist.h"
#include "data/persondata.h"
#include "data/familydata.h"
#include "data/genedata.h"
#include "macro.h"

extern  GeneData        * geneData;

SiblingList::SiblingList(QList<quint16> *pL, QWidget *parent)
{
    setup(pL, tr("Siblings"), true, tr("No stepchilds"), parent);
    connect(this->uiAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
}

void SiblingList::slotAdd()
{
    quint16 parent = 0;
    if (INDI(GENE.currentId).getFather() != 0) parent = INDI(GENE.currentId).getFather();
    else if (INDI(GENE.currentId).getMother() != 0) parent = INDI(GENE.currentId).getMother();

    addChild(parent);
}
