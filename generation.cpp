#include "generation.h"
#include "data/genedata.h"
#include "macro.h"

extern  GeneData        * geneData;

void Generation::setup()
{    
    int generationOfCurrent = INDI(GENE.currentId).generation;
    int generationOfThis = generationOfCurrent + generationOnUi -1;
    uiGeneration->setText(QString::number(generationOfThis));

    uiCount->setText(QString::number(GENE.generationData.value(generationOfThis).count));
    uiFrom->setText(GENE.generationData.value(generationOfThis).min.toString("yyyy"));
}

Generation::Generation(QFrame *parent) : QFrame(parent)
{
    setupUi(this);

    p = this->palette();
    this->setAutoFillBackground(true);
    p.setColor(QPalette::Window, Qt::lightGray);
    this->setPalette(p);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
