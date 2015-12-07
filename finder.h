#ifndef FINDER_H
#define FINDER_H

#include <QObject>
#include <QtXlsx>
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QtAlgorithms>

class Finder : public QObject
{
    Q_OBJECT
public:
    explicit Finder(QObject *parent, QString schedulePath, QString searchedFolder, QString targetFolder, bool isWhite, bool isSigma);
    void abort();

signals:
    void itemFound(QString itemName, bool isFound);
    void signalProgress(int, QString);
    void finished(bool,QString = "");

public slots:
    void findFiles();

private:
    QStringList m_fileList;
    QString m_schedulePath;
    QString m_searchedFolder;
    QString m_targetFolder;
    bool m_isWhite;
    bool m_isSigma;
    bool m_abort;
    int filesCounter;

    bool loadFileList();
    bool rowCount(QXlsx::Document & schedule, int &lastRow);
    void findCells(QXlsx::Document &schedule, QXlsx::Cell *cell, int row, QMap<QString,QColor> & colorsMap);

    bool checkSchedule(QXlsx::Document & schedule);
    QString generateCSV(QStringList &missingFilesList, QStringList &copiedFilesList);
    void removeCopiedFiles();
    QString renameFile(int num, QString fileName);
    QStringList checkMissingFiles(QStringList &copiedFilesList);
    QStringList getFileListIdx(QString fileName);
};

#endif // FINDER_H
