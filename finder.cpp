#include "finder.h"

Finder::Finder(QObject *parent, QString schedulePath, QString searchedFolder, QString targetFolder, bool isWhite, QString searchCriterion) :
    QObject(parent),
    m_schedulePath(schedulePath),
    m_searchedFolder(searchedFolder),
    m_targetFolder(targetFolder),
    m_isWhite(isWhite),
    m_searchCriterion(searchCriterion)
{
    m_abort = false;
}

void Finder::abort()
{
    m_abort = true;
}

void Finder::setSearchCriterion(QString searchCriterion)
{
    m_searchCriterion = searchCriterion;
}

QSet<QString> Finder::getCopartnerSet()
{
    return m_copartnerSet;
}

void Finder::findFiles()
{
    if(!QDir(m_targetFolder).mkdir("Pliki_PDF")) {
        emit finished(false, "Folder \"Pliki_PDF\" już istnieje.");
        return;
    }

    bool isFileListLoaded = loadFileList();
    if(!isFileListLoaded)
        return;

    if(isFileListLoaded && m_fileList.size() == 0) {
        removeCopiedFiles();
        emit finished(false, "Nie znaleziono pasujących pozycji w harmonogramie.");
        return;
    }

    emit signalProgress( 100, "Określenie liczby plików do przeszukania ...");
    QDir dir(m_searchedFolder, QString("*.pdf"), QDir::NoSort, QDir::Files | QDir::NoSymLinks);
    QDirIterator counterIt(dir, QDirIterator::Subdirectories);
    filesCounter = 0;
    while (counterIt.hasNext()) {
            bool abort = m_abort;
            if (abort) {
                removeCopiedFiles();
                emit finished(false);
                return;
            }
            filesCounter++;
            counterIt.next();
    }
    if(filesCounter == 0) {
        emit finished(false, "Nie znaleziono plików PDF w wybranej lokalizacji.");
        return;
    }

    QStringList indexList;
    QStringList copiedFilesList;
    QString renamedFile;
    int count = 0;
    QDirIterator finalIt(dir, QDirIterator::Subdirectories);
    while (finalIt.hasNext()) {

        bool abort = m_abort;
        if (abort) {
            removeCopiedFiles();
            emit finished(false);
            return;
        }

        if(m_fileList.contains(QFileInfo(finalIt.filePath()).fileName(), Qt::CaseInsensitive)) {

            indexList = getFileListIdx(QFileInfo(finalIt.filePath()).fileName());

            for(int i = 0; i < indexList.size(); ++i) {

                renamedFile = renameFile(indexList.at(i).toInt(), QFileInfo(finalIt.filePath()).fileName());

                if(!QFile(m_targetFolder + "/Pliki_PDF/" + renamedFile).exists()) {
                    QFile::copy(QFileInfo(finalIt.filePath()).filePath(), m_targetFolder + "/Pliki_PDF/" + renamedFile);
                    copiedFilesList.append(renamedFile);
                    emit itemFound(renamedFile, true);
                }
            }
        }

        finalIt.next();
        count++;
        emit signalProgress( int((double(count)/double(filesCounter)*100)),
                         "Przeszukiwanie plików: " + QString::number(count) + "/" +
                         QString::number(filesCounter));
    }

    QStringList missedFiless = checkMissingFiles(copiedFilesList);

    QString information = generateCSV(missedFiless,copiedFilesList);
    emit finished(true,information);
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
    QStringList missingFilesList;
    uint index=0;
    if(!copiedFilesList.isEmpty()) {
        for(auto it = m_fileList.begin(); it != m_fileList.end(); ++it) {
            auto foundIt = std::find_if(copiedFilesList.begin(), copiedFilesList.end(),[&,it](QString name){ if(name.contains(*it)) return true; else return false;});
            if( foundIt == copiedFilesList.end())
                missingFilesList << renameFile(index,*it);
            index++;
        }
    }
    return missingFilesList;
}

