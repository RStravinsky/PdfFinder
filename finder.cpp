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

    if(!isFileListLoaded) return;

    if(isFileListLoaded && m_fileList.size() == 0) {
        emit finished(false, "Nie znaleziono plików w harmonogramie.");
        return;
    }

    if(!QDir(m_targetFolder).mkdir("Pliki_PDF")) {
        emit finished(false, "Folder \"Pliki_PDF\" już istnieje.");
        return;
    }

    QDir dir(m_searchedFolder, QString("*.pdf"), QDir::NoSort, QDir::Files);
    QDirIterator dirIt(dir, QDirIterator::Subdirectories);
    filesCounter = 0;
    while (dirIt.hasNext()) {
            filesCounter++;
            dirIt.next();
    }

    qDebug() << "Ilość plików PDF w lokalizacji: " << filesCounter << endl;

    QStringList copiedFilesList;
    searchFolder(m_searchedFolder,copiedFilesList);

    QStringList missedFiless = checkMissingFiles(copiedFilesList);

    QString information = generateCSV(missedFiless);

    emit finished(true,information);
}

void Finder::searchFolder(QString path, QStringList &copiedFilesList)
{
    QDir dir(path);
    QString renamedFile;
    QStringList indexList;
    static int count = 0;

    foreach (QString file, dir.entryList(QStringList("*.pdf"), QDir::Files)) {


        if(m_fileList.contains(QFileInfo(dir, file).fileName(), Qt::CaseInsensitive)) {

            indexList = getFileListIdx(QFileInfo(dir, file).fileName());

            for(int i = 0; i < indexList.size(); ++i) {
                renamedFile = renameFile(indexList.at(i).toInt(), QFileInfo(dir, file).fileName());
                if(!QFile(m_targetFolder + "/Pliki_PDF/" + renamedFile).exists()) {
                    QFile::copy(QFileInfo(dir, file).filePath(), m_targetFolder + "/Pliki_PDF/" + renamedFile);
                    copiedFilesList.append(renamedFile);
                    emit itemFound(renamedFile, true);
                }
            }
        }
        count++;
        emit signalProgress( int((double(count)/double(filesCounter)*100)),
                         "Przeszukiwanie plików: " + QString::number(count) + "/" +
                         QString::number(filesCounter));

    }

    foreach (QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
            searchFolder(path + QDir::separator() + subDir, copiedFilesList);

    qDebug() << "Ilość przeszukanych PDF'ów: " << count << endl;

}

QStringList Finder::getFileListIdx(QString fileName)
{
    QStringList idxList;
    uint step = 0;
    for(auto it=m_fileList.begin(); it!=m_fileList.end(); ++it) {
        if(*it == fileName)
            idxList << QString::number(step);
        step++;
    }
    return idxList;
}

QStringList Finder::checkMissingFiles(QStringList &copiedFilesList)
{
    qDebug() << copiedFilesList.size();

    QStringList missingFilesList;
    uint index=0;
    if(!copiedFilesList.isEmpty()) {
        for(auto it = m_fileList.begin(); it != m_fileList.end(); ++it) {
            auto foundIt = std::find_if(copiedFilesList.begin(), copiedFilesList.end(),[&,it](QString name){ if(name.contains(*it)) return true;});
            if( foundIt == copiedFilesList.end())
                missingFilesList << renameFile(index,*it);
            index++;
        }
    }

    qDebug() << "makeMissingFiles function: " << missingFilesList << endl;
    return missingFilesList;
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

    qDebug() << "Wielkość listy: " << m_fileList.size();
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

QString Finder::generateCSV(QStringList & missingFilesList)
{
    QString information{};
    if(!missingFilesList.isEmpty()) {

        QString scheduleName = QFileInfo(m_schedulePath).fileName().split(".").at(0);
        QFile file(m_targetFolder + "/" + scheduleName + "_BRAK.csv");
        if (file.open(QFile::WriteOnly|QFile::Truncate)) {
            QTextStream stream(&file);
            for(int i=0; i<missingFilesList.size(); ++i) {
                stream << missingFilesList.at(i) << "\n"; // this writes first line with two columns
                emit itemFound(missingFilesList.at(i), false);
            }
            file.close();
        }
        qDebug() << missingFilesList.size() << endl;
        qDebug() << m_fileList.size() << endl;
        information = "Przeszukiwanie zakończone. Brakujące pozycje znajdują się w pliku:\n"
                      + m_targetFolder + "/" + scheduleName + "_BRAK.csv."
                      + "\nSkopiowano: " + QString::number(m_fileList.size()-missingFilesList.size()) + "/"
                      + QString::number(m_fileList.size()) + " plików.";
    }

    else
        information = "Przeszukiwanie zakończone.\nSkopiowano wszystkie pliki.";

    qDebug() << "INFORMACJA: " << information << endl;
    return information;
}


