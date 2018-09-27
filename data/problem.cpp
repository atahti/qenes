#include "data/problem.h"

QString Problem::typeToString()
{
    switch (type) {
    case PROBLEM_UNKNOWN_SEX : return "unknown gender"; break;
    case PROBLEM_UNLINKED : return "unlinked persons"; break;
    case PROBLEM_DATE_BETWEEN_PERSONS : return "Date between persons"; break;
    case PROBLEM_MALENAME : return "Male name"; break;
    case PROBLEM_FEMALENAME : return "Female name"; break;
    case PROBLEM_NAME : return "Name"; break;
    case PROBLEM_HOMOCOUPLE : return "Homo couple"; break;
    case PROBLEM_PLACE : return "Place name"; break;
    case PROBLEM_MOTHER_DEATH : return "Mother death"; break;
    case PROBLEM_FATHER_DEATH : return "Faterh death"; break;
    case PROBLEM_TOO_OLD_MOTHER : return "Too Old Mother"; break;
    case PROBLEM_TOO_OLD_FATHER : return "Too Old Father"; break;
    case PROBLEM_TOO_YOUNG_MOTHER : return "Too Young Mother"; break;
    case PROBLEM_TOO_YOUNG_FATHER : return "Too Young Father"; break;
    case PROBLEM_EVENT_ORDER_VIOLATION : return "Order of Events"; break;
    case PROBLEM_EVENT_DAY_VIOLATION : return "Event Date"; break;
    case PROBLEM_MARRIAGE_CRIME : return "Marriage Crime"; break;
    case PROBLEM_TOO_OLD_SPOUSE : return "Too Old Spouse"; break;
    case PROBLEM_TOO_YOUNG_SPOUSE : return "Too Young Spouse"; break;
    case PROBLEM_SIBLINGS_TOO_CLOSE : return "Birth of Siblings too Close"; break;
    case PROBLEM_SOURCE : return "Source not used"; break;
    case PROBLEM_PLACE_AS_ATTRIBUTE : return "Place name in attribute field"; break;
        break;
    }
    return "error in problem::typeToString";


}
