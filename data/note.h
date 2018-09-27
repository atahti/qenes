#ifndef NOTE_H
#define NOTE_H

#include <QString>
#include <QDateTime>

class Note
{
    public:
        Note();
        ~Note();
        QString     tag;
        QString     text;
        bool        used();
        QString     refn, refnType, rin;
        int         id;
        void        clear();
        QDateTime   change;
//        QString     getPrintNoteText(int restriction, bool replaceToEmptyifRestrected);
};

class NoteRecord : public Note
{
    public:
        NoteRecord();
        ~NoteRecord();
        void copyDataFrom(Note n);
        //int id;
        //Note data;
};

#endif // NOTE_H
