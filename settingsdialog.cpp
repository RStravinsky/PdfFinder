#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent, bool _isTurnOn) :
    QDialog(parent),isTurnOn(_isTurnOn),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    if(isTurnOn) ui->comboBox->setCurrentIndex(0);
    else ui->comboBox->setCurrentIndex(1);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_okButton_clicked()
{
    //qDebug() << "on_okButton_clicked " << isTurnOn << endl;
    SettingsDialog::accept();
}

void SettingsDialog::on_cancelButton_clicked()
{
    //qDebug() << "on_cancelButton_clicked " << isTurnOn << endl;
    //qDebug() << "on_cancelButton_clicked " << previousState << endl;
    isRejected = true;
    isTurnOn = previousState;
    if(isTurnOn) ui->comboBox->setCurrentIndex(0);
    else ui->comboBox->setCurrentIndex(1);
    SettingsDialog::reject();
}

void SettingsDialog::on_comboBox_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(!isRejected) {
        isTurnOn = !isTurnOn;
        qDebug() << "on_comboBox_currentTextChanged " << isTurnOn << endl;
    }
    isRejected = false;
}



