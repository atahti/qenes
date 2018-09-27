#ifndef PANEL_H
#define PANEL_H

#include <QComboBox>
#include "ui/box/panelcommonall.h"
#include <QPushButton>
#include <QObject>
#include <QTimer>

class Panel : public PanelCommonAll
{
    Q_OBJECT

private:
    //void        mousePressEvent(QMouseEvent *event);
    //void        mouseDoubleClickEvent(QMouseEvent *);
    //void        mouseReleaseEvent(QMouseEvent *event);

    QTimer *timer; ///
    bool        ignoreNextRelease;   ///

public:
    Panel();


public slots:
    void        showPerson();
    void        center();
    void        timerTimeOut();
    void        slotPersonAssigned(quint16);
    //void        emitClicked();///
    void        panelPressed();
    void        panelReleased();

signals:
    void        sigUpdate(quint16);
    void        sigStatusBarMessage(QString);
    void        doubleClicked();
};

#endif // PANEL_H
