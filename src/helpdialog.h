#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QtHelp>

namespace Ui {
class HelpDialog;
}

class HelpDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit HelpDialog(QWidget *parent = 0);
    ~HelpDialog();
    
private slots:
    void show_file(QModelIndex idx);

private:
    Ui::HelpDialog *ui;
    QHelpEngine _help_engine;
};

#endif // HELPDIALOG_H
