#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include "fatfilesystem.h"
#include "inodefilesystem.h"
#include "cdrom.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    CDROM* cd;
    ~MainWindow();

private slots:
    void on_reinitializeHDDButton_clicked();
    void updateDiskInformation();
    void updateDisk();
    void updateCD();
    void paintPlate();
    void paintCD();

    void on_createFileButton_clicked();

    void on_createDirButton_clicked();

    void on_deleteSelectionButton_clicked();

    void on_fileSystemTreeView_clicked(const QModelIndex &index);

    void on_defragButton_clicked();

    void on_insertEjectCDButton_clicked();

    void on_moveToCDButton_clicked();

    void on_burnCDButton_clicked();

signals:
    void diskInformationChanged();
    void diskSpaceAltered();
    void CDSpaceAltered();


private:
    bool inserted;
    Ui::MainWindow *ui;
    QFileSystemModel *model;
};
#endif // MAINWINDOW_H
