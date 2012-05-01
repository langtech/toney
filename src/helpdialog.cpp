#include "helpdialog.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog),
    _help_engine(QCoreApplication::applicationDirPath() +
                 QDir::separator() +
                 "toney.qhc")
{
    ui->setupUi(this);
    _help_engine.registeredDocumentations();  // this does a magic
    ui->treeView->setModel(_help_engine.contentModel());
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(show_file(QModelIndex)));
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::show_file(QModelIndex idx)
{
    QUrl url = _help_engine.contentModel()->contentItemAt(idx)->url();
    ui->helpBrowser->setHtml(_help_engine.fileData(url).data());
}
