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
    ui->mergeButton->installEventFilter(this);

    settingsDialog = new SettingsDialog(this,true);
    player = new QMediaPlayer(this);

    QObject::connect(this,SIGNAL(mainButtonReleased(const QPushButton*)),this,SLOT(on_mainButtonReleased(const QPushButton*)));
    fillPaths();

    ui->outputLineEdit->setCursorPosition(0);
    ui->inputLineEdit->setCursorPosition(0);
}

MainWindow::~MainWindow()
{
    if(finder!=nullptr) {
        finder->abort();
        finderThread->wait();
        delete finderThread;
        delete finder;
    }

    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(!processing) {
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

    return false;
}

QString MainWindow::getPathFromFile(readType type)
{
    QString readPath{};
    QDir dir;
    QString projectPath = dir.absolutePath();
    QFile pathFile(projectPath+"/PATH.txt");

    if (!pathFile.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, "Informacja", "Ścieżka nie odczytana poprawnie.\nERROR: can't open PATH.txt");
            return QString("//K1/napps/NAPPS");
        }

    QTextStream out(&pathFile);
    readPath = out.readLine();

    if(!readPath.isEmpty() && type != readType::INIT) {
        QStringList pathList = readPath.split(";");
        if(type == readType::INPUT) readPath = pathList.at(0);
        else if(type == readType::OUTPUT) readPath = pathList.at(1);
    }

    pathFile.close();
    return readPath == "" ? QString("//K1/napps/NAPPS") : readPath;
}

void MainWindow::savePathToFile()
{
    QDir dir;
    QString projectPath = dir.absolutePath();
    QFile pathFile(projectPath+"/PATH.txt");

    if (!pathFile.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, "Informacja", "Ścieżka nie zapisana poprawnie.\nERROR: can't open PATH.txt");
            return;
        }

    QTextStream in(&pathFile);
    in << ui->inputLineEdit->text() + ";" + ui->outputLineEdit->text();
    pathFile.close();
}

void MainWindow::fillPaths()
{
    QString path = getPathFromFile(INIT);

    if( path == QString("//K1/napps/NAPPS") ){
        ui->inputLineEdit->setText(path);
        ui->outputLineEdit->setText(path);
    }
    else {
        QStringList pathList = path.split(";");

        if( pathList.at(0).isEmpty()) ui->inputLineEdit->setText(QString("//K1/napps/NAPPS"));
        else ui->inputLineEdit->setText(pathList.at(0));

        if( pathList.at(1).isEmpty()) ui->outputLineEdit->setText(QString("//K1/napps/NAPPS"));
        else ui->outputLineEdit->setText(pathList.at(1));
    }
}

void MainWindow::setEnabled(bool isEnabled)
{
    if(isEnabled) {
        processing = true;
        ui->whiteButton->setEnabled(false);
        ui->blueButton->setEnabled(false);
        ui->sigmaButton->setEnabled(false);
        ui->othersButton->setEnabled(false);
        ui->listWidget->clear();
        ui->label->setVisible(false);
        ui->inputLineEdit->setEnabled(false);
        ui->outputLineEdit->setEnabled(false);
        ui->searchButton->setText("ANULUJ");
        ui->searchButton->setIcon(QIcon(":/images/images/clear.png"));
    }
    else {   
        processing = false;
        ui->whiteButton->setEnabled(true);
        ui->blueButton->setEnabled(true);
        ui->sigmaButton->setEnabled(true);
        ui->othersButton->setEnabled(true);
        ui->inputLineEdit->setEnabled(true);
        ui->outputLineEdit->setEnabled(true);
        ui->searchButton->setText("WYSZUKAJ");
        ui->searchButton->setIcon(QIcon(":/images/images/search.png"));
    }
}

QString MainWindow::getInputPath()
{
    QString initPath = getPathFromFile(INPUT);
    QString path = QFileDialog::getExistingDirectory(this , tr("Wybierz folder"), initPath );
    return path;
}

QString MainWindow::getOutputPath()
{
    QString initPath = getPathFromFile(OUTPUT);
    QString path = QFileDialog::getExistingDirectory(this , tr("Wybierz folder"), initPath );
    return path;
}

QString MainWindow::getSchedulePath()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Wybierz harmonogram"), tr("//K1/Handlowy/DH/Realizacje"), tr("Pliki XLSX (*.xlsx)"));
    if(!schedulePath.isEmpty() && path.isEmpty()) return schedulePath;
    else return path;
}

void MainWindow::on_inputButton_released()
{
    if(processing) return;

    QString inputPath = getInputPath();
    if ( !inputPath.isEmpty() ) {
        ui->inputLineEdit->setText(inputPath);
        ui->inputLineEdit->setCursorPosition(0);
        savePathToFile();
    }
}

void MainWindow::on_outputButton_released()
{
    if(processing) return;

    QString outputPath = getOutputPath();
    if ( !outputPath.isEmpty() ) {
        ui->outputLineEdit->setText(outputPath);
        ui->outputLineEdit->setCursorPosition(0);
        savePathToFile();
    }
}

void MainWindow::on_mainButtonReleased(const QPushButton *mainButton)
{
    if( mainButton == ui->addButton )
    {
        qApp->processEvents();
        schedulePath = getSchedulePath();
        if ( !schedulePath.isEmpty() ) {
            QStringList pathList  = schedulePath.split("/");
            ui->scheduleLineEdit->setText(pathList.at(pathList.size()-1));
            ui->scheduleLineEdit->setCursorPosition(0);
        }
    }

    else if ( mainButton == ui->exitButton )
        QApplication::quit();

    else if ( mainButton == ui->settingsButton )
    {
        settingsDialog->previousState = settingsDialog->isTurnOn;
        settingsDialog->exec();
    }

    else if ( mainButton == ui->helpButton )
    {
        HelpDialog * helpDialog = new HelpDialog(this);
        helpDialog->exec();
    }

    else if ( mainButton == ui->mergeButton ) {
        QMessageBox::information(this, tr("Informacja"), QString("Opcja aktualnie nie aktywna."));
        //MergeDialog * mergeDialog = new MergeDialog(this,ui->outputLineEdit->text());
        //mergeDialog->exec();
    }
}

