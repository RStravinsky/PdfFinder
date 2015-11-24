#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->addButton->installEventFilter(this);
    ui->settingsButton->installEventFilter(this);
    ui->helpButton->installEventFilter(this);
    ui->exitButton->installEventFilter(this);

    QObject::connect(this,SIGNAL(mainButtonReleased(const QPushButton*)),this,SLOT(on_mainButtonReleased(const QPushButton*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    QPushButton * actualButton = qobject_cast<QPushButton *>(obj);

    if (event->type() == QEvent::HoverEnter) {
        actualButton->setIconSize(QSize(52, 52));
        return true;
    }
    else if (event->type() == QEvent::HoverLeave) {
        actualButton->setIconSize(QSize(48, 48));
        return true;
    }

    else if (event->type() == QEvent::MouseButtonRelease) {
        actualButton->setIconSize(QSize(52, 52));
        emit mainButtonReleased(actualButton);
        return true;
    }

    else if (event->type() == QEvent::MouseButtonPress) {
        actualButton->setIconSize(QSize(30, 30));
        return true;
    }

    return false;
}

QString MainWindow::setPath()
{
    QString path = "";
    path = QFileDialog::getExistingDirectory( 0 , tr("Wybierz folder"), QDir::homePath() );
    return path;
}

QString MainWindow::getSchedulePath()
{
    QString schedulePath = "";
    schedulePath = QFileDialog::getOpenFileName(this, tr("Wybierz folder"), QDir::homePath(), tr("Pliki XLSX (*.xlsx)"));
    return schedulePath;
}

void MainWindow::on_inputButton_released()
{
    QString inputPath = setPath();
    if ( !inputPath.isEmpty() )
        ui->inputLineEdit->setText(inputPath);
}

void MainWindow::on_outputButton_released()
{
    QString outputPath = setPath();
    if ( !outputPath.isEmpty() )
        ui->outputLineEdit->setText(outputPath);
}

void MainWindow::on_mainButtonReleased(const QPushButton *button)
{
    if( button == ui->addButton )
    {
        QString schedulePath = getSchedulePath();
        if ( !schedulePath.isEmpty() ) {
            QStringList pathList  = schedulePath.split("/");
            ui->listLabel->setText("Lista plików dla harmonogramu: " + pathList.at(pathList.size()-1)  );
        }
    }
}


