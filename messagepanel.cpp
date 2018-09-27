#include "messagepanel.h"
#include "ui_messagepanel.h"
#include "data/genedata.h"

MessagePanel::MessagePanel(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MessagePanel)
{    
    this->setFont(GeneData::panelFont);
    ui->setupUi(this);
}

MessagePanel::~MessagePanel()
{
    delete ui;
}

void MessagePanel::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MessagePanel::addMessage(QString text)
{
    ui->uiMessage->appendPlainText(text);

}
