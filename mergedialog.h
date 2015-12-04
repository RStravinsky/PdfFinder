#ifndef MERGEDIALOG_H
#define MERGEDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QDebug>
#include <QDesktopServices>
#include <QProcess>
#include <QMessageBox>
#include <QDirIterator>

namespace Ui {
class MergeDialog;
}

class MergeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MergeDialog(QWidget *parent = 0, QString savePath = "");
    ~MergeDialog();

private slots:
    void on_buttonName_clicked();
    void on_buttonFiles_clicked();
    void on_buttonFolder_clicked();

private:
    Ui::MergeDialog *ui;
    QString m_savePath{};
    QString m_fileName{};
    QStringList m_mergeList{};
};

#endif // MERGEDIALOG_H
