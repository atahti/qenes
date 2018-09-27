#ifndef DIALOG_S_H
#define DIALOG_S_H

#include <QMainWindow>
#include <QDialog>
//#include "ui/box/panelcommonall.h"
#include "ui/box/panelcommonall.h"
#include <QStringListModel>
#include <QComboBox>
#include <QGraphicsView>
#include <QMessageBox>
//#include <QGroupBox>
#include "ui_current.h"

class Current : public QFrame, public Ui::current
{
    Q_OBJECT

    private:
        Ui::current         ui;
        void                picture(QString currentFile);
        void                picture();
        QGraphicsScene      pictureCurrentScene;
        QPalette            p;        
        //void                resizeEvent(QResizeEvent *event);
        int                 oldHeight;

    public:
        Current(QMainWindow *parent = 0);
        void                update(int id);
        QString             place(QString input);
        QStringList         spouseNameList;
        QList<quint16>      spouseIdList;
        QStringListModel    spouseModel;
        QPoint              rightPos(QPoint global);
        PanelCommonAll      * current;
        PanelCommonAll      * spouse;

    protected slots:

        void                slotSpouseClicked(int);
        void                slotAddSpouse();
        void                slotShowSpouse();
        void                slotSpouseAssigned(quint16 newSpouse);

    public slots:
        void                slotShowPerson();

signals:
        void                sigUpdate();
        void                sigUpdate(quint16);
};


#endif // DIALOG_S_H
