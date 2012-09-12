#ifndef GETF0PARAMSDIALOG_H
#define GETF0PARAMSDIALOG_H

#include <QDialog>
extern "C" {
    #include "get_f0/f0.h"
}

namespace Ui {
class GetF0ParamsDialog;
}

class GetF0ParamsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GetF0ParamsDialog(QWidget *parent = 0);
    ~GetF0ParamsDialog();
    get_f0_session *newGetF0Session();
    bool paramsChanged() const;

private slots:
    virtual void accept();
    virtual void reject();
    void minf0_changed();
    void maxf0_changed();

private:
    void reset_values();

private:
    Ui::GetF0ParamsDialog *ui;
    bool _changed;
    int _min_sample_rate;
    int _minf0;
    int _maxf0;
    double _windur;
    double _step;
};

#endif // GETF0PARAMSDIALOG_H
