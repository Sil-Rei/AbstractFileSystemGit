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
#include <QChar>

Disk* disk;
abstractFilesystem* fs;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(diskInformationChanged()), this, SLOT(updateDiskInformation()));
    connect(this, SIGNAL(diskSpaceAltered()), this, SLOT(updateDisk()));
    connect(this, SIGNAL(CDSpaceAltered()), this, SLOT(updateCD()));

    // Create cd
    cd = new CDROM();
    inserted = false;
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
    if(QDir("FileSystem/cd").exists()){
        QDir("FileSystem/cd").removeRecursively();
    }
    // Set the view to only that specific dir
    model->setRootPath("FileSystem");
    model->setReadOnly(false);

    ui->fileSystemTreeView->setModel(model);
    QModelIndex idx = model->index("FileSystem");

    rootPath = model->fileInfo(idx).path() + "/FileSystem/";
    ui->fileSystemTreeView->setRootIndex(idx);
    ui->fileSystemTreeView->hideColumn(1);

    // set rect icons for legend
    QPixmap pixmapFree(20,20);
    QPainter painterFree(&pixmapFree);
    painterFree.setBrush(QBrush(Qt::green));
    painterFree.drawRect(0,0,20,20);
    ui->freeLegendIconLabel->setPixmap(pixmapFree);

    QPixmap pixmapUsed(20,20);
    QPainter painterUsed(&pixmapUsed);
    painterUsed.setBrush(QBrush(Qt::yellow));
    painterUsed.drawRect(0,0,20,20);
    ui->usedLegendIconLabel->setPixmap(pixmapUsed);

    QPixmap pixmapReserved(20,20);
    QPainter painterReserved(&pixmapReserved);
    painterReserved.setBrush(QBrush(Qt::blue));
    painterReserved.drawRect(0,0,20,20);
    ui->reservedLegendIconLabel->setPixmap(pixmapReserved);

    QPixmap pixmapNotInit(20,20);
    QPainter painterNotInit(&pixmapNotInit);
    painterNotInit.setBrush(QBrush(Qt::gray));
    painterNotInit.drawRect(0,0,20,20);
    ui->notInitLegendIconLabel->setPixmap(pixmapNotInit);

}

MainWindow::~MainWindow()
{
    delete cd;
    delete disk;
    delete ui;
}

/**
 * This function is executed once when the hdd is initialized
 * @brief MainWindow::updateDiskInformation
 */
void MainWindow::updateDiskInformation(){
    ui->sizeInKbLabel->setText(QString("Gr????e (in kB):               %1").arg(disk->getDiskSize()));
    ui->blockSizeLabel->setText(QString("Blockgr????e:                  %1").arg(disk->getBlockSize()));
    ui->blockAmountLabel->setText(QString("Block Anzahl:                %1").arg(disk->getAmountOfBlocks()));
    ui->freeBlocksLabel->setText(QString("Freie Bl??cke:                 %1").arg(disk->getFreeDiskSpaceInBlocks()));
    ui->freeDiskSpaceLabel->setText(QString("Freier Speicher(in kB):   %1").arg(disk->getFreeDiskSpaceInBlocks()*disk->getBlockSize()/1000));
    ui->fragmentationLabel->setText(QString("Fragmentierung:            %1%").arg(100-disk->getFragmentation()));
    paintPlate();
}

/**
 * This function is executed whenever any information about the disk is changed to keep the labels and the graphic up-to-date
 * @brief MainWindow::updateDisk keeps labels and graphic up-to-date
 */
void MainWindow::updateDisk(){
    ui->freeBlocksLabel->setText(QString("Freie Bl??cke:                 %1").arg(disk->getFreeDiskSpaceInBlocks()));
    ui->fragmentationLabel->setText(QString("Fragmentierung:            %1%").arg(100-disk->getFragmentation()));
    ui->freeDiskSpaceLabel->setText(QString("Freier Speicher(in kB):   %1").arg(disk->getFreeDiskSpaceInBlocks()*disk->getBlockSize()/1000));
    paintPlate();
}

/**
 * Updates the CD graphic
 * @brief MainWindow::updateCD Updates the CD graphic
 */
void MainWindow::updateCD(){
    paintCD();
}

