#ifndef MACRO_H
#define MACRO_H

#include <QFont>
#include <QDebug>

enum PANELTYPE { PANEL_MALE, PANEL_FEMALE, PANEL_CURRENT, PANEL_SPOUSE };
enum PRIVACYMETHOD { PRIVACY_0_SHOW, PRIVACY_1_BLUR_DETAILS, PRIVACY_2_HIDE_DETAILS, PRIVACY_3_BLUR_NAME, PRIVACY_4_HIDE };
enum PPOLICY { PRINTPOLICY_1_EVERYTHING, PRINTPOLICY_2_GENE, PRINTPOLICY_3_PUBLIC, PRINTPOLICY_4_ALL };

#define RUN_CHECK   true // jos tämä on false, tietokantatarkastukset jätetään yleensä tekemättä
#define GENE                            geneData[GeneData::currentGd]
#define INDI(indix)                     geneData[GeneData::currentGd].indi[indix]
#define FAM(famx)                       geneData[GeneData::currentGd].fami[famx]
#define SOUR(sourx)                     geneData[GeneData::currentGd].sour[sourx]
#define GET_IEVENT_SETTING(ievent)      (quint64)sets.value(QString("indi/")+QString::number(ievent)).toInt()

// Raskausaika päivinä. Tätä tarvitaan siihen, että lasketaan maksimiaika jolloin lapsi voi syntyä
// isän kuoleman jälkeen.
#define PREGNANCY_PERIOD    300

#define s_setPrivacyMethod(policy, method)        sets.setValue(QString("privacy/")+QString::number(policy), method )
#define s_privacyMethod(level)                    sets.value(QString("privacy/")+QString::number(level)).toInt()

/*
  x     age     method
  1     5       3
  2     50      2
  3     120     1
  */
#define s_privacyMethodAutoAge(x)                 sets.value(QString("privacy/autoage/")+QString::number(x)).toInt()
#define s_privacyMethodAuto(x)                    sets.value(QString("privacy/auto/")+QString::number(x)).toInt()
#define s_setPrivacyMethodAuto(x, method)         sets.setValue(QString("privacy/auto/")+QString::number(x), method)
#define s_setPrivacyMethodAutoAge(x, age)         sets.setValue(QString("privacy/autoage/")+QString::number(x), age)

#define GET_I_MAX(type)                 sets.value(QString("indi/max/")+QString::number(type)).toInt()
#define GET_I_MIN(type)                 sets.value(QString("indi/min/")+QString::number(type)).toInt()
#define GET_I_CAL(type)                 sets.value(QString("indi/cal/")+QString::number(type)).toInt()
#define GET_I_USE(type)     sets.value(QString("indi/use/")+QString::number(type)).toBool()
#define GET_I_MANY(type)    sets.value(QString("indi/many/")+QString::number(type)).toBool()
#define GET_I_ENTRY_MIN(type)   sets.value(QString("indi/entry/min/")+QString::number(type)).toInt()
#define GET_I_ENTRY_MAX(type)   sets.value(QString("indi/entry/max/")+QString::number(type)).toInt()

#define GET_F_MAX(type)     sets.value(QString("fam/max/")+QString::number(type)).toInt()
#define GET_F_MIN(type)     sets.value(QString("fam/min/")+QString::number(type)).toInt()
#define GET_F_CAL(type)     sets.value(QString("fam/cal/")+QString::number(type)).toInt()
#define GET_F_USE(type)     sets.value(QString("fam/use/")+QString::number(type)).toBool()
#define GET_F_MANY(type)    sets.value(QString("fam/many/")+QString::number(type)).toBool()
#define GET_F_ENTRY_MIN(type)   sets.value(QString("fam/entry/min/")+QString::number(type)).toInt()
#define GET_F_ENTRY_MAX(type)   sets.value(QString("fam/entry/max/")+QString::number(type)).toInt()

#define GET_I_PRINT(type)   sets.value(QString("indi/print/")+QString::number(type)).toBool()
#define GET_F_PRINT(type)   sets.value(QString("fam/print/")+QString::number(type)).toBool()

#define SET_I_PRINT(type, print) sets.setValue(QString("indi/print/")+QString::number(type), print )
#define SET_F_PRINT(type, print) sets.setValue(QString("fam/print/")+QString::number(type), print )

