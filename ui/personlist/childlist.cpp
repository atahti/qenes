#include "childlist.h"
#include "data/genedata.h"
#include "macro.h"

extern  GeneData        * geneData;

ChildList::ChildList(QList<quint16> *pL, QWidget *parent)
{

    setup(pL, tr("Childs"), true, tr("No stepchilds"), parent);
    connect(uiAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
}

void ChildList::slotAdd()
{
    this->addChild(GENE.currentId);
}
