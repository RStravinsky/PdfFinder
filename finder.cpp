#include "finder.h"

Finder::Finder(QString folderPath, QObject *parent) : QObject(parent)
{
    m_folder = folderPath;
}

void Finder::findFiles()
{
    QDirIterator dirIt(m_folder, QDirIterator::Subdirectories);

    for(int i=0; i<m_fileList.count(); ++i)
    {
        while (dirIt.hasNext())
        {
            dirIt.next();
            if (QFileInfo(dirIt.filePath()).isFile())
                if (QFileInfo(dirIt.filePath()).fileName() == m_fileList.at(i))
                    qDebug()<<dirIt.filePath();
        }
    }
}

void Finder::loadFileList(QString schedulePath)
{
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
                        m_fileList << cell->value().toString() + ".pdf";
                        m_fileList << cell->value().toString() + "_WYKAZ.pdf";
                    }
                }
            }

            if ((cell->format().patternBackgroundColor().toRgb() == orange && !cell->value().toString().isEmpty()) ||
                (cell->format().patternBackgroundColor().toRgb() == orange2 && !cell->value().toString().isEmpty()))
            {
                m_fileList << cell->value().toString() + ".pdf";
            }

            if(cell->format().patternBackgroundColor().toRgb() == yellow &&
               !cell->value().toString().isEmpty() &&
               schedule.cellAt(row, 10)->value().toString().contains("Sigma", Qt::CaseInsensitive))
            {
                m_fileList << cell->value().toString() + ".pdf "  + schedule.cellAt(row, 4)->value().toString();
            }
        }
    }

   for(int i=0; i<m_fileList.count(); ++i)
   qDebug() << m_fileList.at(i);
}
