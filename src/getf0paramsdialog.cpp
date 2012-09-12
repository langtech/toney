#include "getf0paramsdialog.h"
#include "ui_getf0paramsdialog.h"
#include <QDebug>

GetF0ParamsDialog::GetF0ParamsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GetF0ParamsDialog),
    _changed(false),
    _min_sample_rate(8000),
    _minf0(60),
    _maxf0(650),
    _windur(0.01),
    _step(0.005)
{
    ui->setupUi(this);

    connect(ui->param_minf0, SIGNAL(editingFinished()), SLOT(minf0_changed()));
    connect(ui->param_maxf0, SIGNAL(editingFinished()), SLOT(maxf0_changed()));

    reset_values();
}

GetF0ParamsDialog::~GetF0ParamsDialog()
{
    delete ui;
}

get_f0_session *GetF0ParamsDialog::newGetF0Session()
{
    get_f0_session *session = init_get_f0();
    session->par->min_f0 = _minf0;
    session->par->max_f0 = _maxf0;
    session->par->wind_dur = _windur;
    session->par->frame_step = _step;
    return session;
}

bool GetF0ParamsDialog::paramsChanged() const
{
    return _changed;
}

void GetF0ParamsDialog::accept()
{
    int minf0 = _minf0;
    int maxf0 = _maxf0;
    double windur = _windur;
    double step = _step;

    _minf0 = ui->param_minf0->value();
    _maxf0 = ui->param_maxf0->value();
    _windur = ui->param_windur->value();
    _step = ui->param_step->value();
    reset_values();

    _changed = minf0 != _minf0 ||
            maxf0 != _maxf0 ||
            windur != _windur ||
            step != _step;

    QDialog::accept();
}

void GetF0ParamsDialog::reject()
{
    reset_values();
    QDialog::reject();
}

void GetF0ParamsDialog::minf0_changed()
{
    int v = ui->param_minf0->value();
    ui->param_maxf0->setMinimum(v);
}

void GetF0ParamsDialog::maxf0_changed()
{
    int v = ui->param_maxf0->value();
    ui->param_minf0->setMaximum(v);
}

void GetF0ParamsDialog::reset_values()
{
    ui->param_minf0->setMinimum(0);
    ui->param_minf0->setMaximum(_maxf0);
    ui->param_maxf0->setMinimum(_minf0);
    ui->param_maxf0->setMaximum(_min_sample_rate / 2);

    _minf0 = ui->param_minf0->value();
    _maxf0 = ui->param_maxf0->value();

    ui->param_minf0->setValue(_minf0);
    ui->param_maxf0->setValue(_maxf0);
    ui->param_windur->setValue(_windur);
    ui->param_step->setValue(_step);

}
