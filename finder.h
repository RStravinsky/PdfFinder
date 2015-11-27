#ifndef FINDER_H
#define FINDER_H

#include <QObject>
#include <QtXlsx>
#include <QDir>
#include <QDirIterator>
#include <QTimer>
#include <QThread>

class Finder : public QObject
{
    Q_OBJECT
public:
    explicit Finder(QString folderPath, QString schedulePath, QObject *parent = 0);
    QStringList m_fileList;
    void requestWork();
    void abort();

signals:
    void itemFound(QString itemName, bool isFound);
    void signalProgress(int, QString);
    void finished(bool);
    void workRequested();

public slots:
    void findFiles();

private:
    bool loadFileList();
    QString m_folder;
    QString m_schedulePath;
    QMutex mutex;
    bool m_abort;
    bool m_working;
};

#endif // FINDER_H
