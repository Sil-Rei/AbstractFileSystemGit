#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QDebug>
#include <iostream>
#include <QPainter>
#include "disk.h"
#include "statusTypes.h"
#include "abstractfilesystem.h"
#include <math.h>
#include <QFormLayout>
#include <QMessageBox>

Disk* disk;
abstractFilesystem* fs;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(diskInformationChanged()), this, SLOT(updateDiskInformation()));
    connect(this, SIGNAL(diskSpaceAltered()), this, SLOT(updateDisk()));


    // Init FileSystemView
    model = new QFileSystemModel(this);
    // Check if root folder exists, if not create it
    if(!QDir("FileSystem").exists()){
        QDir().mkdir("FileSystem");
    }
    if(!QDir("FileSystem/root").exists()){
        QDir().mkdir("FileSystem/root");
    }else{
       QDir("FileSystem/root").removeRecursively();
       QDir().mkdir("FileSystem/root");
    }
    // Set the view to only that specific dir
    model->setRootPath("FileSystem");
    model->setReadOnly(false);

    ui->fileSystemTreeView->setModel(model);
    QModelIndex idx = model->index("FileSystem");
    ui->fileSystemTreeView->setRootIndex(idx);

    // set rect icons for legend
    QPixmap pixmapFree(20,20);
    QPainter painterFree(&pixmapFree);
    ui->freeLegendIconLabel->setPixmap(pixmapFree);
    painterFree.setBrush(QBrush(Qt::green));
    painterFree.drawRect(0,0,20,20);
    ui->usedLegendIconLabel->setPixmap(pixmapFree);

    QPixmap pixmapUsed(20,20);
    QPainter painterUsed(&pixmapUsed);
    ui->freeLegendIconLabel->setPixmap(pixmapUsed);
    painterUsed.setBrush(QBrush(Qt::yellow));
    painterUsed.drawRect(0,0,20,20);
    ui->usedLegendIconLabel->setPixmap(pixmapUsed);

    QPixmap pixmapReserved(20,20);
    QPainter painterReserved(&pixmapReserved);
    ui->freeLegendIconLabel->setPixmap(pixmapReserved);
    painterReserved.setBrush(QBrush(Qt::blue));
    painterReserved.drawRect(0,0,20,20);
    ui->reservedLegendIconLabel->setPixmap(pixmapReserved);

    QPixmap pixmapNotInit(20,20);
    QPainter painterNotInit(&pixmapNotInit);
    ui->freeLegendIconLabel->setPixmap(pixmapNotInit);
    painterNotInit.setBrush(QBrush(Qt::gray));
    painterNotInit.drawRect(0,0,20,20);
    ui->notInitLegendIconLabel->setPixmap(pixmapNotInit);



}

MainWindow::~MainWindow()
{
    delete disk;
    delete ui;
}

// Setzt die Label und callt paintShit
void MainWindow::updateDiskInformation(){
    ui->sizeInKbLabel->setText(QString("Größe (in kB):               %1").arg(disk->getDiskSize()));
    ui->blockSizeLabel->setText(QString("Blockgröße:                  %1").arg(disk->getBlockSize()));
    ui->blockAmountLabel->setText(QString("Block Anzahl:                %1").arg(disk->getAmountOfBlocks()));
    ui->freeBlocksLabel->setText(QString("Freie Blöcke:                 %1").arg(disk->getFreeDiskSpaceInBlocks()));
    ui->freeDiskSpaceLabel->setText(QString("Freier Speicher(in kB):   %1").arg(disk->getFreeDiskSpaceInBlocks()*disk->getBlockSize()/1000));
    ui->fragmentationLabel->setText(QString("Fragmentierung:            %1%").arg(100-disk->getFragmentation()));
    paintPlate();
}

void MainWindow::updateDisk(){
    ui->freeBlocksLabel->setText(QString("Freie Blöcke:                 %1").arg(disk->getFreeDiskSpaceInBlocks()));
    ui->fragmentationLabel->setText(QString("Fragmentierung:            %1%").arg(100-disk->getFragmentation()));
    ui->freeDiskSpaceLabel->setText(QString("Freier Speicher(in kB):   %1").arg(disk->getFreeDiskSpaceInBlocks()*disk->getBlockSize()/1000));
    paintPlate();
}

