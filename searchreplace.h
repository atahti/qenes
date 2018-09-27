#ifndef SEARCHREPLACE_H
#define SEARCHREPLACE_H

#include <QDialog>

namespace Ui {
class SearchReplace;
}

class SearchReplace : public QDialog
{
    Q_OBJECT

public:
    explicit SearchReplace(QWidget *parent = 0);
    ~SearchReplace();

private:
    Ui::SearchReplace *ui;
};

#endif // SEARCHREPLACE_H
