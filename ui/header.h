#ifndef HEADER_H
#define HEADER_H

#include <QDialog>
#include "ui_header.h"
#include "data/familydata.h"
#include <QPixmap>
#include <QGraphicsView>

class Charset
{
    public:
        QString name;
        QString version;
};

class Header
{
    public:
        Source source;
        QString receivingSystemName;
        QDate dateTransmission;
        QDate dataDate;
        QTime time;
        QString submTag, subnTag;
        int subm, subn;
        QString file, copyright, gedComVersion, gedComForm;
        Charset charset;
        QString language;
        QString plac, placForm;
        QString nameOfBusiness;
        QString note;
};

class HeaderDialog : public QDialog, private Ui::uiHeaderDialog
{
    Q_OBJECT

    public:
        HeaderDialog(Header header, QWidget *parent = 0);
        ~HeaderDialog();
        bool photo(QString fileName, QGraphicsScene *pPicture, QGraphicsView *uiPicture);
        void submitter();
        void saveSubmitter();

    private slots:
        void closeDialog();
};

#endif // HEADER_H