#define SET_I_MIN(ievent, age)   sets.setValue(QString("indi/min/")+QString::number(ievent), age )
#define SET_I_CAL(ievent, age)   sets.setValue(QString("indi/cal/")+QString::number(ievent), age )
#define SET_I_MAX(ievent, age)   sets.setValue(QString("indi/max/")+QString::number(ievent), age )
#define SET_I_MANY(ievent, many) sets.setValue(QString("indi/many/")+QString::number(ievent), many )
#define SET_I_USE(ievent, used)  sets.setValue(QString("indi/use/")+QString::number(ievent), used )
#define SET_I_ENTRY_MIN(ievent, value)  sets.setValue(QString("indi/entry/min/")+QString::number(ievent), value )
#define SET_I_ENTRY_MAX(ievent, value)  sets.setValue(QString("indi/entry/max/")+QString::number(ievent), value )

#define SET_F_MIN(ievent, age)   sets.setValue(QString("fam/min/")+QString::number(ievent), age )
#define SET_F_CAL(ievent, age)   sets.setValue(QString("fam/cal/")+QString::number(ievent), age )
#define SET_F_MAX(ievent, age)   sets.setValue(QString("fam/max/")+QString::number(ievent), age )
#define SET_F_MANY(ievent, many) sets.setValue(QString("fam/many/")+QString::number(ievent), many )
#define SET_F_USE(ievent, used)  sets.setValue(QString("fam/use/")+QString::number(ievent), used )
#define SET_F_ENTRY_MIN(ievent, value)  sets.setValue(QString("fam/entry/min/")+QString::number(ievent), value )
#define SET_F_ENTRY_MAX(ievent, value)  sets.setValue(QString("fam/entry/max/")+QString::number(ievent), value )

#define SET_EVENT_TIMELIMITS(yesno) sets.setValue( "timelimits", yesno )
#define GET_EVENT_TIMELIMITDISABLED    sets.value("timelimits").toBool()

#define GET_AGE_MOTHERCHILD_MIN         sets.value("ageBetweenIndi/motherChild/min").toInt()
#define GET_AGE_MOTHERCHILD_CAL         sets.value("ageBetweenIndi/motherChild/cal").toInt()
#define GET_AGE_MOTHERCHILD_MAX         sets.value("ageBetweenIndi/motherChild/max").toInt()
#define GET_AGE_FATHERCHILD_MIN         sets.value("ageBetweenIndi/fatherChild/min").toInt()
#define GET_AGE_FATHERCHILD_CAL         sets.value("ageBetweenIndi/fatherChild/cal").toInt()
#define GET_AGE_FATHERCHILD_MAX         sets.value("ageBetweenIndi/fatherChild/max").toInt()

#define SET_AGE_MOTHERCHILD_MIN(age)    sets.setValue("ageBetweenIndi/motherChild/min", age )
#define SET_AGE_MOTHERCHILD_CAL(age)    sets.setValue("ageBetweenIndi/motherChild/cal", age )
#define SET_AGE_MOTHERCHILD_MAX(age)    sets.setValue("ageBetweenIndi/motherChild/max", age )
#define SET_AGE_FATHERCHILD_MIN(age)    sets.setValue("ageBetweenIndi/fatherChild/min", age )
#define SET_AGE_FATHERCHILD_CAL(age)    sets.setValue("ageBetweenIndi/fatherChild/cal", age )
#define SET_AGE_FATHERCHILD_MAX(age)    sets.setValue("ageBetweenIndi/fatherChild/max", age )

#define GET_AGE_SPOUSES_MIN             sets.value("ageBetweenIndi/spouses/min").toInt()
#define GET_AGE_SPOUSES_CAL             sets.value("ageBetweenIndi/spouses/cal").toInt()
#define GET_AGE_SPOUSES_MAX             sets.value("ageBetweenIndi/spouses/max").toInt()

#define SET_AGE_SPOUSES_MIN(age)        sets.setValue("ageBetweenIndi/spouses/min", age )
#define SET_AGE_SPOUSES_CAL(age)        sets.setValue("ageBetweenIndi/spouses/cal", age )
#define SET_AGE_SPOUSES_MAX(age)        sets.setValue("ageBetweenIndi/spouses/max", age )

