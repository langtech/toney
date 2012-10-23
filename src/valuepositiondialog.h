#ifndef VALUEPOSITIONDIALOG_H
#define VALUEPOSITIONDIALOG_H

#include <QDialog>

namespace Ui {
class ValuePositionDialog;
}

class ValuePositionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ValuePositionDialog(QWidget *parent = 0);
    ~ValuePositionDialog();

    int getPosition() const;

private slots:
    virtual void accept();
    virtual void reject();

private:
    Ui::ValuePositionDialog *ui;
    int _pos;
};

#endif // VALUEPOSITIONDIALOG_H
