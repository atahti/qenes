/*
 * TabWidgetan piirret‰‰n box, ja boxiin piirret‰‰n entryt. TabWidget ei laske en‰‰ entrylinej‰, vaan boxien lukum‰‰r‰‰
 * Jokainen boxi pit‰‰ sis‰ll‰‰n tiedon, montako entryline‰ siell‰ on
 */

#ifndef PERSON_H
#define PERSON_H

#include "mainwindow.h"
#include <QShortcut>
#include <QDialog>
#include <QStringListModel>
#include "ui_person.h"
#include "data/persondata.h"

/* Individual UI elements that will be drawn to a UI line. Eg typecombo = QCombobox
 */
enum ENTRYELEMENT { TYPECOMBO, NEWCOMBO, ACCCOMBO, ADOPCOMBO, USRCOMBO, VALUE, DAY1, DAY2, TO, PLACE,
                    BUTTONX, BUTTONM, NOELEMENT, ADDRLINE, MORE_LABEL,
                    POST, CITY, STATE, COUNTRY, PHONE, SOURCE, SOURCE_PAGE, SOURCE_TEXT,
                    SOURCE_QLTY, PHOTO, NOTES, PHOTONOTES, RESTRICTION, MORE_ADD, PHOTOFILE, SHOWPHOTO, SPACELINE, DELETE };

/* Types of lines for different event types. The content for each line will be defined in cpp file
 */
enum ENTRYLINETYPE { ENTRYINFO_NULL, ENTRYINFO_ADD, ENTRYINFO_BASIC, ENTRYINFO_ADDRLINE, ENTRYINFO_POST_CITY,
                   ENTRYINFO_STATE_COUNTRY,
                   ENTRYINFO_SOURCE_PAGE_QUALITY, ENTRYINFO_SOURCE_TEXT,
                   ENTRYINFO_NOTES, ENTRYINFO_PHOTO, ENTRYINFO_RESTRICTIONS, ENTRYINFO_LINE };

class EntryBox;
class EntryLine;
class EntrySettings;
class TabWidget;
class Person;

// t‰m‰ tehtiin kun tutkin miten widgettiin piirret‰‰n. Nyt turha, mutta mit‰s t‰t‰ turhaan toisaalta tuhoamakaan.


/*
 * Se widget, johon eventit piirret‰‰n ja joka voi olla isompi kuin se jonka k‰ytt‰j‰ n‰kee
 */

class EventFrame : public QFrame
{
public:
    EventFrame(TabWidget *ea);
    TabWidget   *   tabWidget;
    QVBoxLayout *   grid;
    QSpacerItem *   spacerItem;

    void addSpacerItem() {
        spacerItem = new QSpacerItem(20, 1000, QSizePolicy::Expanding, QSizePolicy::Expanding);
        grid->addItem(this->spacerItem);

    }
};

class TextEdit2 : public QPlainTextEdit
{
    Q_OBJECT
public:
    TextEdit2(QWidget *parent = 0);
    //void                reHeight();             ///< Resize the height of qte to fit the content

        QSize sizeHint() const;
        QSize minimumSizeHint() const;

private:
    //quint16 getUIHeight();
};

class TabWidget : public QWidget
{
    public:
        TabWidget(QTabWidget*);
        TabWidget();
        QGridLayout     *   grid;               ///< Tabwidgt grid, where name, genre, publicity and event area will be drawn
        bool                indi;               ///< For person data, this is true. For family data, this is false.
        QList<Entry>    *   entrys;             ///< Pointer to entrys (QList)
        void                set();              ///< Set some tabwidget related UI setup values
        QList<EntryBox*>    pEntryBox;          ///< Pointer to entry boxes, that holds the event widgets.
        QScrollArea     *   eventScrollArea;    ///< The scrollarea for events
        EventFrame      *   eventFrame;         ///< the widget, where event boxes will be drawn. The eventboxes will be piled vertically, and the event info will be drawn inside the event box.

};

class EntryLine : public QObject
{
Q_OBJECT

    private:        
        bool                getPhotoFile(QString *fileName);
        QString             getChangedText(TextEdit2 *qte);
//        TextEdit            * editNotes;
        //void                resize(TextEdit2 *qte);         ///< Resize the height of qte to fit the content

    public:        
        QList<QWidget*>     uiWidgets;                      ///< UI widgets of this line
        EntryBox        *   entryBox;                       ///< Pointer to box that holds this line
        void                deleteUiWidgets();              ///< delete widgets of this line, but leave the line object
        EntryLine(EntryBox *box);
        ~EntryLine();
        quint8              row;                            ///< row of line inside box
        void                makeUi(QGridLayout *grid);
        void                deleteUi();                     ///< delete widgets and the box UI
        void                addEntry(int);                  ///< add new entry
        ENTRYLINETYPE       entryLineType;


