#ifndef DIALOG_LOADPROGRESS_H
#define DIALOG_LOADPROGRESS_H

#include "ui_loadprogress.h"

class LoadProgress : public QDialog, private Ui::uiLoadDialog
{
    public:
        LoadProgress();        
        void setMax(int max);
        void set(int header, int indi, int fam, int multi, int note, int repo, int sour, int subm, int prog);
    private:
        int maxim;
};

#endif // DIALOG_LOADPROGRESS_H
