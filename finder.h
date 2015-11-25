#ifndef FINDER_H
#define FINDER_H

#include <QObject>
#include <QtXlsx>

class Finder : public QObject
{
    Q_OBJECT
public:
    explicit Finder(QObject *parent = 0);

signals:

public slots:
    void loadFileList();

private:
    QStringList fileList;
};

#endif // FINDER_H
