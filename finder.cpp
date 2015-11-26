#include "finder.h"

Finder::Finder(QString searchedFolder, QString targetFolder, QObject *parent) :m_searchedFolder(searchedFolder), m_targetFolder(targetFolder), QObject(parent)
{

}


void Finder::findFiles(const QString schedulePath)
{
    loadFileList(schedulePath);
    bool isFound;
    QStringList missingFiles;
    int count{};

    qDebug() << "searched: " << m_searchedFolder << endl;
    qDebug() << "target: " << m_targetFolder << endl;

    for(int i=0; i<m_fileList.size(); ++i)
    {
        isFound = false;
        QDirIterator dirIt(m_searchedFolder, QDirIterator::Subdirectories);
        while (dirIt.hasNext())
        {
            dirIt.next();
            if (QFileInfo(dirIt.filePath()).isFile()) {
                if (QString::compare(QFileInfo(dirIt.filePath()).fileName(), m_fileList.at(i), Qt::CaseInsensitive) == 0) {
                    isFound = true;
                    QFile::copy(QFileInfo(dirIt.filePath()).filePath(), m_targetFolder + "/" + renameFile(i, QFileInfo(dirIt.filePath()).fileName()));
                    emit itemFound(QFileInfo(dirIt.filePath()).fileName(),true);
                    qApp->processEvents();
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
            //qDebug() << i << endl;
            emit signalProgress( int((double(i)/double(m_fileList.size())*100))+1,
                                 "Kopiowanie plików: " + QString::number(count++) + "/" + QString::number(m_fileList.size()));

        }
    }

    QString scheduleName = QFileInfo(schedulePath).fileName().split(".").at(0);
    QMessageBox info;
    info.setWindowIcon(QIcon(":/images/images/logo.png"));
    info.setIcon(QMessageBox::Information);
    info.setWindowTitle("Informacja");


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

        info.setText("Kopiowanie zakończone. Brakujące pozycje znajdują się w pliku:\n");
        info.setInformativeText(m_targetFolder + "/" + scheduleName + "_BRAK.csv");
    }
    else
    {
        info.setText("Kopiowanie zakończone. Wszystkie pliki zostały znalezione.\n");
    }

    info.exec();
}

void Finder::loadFileList(const QString schedulePath)
{

    emit signalProgress(100, "Wczytywanie harmonogramu ...");
    qApp->processEvents();

    QXlsx::Document schedule(schedulePath);
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
    //qApp->processEvents();
    }

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


