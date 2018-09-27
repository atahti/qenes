#ifndef DESCENDANTS_H
#define DESCENDANTS_H

#include <QDialog>

namespace Ui {
class Descendants;
}

class Descendants : public QDialog
{
    Q_OBJECT

public:
    explicit Descendants(quint16 *pId, QWidget *parent = 0);
    ~Descendants();

private:
    Ui::Descendants *ui;
};

#endif // DESCENDANTS_H