    private slots:
        void                deleteThisPerson();           ///< delete this person
        void                newEventPhoto();                ///< a new photo for an event
        void                slotAccChanged(int acc);        ///< Accuracy of date changed
        void                slotDeleteEntry();              ///< x button on UI pressed and event will be deleted
        void                slotTypeChanged(int newType);   ///< Type of existing event has changed
        void                slotUsrChanged(int newType);    ///< usr combo changed
        void                date1changed(QDate date);       ///< date 1 changed
        void                date2changed(QDate date);       ///< date 2 changed
        void                placeChanged();                 ///< place changed
        void                adopChanged(int newValue);      ///< adopted combo changed
        void                attrValueChanged();             ///< attribute value changed
        void                slotAddrLineChanged();          ///< address line changed
        void                slotNotesChanged();             ///< notes changed
        void                slotPhotoNotesChanged();        ///< notes of photo changed (both event photo and portrait photo)
        void                slotPostChanged();              ///< post changed
        void                slotCityChanged();              ///< city changed
        void                slotStateChanged();             ///< state changed
        void                slotCountryChanged();           ///< country changed
        void                slotPhoneChanged();             ///< phone number changed
        void                slotSourceChanged(int index);   ///< source combo changed
        void                slotSourcePageChanged();        ///< page of source changed
        void                slotSourceTextChanged();        ///< text of source changed
        void                slotSourceQualityChanged(int index);///< Source quality combo changed
        void                slotPrivacyPolicyChanged(int value);///< Privacy policy combo changed
        void                slotEntryAddMore(int index);    ///< Selecting new additional rows for an entry
        void                slotChangePhoto();              ///< fileName button pressed for changing photo
        void                slotNewPhoto();                 ///< add-photo button pressed for adding a photo
        void                slotEraseAddrLine();            ///< x-button had pressed for erasing address line
        void                slotEraseNotes();               ///< x-button had pressed for erasing notes line
        void                slotErasePostCity();            ///< x-button had pressed for erasing post and city line
        void                slotEraseStateCountry();        ///< x-button had pressed for erasing state-country line
        void                slotErasesourePgQlty();         ///< x-button had pressed for erasing source page and quality line
        void                slotEraseSourceText();          ///< x-button had pressed for erasing source text line
        void                slotEraseRestriction();         ///< x-button had pressed for erasing print-restriction line
        void                slotEraseEntryPhoto();          ///< x-button had pressed for erasing photo line

public slots:
        void                slotEntryMore();        ///< slot to be called when + (more) button is pressed
};

class EntryBox : public QFrame
{
// boxi, jossa n‰ytet‰‰n yksi eventti. Perustiedot, lis‰tiedot yms. Tausta on v‰ritetty niin ett‰ eventit
// erottaa toisistaan

public:
    quint8                  getType();              ///< get the event type from the entry variable, or 0 if no entry variable
    quint8                  boxId;                  ///< id number of this box
    void                    setBackGroundColour();  ///< set background colour of this box, and red frame depending on security settings
    QGridLayout         *   grid;                   ///< Grid layout, where event info will be drawn. Eg. buttons, combos, text lines etc...
    QList<EntryLine*>       pEntryLine;             ///< Pointers to EntryLines. Each EntryLine is a line inside EntryBox.
    EntryBox(Person *parent);
    quint8                  entryId;                ///< id number of entry
    quint8                  photoId;                ///< id of photo
    void                    makeSelectableEvents(); ///< Update the event type combobox for events, that are allowed for an event in this box.
    QStringList             selectableEvents;       ///< return list of events, that can be selected in this box
    QList<int>              eventMapping;           ///< maps combo-id to event type. For example, if 3rd selection is event nro 4, eventMapping.at(3)=4
    QStringListModel        typeModel;              ///< StringListModel holding the selection texts for type combos
    Entry               *   entry;                  ///< Pointer to entry variable of this entryBox
    bool                    eventUsed(quint8 type); ///< Has the event of type been used with this view (either person or family data). Return false or true.
    QComboBox           *   uiComboType;            ///< QComboBox holding the type of the event in this box.
    QComboBox           *   uiComboAcc;             ///< QComboBox holding accuracy info of dates
    QDateEdit           *   uiDate1;                ///< QDateEdit for holding the date value
    QDateEdit           *   uiDate2;                ///< QDateEdit for holding the latest date value. Visible only if to...from accuracy is used.
    void                    makeEntryLine(ENTRYLINETYPE type);
    QStringList         *   ownEvents;              ///< A list of own selectable events
    QStringListModel    *   usrEventModel;
    Person              *   par;                    ///< Pointer to parent class "person", this dialog window
private:

};

