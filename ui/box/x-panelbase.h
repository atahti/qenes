#ifndef LABELBASE_H
#define LABELBASE_H

#include "macro.h"
#include "ui/box/panelcommonall.h"
#include <QPushButton>
#include <QLabel>
#include <QGraphicsView>

class PanelBase : public QPushButton, public PanelCommonAll
{
    Q_OBJECT

public:
    PanelBase();

    int         id, child;
    BOXKIND     boxKind;
    void updt(int id, int child);
    QString     place(QString input);
    void        clear();
    QPushButton * uiCenter;
    QLabel      * uiRelation;
    QLabel      * uiName;
    QLabel      * uiBirthDay;
    QLabel      * uiDeathDay;
    QLabel      * uiBirthPlace;
    QLabel      * uiDeathPlace;
    QGraphicsView*      uiPhoto;
    void        setStyle(QString style);
    QPoint      rightPos(QPoint global);
    QPoint      leftPos(QPoint global);

protected slots:
    void        showPerson();
    void        center();
    void        slotPersonAssigned(int);

signals:
    void        sigUpdate(int);
    void        sigStatusBarMessage(QString);
    //void        sigPersonAssiged(int);

};

#endif // LABELBASE_H
