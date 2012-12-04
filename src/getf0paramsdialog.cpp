#include "getf0paramsdialog.h"
#include "ui_getf0paramsdialog.h"
#include "annotation.h"
#include <QDebug>

GetF0ParamsDialog::GetF0ParamsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GetF0ParamsDialog),
    _changed(false),
    _min_sample_rate(8000)
{
    ui->setupUi(this);

    save_values();

    connect(ui->param_min_f0, SIGNAL(editingFinished()), SLOT(minf0_changed()));
    connect(ui->param_max_f0, SIGNAL(editingFinished()), SLOT(maxf0_changed()));

    reset_values();
}

GetF0ParamsDialog::~GetF0ParamsDialog()
{
    delete ui;
}

get_f0_session *GetF0ParamsDialog::newGetF0Session()
{
    get_f0_session *session = init_get_f0();
    session->par->min_f0 = _min_f0;
    session->par->max_f0 = _max_f0;
    session->par->wind_dur = _wind_dur;
    session->par->frame_step = _frame_step;
    session->par->cand_thresh = _cand_thresh;
    session->par->double_cost = _double_cost;
    session->par->freq_weight = _freq_weight;
    session->par->lag_weight = _lag_weight;
    session->par->n_cands = _n_cands;
    session->par->trans_cost = _trans_cost;
    return session;
}

bool GetF0ParamsDialog::paramsChanged() const
{
    return _changed;
}

void GetF0ParamsDialog::show()
{
    int sr = Annotation::min_sample_rate();
    if (sr > 0) {
        _min_sample_rate = sr;
        reset_values();
    }
    QDialog::show();
}

void GetF0ParamsDialog::hide()
{
    QDialog::hide();
}

void GetF0ParamsDialog::accept()
{
    int minf0 = _min_f0;
    int maxf0 = _max_f0;
    double windur = _wind_dur;
    double step = _frame_step;

    save_values();
    reset_values();

    _changed = minf0 != _min_f0 ||
            maxf0 != _max_f0 ||
            windur != _wind_dur ||
            step != _frame_step;

    QDialog::accept();
}

void GetF0ParamsDialog::reject()
{
    reset_values();
    QDialog::reject();
}

void GetF0ParamsDialog::minf0_changed()
{
    int v = ui->param_min_f0->value();
    ui->param_max_f0->setMinimum(v);
}

void GetF0ParamsDialog::maxf0_changed()
{
    int v = ui->param_max_f0->value();
    ui->param_min_f0->setMaximum(v);
}

// Reset the form using original values.
void GetF0ParamsDialog::reset_values()
{
    ui->param_min_f0->setMaximum(_max_f0);
    ui->param_max_f0->setRange(_min_f0, _min_sample_rate / 2);
    ui->param_frame_step->setMinimum(1.0 / _min_sample_rate);
    ui->param_wind_dur->setMinimum(10.0 / _min_sample_rate);

    ui->param_min_f0->setValue(_min_f0);
    ui->param_max_f0->setValue(_max_f0);
    ui->param_wind_dur->setValue(_wind_dur);
    ui->param_frame_step->setValue(_frame_step);
    ui->param_cand_thresh->setValue(_cand_thresh);
    ui->param_double_cost->setValue(_double_cost);
    ui->param_freq_weight->setValue(_freq_weight);
    ui->param_lag_weight->setValue(_lag_weight);
    ui->param_n_cands->setValue(_n_cands);
    ui->param_trans_cost->setValue(_trans_cost);
}

// Save the form values.
void GetF0ParamsDialog::save_values()
{
    _min_f0 = ui->param_min_f0->value();
    _max_f0 = ui->param_max_f0->value();
    _wind_dur = ui->param_wind_dur->value();
    _frame_step = ui->param_frame_step->value();
    _cand_thresh = ui->param_cand_thresh->value();
    _double_cost = ui->param_double_cost->value();
    _freq_weight = ui->param_freq_weight->value();
    _lag_weight = ui->param_lag_weight->value();
    _n_cands = ui->param_n_cands->value();
    _trans_cost = ui->param_trans_cost->value();
}
