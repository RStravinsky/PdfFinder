#include "finder.h"

Finder::Finder(QObject *parent, QString schedulePath, QString searchedFolder, QString targetFolder) :
    QObject(parent),
    m_schedulePath(schedulePath),
    m_searchedFolder(searchedFolder),
    m_targetFolder(targetFolder)
{
    m_abort = false;
}

void Finder::abort()
{
    m_abort = true;
}

void Finder::findFiles()
{
    bool isFileListLoaded = loadFileList();

    if(!isFileListLoaded)return;

    if(isFileListLoaded && m_fileList.size() == 0) {
        emit finished(false, "Nie znaleziono plików w harmonogramie.");
        return;
    }

    bool isFound;
    QStringList missingFiles;
    QStringList copiedFiles;
    QString renamedFile;

    if(!QDir(m_targetFolder).mkdir("Pliki_PDF")) {
        emit finished(false, "Nie można utworzyć folderu na pliki do skopiowania.");
        return;
    }

    QDir dir(m_searchedFolder, QString("*.pdf"), QDir::NoSort, QDir::Files);
    QDirIterator dirIt(dir, QDirIterator::Subdirectories);
    filesCounter = 0;
    while (dirIt.hasNext()) {
            filesCounter++;
            dirIt.next();
    }

    QStringList copiedFilesAmount;
    if(searchFolder(m_searchedFolder,copiedFilesAmount).isEmpty())
        return;

    QString information = generateCSV(copiedFilesAmount);
    emit finished(true,information);
}

QStringList Finder::searchFolder(QString path, QStringList &copiedFilesAmount)
{
    QDir dir(path);
    QStringList filesList;
    QString renamedFile;
    QStringList indexList;
    int count = 0;

    foreach (QString file, dir.entryList(QStringList("*.pdf"), QDir::Files)) {

        bool abort = m_abort;
        if (abort) {
            removeCopiedFiles();
            emit finished(false);
            return QStringList();
        }

        if(m_fileList.contains(QFileInfo(dir, file).fileName(), Qt::CaseInsensitive)) {

        indexList = getFileListIdx(QFileInfo(dir, file).fileName());

            for(int i = 0; i < indexList.size(); ++i) {
                renamedFile = renameFile(indexList.at(i).toInt(), QFileInfo(dir, file).fileName());
                QFile::copy(QFileInfo(dir, file).filePath(), m_targetFolder + "/Pliki_PDF/" + renamedFile);
                emit itemFound(QFileInfo(dir, file).fileName(), true);
                emit signalProgress( int((double(count+1)/double(filesCounter)*100))+1,
                                 "Przeszukiwanie plików: " + QString::number(count+1) + "/" +
                                 QString::number(filesCounter));
                copiedFilesAmount.append(QFileInfo(dir, file).fileName());
            }
        }

        count++;
    }

    foreach (QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
            filesList << searchFolder(path + QDir::separator() + subDir, copiedFilesAmount);

    return filesList;

}

QStringList Finder::getFileListIdx(QString fileName)
{
    QStringList idxList;

    for(int i = 0; i < m_fileList.size(); ++i) {
        if(m_fileList.at(i) == fileName)
            idxList << QString::number(i);
    }

    return idxList;
}

bool Finder::loadFileList()
{
    emit signalProgress(100, "Wczytywanie harmonogramu ...");

    QXlsx::Document schedule(m_schedulePath);

    if(!checkSchedule(schedule)) {
        emit finished(false, "Harmonogram niepoprawny.");
        return false;
    }

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
            emit finished(false);
            return false;
        }

        if (QXlsx::Cell *cell=schedule.cellAt(row, 3))
        {
            if(cell->format().patternBackgroundColor().toRgb() == nocolor && !cell->value().toString().isEmpty())
            {
                m_fileList << cell->value().toString().trimmed() + ".pdf";
                //qDebug() << schedule.cellAt(row, 2)->value().toString()  << m_fileList.back();

                currentCellNumber = schedule.cellAt(row, 2)->value().toString();

                if(QXlsx::Cell *nextCell = schedule.cellAt(row+1, 3))
                {
                    if(schedule.cellAt(row+1,2)->value().toString().contains(currentCellNumber))
                    {

                        m_fileList << cell->value().toString().trimmed() + "_wykaz.pdf";
                        //qDebug() << schedule.cellAt(row, 2)->value().toString()  << m_fileList.back();
                    }
                }
            }

            if ((cell->format().patternBackgroundColor().toRgb() == orange && !cell->value().toString().isEmpty()) ||
                (cell->format().patternBackgroundColor().toRgb() == orange2 && !cell->value().toString().isEmpty()))
            {
                m_fileList << cell->value().toString().trimmed() + ".pdf";
                //qDebug() << schedule.cellAt(row, 2)->value().toString()  << m_fileList.back();
            }

            if(cell->format().patternBackgroundColor().toRgb() == yellow &&
               !cell->value().toString().isEmpty() &&
               schedule.cellAt(row, 10)->value().toString().contains("Sigma", Qt::CaseInsensitive))
            {
                m_fileList << cell->value().toString().trimmed() + ".pdf";
                //qDebug() << schedule.cellAt(row, 2)->value().toString() << m_fileList.back();
            }
        }
        emit signalProgress(int((double(row)/double(lastRow)*100))+1, "Tworzenie listy plików ...");
    }


    return true;
}

bool Finder::checkSchedule(QXlsx::Document &schedule)
{
    QString orderDigit = schedule.cellAt(6, 2)->value().toString();
    QString drawingNr = schedule.cellAt(6, 3)->value().toString();
    QString cooperator = schedule.cellAt(6, 10)->value().toString();

    if( orderDigit.contains("L.p.") && drawingNr.contains("Nr rys.") && cooperator.contains("Kooperant") )
        return true;
    else
        return false;
}



QString Finder::renameFile(int num, QString fileName)
{
    QString resultName{""};

    if(num < 10)
        resultName = QString::number(0) + QString::number(num) + QString("_") + fileName;
    else
        resultName = QString::number(num) + QString("_") + fileName;

    return resultName;
}


void Finder::removeCopiedFiles()
{

    emit signalProgress(100, "Usuwanie plików ...");
    QDir(m_targetFolder + "/Pliki_PDF").removeRecursively();
}

QString Finder::generateCSV(QStringList copiedFilesAmount)
{
    QString scheduleName = QFileInfo(m_schedulePath).fileName().split(".").at(0);
    QString information{""};

    if(copiedFilesAmount.size() != 0)
    {
        QFile file(m_targetFolder + "/" + scheduleName + "_BRAK.csv");
        if (file.open(QFile::WriteOnly|QFile::Truncate))
        {
        QTextStream stream(&file);
        for(int i=0; i<copiedFilesAmount.size(); ++i)
            stream << copiedFilesAmount.at(i) << "\n"; // this writes first line with two columns
        file.close();
        }

        information = "Przeszukiwanie zakończone. Brakujące pozycje znajdują się w pliku:\n"
                      + m_targetFolder + "/" + scheduleName + "_BRAK.csv."
                      + "\nSkopiowano: " + QString::number(copiedFilesAmount.size()) + "/"
                      + QString::number(m_fileList.size()) + " plików.";
    }
    else
    {
        information = "Przeszukiwanie zakończone.\nSkopiowano: " + QString::number(copiedFilesAmount.size()) + " plików.";
    }

    return information;
}