// t‰ss‰ on ui public koska entrylinen pit‰‰ voida kirjoittaa ui widgetetihin
class Person : public QDialog, public Ui::Person
{
    Q_OBJECT

    public:
        Person(bool showMales, bool showFemales, quint16 *pId, QWidget *parent = 0);
        ~Person();

        void                makeEntryBox(quint8 i, quint8 i2, ENTRYLINETYPE elt);
        void                backGroundColorBoxes();
        void                debugEvents();      ///< for development purposes
        bool                showMales;          ///< in "assign person" dialog window, boolean for showing just males
        bool                showFemales;        ///< in "assign person" dialog window, boolean for showing just females
        void                updateTypes();
        QGraphicsScene      picture;            ///< picture to be shown in photowidget-method, which is a 100x100 pix thumbnail
        void                updateDates();
        void                addNewEntryComboAndBox(); ///< add a new box and a new entry selection combo for that box
        void                addFamilyEvents();
        void                addIndiEvents();
        QStringListModel    sourceModel;        ///< model for source combo box
        QStringListModel    entryMoreModel;     ///< model for "add more" combo box
        QStringListModel    entryRestModel;     ///< restriction model for entry
        quint16             id;                 ///< id number of a person in this dialog window
        TabWidget       *   visibleTabWidget;   ///< pointer to a tab widget, that is currently visible and user editable
        quint8              guiBoxOfEntryType(quint8 type); ///< give the box number of box of type
        QStringListModel    indiEventModel, famEventModel;

    private:                
        Ui::Person          ui;
        QList<quint16>      spouseIdList;        
        void                setValues();                ///< Set values to dialog, expect events on event area
        QStringList         sourceList;
        void                indiEntryShortcutSelected(ENTRY entryType);
        void                famEntryShortcutSelected(ENTRYf entryType);
        void                shortCutToEventN(quint8 n);
        void                addEntryBox(EntryBox *box);

    private slots:
        void                save();                     ///< save the content of dialog to variables and close the window
        void                closeWithoutSaving();       ///< save the content of dialog to variables but dont close the window
        void                nameEdited(QString);         ///< slot to be called when user has edited a text edit widget. Change will be saved to variable here.
        void                assignNewPersoninDb();
        void                breakRelationshipToMother(); ///< change the database so, that mother of this person is no longer mother. Persons will stay in database.
        void                breakRelationshipToFather(); ///< change the database so, that father of this person is no longer father. Persons will stay in database.
        void                breakRelationshipToSpouse(); ///< change the database so, that spouse of this person is no longer spouse. Persons will stay in database.
        void                slotShortcut1();
        void                slotShortcut2();
        void                slotShortcut3();
        void                slotShortcut4();
        void                slotShortcut5();
        void                slotShortcut6();
        void                slotShortcut7();
        void                slotShortcut8();
        void                slotShortcut9();
        void                slotShortcut0();
        void                slotShortcutB();
        void                slotShortcutD();
        void                slotShortcutO();
        void                slotShortcutE();
        void                slotShortcutC();
        void                slotShortcutG();
        void                slotShortcutA();
        void                slotShortcutU();
        void                slotShortcutW();
        void                slotShortcutR();
        void                slotShortcutP();
        void                slotShortcutN();
        void                slotShortcutT();
        void                slotShortcutI();
        void                slotShortcutF1();
        void                slotShortcutF2();
        void                slotShortcutF3();
        void                slotShortcutF4();
        void                slotShortcutEnter();
        void        slotShortcutMM();
        void        slotFocus();
        void        slotNextTab();
        void        slotPrevTab();
        void        slotTabChanged(int);
        void        privacyPolicyChanged(int);

    signals:
        void        sigUpdate(quint16);     ///< signal p‰‰ikkunalle
        void        sigPersonAssigned(quint16 id);
};

class PhotoWidget : public QGraphicsView
{
   Q_OBJECT

public:
    PhotoWidget(QString fn);
    QWidget         par;

private:
    QString         fileName;
    QPixmap         bmpOrginal;     ///< file that has been loaded from file
    QPixmap         bmpScaled;      ///< bmpOrginal that has been scaled to 100x100, keeping aspect ratio
    QGraphicsScene *picture;        ///< QGraphicsScene, that has bmpScaled included, and will be shown on photowidget


protected:
    void mousePressEvent(QMouseEvent *event);
};

class QDialog2 : public QDialog
{
    Q_OBJECT

public:
    QDialog2(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event);
};


#endif // PERSON_H


