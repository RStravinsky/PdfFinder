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

void MergeDialog::on_buttonFiles_clicked()
{
    qApp->processEvents();
    if(ui->nameLineEdit->text().isEmpty())
                QMessageBox::information(this,tr("Informacja"),tr("Pole tekstowe jest nieuzupełnione."));

    else {
        m_mergeList = QFileDialog::getOpenFileNames(this, tr("Wybierz pliki"), QString(m_savePath+"/Pliki_PDF"), tr("Pliki PDF (*.pdf)"));

        qDebug() << "File size: " << m_mergeList.size() << endl;

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

            ui->buttonFiles->setIcon(QIcon(":/images/images/wait.png"));
            ui->buttonFiles->setText("");
            ui->progressBar->setValue(100);
            while(!ghostScript->waitForFinished()) {

            }
            delete ghostScript;

            QMessageBox::information(this,tr("Informacja"),tr("Pliki zostały scalone."));
            MergeDialog::accept();
        }
        else
            return;
    }
}

void MergeDialog::on_buttonFolder_clicked()
{
    qApp->processEvents();
    if(ui->nameLineEdit->text().isEmpty())
                QMessageBox::information(this,tr("Informacja"),tr("Pole tekstowe jest nieuzupełnione."));

    else {

        QString folderPath = QFileDialog::getExistingDirectory(this , tr("Wybierz folder"), QString(m_savePath) );
        QDir dir(folderPath, QString("*.pdf"), QDir::NoSort, QDir::Files | QDir::NoSymLinks);
        QDirIterator dirIt(dir, QDirIterator::Subdirectories);
        while (dirIt.hasNext()) {
                dirIt.next();
                m_mergeList.append(QFileInfo(dirIt.filePath()).absoluteFilePath());
        }

        sortPathList(m_mergeList);

        if(m_mergeList.size() != 0) {

            QFile file(folderPath + "/list.lst");
            if (file.open(QFile::WriteOnly|QFile::Truncate)) {
                QTextStream stream(&file);
                for(int i=0; i<m_mergeList.size(); ++i) {
                    stream << "\"" + m_mergeList.at(i) + "\"" << "\n"; // this writes first line with two columns
                }
                file.close();


            QProcess * ghostScript = new QProcess(this);
            ghostScript->start("gswin64 -dNOPAUSE -dEmbedAllFonts=true -sDEVICE=pdfwrite -sOUTPUTFILE="
                               +m_fileName+
                               " -dBATCH @"+folderPath+"/list.lst");

            ui->buttonFolder->setIcon(QIcon(":/images/images/wait.png"));
            ui->buttonFolder->setText("");
            ui->progressBar->setValue(100);
            while(!ghostScript->waitForFinished()) {

            }
            delete ghostScript;

            QMessageBox::information(this,tr("Informacja"),tr("Pliki zostały scalone."));
            MergeDialog::accept();
        }
        else
            return;
    }
   }
}

void MergeDialog::sortPathList(QStringList & list)
{
    qSort(list.begin(), list.end(), [&](QString s1, QString s2){ return (s1.split("/").last().split("_").first().toInt() < s2.split("/").last().split("_").first().toInt()); } );
}
