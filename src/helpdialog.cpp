#include "helpdialog.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog),
    _help_engine(QDir(QCoreApplication::applicationDirPath())
                 .absoluteFilePath("toney.qhc"))
{
    ui->setupUi(this);
    _help_engine.registeredDocumentations();  // this does a magic
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,3);
    ui->treeView->setModel(_help_engine.contentModel());
    ui->helpBrowser->setHelpEngine(&_help_engine);
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
    ui->helpBrowser->setSource(url);
}
