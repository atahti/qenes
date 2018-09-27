#ifndef REPO_H
#define REPO_H

#include <QString>
#include "data/address.h"
#include "data/note.h"

class Repo
{
    public:
        Repo();
        ~Repo();
        void            clear();
        int             id;
        QString         name;
        Address         address;
        Note            note;
        QString         refn, refnType, rin;
        QDateTime       changed;
};

#endif // REPO_H
