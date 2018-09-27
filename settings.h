#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_settings.h"
#include "data/setupvalues.h"
#include <QFile>
#include <QStringListModel>

class Settings : public QDialog, public Ui::uiSettings
{
    Q_OBJECT

    public:
        Settings(quint8 sTab, QWidget *parent = 0);
        static bool changed;
        static void resetSettings();
        static void makeDefaultValues();
        static void makeEventMappings();
        static QList<int> indiEventMap[28];
        static QList<int> famiEventMap[12];
        static void                saveFromRamToDisk();

    private:
        int                 dateFormatIndex(QString str);
        QString             dateFormatString(int i);
        void                saveFromUitoRam();
        QListWidgetItem     *listItem;
        void                updateUsrEvents();
        void                updateNames();
        QStringList         orderString;
        QStringListModel    orderModel;
        static void         fromResourcetoFile(QString fileName);
        quint8 startTab;

    public slots:
        void save();

    private slots:
        void ok();
        void addFamEvent();
        void addIndiEvent();
        void addFemaleName();
        void addPlace();
        void addMaleName();
        void deletePlace();
        void deleteFemaleName();
        void deleteMaleName();
        void addPatronym();
        void deletePatronym();
        void updatePrivacyLimitMaxValues();
};

#endif // SETTINGS_H