#define GET_ACC_ABT             sets.value("acc/abt").toString()
#define GET_ACC_EST             sets.value("acc/est").toString()
#define GET_ACC_CAL             sets.value("acc/cal").toString()
#define GET_ACC_BEF             sets.value("acc/bef").toString()
#define GET_ACC_AFT             sets.value("acc/aft").toString()
#define GET_ACC_EXA             sets.value("acc/exa").toString()
#define GET_ACC_FRO             sets.value("acc/fro").toString()

#define SET_ACC_ABT(value)      sets.setValue("acc/abt", value)
#define SET_ACC_EST(value)      sets.setValue("acc/est", value)
#define SET_ACC_CAL(value)      sets.setValue("acc/cal", value)
#define SET_ACC_BEF(value)      sets.setValue("acc/bef", value)
#define SET_ACC_AFT(value)      sets.setValue("acc/aft", value)
#define SET_ACC_EXA(value)      sets.setValue("acc/exa", value)
#define SET_ACC_FRO(value)      sets.setValue("acc/fro", value)

#define GET_RELATION_COUSIN3    sets.value("relationship/cousin3rd").toString()
#define GET_RELATION_COUSIN2    sets.value("relationship/cousin2nd").toString()
#define GET_RELATION_COUSIN     sets.value("relationship/cousin").toString()
#define GET_RELATION_FUNCLE     sets.value("relationship/fatherUncle").toString()
#define GET_RELATION_MUNCLE     sets.value("relationship/motherUncle").toString()
#define GET_RELATION_AUNT       sets.value("relationship/aunt").toString()
#define GET_RELATION_SIBLING    sets.value("relationship/sibling").toString()
#define GET_RELATION_MOTHER     sets.value("relationship/mother").toString()
#define GET_RELATION_FATHER     sets.value("relationship/father").toString()
#define GET_RELATION_CHILD      sets.value("relationship/child").toString()

#define SET_RELATION_COUSIN3(value)     sets.setValue("relationship/cousin3rd", value)
#define SET_RELATION_COUSIN2(value)     sets.setValue("relationship/cousin2nd", value)
#define SET_RELATION_COUSIN(value)      sets.setValue("relationship/cousin", value)
#define SET_RELATION_FUNCLE(value)      sets.setValue("relationship/fatherUncle", value)
#define SET_RELATION_MUNCLE(value)      sets.setValue("relationship/motherUncle", value)
#define SET_RELATION_AUNT(value)        sets.setValue("relationship/aunt", value)
#define SET_RELATION_SIBLING(value)     sets.setValue("relationship/sibling", value)
#define SET_RELATION_MOTHER(value)      sets.setValue("relationship/mother", value)
#define SET_RELATION_FATHER(value)      sets.setValue("relationship/father", value)
#define SET_RELATION_CHILD(value)       sets.setValue("relationship/child", value)

#define GET_RELATION_COUSIN3_USE        sets.value("relationship/cousin3rd/use").toBool()
#define GET_RELATION_COUSIN2_USE        sets.value("relationship/cousin2nd/use").toBool()
#define GET_RELATION_COUSIN_USE         sets.value("relationship/cousin/use").toBool()
#define GET_RELATION_FUNCLE_USE         sets.value("relationship/fatherUncle/use").toBool()
#define GET_RELATION_MUNCLE_USE         sets.value("relationship/motherUncle/use").toBool()
#define GET_RELATION_AUNT_USE           sets.value("relationship/aunt/use").toBool()
#define GET_RELATION_SIBLING_USE        sets.value("relationship/sibling/use").toBool()
#define GET_RELATION_CHILD_USE          sets.value("relationship/child/use").toBool()

#define SET_RELATION_COUSIN3_USE(use) sets.setValue("relationship/cousin3rd/use", use)
#define SET_RELATION_COUSIN2_USE(use) sets.setValue("relationship/cousin2nd/use", use)
#define SET_RELATION_COUSIN_USE(use)  sets.setValue("relationship/cousin/use", use)
#define SET_RELATION_FUNCLE_USE(use)  sets.setValue("relationship/fatherUncle/use", use)
#define SET_RELATION_MUNCLE_USE(use)  sets.setValue("relationship/motherUncle/use", use)
#define SET_RELATION_AUNT_USE(use)    sets.setValue("relationship/aunt/use", use)
#define SET_RELATION_SIBLING_USE(use)    sets.setValue("relationship/sibling/use", use)
#define SET_RELATION_CHILD_USE(use)  sets.setValue("relationship/child/use", use)

