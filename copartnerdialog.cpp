#include "copartnerdialog.h"
#include "ui_copartnerdialog.h"

CopartnerDialog::CopartnerDialog(QSet<QString> copartnerSet, QDialog *parent):
    QDialog(parent),
    ui(new Ui::CopartnerDialog)
{
    ui->setupUi(this);
    QList<QString> copartnerList = copartnerSet.toList();
    qSort(copartnerList.begin(),copartnerList.end(),qLess<QString>());
    ui->comboBox->addItems(copartnerList);

}

CopartnerDialog::~CopartnerDialog()
{
    delete ui;
}

void CopartnerDialog::on_okButton_clicked()
{
    copartner = ui->comboBox->currentText();
    CopartnerDialog::accept();
}

void CopartnerDialog::on_cancelButton_clicked()
{
    CopartnerDialog::reject();
}