bool Finder::loadFileList()
{
    emit signalProgress(100, "Wczytywanie harmonogramu ...");
    QXlsx::Document schedule(m_schedulePath);

    if(!checkSchedule(schedule)) {
        removeCopiedFiles();
        emit finished(false, "Harmonogram niepoprawnie sformatowany.");
        return false;
    }

    // find last row of schedule
    int lastRow = 0;
    if(!rowCount(schedule,lastRow))
        return false;

    if( m_searchCriterion == "Others") {
        if(!m_copartnerSet.isEmpty())
            emit showCopartnerDialog();
        else {
            removeCopiedFiles();
            emit finished(false, "Nie znaleziono kooperantów w harmonogramie.");
            return false;
        }
        if(m_searchCriterion == "Others") {
            removeCopiedFiles();
            emit finished(false, "Anulowano wyszukiwanie.");
            return false;
        }
    }

    QMap<QString,QColor> colorsMap;
    QColor color;
    colorsMap["nocolor"] = color;
    color.setRgbF(1,0.8,0,1);
    colorsMap["orange"] = color;
    color.setRgbF(1,0.752941,0,1);
    colorsMap["orange2"] = color;
    color.setRgbF(1,0.8,0.6,1);
    colorsMap["orange3"] = color;
    color.setRgbF(1,1,0,1);
    colorsMap["yellow"] = color;
    color.setRgbF(1,1,0.4,1);
    colorsMap["yellow2"] = color;

    if(m_isWhite)
    {
        for (int row=7; row<=lastRow; ++row)
        {
            bool abort = m_abort;
            if (abort) {
                emit finished(false);
                return false;
            }

            if (QXlsx::Cell *cell=schedule.cellAt(row, 3))
                findCells(schedule,cell,row,colorsMap);

            emit signalProgress(int((double(row)/double(lastRow)*100))+1, "Tworzenie listy plików ...");
        }
    }

    else
    {
        for (int row=7; row<=lastRow; ++row)
        {
            bool abort = m_abort;
            if (abort) {
                emit finished(false);
                return false;
            }

            QXlsx::Cell *cell=schedule.cellAt(row, 3);
            if (schedule.cellAt(row, 2)->format().patternBackgroundColor().toRgb() != colorsMap["nocolor"])
                findCells(schedule,cell,row,colorsMap);

            emit signalProgress(int((double(row)/double(lastRow)*100))+1, "Tworzenie listy plików ...");
        }
    }

    qDebug() << "Wielkość listy: " << m_fileList.size();
    return true;
}

bool Finder::rowCount(QXlsx::Document &schedule,int & lastRow)
{
    for (int row=7; row<65536; ++row)
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

        if(m_searchCriterion == "Others") {
            if(QXlsx::Cell *cell=schedule.cellAt(row, 10))
                if(!cell->value().toString().isEmpty()){
                    QStringList list = cell->value().toString().split(" ");
                    if(list.back()!="")m_copartnerSet.insert(cell->value().toString().toLower());
                    else m_copartnerSet.insert(cell->value().toString().toLower().replace(" ",""));
                }
        }
    }
    return true;
}


