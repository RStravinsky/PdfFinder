#include "helpdialog.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    ui->label_2->setTextFormat(Qt::RichText);
    ui->label_2->setOpenExternalLinks(true);
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::on_pushButton_clicked()
{
    this->close();
}
