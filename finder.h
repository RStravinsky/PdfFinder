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
    explicit Finder(QObject *parent, QString schedulePath, QString searchedFolder, QString targetFolder);
    void abort();

signals:
    void itemFound(QString itemName, bool isFound);
    void signalProgress(int, QString);
    void finished(bool,QString = "");

public slots:
    void findFiles();

private:
    int filesCounter;
    bool loadFileList();
    bool checkSchedule(QXlsx::Document & schedule);
    QString generateCSV(QStringList &missingFilesList);
    void removeCopiedFiles();
    QString renameFile(int num, QString fileName);

    QStringList m_fileList;
    QString m_schedulePath;
    QString m_searchedFolder;
    QString m_targetFolder;
    bool m_abort;
    QThread * scheduleThread;

    QStringList checkMissingFiles(QStringList &copiedFilesList);
    void searchFolder(QString path, QStringList &copiedFilesList);
    QStringList getFileListIdx(QString fileName);
};

#endif // FINDER_H
