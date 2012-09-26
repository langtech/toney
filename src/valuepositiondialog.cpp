#include "valuepositiondialog.h"
#include "ui_valuepositiondialog.h"

ValuePositionDialog::ValuePositionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ValuePositionDialog),
    _pos(0)
{
    ui->setupUi(this);
}

ValuePositionDialog::~ValuePositionDialog()
{
    delete ui;
}

int ValuePositionDialog::getPosition() const
{
    return _pos;
}

void ValuePositionDialog::accept()
{
    _pos = ui->position->value();
    QDialog::accept();
}

void ValuePositionDialog::reject()
{
    ui->position->setValue(_pos);
    QDialog::reject();
}