void Finder::findCells(QXlsx::Document &schedule, QXlsx::Cell *cell, int row, QMap<QString, QColor> &colorsMap)
{
    QString currentCellNumber=0;

    if(m_searchCriterion.isEmpty()) {


        if(cell->format().patternBackgroundColor().toRgb() == colorsMap["nocolor"] && !cell->value().toString().isEmpty())
        {
            m_fileList << cell->value().toString().trimmed() + ".pdf";
            currentCellNumber = schedule.cellAt(row, 2)->value().toString();

            if(QXlsx::Cell *nextCell = schedule.cellAt(row+1, 3))
            {
                if(schedule.cellAt(row+1,2)->value().toString().contains(currentCellNumber))
                    m_fileList << cell->value().toString().trimmed() + "_wykaz.pdf";
            }
        }

        if ((cell->format().patternBackgroundColor().toRgb() == colorsMap["orange"] && !cell->value().toString().isEmpty()) ||
            (cell->format().patternBackgroundColor().toRgb() == colorsMap["orange2"] && !cell->value().toString().isEmpty()) ||
            (cell->format().patternBackgroundColor().toRgb() == colorsMap["orange3"] && !cell->value().toString().isEmpty()))
        {
            m_fileList << cell->value().toString().trimmed() + ".pdf";

        }

        if((cell->format().patternBackgroundColor().toRgb() == colorsMap["yellow"] && !cell->value().toString().isEmpty() && schedule.cellAt(row, 10)->value().toString().contains("Sigma", Qt::CaseInsensitive)) ||
          (cell->format().patternBackgroundColor().toRgb() == colorsMap["yellow2"] && !cell->value().toString().isEmpty() && schedule.cellAt(row, 10)->value().toString().contains("Sigma", Qt::CaseInsensitive)))
        {
            m_fileList << cell->value().toString().trimmed() + ".pdf";
        }
    }

    else {

        if(cell->format().patternBackgroundColor().toRgb() == colorsMap["nocolor"] &&
           !cell->value().toString().isEmpty() &&
           schedule.cellAt(row, 10)->value().toString().contains(m_searchCriterion, Qt::CaseInsensitive))
        {
            m_fileList << cell->value().toString().trimmed() + ".pdf";
            currentCellNumber = schedule.cellAt(row, 2)->value().toString();

            if(QXlsx::Cell *nextCell = schedule.cellAt(row+1, 3))
            {
                if(schedule.cellAt(row+1,2)->value().toString().contains(currentCellNumber))
                    m_fileList << cell->value().toString().trimmed() + "_wykaz.pdf";
            }
        }

        if ((cell->format().patternBackgroundColor().toRgb() == colorsMap["orange"] &&
             schedule.cellAt(row, 10)->value().toString().contains(m_searchCriterion, Qt::CaseInsensitive) &&
             !cell->value().toString().isEmpty()) ||
            (cell->format().patternBackgroundColor().toRgb() == colorsMap["orange2"] &&
             schedule.cellAt(row, 10)->value().toString().contains(m_searchCriterion, Qt::CaseInsensitive) &&
             !cell->value().toString().isEmpty()) ||
            (cell->format().patternBackgroundColor().toRgb() == colorsMap["orange3"] &&
             schedule.cellAt(row, 10)->value().toString().contains(m_searchCriterion, Qt::CaseInsensitive) &&
             !cell->value().toString().isEmpty()))
        {
            m_fileList << cell->value().toString().trimmed() + ".pdf";

        }

        if((cell->format().patternBackgroundColor().toRgb() == colorsMap["yellow"] &&
           !cell->value().toString().isEmpty() &&
           schedule.cellAt(row, 10)->value().toString().contains(m_searchCriterion, Qt::CaseInsensitive)) ||
           (cell->format().patternBackgroundColor().toRgb() == colorsMap["yellow2"] &&
           !cell->value().toString().isEmpty() &&
           schedule.cellAt(row, 10)->value().toString().contains(m_searchCriterion, Qt::CaseInsensitive)))
        {
            m_fileList << cell->value().toString().trimmed() + ".pdf";
        }
    }
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
    QDir(m_targetFolder + "/Pliki_PDF").removeRecursively();
}

QString Finder::generateCSV(QStringList & missingFilesList, QStringList & copiedFilesList)
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
        information = "Przeszukiwanie zakończone. Brakujące pozycje znajdują się w pliku:\n"
                      + m_targetFolder + "/" + scheduleName + "_BRAK.csv."
                      + "\nSkopiowano: " + QString::number(m_fileList.size()-missingFilesList.size()) + "/"
                      + QString::number(m_fileList.size()) + " plików.";
    }
    else if (missingFilesList.isEmpty() && !copiedFilesList.isEmpty())
        information = "Przeszukiwanie zakończone.\nSkopiowano wszystkie pliki.";
    else if (missingFilesList.isEmpty() && copiedFilesList.isEmpty()) {
        removeCopiedFiles();
        information = "Przeszukiwanie zakończone.\nNie skopiowano żadnego pliku.";
    }

    return information;
}


