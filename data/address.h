#ifndef ADDRESS_H
#define ADDRESS_H

#include <QString>

class Address
{
    public:
        Address();
        ~Address();
        void clear();
        bool used();
        QString line, line1, line2, city, state, post, country, email;
};

#endif // ADDRESS_H