// click auf button löst eingabefenster aus und weist der disk die größen je nach usereingabe zu legt neue disk an.
// callt dann diskInformationChanged welche über die connect mit der updateDiskInformation verbunden ist und den stuff aufruft.
void MainWindow::on_reinitializeHDDButton_clicked()
{
    ui->defragButton->setEnabled(true);
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    // Add some text above the fields
    form.addRow(new QLabel("Geben Sie die Eckdaten der HDD ein: "));

    // Add the lineEdits with their respective labels
    QList<QLineEdit *> fields;
    QLineEdit *lineEdit = new QLineEdit(&dialog);
    lineEdit->setValidator(new QIntValidator(0, 1000000, this));
    QString label = QString("Größe der HDD in kB:");
    form.addRow(label, lineEdit);

    fields.append(lineEdit);

    QLineEdit *lineEdit2 = new QLineEdit(&dialog);
    lineEdit2->setValidator(new QIntValidator(0, 1000000, this));
    QString label2 = QString("Blockgröße: ");
    form.addRow(label2, lineEdit2);

    fields.append(lineEdit2);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        try {
            // If the user didn't dismiss the dialog, do something with the fields
            int diskSize = fields.at(0)->text().toInt();
            int blockSize = fields.at(1)->text().toInt();
            qDebug() << "DiskSize: " << diskSize << " Blocksize: " << blockSize;

            // Check if size is valid
            if(diskSize*1000 / blockSize > 2500){
                QMessageBox box;
                box.setText("Zu kleine Blöckgröße bzw. zu große Plattengröße");
                box.setIcon(QMessageBox::Warning);
                box.addButton("OK", QMessageBox::AcceptRole);
                box.exec();
                return;
            }

            disk = new Disk(diskSize, blockSize);
            if(ui->fatRadioButton->isChecked()){
                fs = new fatFileSystem(disk);
            }else{
                fs = new inodefilesystem(disk);
            }
            emit diskInformationChanged();
        } catch (...) {
        }
    }
}

/**
 * @brief MainWindow::paintShit
 paint shit geht array durch und prüft jeden eintrag ob free oder occupied und färbt je nach dem rot oder grün
 */
void MainWindow::paintPlate(){
    int size = disk->getAmountOfBlocks();
    // Prep test data

    // Draw shit
    int plateHeight = ui->plateGraphicInfoLabel->height();
    int plateWidth = ui->plateGraphicInfoLabel->width();
    QPixmap pixmap(plateWidth, plateHeight);

    pixmap.fill(QColor("transparent"));
    QPainter painter(&pixmap);

    // Calculate the height / width of square with -> ((with * height of plate) / amountOfBlocks)^0.5
    int widthHeightOfSquare = ((int)sqrt((plateHeight * plateWidth) / size)*0.9);
    int blocksPerRow = plateWidth / widthHeightOfSquare;

    for(int arrIndex = 0, row = 0; arrIndex < size; arrIndex++){
        if(((arrIndex)%blocksPerRow) == 0){
            row++;
        }
        if(disk->getPlate()[arrIndex] == FREE){
            painter.setBrush(QBrush(Qt::green));
            painter.drawRect((arrIndex%blocksPerRow)*widthHeightOfSquare,row * widthHeightOfSquare,widthHeightOfSquare,widthHeightOfSquare);
        }else if(disk->getPlate()[arrIndex] == OCCUPIED){
            painter.setBrush(QBrush(Qt::yellow));
            painter.drawRect((arrIndex%blocksPerRow)*widthHeightOfSquare,row * widthHeightOfSquare,widthHeightOfSquare,widthHeightOfSquare);
        }else if(disk->getPlate()[arrIndex] == RESERVED){
            painter.setBrush(QBrush(Qt::blue));
            painter.drawRect((arrIndex%blocksPerRow)*widthHeightOfSquare,row * widthHeightOfSquare,widthHeightOfSquare,widthHeightOfSquare);
        }else{
            painter.setBrush(QBrush(Qt::red));
            painter.drawRect((arrIndex%blocksPerRow)*widthHeightOfSquare,row * widthHeightOfSquare,widthHeightOfSquare,widthHeightOfSquare);
        }
    }
    ui->plateGraphicInfoLabel->setPixmap(pixmap);
}


