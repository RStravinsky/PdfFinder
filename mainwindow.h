#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QDebug>

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
      void mainButtonReleased( const QPushButton * button );

private slots:
    void on_inputButton_released();
    void on_outputButton_released();
    void on_mainButtonReleased( const QPushButton * mainButton );
    void on_searchButton_clicked();

private:
    Ui::MainWindow *ui;
    QString schedulePath{};

    bool eventFilter(QObject *obj, QEvent *event);
    QString setPath();
    QString getSchedulePath();
};

#endif // MAINWINDOW_H
