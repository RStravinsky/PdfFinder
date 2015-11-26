#ifndef FINDER_H
#define FINDER_H

#include <QObject>
#include <QtXlsx>
#include <QDir>
#include <QDirIterator>

class Finder : public QObject
{
    Q_OBJECT
public:
    explicit Finder(QString folderPath, QObject *parent = 0);
    void findFiles();

signals:

public slots:
    void loadFileList(QString schedulePath);

private:
    QStringList m_fileList;
    QString m_folder;
};

#endif // FINDER_H
