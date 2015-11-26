#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "finder.h"
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <settingsdialog.h>
#include <QMediaPlayer>

#include <stddef.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
      void mainButtonReleased(const QPushButton * mainButton);

private slots:
    void on_inputButton_released();
    void on_outputButton_released();
    void on_mainButtonReleased(const QPushButton * mainButton);
    void on_searchButton_clicked();

private:
    Ui::MainWindow *ui;

    QMediaPlayer * player{nullptr};
    SettingsDialog * settingsDialog{nullptr};

    enum readType
    {
        INIT = 0,
        INPUT = 1,
        OUTPUT = 2
    };

    bool eventFilter(QObject *obj, QEvent *event);
    QString getInputPath();
    QString getOutputPath();
    QString getSchedulePath();

    QString getPathFromFile(readType type);
    void savePathToFile();
    void fillPaths();


};

#endif // MAINWINDOW_H
