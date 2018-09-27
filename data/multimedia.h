#ifndef MULTIMEDIA_H
#define MULTIMEDIA_H

#include <QString>
#include <QDateTime>
#include "data/note.h"

enum MULTIMEDIA_FORMAT { NO_MM, BMP_MM, GIF_MM, JPEG_MM, OLE_MM, PCX_MM, TIFF_MM, WAV_MM };

class MultiMedia // link ja record
{
    public:
        MultiMedia();
        ~MultiMedia();
        MULTIMEDIA_FORMAT format;
        QString     title, file;
        QString     refn;
        QString     refnType;
        QString     rin;
        QString     tag;
        int         id;
        Note        note;
        void        clear();
        bool        used();
        QDateTime   change;
};

class MultiMediaRecord : public MultiMedia
{
    public:
        MultiMediaRecord();
        ~MultiMediaRecord();
        void copyDataFrom(MultiMedia m);
        //int id;
        //MultiMedia data;
};

#endif // MULTIMEDIA_H
