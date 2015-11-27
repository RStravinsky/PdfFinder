#include "finder.h"

Finder::Finder(QObject *parent, QString schedulePath, QString searchedFolder, QString targetFolder) :
    QObject(parent),
    m_schedulePath(schedulePath),
    m_searchedFolder(searchedFolder),
    m_targetFolder(targetFolder)
{
    m_working = false;
    m_abort = false;
}

void Finder::abort()
{
    if (m_working) {
        m_abort = true;
        qDebug()<<"Request worker aborting in Thread "<<thread()->currentThreadId();
    }
}

void Finder::requestWork()
{
    m_working = true;
    m_abort = false;
    emit workRequested();
}

void Finder::findFiles()
{
    if(!loadFileList())return;
    bool isFound;
    QStringList missingFiles;
    QStringList copiedFiles;
    int count{};

    for(int i=0; i<m_fileList.size(); ++i)
    {
        isFound = false;
        QDirIterator dirIt(m_searchedFolder, QDirIterator::Subdirectories);
        while (dirIt.hasNext())
        {

            bool abort = m_abort;
            if (abort) {
                removeCopiedFiles(copiedFiles);
                m_working = false;
                emit finished(false);
                return;
            }

            dirIt.next();
            if (QFileInfo(dirIt.filePath()).isFile()) {
                if (QString::compare(QFileInfo(dirIt.filePath()).fileName(), m_fileList.at(i), Qt::CaseInsensitive) == 0) {
                    isFound = true;
                    QString renamedFile = renameFile(i, QFileInfo(dirIt.filePath()).fileName());
                    QFile::copy(QFileInfo(dirIt.filePath()).filePath(), m_targetFolder + "/" + renamedFile);
                    copiedFiles << renamedFile;
                    emit itemFound(QFileInfo(dirIt.filePath()).fileName(),true);
                    break;
                }
            }
        }

        if(!isFound)
        {
            emit itemFound(m_fileList.at(i),false);
            missingFiles << m_fileList.at(i);
        }
        else {
            emit signalProgress( int((double(i)/double(m_fileList.size())*100))+1,
                                 "Kopiowanie plików: " + QString::number(count++) + "/" + QString::number(m_fileList.size()));

        }
    }

    QString information = generateCSV(missingFiles);

    m_working = false;
    emit finished(true,information);
}

bool Finder::loadFileList()
{
    emit signalProgress(100, "Wczytywanie harmonogramu ...");

    QXlsx::Document schedule(m_schedulePath);
    int lastRow = 0;
    QString currentCellNumber;

    //COLORS
    QColor nocolor;
    QColor orange; orange.setRgbF(1, 0.8, 0, 1);
    QColor orange2; orange2.setRgbF(1, 0.752941, 0, 1);
    QColor yellow; yellow.setRgbF(1, 1, 0, 1);
    QColor red; red.setRgbF(1, 0, 0, 1);

    for (int row=7; row<65000; ++row)
    {
        bool abort = m_abort;
        if (abort) {
            m_working = false;
            emit finished(false);
            return false;
        }

        if(QXlsx::Cell *cell=schedule.cellAt(row, 6))
        {
            if(cell->value() == QVariant("Masa"))
            {
                lastRow = row - 2;
                break;
            }
        }
    }

    for (int row=7; row<=lastRow; ++row)
    {
        bool abort = m_abort;
        if (abort) {
            m_working = false;
            emit finished(false);
            return false;
        }

        if (QXlsx::Cell *cell=schedule.cellAt(row, 3))
        {
            if(cell->format().patternBackgroundColor().toRgb() == nocolor && !cell->value().toString().isEmpty())
            {
                currentCellNumber = schedule.cellAt(row, 2)->value().toString();

                if(QXlsx::Cell *nextCell = schedule.cellAt(row+1, 3))
                {
                    if((!nextCell->value().toString().isEmpty()) && schedule.cellAt(row+1,2)->value().toString().contains(currentCellNumber))
                    {
                        m_fileList << cell->value().toString().trimmed() + ".pdf";
                        m_fileList << cell->value().toString().trimmed() + "_wykaz.pdf";
                    }
                }
            }

            if ((cell->format().patternBackgroundColor().toRgb() == orange && !cell->value().toString().isEmpty()) ||
                (cell->format().patternBackgroundColor().toRgb() == orange2 && !cell->value().toString().isEmpty()))
            {
                m_fileList << cell->value().toString().trimmed() + ".pdf";
            }

            if(cell->format().patternBackgroundColor().toRgb() == yellow &&
               !cell->value().toString().isEmpty() &&
               schedule.cellAt(row, 10)->value().toString().contains("Sigma", Qt::CaseInsensitive))
            {
                m_fileList << cell->value().toString().trimmed() + ".pdf";
            }
        }
        emit signalProgress(int((double(row)/double(lastRow)*100))+1, "Tworzenie listy plików ...");
    }

    return true;
}

QString Finder::renameFile(int num, QString fileName)
{
    QString resultName;

    if(num < 10)
        resultName = QString::number(0) + QString::number(num) + QString("_") + fileName;
    else
        resultName = QString::number(num) + QString("_") + fileName;

    return resultName;
}

void Finder::removeCopiedFiles(QStringList &copiedFiles)
{
    uint count{};
    for(auto & fileToRemove: copiedFiles) {
        QFile file(m_targetFolder + "/" + fileToRemove);
        file.remove();
        emit signalProgress( int((double(count)/double(copiedFiles.size())*100))+1,
                             "Usuwanie plików: " + QString::number(count++) + "/" + QString::number(copiedFiles.size()));
    }
}

QString Finder::generateCSV(QStringList &missingFiles)
{
    QString scheduleName = QFileInfo(m_schedulePath).fileName().split(".").at(0);
    QString information;

    if(missingFiles.size() != 0)
    {
        QFile file(m_targetFolder + "/" + scheduleName + "_BRAK.csv");
        if (file.open(QFile::WriteOnly|QFile::Truncate))
        {
        QTextStream stream(&file);
        for(int i=0; i<missingFiles.size(); ++i)
            stream << missingFiles.at(i) << "\n"; // this writes first line with two columns
        file.close();
        }

        information = "Kopiowanie zakończone. Brakujące pozycje znajdują się w pliku:\n" + m_targetFolder + "/" + scheduleName + "_BRAK.csv";
    }
    else
    {
        information = "Kopiowanie zakończone. Wszystkie pliki zostały znalezione.\n";
    }

    return information;
}


