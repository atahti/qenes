#include "descendants.h"
#include "ui_descendants.h"
#include "data/genedata.h"
#include "ui/descendants.h"

extern  GeneData        * geneData;

int Descendants::descendantTableBuilder(QList<DescentantButton> *buttons, quint8 *row, quint8 col, quint16 thisId)
{
    quint8 childCount = 0;
    QList<quint16> childList;

    INDI(thisId).getChilds(&childList, false);

    DescentantButton newButton;
    qDebug() << "new but " << INDI(thisId).nameFirst << row;
    newButton.id = thisId;
    newButton.row = *row;
    newButton.col = col;

    for (quint8 c=0 ; c<childList.size() ; c++) {
        if (c>0) (*row)++;
        childCount += descendantTableBuilder(buttons, row, col+1, childList.at(c));
    }

    if (childCount == 0) childCount = 1;
//qDebug() << "q" << thisId << childCount;
    newButton.height = childCount; //    rivit->append(childCount);
    buttons->append(newButton);

    return childCount;
}

Descendants::Descendants(quint16 *pId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Descendants)
{

    ui->setupUi(this);

    quint16 i=GENE.currentId;

    QString output = "";
    quint8 row=1;
    quint8 col=1;

    QList<DescentantButton> buttons;

    descendantTableBuilder(&buttons, &row, col, i);

    for (int x=0 ; x<buttons.size() ; x++) {
        qDebug() << INDI(buttons.at(x).id).nameFirst << buttons.at(x).col << buttons.at(x).row << buttons.at(x).height;
    QPushButton *button = new QPushButton(INDI(buttons.at(x).id).nameFirst);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->ui->uiGrid->addWidget(button, buttons.at(x).row, buttons.at(x).col, buttons.at(x).height, 1, 0);
    }

}

Descendants::~Descendants()
{
    delete ui;
}
