#ifndef FINDER_H
#define FINDER_H

#include <QObject>
#include <QtXlsx>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>

class Finder : public QObject
{
    Q_OBJECT
public:
    explicit Finder(QString searchedFolder, QString targetFolder, QObject *parent = 0);
    void findFiles(QString schedulePath);

signals:
    void itemFound(QString itemName, bool isFound);

public slots:


private:
    QStringList m_fileList;
    QString m_searchedFolder;
    QString m_targetFolder;
    void loadFileList(QString schedulePath);
    QString renameFile(int num, QString fileName);
};

#endif // FINDER_H
