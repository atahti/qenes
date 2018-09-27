#ifndef PANELCOMMONALL_H
#define PANELCOMMONALL_H

#include <QString>
#include <QPoint>
#include <QObject>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QGraphicsView>
#include <QGridLayout>
#include "macro.h"

class Panel;

class PanelCommonAll : public QPushButton
{
public:
  //  void        mouseDoubleClickEvent();
    PanelCommonAll();    
    int         heightId;
    static int  suggestHeight(int spaceAvailable);
    void        setup(int height, bool centerButton, bool spouseSelector);
    quint16     id;
    PANELTYPE   panelType;
    void        updt();
    void        updt(quint16 id);
    void        updtAll(quint16 c);
    QString     place(QString input);
    void        clear();
    //QPushButton * uiCenter;
    QLabel      * uiRelation;
    QLabel      * uiName;
    Panel       * pMother, * pFather;
    QLabel      * uiDay1;
    QLabel      * uiDay2;
    QLabel      * uiDay3;
    QLabel      * uiPlace1;
    QLabel      * uiPlace2;
    QLabel      * uiPlace3;
    QLabel      * uiTitle1;
    QLabel      * uiValue1;
    QLabel      * uiTitle2;
    QLabel      * uiValue2;

    QComboBox   * uiSpouseList;
    QPushButton * uiAddSpouse;
    QGraphicsView * uiPhoto;
    void        setStyle(QString style);

    quint16     idWhenOpened;
    quint16     * pChildId;
    static      QString birthString(quint16 id);
    static      QString deathString(quint16 id);

private:
    void        makeHeight1();
    void        makeHeight2();
    void        makeHeight3();
    void        makeHeight4();
    void        makeHeight5();
    bool        centerButton;
    bool        spouseSelector;
    void        resizeEvent(QResizeEvent *event);
    QFont       font, fontBold;
    void        setPanelFont(quint8 height);
    void        setPanelFontAutomatically();
    QGridLayout * grid;


};

#endif // PANELCOMMONALL_H
