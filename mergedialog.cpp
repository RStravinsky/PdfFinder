#include "mergedialog.h"
#include "ui_mergedialog.h"

MergeDialog::MergeDialog(QWidget *parent, QString savePath) :
    QDialog(parent),
    m_savePath(savePath),
    ui(new Ui::MergeDialog)
{
    ui->setupUi(this);
}

MergeDialog::~MergeDialog()
{
    delete ui;
}

void MergeDialog::on_buttonName_clicked()
{
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                               tr("Pliki PDF (*.pdf)"));
    QStringList namePathList = fileName.split("/");
    ui->nameLineEdit->setText(namePathList.at(namePathList.size()-1));
}

void MergeDialog::on_buttonPath_clicked()
{
    if(m_savePath.isEmpty())
        m_savePath = QDir::homePath();
    const QString path = QFileDialog::getExistingDirectory(this , tr("Wybierz folder"), m_savePath );
    ui->pathLineEdit->setText(path);
    ui->pathLineEdit->setCursorPosition(0);
}

void MergeDialog::on_buttonOk_clicked()
{
    if(ui->nameLineEdit->text().isEmpty() || ui->pathLineEdit->text().isEmpty())
                QMessageBox::information(this,tr("Informacja"),tr("Pola tekstowe są nieuzupełnione."));

    else {
        m_mergeList = QFileDialog::getOpenFileNames(this, tr("Wybierz folder"), QString(m_savePath), tr("Pliki PDF (*.pdf)"));
        if(m_mergeList.size() != 0) {
            QString filesToMerge;
            for (QStringList::iterator it = m_mergeList.begin(); it != m_mergeList.end(); ++it) {
                   QString current = *it;
                   filesToMerge += current + " ";
               }

            QProcess * ghostScript = new QProcess(this);
            ghostScript->start("gswin64 -dNOPAUSE -sDEVICE=pdfwrite -sOUTPUTFILE="
                               +ui->pathLineEdit->text()+"/"
                               +ui->nameLineEdit->text()+
                               " -dBATCH "+filesToMerge+"");
            if(ghostScript->waitForFinished())
                delete ghostScript;

            QMessageBox::information(this,tr("Informacja"),tr("Pliki zostały scalone."));
            MergeDialog::accept();
        }
        else
            return;
    }
}
