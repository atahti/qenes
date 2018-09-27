#ifndef DIALOG_S_H
#define DIALOG_S_H

#include <QMainWindow>
#include <QtGui/QDialog>
#include "ui/box/panelcommonall.h"
#include <QStringListModel>
#include <QComboBox>
#include <QGraphicsView>
#include <QMessageBox>
#include <QGroupBox>
#include "ui_current.h"

class Current : public QGroupBox, public Ui::current, public PanelCommonAll
{
    Q_OBJECT

    private:
        void                clear();
        Ui::current         ui;
        void                picture(QString currentFile);
        void                picture();
        QGraphicsScene      pictureCurrentScene;
        QPalette            p;

    public:
        Current(QMainWindow *parent = 0);
        void                update(int id);
        QString             place(QString input);
        QStringList         spouseNameList;
        QList<int>          spouseIdList;
        QStringListModel    spouseModel;
        QPoint              rightPos(QPoint global);

    protected slots:
        void                slotShowPerson();
        void                slotSpouseClicked(int);
        void                slotAddSpouse();
        void                slotShowSpouse();
        void                slotSpouseAssigned(int newSpouse);

signals:
        void                sigUpdate();
        void                sigUpdate(int);
};


#endif // DIALOG_S_H
