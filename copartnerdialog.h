#ifndef COPARTNERDIALOG_H
#define COPARTNERDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QSet>
#include <QtAlgorithms>

namespace Ui {
class CopartnerDialog;
}

class CopartnerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CopartnerDialog(QSet<QString> copartnerSet, QDialog *parent = 0);
    ~CopartnerDialog();
    QString copartner;

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::CopartnerDialog *ui;
};

#endif // COPARTNERDIALOG_H
