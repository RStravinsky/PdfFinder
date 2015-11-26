#ifndef FINDER_H
#define FINDER_H

#include <QObject>
#include <QtXlsx>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QTimer>


class Finder : public QObject
{
    Q_OBJECT
public:

    explicit Finder(QString searchedFolder, QString targetFolder, QObject *parent = 0);
    void findFiles(const QString schedulePath);

signals:
    void itemFound(QString itemName, bool isFound);
    void signalProgress(int, QString);

public slots:


private:
    QStringList m_fileList;
    QString m_searchedFolder;
    QString m_targetFolder;
    void loadFileList(const QString schedulePath);
    QString renameFile(int num, QString fileName);

};

#endif // FINDER_H
