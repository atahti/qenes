#ifndef MESSAGEPANEL_H
#define MESSAGEPANEL_H

#include <QFrame>

namespace Ui {
    class MessagePanel;
}

class MessagePanel : public QFrame
{
    Q_OBJECT

public:
    explicit MessagePanel(QWidget *parent = 0);
    ~MessagePanel();
    void addMessage(QString text);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MessagePanel *ui;
};

#endif // MESSAGEPANEL_H
