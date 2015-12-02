#ifndef FINDER_H
#define FINDER_H

#include <QObject>
#include <QtXlsx>
#include <QDir>
#include <QDirIterator>
#include <QThread>

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
    bool loadFileList();
    bool checkSchedule(QXlsx::Document & schedule);
    QString generateCSV(QStringList & missingFiles);
    void removeCopiedFiles(QStringList & copiedFiles);
    QString renameFile(int num, QString fileName);

    QStringList m_fileList;
    QString m_schedulePath;
    QString m_searchedFolder;
    QString m_targetFolder;
    bool m_abort;
    QThread * scheduleThread;
};

#endif // FINDER_H
