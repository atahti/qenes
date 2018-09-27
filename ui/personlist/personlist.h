#ifndef PERSONLIST_H
#define PERSONLIST_H

#include <QWidget>
#include <QThread>

#include "ui_personlist.h"

class PersonListModel;

class PersonList : public QFrame, public Ui::PersonList
{
    Q_OBJECT

public:
    void enableAddButton(bool enabled);
    PersonListModel* personModel;
    void    update();
    QList<quint16> *personList;
    bool    noStepRelativesChecked;
    void    trans(QString title, QString checkText);
    void    setup(QList<quint16> *pL, QString title, bool addButton, QString checkBoxText, QWidget *parent = 0);
    void    addChild(quint16 parent);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PersonList ui;
    QPalette    p;

private slots:
    void slotNoStepRelatives();
    void clicked(QModelIndex);

signals:
    void sigUpdate(quint16);
    void sigUpdate();
    void noStepRelatives(bool);
};

class PersonListModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        PersonListModel(QObject *parent=0) : QAbstractTableModel(parent) {  }
        int rowCount(const QModelIndex &parent) const;
        int columnCount(const QModelIndex &parent) const;
        QVariant data(const QModelIndex &index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role);
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        void updt();
        QList<quint16> *personList;
};

#endif // PERSONLIST_H
