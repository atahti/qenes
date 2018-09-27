#ifndef LEVEL_H
#define LEVEL_H

#include <QDialog>
#include <QDate>
#include "ui_generation.h"

class GenerationData
{
public:
    QDate min, max, estMin, estMax;
    int count;
};

class Generation : public QFrame, Ui::Generation
{
    Q_OBJECT

    private:
        QPalette p;
    public:
        Generation(QFrame *parent = 0);
        void setup();
        int generationOnUi;
};

#endif // LEVEL_H
