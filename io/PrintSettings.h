#ifndef PRINTSETTINGS_H
#define PRINTSETTINGS_H

#include <QString>

class PrintSettings
{
    public:
        bool        printPdf;
        bool        printJava;
        QString     dirName;
        void        clear();
};

#endif // PRINTSETTINGS_H
