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

public slots:
    void show();
    void hide();

private slots:
    virtual void accept();
    virtual void reject();
    void minf0_changed();
    void maxf0_changed();

private:
    void reset_values();
    void save_values();

private:
    Ui::GetF0ParamsDialog *ui;
    bool _changed;
    int _min_sample_rate;
    int _min_f0;
    int _max_f0;
    double _wind_dur;
    double _frame_step;
    double _cand_thresh;
    double _double_cost;
    double _freq_weight;
    double _lag_weight;
    int _n_cands;
    double _trans_cost;
};

#endif // GETF0PARAMSDIALOG_H
