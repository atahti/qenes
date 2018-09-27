#include "descendants.h"
#include "ui_descendants.h"
#include "data/genedata.h"

extern  GeneData        * geneData;

Descendants::Descendants(quint16 *pId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Descendants)
{

    ui->setupUi(this);
    QPushButton *btn, *btn2, *btn3, *btn4;

    btn = new QPushButton("test");
    btn2 = new QPushButton("test2");
    btn3 = new QPushButton("test3");
    btn4 = new QPushButton("test4");


    this->ui->uiGrid->addWidget(btn, 0, 0, 1, 1, 0);
    this->ui->uiGrid->addWidget(btn2, 0, 1, 1, 1, 0);
    this->ui->uiGrid->addWidget(btn3, 1, 0, 1, 1, 0);
    this->ui->uiGrid->addWidget(btn4, 1, 1, 1, 1, 0);

}

Descendants::~Descendants()
{
    delete ui;
}