#define GET_ALLMINDAY           sets.value("allMinDay").toDate()
#define GET_ALLMAXDAY           sets.value("allMaxDay").toDate()

#define SET_ALLMINDAY(date)           sets.setValue("allMinDay", date)
#define SET_ALLMAXDAY(date)          sets.setValue("allMaxDay", date)

#define GET_SUGGEST_FAMILYNAME   sets.value("names/suggestFamilyName").toBool()
#define GET_SUGGEST_SEX         sets.value("names/suggestSex").toBool()

#define SET_SUGGEST_FAMILYNAME(value) sets.setValue("names/suggestFamilyName", value)
#define SET_SUGGEST_SEX(value)                 sets.setValue("names/suggestSex", value)

#define GET_DIRECTORY           sets.value("fileDirectory").toString()
#define SET_DIRECTORY(dir)      sets.setValue("fileDirectory", dir)

#define GET_PRINT_WHAT          sets.value("print/print").toInt()
#define GET_PRINT_ANCDEC        sets.value("print/inclAncDec").toBool()
//#define GET_PRINT_COLORS        sets.value("print/colors").toInt()
#define GET_PRINT_BOTHPARENTS   sets.value("print/inclBothParents").toBool()
#define GET_PRINT_PHOTOS        sets.value("print/inclPhoto").toBool()
//#define GET_PRINT_EVENTCALENDAR sets.value("print/inclEventCalendar").toBool()
#define set_printPolicy         sets.value("print/privacy").toInt()
#define GET_PRINT_ESTIMATEDBIRTHS sets.value("print/estimatedBirth").toBool()

#define SET_PRINT_CHILDS(value)        sets.setValue("print/inclChilds", value)
#define SET_PRINT_EXC_OLD(value)       sets.setValue("print/exclOlds", value)
#define SET_PRINT_EXCLUDE_OLD_LIMIT(value) sets.setValue("print/exclOldLimit", value)
#define SET_PRINT_WHAT(value)          sets.setValue("print/print", value)
#define SET_PRINT_PARENTS(value)       sets.setValue("print/inclParents", value)
#define SET_PRINT_ANCDEC(value)        sets.setValue("print/inclAncDec", value)
#define SET_PRINT_NUMBERING(value)     sets.setValue("print/numbering", value)
#define SET_PRINT_COLORS               sets.setValue("print/colors", value)
#define SET_PRINT_NOTES(value)         sets.setValue("print/inclNotes", value)
#define SET_PRINT_EXC_LIVINGS(value)   sets.setValue("print/exclLivings", value)
#define SET_PRINT_EVENTS(value)        sets.setValue("print/inclEvents", value)
#define SET_PRINT_BOTHPARENTS(value)   sets.setValue("print/inclBothParents", value)
#define SET_PRINT_PHOTOS(value)        sets.setValue("print/inclPhoto", value)
#define SET_PRINT_SOURCE(value)        sets.setValue("print/inclSource", value)
//#define SET_PRINT_EVENTCALENDAR(value) sets.setValue("print/inclEventCalendar", value)
//#define SET_PRINT_RESTRICTIONS(value)  sets.setValue("print/restrictions", value)

#define MIN(a,b) ((a)>=(b)?(b):(a))
#define MAX(a,b) ((a)<=(b)?(b):(a))
#define MAX3(a, b, c)   ((a)<=(MAX(b,c))?(MAX(b,c)):(a))
#define MIN3(a, b, c)   ((a)<=(MIN(b,c))?(a):(MIN(b,c)))
#define MID3(a, b, c)   ( (a)<=(MIN(b,c))?(MIN(b,c)):((b)<=(MIN(a,c))?(MIN(a,c)):MIN(a,b)))


// privacy policyt.
#define SHOW 0
#define DONT_SHOW 1
#define CHANGE_TO_TEXT 2

#define COLOR_PRINTYELLOW   "#FFFFCC"
#define COLOR_PRINTRED      "#FFDDAA"
#endif // MACRO_H

