# -------------------------------------------------
# Project created by QtCreator 2009-07-06T15:16:07
# -------------------------------------------------
TARGET = qenes
TEMPLATE = app
TRANSLATIONS = english.ts \
    suomi.ts \
    svenska.ts
SOURCES += main.cpp \
    mainwindow.cpp \
    data/persondata.cpp \
    data/familydata.cpp \
    dbview.cpp \
    ui/header.cpp \
    settings.cpp \
    problemfinder.cpp \
    data/date.cpp \
    data/multimedia.cpp \
    data/address.cpp \
    data/source.cpp \
    data/repo.cpp \
    data/submitter.cpp \
    data/note.cpp \
    data/entry.cpp \
    ui/person.cpp \
    ui/box/current.cpp \
    io/reporter.cpp \
    io/loadprogress.cpp \
    io/loadfile.cpp \
    io/savefile.cpp \
    ui/personlist/personlist.cpp \
    ui/personlist/childlist.cpp \
    ui/personlist/siblinglist.cpp \
    ui/personlist/cousinlist.cpp \
    data/genedata.cpp \
    ui/searchandhistory.cpp \
    io/PrintSettings.cpp \
    generation.cpp \
    dbview2.cpp \
    ui/box/panelcommonall.cpp \
    linewidget.cpp \
    ui/box/panel.cpp \
    messagepanel.cpp \
    sourceview.cpp \
    data/problem.cpp \
    searchreplace.cpp \
    ui/descendants.cpp
HEADERS += mainwindow.h \
    ui/person.h \
    ui_person.h \
    data/persondata.h \
    ui_mainwindow.h \
    data/familydata.h \
    dbview.h \
    ui_current.h \
    ui_boxsize1.h \
    settings.h \
    problemfinder.h \
    data/genedata.h \
    data/repo.h \
    data/address.h \
    data/note.h \
    data/multimedia.h \
    data/source.h \
    data/date.h \
    data/entry.h \
    data/submitter.h \
    data/problem.h \
    macro.h \
    ui/header.h \
    ui/box/current.h \
    io/reporter.h \
    io/loadprogress.h \
    io/loadfile.h \
    io/savefile.h \
    ui/personlist/personlist.h \
    ui/personlist/childlist.h \
    ui/personlist/siblinglist.h \
    ui/personlist/cousinlist.h \
    ui/searchandhistory.h \
    io/PrintSettings.h \
    generation.h \
    dbview2.h \
    ui/box/panelcommonall.h \
    linewidget.h \
    ui/box/panel.h \
    messagepanel.h \
    gui.h \
    sourceview.h \
    searchreplace.h \
    ui/descendants.h
FORMS += mainwindow.ui \
    databaseview.ui \
    ui/settings.ui \
    problemFinder.ui \
    ui/person.ui \
    ui/header.ui \
    io/printsettings.ui \
    io/loadprogress.ui \
    ui/personlist/personlist.ui \
    ui/SearchAndHistory.ui \
    generation.ui \
    databaseview2.ui \
    messagepanel.ui \
    sourceview.ui \
    ui/box/current.ui \
    searchreplace.ui \
    ui/descendants.ui
//QT += webenginewidgets
QT += printsupport

OTHER_FILES += ../korjaa.txt \
    notes.txt \
    ../qenes-build-desktop/qml/main.qml
RESOURCES += \
    resources.qrc
