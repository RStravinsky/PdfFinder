#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QMediaPlayer>
#include <QThread>
#include <QSet>

#include <stddef.h>
#include <finder.h>
#include <settingsdialog.h>
#include <mergedialog.h>
#include <helpdialog.h>
#include <copartnerdialog.h>

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
    void on_itemFound(QString itemName, bool isFound);
    void on_setValue(int value, QString labelText);
    void on_processingFinished(bool isSuccess, QString information);  
    void on_sigmaButton_clicked();
    void on_othersButton_clicked();
    void on_blueButton_clicked();
    void on_whiteButton_clicked();
    void on_allButton_clicked();
    void on_showCopartnerDialog();

private:
    Ui::MainWindow *ui;
    QThread * finderThread{nullptr};
    QThread * listWidgetThread{nullptr};
    QMediaPlayer * player{nullptr};
    SettingsDialog * settingsDialog{nullptr};
    Finder * finder{nullptr};
    QString schedulePath{};
    bool processing{false};
    bool isWhite{true};
    QString searchCriterion{"Sigma"};
    void sortListWidget();

    enum readType
    {
        INIT = 0,
        INPUT = 1,
        OUTPUT = 2
    };

    QString getInputPath();
    QString getOutputPath();
    QString getSchedulePath();
    QString getPathFromFile(readType type);
    void savePathToFile();
    void fillPaths();

    void setEnabled(bool isEnabled);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MAINWINDOW_H
