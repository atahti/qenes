#ifndef SUBMITTER_H
#define SUBMITTER_H

#include <QString>
#include "data/multimedia.h"
#include "data/address.h"

class Submitter
{
    public:
        Submitter();
        ~Submitter();
        void        clear();
        int         id;     // will be used for giving id number if there's many submitters
        QString     tag;
        QString     name;
        QString     language;
        MultiMedia  multimedia;
        Address     address;
        QString     rfn, rin;
        QDateTime   changed;
};

#endif // SUBMITTER_H
