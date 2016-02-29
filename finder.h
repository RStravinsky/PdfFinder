#ifndef FINDER_H
#define FINDER_H

#include <QObject>
#include <QtXlsx>
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QtAlgorithms>
#include <QSet>
#include <QMetaType>
#include <QString>

class Finder : public QObject
{
    Q_OBJECT

public:
    explicit Finder(QObject *parent, QString schedulePath, QString searchedFolder, QString targetFolder, bool isWhite, QString searchCriterion);
    void abort();
    void setSearchCriterion(QString searchCriterion);
    QSet<QString> getCopartnerSet();

signals:
    void itemFound(QString itemName, bool isFound);
    void signalProgress(int, QString);
    void finished(bool,QString = "");
    void showCopartnerDialog();

public slots:
    void findFiles();

private:
    QStringList m_fileList;
    QString m_schedulePath;
    QString m_searchedFolder;
    QString m_targetFolder;
    bool m_abort;
    bool m_isWhite;
    QString m_searchCriterion;
    int filesCounter;
    QSet<QString> m_copartnerSet;

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
