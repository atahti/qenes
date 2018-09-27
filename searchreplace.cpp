#include "searchreplace.h"
#include "ui_searchreplace.h"

SearchReplace::SearchReplace(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchReplace)
{
    ui->setupUi(this);
}

SearchReplace::~SearchReplace()
{
    delete ui;
}