void MainWindow::on_searchButton_clicked()
{
    if(!processing) {

        QStringList missingPaths;

        if( ui->inputLineEdit->text().isEmpty() || !ui->inputLineEdit->text().contains("/"))
            missingPaths << "ścieżka wyszukiwania";
        if( ui->outputLineEdit->text().isEmpty() || !ui->outputLineEdit->text().contains("/"))
            missingPaths << "ścieżka zapisu";
        if( ui->scheduleLineEdit->text().isEmpty() || !ui->scheduleLineEdit->text().contains("xlsx"))
            missingPaths << "ścieżka harmonogramu";

        if (!missingPaths.isEmpty()) {
            if( ui->inputLineEdit->text() == ui->outputLineEdit->text() )
                QMessageBox::information(this, tr("Informacja"), QString("Ścieżka wyszukiwania nie może być scieżką zapisu."));
            else
                QMessageBox::information(this, tr("Informacja"), QString("Brakujące ścieżki: "+missingPaths.join(",")+"" + "."));
        }
        else {      
            setEnabled(true);

            if(finder!=nullptr) delete finder;
            if(finderThread!=nullptr) delete finderThread;

            finder = new Finder(0, schedulePath, ui->inputLineEdit->text(), ui->outputLineEdit->text(), isWhite, isSigma);
            finderThread = new QThread;
            finder->moveToThread(finderThread);

            QObject::connect( finder, SIGNAL(finished(bool,QString)), this, SLOT(on_processingFinished(bool,QString)));
            QObject::connect( finder, SIGNAL(itemFound(QString,bool)), this, SLOT(on_itemFound(QString,bool)));
            QObject::connect( finder, SIGNAL(signalProgress(int,QString) ), this, SLOT( on_setValue(int,QString)));

            QObject::connect( finderThread, SIGNAL(started()), finder, SLOT(findFiles())); // start searching
            QObject::connect( finder, SIGNAL(finished(bool,QString)), finderThread, SLOT(quit()),Qt::DirectConnection);

            finderThread->start();
        }
    }

    else {
        finder->abort();
        finderThread->wait();
        delete finderThread;
        delete finder;
        finder = nullptr;
        finderThread = nullptr;
    }
}

void MainWindow::on_itemFound(QString itemName, bool isFound)
{
    if(isFound) ui->listWidget->insertItem(0,new QListWidgetItem(QIcon(":/images/images/found.png"),itemName));
    else ui->listWidget->insertItem(0,new QListWidgetItem(QIcon(":/images/images/notfound.png"),itemName));
}

void MainWindow::on_setValue(int value, QString labelText)
{
    ui->progressBar->setValue(value);
    ui->statusLabel->setText(labelText);
}

void MainWindow::on_processingFinished(bool isSuccess, QString information)
{
    setEnabled(false);
    if (isSuccess) {
        if(settingsDialog->isTurnOn) {
            player -> setMedia( QUrl("qrc:/images/images/success.mp3") );
            player -> setVolume( 50);
            player -> play();
            }
        QMessageBox info;
        info.setWindowIcon(QIcon(":/images/images/logo.png"));
        info.setIcon(QMessageBox::Information);
        info.setText(information);
        info.setWindowTitle("Informacja");
        info.exec();
    }
    else {
        ui->listWidget->clear();
        ui->statusLabel->clear();
        ui->progressBar->setValue(0);

        if(!information.isEmpty()) {
            QMessageBox emptyListMessage;
            emptyListMessage.setWindowIcon(QIcon(":/images/images/logo.png"));
            emptyListMessage.setIcon(QMessageBox::Information);
            emptyListMessage.setText(information);
            emptyListMessage.setWindowTitle("Informacja");
            emptyListMessage.exec();
        }
    }
}

void MainWindow::on_sigmaButton_clicked()
{
    isSigma = true;
    ui->sigmaButton->setStyleSheet("QPushButton {border-radius: 5px;border: 2px solid rgb(90,90,90);background: white}");
    ui->othersButton->setStyleSheet("QPushButton {border-radius: 5px;border: 2px solid white;background: white}");
}

void MainWindow::on_othersButton_clicked()
{
    isSigma = false;
    ui->sigmaButton->setStyleSheet("QPushButton {border-radius: 5px;border: 2px solid white;background: white}");
    ui->othersButton->setStyleSheet("QPushButton {border-radius: 5px;border: 2px solid rgb(90,90,90);background: white}");
}

void MainWindow::on_blueButton_clicked()
{
    isWhite = false;
    ui->whiteButton->setStyleSheet("QPushButton {border-radius: 5px;border: 2px solid white;background: white}");
    ui->blueButton->setStyleSheet("QPushButton {border-radius: 5px;border: 2px solid rgb(90,90,90);background: #00B0F0}");
}

void MainWindow::on_whiteButton_clicked()
{
    isWhite = true;
    ui->whiteButton->setStyleSheet("QPushButton {border-radius: 5px;border: 2px solid rgb(90,90,90);background: white}");
    ui->blueButton->setStyleSheet("QPushButton {border-radius: 5px;border: 2px solid #00B0F0;background: #00B0F0}");
}