/**
 * The init function to start the simulation. After inputting the general conditions it sets the filesystem variable to inode or fat
 * @brief MainWindow::on_reinitializeHDDButton_clicked inits the filesystem and drive
 */
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
    QString label = QString("Gr????e der HDD in kB:");
    form.addRow(label, lineEdit);

    fields.append(lineEdit);

    QLineEdit *lineEdit2 = new QLineEdit(&dialog);
    lineEdit2->setValidator(new QIntValidator(0, 1000000, this));
    QString label2 = QString("Blockgr????e: ");
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
                box.setText("Zu kleine Bl??ckgr????e bzw. zu gro??e Plattengr????e");
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
 * Paints the blocks to the graphic label using a pixmap
 * @brief MainWindow::paintPlate paints the colored blocks to label
 */
void MainWindow::paintPlate(){
    int size = disk->getAmountOfBlocks();

    // Draw
    int plateHeight = ui->plateGraphicInfoLabel->height();
    int plateWidth = ui->plateGraphicInfoLabel->width();
    QPixmap pixmap(plateWidth, plateHeight);

    pixmap.fill(QColor("transparent"));
    QPainter painter(&pixmap);

    // Calculate the height / width of square with -> ((with * height of plate) / amountOfBlocks)^0.5
    int widthHeightOfSquare = ((int)sqrt((plateHeight * plateWidth) / size)*0.9);
    int blocksPerRow = plateWidth / widthHeightOfSquare;

    // Loop through plate, paint square for each status
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
/**
 * @brief MainWindow::paintCD paints the block stati to the pixmap label
 */
void MainWindow::paintCD(){
    if(!inserted){
        QPixmap map;

        map.fill(QColor("transparent"));
        ui->CDGraphicInfoLabel->setPixmap(map);
        return;
    }

    int size = cd->getAmountOfBlocks();

    int plateHeight = ui->CDGraphicInfoLabel->height();
    int plateWidth = ui->CDGraphicInfoLabel->width();
    QPixmap pixmap(plateWidth, plateHeight);

    pixmap.fill(QColor("transparent"));
    QPainter painter(&pixmap);

    // Calculate the height / width of square with -> ((with * height of plate) / amountOfBlocks)^0.5
    int widthHeightOfSquare = ((int)sqrt((plateHeight * plateWidth) / size)*0.6);
    int blocksPerRow = plateWidth / widthHeightOfSquare;

    for(int arrIndex = 0, row = 0; arrIndex < size; arrIndex++){
        if(((arrIndex)%blocksPerRow) == 0){
            row++;
        }
        if(cd->getPlate()[arrIndex] == FREE){
            painter.setBrush(QBrush(Qt::green));
            painter.drawRect((arrIndex%blocksPerRow)*widthHeightOfSquare,row * widthHeightOfSquare,widthHeightOfSquare,widthHeightOfSquare);
        }else if(cd->getPlate()[arrIndex] == OCCUPIED){
            painter.setBrush(QBrush(Qt::yellow));
            painter.drawRect((arrIndex%blocksPerRow)*widthHeightOfSquare,row * widthHeightOfSquare,widthHeightOfSquare,widthHeightOfSquare);
        }else if(cd->getPlate()[arrIndex] == RESERVED){
            painter.setBrush(QBrush(Qt::blue));
            painter.drawRect((arrIndex%blocksPerRow)*widthHeightOfSquare,row * widthHeightOfSquare,widthHeightOfSquare,widthHeightOfSquare);
        }else{
            painter.setBrush(QBrush(Qt::red));
            painter.drawRect((arrIndex%blocksPerRow)*widthHeightOfSquare,row * widthHeightOfSquare,widthHeightOfSquare,widthHeightOfSquare);
        }
    }
    ui->CDGraphicInfoLabel->setPixmap(pixmap);
}

/**
 * Creates a file on the simulated array and in the folder used by the explorer view with the inputted sizes
 * @brief MainWindow::on_createFileButton_clicked creates a file
 */
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
    QString label2 = QString("Gr????e des Files : ");
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
        //qDebug() << "Freie Bl??cke: " << disk->getFreeDiskSpaceInBlocks() << " Ben??tigte Bl??cke: " << ceil((double)fileSize / disk->getBlockSize());
        if(fileSize <= 0){
            QMessageBox box;
            box.setText("Filegr????e muss ??ber 0 sein.");
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

/**
 * @brief MainWindow::on_createDirButton_clicked creates a directory on the explorer view
 */
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

/**
 * @brief removeDir is a function to recursively delete all contents of a folder, most of the code of this function by of the QT Documentation and stackoverflow
 * @param dirName name of the directory
 * @return returns if deletion was successful
 */
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

/**
 * @brief MainWindow::on_deleteSelectionButton_clicked checks if wether its a file or a directory and deletes it
 */
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

/**
 * @brief MainWindow::on_fileSystemTreeView_clicked this function is executed whenever the user clicks on the explorer view and updates buttons and info
 * @param index
 */
void MainWindow::on_fileSystemTreeView_clicked(const QModelIndex &index)
{
    QString fileName = model->fileInfo(index).fileName();
    ui->fileNameInfoLabel->setText(QString("Name: %1").arg(fileName));
    int fileSize = 0;
    if(fs != nullptr){
        ui->createFileButton->setEnabled(true);
        ui->createDirButton->setEnabled(true);
        ui->deleteSelectionButton->setEnabled(true);
        fileSize = fs->getFileSize(fileName);
    }

    ui->sizeInfoLabel->setText(QString("Gr????e: %1").arg(fileSize));

    QString filePath = model->fileInfo(index).filePath();
    if(filePath.contains("root")){
        filePath ="root" + filePath.split("root")[1];
    }else{
        filePath ="cd" + filePath.split("cd")[1];
    }

    ui->pathInfoLabel->setText(QString("Pfad: %1").arg(filePath));

}

/**
 * @brief MainWindow::on_defragButton_clicked defrags the disk
 */
void MainWindow::on_defragButton_clicked()
{
    fs->defrag();
    emit diskSpaceAltered();
}

/**
 * @brief MainWindow::on_insertEjectCDButton_clicked swaps the state of the cd (inserted/ejected)
 */
void MainWindow::on_insertEjectCDButton_clicked()
{
    if(!inserted){
        ui->burnCDButton->setEnabled(true);
        ui->moveToCDButton->setEnabled(true);
        ui->insertEjectCDButton->setText("CD auswerfen");
        // Check if root folder exists, if not create it
        if(!QDir("FileSystem/cd").exists()){
            QDir().mkdir("FileSystem/cd");
        }else{
           QDir("FileSystem/cd").removeRecursively();
           QDir().mkdir("FileSystem/cd");
        }
    }else{
        ui->insertEjectCDButton->setText("CD einlegen");
        ui->burnCDButton->setEnabled(false);
        cd->clear();
        cd->getFilesToBeBurned();
        ui->moveToCDButton->setEnabled(false);
        if(QDir("FileSystem/cd").exists()){
            QDir("FileSystem/cd").removeRecursively();
        }
    }

    inserted = !inserted;

    emit CDSpaceAltered();
}

/**
 * @brief MainWindow::on_moveToCDButton_clicked queues a file to be moved to the cd
 */
void MainWindow::on_moveToCDButton_clicked()
{

    // Get current coursor index, if its a dir return
    QModelIndex index = ui->fileSystemTreeView->currentIndex();
    if(!index.isValid()) return;

    if(model->fileInfo(index).isDir()) return;


    // Get filename of current index
    QString fileName = model->fileInfo(index).fileName();

    //Check if file is already moved to CD
    if(cd->getFilesToBeBurned().contains(fileName)){
        QMessageBox box;
        box.setText("File wurde schon auf CD geschoben.");
        box.setIcon(QMessageBox::Warning);
        box.addButton("OK", QMessageBox::AcceptRole);
        box.exec();
        return;
    }

    long fileSize = fs->getFileSize(fileName);

    // Check if file fits the cd
    int blocksNeeded = ceil((double)fileSize / cd->blockSize);
    if(cd->getFreeDiskSpaceInBlocks() < blocksNeeded){
        QMessageBox box;
        box.setText("File ist zu gro?? f??r CD");
        box.setIcon(QMessageBox::Warning);
        box.addButton("OK", QMessageBox::AcceptRole);
        box.exec();
        return;
    }
    cd->pushFileToCd(fileName, fileSize);
    emit CDSpaceAltered();
}

/**
 * @brief MainWindow::on_burnCDButton_clicked burns the queued files to the cd
 */
void MainWindow::on_burnCDButton_clicked()
{
    if(cd->getFilesToBeBurned().empty()){
        QMessageBox box;
        box.setText("CD ist leer.");
        box.setIcon(QMessageBox::Warning);
        box.addButton("OK", QMessageBox::AcceptRole);
        box.exec();
        return;
    }

    cd->burnCD(rootPath);
    ui->burnCDButton->setEnabled(false);
    ui->moveToCDButton->setEnabled(false);
    emit CDSpaceAltered();
}

