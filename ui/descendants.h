#ifndef DESCENDANTS_H
#define DESCENDANTS_H

#include <QDialog>

namespace Ui {
class Descendants;
}

class DescentantButton
{
public:
    quint8 col, row, height;
    quint32 id;
};

class Descendants : public QDialog
{
    Q_OBJECT

public:
    explicit Descendants(quint16 *pId, QWidget *parent = 0);
    ~Descendants();

private:
    Ui::Descendants *ui;
    int descendantTableBuilder(QList<DescentantButton> *buttons, quint8 *ow, quint8 col, quint16 child);
};

#endif // DESCENDANTS_H
