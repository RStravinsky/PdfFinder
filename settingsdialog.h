#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <settingsdialog.h>
#include <QDebug>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0, bool _isTurnOn = true);
    ~SettingsDialog();
     bool isTurnOn;
     bool previousState;

private slots:    
    void on_okButton_clicked();
    void on_cancelButton_clicked();
    void on_comboBox_currentTextChanged(const QString &arg1);

private:
    Ui::SettingsDialog *ui;
    bool isRejected{false};
};

#endif // SETTINGSDIALOG_H
