#include "mergedialog.h"
#include "ui_mergedialog.h"

MergeDialog::MergeDialog(QWidget *parent, QString savePath) :
    QDialog(parent),
    m_savePath(savePath),
    ui(new Ui::MergeDialog)
{
    ui->setupUi(this);
    if(m_savePath.isEmpty())
        m_savePath = QDir::homePath();
    else
        m_savePath = m_savePath+"/Pliki_PDF";
}

MergeDialog::~MergeDialog()
{
    delete ui;
}

void MergeDialog::on_buttonName_clicked()
{
    m_fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                               tr("Pliki PDF (*.pdf)"));
    QStringList namePathList = m_fileName.split("/");
    ui->nameLineEdit->setText(namePathList.at(namePathList.size()-1));
}

void MergeDialog::on_buttonOk_clicked()
{
    qApp->processEvents();
    if(ui->nameLineEdit->text().isEmpty())
                QMessageBox::information(this,tr("Informacja"),tr("Pole tekstowe jest nieuzupełnione."));

    else {
        m_mergeList = QFileDialog::getOpenFileNames(this, tr("Wybierz pliki"), QString(m_savePath), tr("Pliki PDF (*.pdf)"));

        if(m_mergeList.size() != 0) {


            QString filesToMerge;
            for (QStringList::iterator it = m_mergeList.begin(); it != m_mergeList.end(); ++it) {
                   QString current = *it;
                   filesToMerge += "\"" + current + "\" ";
               }

            QProcess * ghostScript = new QProcess(this);
            ghostScript->start("gswin64c -dNOPAUSE -sDEVICE=pdfwrite -sOUTPUTFILE="
                               +m_fileName+
                               " -dBATCH "+filesToMerge+"");

            ui->buttonOk->setText("Proszę czekać");
            ui->buttonOk->setIcon(QIcon(":/images/images/wait.png"));
            ui->progressBar->setValue(100);
            while(ghostScript->waitForFinished()) {
                delete ghostScript;
            }

            QMessageBox::information(this,tr("Informacja"),tr("Pliki zostały scalone."));
            MergeDialog::accept();
        }
        else
            return;
    }
}