void MainWindow::on_createFileButton_clicked()
{
    ui->defragButton->setEnabled(true);
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    // Add some text above the fields
    form.addRow(new QLabel("Geben Sie die Eckdaten des Files an: "));

    // Add the lineEdits with their respective labels
    QList<QLineEdit *> fields;
    QLineEdit *lineEdit = new QLineEdit(&dialog);
    lineEdit->setText("*.txt");
    QString label = QString("Name des Files: ");
    form.addRow(label, lineEdit);

    fields.append(lineEdit);

    QLineEdit *lineEdit2 = new QLineEdit(&dialog);
    lineEdit2->setValidator(new QIntValidator(0, 1000000, this));
    QString label2 = QString("Größe des Files : ");
    form.addRow(label2, lineEdit2);

    fields.append(lineEdit2);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        QString fileName = fields.at(0)->text();
        // if fs is fat, convert name to upper
        if(dynamic_cast<inodefilesystem*>(fs) == nullptr){
            fileName = fileName.toUpper();
        }
        // Check for integrity
        if(!fs->checkName(fileName)){
            return;
        }
        int fileSize = fields.at(1)->text().toInt();
        //qDebug() << "Freie Blöcke: " << disk->getFreeDiskSpaceInBlocks() << " Benötigte Blöcke: " << ceil((double)fileSize / disk->getBlockSize());
        if(fileSize <= 0){
            QMessageBox box;
            box.setText("Filegröße muss über 0 sein.");
            box.setIcon(QMessageBox::Warning);
            box.addButton("OK", QMessageBox::AcceptRole);
            box.exec();
            return;
        }else if(ceil((double)fileSize / disk->getBlockSize()) > disk->getFreeDiskSpaceInBlocks()){
            QMessageBox box;
            box.setText("Nicht genug Speicherplatz");
            box.setIcon(QMessageBox::Warning);
            box.addButton("OK", QMessageBox::AcceptRole);
            box.exec();
            return;
        }
        // if cursor is currently on a file, set it to the parent folder to create new files there
        QModelIndex index = ui->fileSystemTreeView->currentIndex();
        if(model->fileInfo(index).isFile()){
            index = index.parent();
        }
        if(!index.isValid()) return;
        QString filePath = model->fileInfo(index).filePath();

        QFile file(filePath + "/" + fileName);
        file.open(QIODevice::ReadWrite);

        fs->createFile(fileSize, fileName, 1);
        qDebug() << fileName << " erstellt";
        emit diskSpaceAltered();
    }
}

void MainWindow::on_createDirButton_clicked()
{
    QModelIndex index = ui->fileSystemTreeView->currentIndex();
    if(model->fileInfo(index).isFile()){
        index = index.parent();
    }
    if(!index.isValid()) return;

    QString name = QInputDialog::getText(this, "Name", "Enter a name");

    if(name.isEmpty()) return;

    model->mkdir(index, name);
    emit diskSpaceAltered();
}

bool removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            } else {
                fs->deleteFile(info.fileName());
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

void MainWindow::on_deleteSelectionButton_clicked()
{
    QModelIndex index = ui->fileSystemTreeView->currentIndex();
    if(!index.isValid()) return;
    if(model->fileInfo(index).fileName() == "root") return;
    if(model->fileInfo(index).isDir()){
        QString path = model->fileInfo(index).absoluteFilePath();
        qDebug() << "Pfad: " << path;
        removeDir(path);
    }else{
        fs->deleteFile(model->fileInfo(index).fileName());
        model->remove(index);
    }


    emit diskSpaceAltered();
}


void MainWindow::on_fileSystemTreeView_clicked(const QModelIndex &index)
{
    if(fs != nullptr){
        ui->createFileButton->setEnabled(true);
        ui->createDirButton->setEnabled(true);
        ui->deleteSelectionButton->setEnabled(true);
    }

    QString fileName = model->fileInfo(index).fileName();
    ui->fileNameInfoLabel->setText(QString("Name: %1").arg(fileName));

    qint64 fileSize = model->fileInfo(index).size();

    ui->sizeInfoLabel->setText(QString("Größe: %1").arg(fileSize));

    QString filePath = model->fileInfo(index).filePath();
    filePath ="root" + filePath.split("root")[1];
    ui->pathInfoLabel->setText(QString("Pfad: %1").arg(filePath));
}

void MainWindow::on_defragButton_clicked()
{
    fs->defrag();
    emit diskSpaceAltered();
}


void MainWindow::on_startUpButton_clicked()
{
    emit diskSpaceAltered();
}

