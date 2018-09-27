#include "linewidget.h"
#include <QPainter>
#include <QDebug>

#include "macro.h"
#include "data/genedata.h"
#include "data/persondata.h"

extern  GeneData        * geneData;

LineWidget::LineWidget(Current *c1, Panel *p2, Panel *p3)
{
    current = c1;
    panelC = NULL;
    panelF = p2;
    panelM = p3;
    this->setFixedWidth(20);
}

LineWidget::LineWidget(Panel *p1, Panel *p2, Panel *p3)
{
    current = NULL;
    panelC = p1;
    panelF = p2;
    panelM = p3;        
    this->setFixedWidth(20);
}

void LineWidget::paintEvent(QPaintEvent *)
{
    if (!panelC && !current) return;
    if (panelM && !panelM->isVisible()) return;

    QPen penF, penM, penC;
    penF.setWidth(4);
    penM.setWidth(4);
    penC.setWidth(4);

    /*
    penF.setColor(Qt::darkGray);
    penM.setColor(Qt::darkGray);
    penC.setColor(Qt::darkGray);*/

    penF.setColor(Qt::lightGray);
    penM.setColor(Qt::lightGray);
    penC.setColor(Qt::lightGray);

    int yF, yC, yM;
    yF = 0;
    yC = 0;
    yM = 0;
    QPoint mapToCurrent = mapToParent(QPoint(0, 0));
    int halfWidth = this->width()/2;
    int width = this->width();

    yF = this->panelF->y() - mapToCurrent.y() + this->panelF->height()/2;
    yM = this->panelM->y() - mapToCurrent.y() + this->panelM->height()/2;

    ADOPTED_BY adopted = NO_ADOP;

    if (panelC) {
        adopted = INDI(panelC->id).adoptedBy;
        yC = this->panelC->y() - mapToCurrent.y() + this->panelC->height()/2;
    }

    if (current) {
        adopted = INDI(GENE.currentId).adoptedBy;
        yC = this->current->y() - mapToCurrent.y() + this->current->height()/2;
    }

    if (
        (adopted == HUSB_ADOP) ||
        (adopted == BOTH_ADOP) ||
        (adopted == YES_ADOP) ){
        penF.setColor(Qt::red);
        penF.setStyle(Qt::DashLine);
    }

    if (
        (adopted == WIFE_ADOP) ||
        (adopted == BOTH_ADOP) ||
        (adopted == YES_ADOP) ) {
        penM.setColor(Qt::red);
        penM.setStyle(Qt::DashLine);
    }

    if (panelM) if ( !(panelM->id) ) penM.setStyle(Qt::DotLine);
    if (panelF) if ( !(panelF->id) ) penF.setStyle(Qt::DotLine);
    if (panelF && panelM) if ( !(panelF->id) && !(panelM->id) ) penC.setStyle(Qt::DotLine);



    // alla penF:n tilalla voisi olla myös penM tai penC
    if (yC<0) yC = penF.width();
    if (yC > this->height()) yC = this->height() - penF.width();

    QPainter painter(this);
    painter.setPen(penC);
    painter.drawLine(0, yC, halfWidth, yC);

    painter.setPen(penF);
    painter.drawLine(halfWidth, yF, width, yF);
    painter.drawLine(halfWidth, MIN(yF, yC), halfWidth, yC);

    painter.setPen(penM);
    painter.drawLine(halfWidth, yM, width, yM);
    painter.drawLine(halfWidth, yC, halfWidth, MAX(yM, yC));

    //painter.drawLine(halfWidth, MIN(yF, yC), halfWidth, MAX(yM, yC));
}

void LineWidget::koe()
{
    qDebug() << "koe";
}
