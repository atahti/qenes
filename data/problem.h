#ifndef PROBLEM_H
#define PROBLEM_H

#include <QString>

enum PROBLEM_TYPE { PROBLEM_UNKNOWN_SEX,
                    PROBLEM_UNLINKED,
                    PROBLEM_DATE_BETWEEN_PERSONS,
                    PROBLEM_MALENAME,
                    PROBLEM_FEMALENAME,
                    PROBLEM_NAME,                    
                    PROBLEM_HOMOCOUPLE,
                    PROBLEM_PLACE,
                    PROBLEM_MOTHER_DEATH,
                    PROBLEM_FATHER_DEATH,
                    PROBLEM_TOO_OLD_MOTHER,
                    PROBLEM_TOO_OLD_FATHER,
                    PROBLEM_TOO_YOUNG_MOTHER,
                    PROBLEM_TOO_YOUNG_FATHER,
                    PROBLEM_EVENT_ORDER_VIOLATION,
                    PROBLEM_EVENT_DAY_VIOLATION,
                    PROBLEM_MARRIAGE_CRIME,
                    PROBLEM_TOO_OLD_SPOUSE,
                    PROBLEM_TOO_YOUNG_SPOUSE,
                    PROBLEM_SIBLINGS_TOO_CLOSE,
                    PROBLEM_SOURCE,
                    PROBLEM_PLACE_AS_ATTRIBUTE
                };

class Problem
{
public:
    quint16         id;
    QString         attribute;
    PROBLEM_TYPE    type;
    bool            indi;
    QString         explanation;
    QString         typeToString();

};

#endif // PROBLEM_H
