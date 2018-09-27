#include "searchandhistory.h"
#include "macro.h"
#include "data/genedata.h"

extern  GeneData        * geneData;

void BrowseHistoryModel::updt()
{
   this->layoutChanged();
}

int BrowseHistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    const int tmp = personList->size();

    return tmp;
}

int BrowseHistoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant BrowseHistoryModel::data(const QModelIndex &index, int role) const
{
    quint16 row = index.row();
    quint16 id = personList->at(row);

    if (!index.isValid()) return QVariant(); // tää oli jossain esimerkissä, mitä tämä tekee?
    if ( role == Qt::BackgroundColorRole ) {
        if (INDI(id).sex == FEMALE ) return ( QColor(255, 199, 199) );
        if (INDI(id).sex == MALE ) return ( QColor(199, 199, 225) );
        if (INDI(id).sex == UNKNOWN) return ( QColor(180, 180, 180) );
    }
    if ( role == Qt::TextAlignmentRole ) { return ( Qt::AlignLeft ) ;  }
    if ( role == Qt::DecorationRole ) QVariant();
    if ( role == Qt::DisplayRole ) {
        QString name;

        //name = "(" + INDI(id).getShortFamilyRelation() + ") ";
        name = INDI(id).nameFamily;
        if ( INDI(id).nameFirst != "") name += " " + INDI(id).nameFirst;
        if ( INDI(id).name2nd != "" ) name += " " + INDI(id).name2nd;
        if ( INDI(id).name3rd != "" ) name += " " + INDI(id).name3rd;

        if (!INDI(id).familyRelation.contains("c") && !INDI(id).familyRelation.contains("s")) name = "*" + name;


        //if (INDI(id).directAncestor) name = "*" + name;

        if (index.column() == 0) return name;

        return row;
    }
    return QVariant();
}

Qt::ItemFlags BrowseHistoryModel::flags(const QModelIndex &index) const
 {
    if (!index.isValid())
         return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled;
 }

 //----------------------------------------------------------------------------

void SearchAndHistory::updt()
{
    if ( INDI(GENE.currentId).getMotherInLaw()) this->uiMotherInLaw->setEnabled(true);
    else this->uiMotherInLaw->setEnabled(false);

    if ( GENE.indiLastUsed != 0) this->ui1st->setEnabled(true);
    else this->ui1st->setEnabled(false);

    browseModel->personList = &GENE.browseHistory;
    this->uiLastClicked->setModel(browseModel);
    this->browseModel->updt();
}

SearchAndHistory::SearchAndHistory(QFrame *parent) : QFrame(parent)
{
    setupUi(this);
    p = this->palette();
    this->setAutoFillBackground(true);
    p.setColor(QPalette::Window, Qt::lightGray);
    this->setPalette(p);

    this->setFont(GeneData::panelFont);

    browseModel = new BrowseHistoryModel;

    connect(ui1st,              SIGNAL(clicked()),       this, SLOT(slot1st()));
    connect(uiMotherInLaw,      SIGNAL(clicked()),       this, SLOT(slotMotherinlaw()));
    connect(uiClear,            SIGNAL(clicked()),       this, SLOT(slotClear()));
    connect(uiSearch,           SIGNAL(clicked()),       this, SLOT(slotSearch()));
    connect(uiLastClicked,      SIGNAL(clicked(QModelIndex)), this, SLOT(browseHistoryClicked(QModelIndex)));

}

void SearchAndHistory::browseHistoryClicked(QModelIndex qmi)
{
    quint16 row = qmi.row();
    emit update(GENE.browseHistory.at(row));
}

void SearchAndHistory::slot1st()
{
    if (GENE.rootId) emit update(GENE.rootId);
    else emit update(1);
}

void SearchAndHistory::slotMotherinlaw()
{
    quint16 mil = INDI(GENE.currentId).getMotherInLaw();

    if (mil) emit update(mil);
}

void SearchAndHistory::slotSearch()
{
    bool searchName     = uiSearchFistName->isChecked();
    bool searchFamily   = uiSearchFamilyName->isChecked();
    bool searchEvents   = uiSearchEvents->isChecked();
    QString searchText  = uiSearchText->text();

    for (quint16 i = 0 ; i<=GENE.indiLastUsed ; i++) {
        if ( (INDI(i).familyRelation == searchText) ) {
            GENE.browseHistory.removeAt(i);
            GENE.browseHistory.prepend(i);
        }

        if (searchText.left(1) == "#") {
            QString levelText = searchText.right(searchText.length()-1);

            if (QString::number(INDI(i).generation) == levelText ) {
                GENE.browseHistory.removeAt(i);
                GENE.browseHistory.prepend(i);
                qDebug() << INDI(i).nameFamily << INDI(i).nameFirst << INDI(i).generation << INDI(i).ientry(BIRTH).day.day1;
                //qDebug() << INDI(i).nameFamily << INDI(i).nameFirst << INDI(i).level << INDI(i).estimatedBirth.day1 << INDI(i).estimatedBirth.day2;
            }
        }


        if (searchText != "") {
            if (searchName) {
                if (INDI(i).nameFirst == searchText || INDI(i).name2nd == searchText || INDI(i).name3rd == searchText ) {
                    GENE.browseHistory.removeAt(i);
                    GENE.browseHistory.prepend(i);
                }
            }

            if (searchFamily) {
                if (INDI(i).nameFamily == searchText) {
                    GENE.browseHistory.removeAt(i);
                    GENE.browseHistory.prepend(i);
                }
            }

            if (searchEvents) {
                bool found = false;

                if (INDI(i).note.text.contains(searchText)) found = true;

                for (quint8 y=0 ; y<INDI(i).entry.size() ; y++) {
                    if (   INDI(i).entry.at(y).place.contains(searchText)
                        || INDI(i).entry.at(y).note.text.contains(searchText)
                        || INDI(i).entry.at(y).address.city.contains(searchText)
                        || INDI(i).entry.at(y).address.state.contains(searchText)
                        || INDI(i).entry.at(y).address.line.contains(searchText)
                        || INDI(i).entry.at(y).address.line1.contains(searchText)
                        || INDI(i).entry.at(y).address.line2.contains(searchText)
                        || INDI(i).entry.at(y).address.country.contains(searchText)
                        ) found = true;
                }
                if (found) {
                    GENE.browseHistory.removeAt(i);
                    GENE.browseHistory.prepend(i);
                }
            }
        }
    }
    emit update(0);
}

void SearchAndHistory::slotClear()
{
    GENE.browseHistory.clear();
    emit update(0);
}

