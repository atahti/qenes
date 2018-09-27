#ifndef LINEWIDGET_H
#define LINEWIDGET_H

#include <QWidget>
#include "ui/box/panel.h"
#include "ui/box/current.h"

class LineWidget : public QWidget
{
public:
    LineWidget(Panel *panelC, Panel *panelF, Panel *panelM);
    LineWidget(Current *p1, Panel *p2, Panel *p3);
    void    koe();
private:
    void    paintEvent(QPaintEvent *);
    Panel   * panelF;
    Panel   * panelM;
    Panel   * panelC;
    Current * current;
};

#endif // LINEWIDGET_H
