#ifndef FINDER_H
#define FINDER_H

#include <QObject>
#include <QtXlsx>
#include <QDir>
#include <QDirIterator>
#include <QTimer>

class Finder : public QObject
{
    Q_OBJECT
public:
    explicit Finder(QString folderPath, QObject *parent = 0);
    void findFiles(const QString schedulePath);
    QStringList m_fileList;

signals:
    void itemFound(QString itemName, bool isFound);
    void signalProgress(int, QString);

private:
    QString m_folder;
    void loadFileList(const QString schedulePath);
};

#endif // FINDER_H
