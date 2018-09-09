#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "configeditor.h"
#include "adddialog.h"
#include "fileop.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QDirIterator>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Read config, data returned as struct
    auto config = ConfigEditor::readConfig();

    pathOrbiter = config.pathOrbiter;

    dataAddons = config.dataAddons;

    // Enable/Disable widgets based on Orbiter path
    setWidgets();

    // Scan Orbiter directory for addons
    addonsScan();

    // Define the backup directory
    dirBackup = QDir::currentPath() + "/OrbiterBackup/";

    QDir dirBackCheck(dirBackup);

    if(!dirBackCheck.exists()) dirBackCheck.mkpath(".");

}

// Enable/Disable widgets based on Orbiter path
void MainWindow::setWidgets(){

    if(!pathOrbiter.isEmpty() && !pathOrbiter.isNull()){

        // Check if orbiter.exe is exists in the given path
        QFileInfo info(pathOrbiter + "orbiter.exe");

        if(info.exists()){

            // Enable widgets if it exists
            ui->addonsList->setEnabled(true);

            ui->addButton->setEnabled(true);

            ui->uninstallButton->setEnabled(true);

        }

        else {

            QMessageBox::warning(this, "Path is wrong", "Please select the correct Orbiter path");

            // Open select Orbiter path window if the given path doesn't include orbiter.exe
            on_actionPath_triggered();
        }
    // Open file dialog
    } else on_actionPath_triggered();

}

void MainWindow::addonsScan(){

    // Clear UI list and addons list
    ui->addonsList->clear();

    addons.clear();

    foreach(QString addon, dataAddons.keys()){

        // Send the Orbiter directory with addon file list to check if it exists
        // If it exists, will add it to addons list
        if(FileOp::checkFiles(pathOrbiter, dataAddons.value(addon))) addons.insert(addon, dataAddons.value(addon));

    }

    foreach(QString item, addons.keys()){

        // Add addons items to UI list
        if(!item.isEmpty() && !item.isNull()) ui->addonsList->addItem(item);

    }

}

// Install/Add button
void MainWindow::on_addButton_clicked()
{
    AddDialog dialogAdd;

    dialogAdd.exec();

    // If user didn't press Ok button in the dialog
    if (!dialogAdd.check) return;

    // If addon name or path is empty
    if(dialogAdd.nameAddon.isEmpty() || dialogAdd.pathAddon.isEmpty()){

        QMessageBox::critical(this, "Addon hasn't installed/added",
                              "Addon hasn't installed/added, you must enter the addon name or/and path");

        return;

    }

    QDirIterator it(dialogAdd.pathAddon, QDirIterator::Subdirectories);

    QList<QString> files = FileOp::scanDirectory(dialogAdd.pathAddon);

    bool exists = false;

    foreach(QString addon, dataAddons.keys()){

        // Check if the addon files exists in another name. If not, it will add the addon to the database
        if(FileOp::checkFiles(files, dataAddons.value(addon))){exists = true; break;}

    }

    if(!exists){dataAddons.insert(dialogAdd.nameAddon + " (User)", files);}

    // If install checkbox is checked
    if(dialogAdd.install){

        // If install Orbiter sdk folder checkbox is checked, will remove the Orbitersdk folder from addon path
        if(!dialogAdd.sdkInstall){QDir(dialogAdd.pathAddon + "Orbitersdk").removeRecursively();}

        foreach(QString file, files){

            QString filePath = pathOrbiter + file;

            QFileInfo infoFile(filePath);

            // If a file in the addon exists in Orbiter
            if(infoFile.exists()){

                // Set the parent directory to the file
                QDir backDir(dirBackup + QFileInfo(file).dir().path());

                // Create the parent directory if not exists in the backup path
                if(!backDir.exists()) backDir.mkpath(".");

                QFile::copy(filePath, dirBackup + file);

                QFile::remove(filePath);

            }

            // Create the parent directory to the addon if not exists
            if(!infoFile.dir().exists()){infoFile.dir().mkpath(".");}

            QFile::copy(dialogAdd.pathAddon + file, filePath);

        }

        if(dialogAdd.removeDir){QDir(dialogAdd.pathAddon).removeRecursively();}

    }

    addonsScan();

    QMessageBox::information(this, "Addon installed/added successfully",
                             "Addon " + dialogAdd.nameAddon + " installed/added successfully");
}

void MainWindow::on_uninstallButton_clicked()
{
    QString nameAddon = ui->addonsList->currentItem()->text();

    QMessageBox::StandardButton response = QMessageBox::question(this, "Uninstallation confirm",
                                                                 "Are you sure you want to uninstall " + nameAddon + "?");

    if(response == QMessageBox::No) return;

    foreach(QString file, addons.value(nameAddon)){

        QString pathFile = pathOrbiter + file;

        // Check if the file exists in the backup directory to restore it
        QFileInfo infobackFile(dirBackup + file);

        if(infobackFile.exists()){

            QFile::remove(pathFile);

            QFile::copy(infobackFile.filePath(), pathFile);

            QFile::remove(infobackFile.filePath());

        } else {

            QFileInfo infoFile(pathFile);

            QFile::remove(pathFile);

            // Remove the parent folder if empty
            infoFile.dir().rmdir(infoFile.dir().absolutePath());

        }
    }

    addonsScan();

    QMessageBox::information(this, "Addon uninstalled successfully",
                             "Addon " + nameAddon + " uninstalled successfully");
}

void MainWindow::on_actionPath_triggered()
{
    QString path = QFileDialog::getExistingDirectory(this, "Select Orbiter directory", "/home");

    if(path.isEmpty()) return;

    QFileInfo info(path + "/orbiter.exe");

    // Check if orbiter.exe exists in given path
    if(info.exists()){

        pathOrbiter = path + "/";

        setWidgets();

        addonsScan();

        ui->statusBar->showMessage("Orbiter path is set to " + pathOrbiter, 3000);

    }

    // Open file dialog again (Repeat this function)
    else {QMessageBox::warning(this, "Path is wrong", "Please select the correct Orbiter path"); on_actionPath_triggered();}
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About Orbiter Addons Manager","Orbiter Addons Manager <br>"
                                                            "A tool to organize your Orbiter addons <br> <br>"

                                                            "Version: 1.0 <br>"
                                                            "Build date: Sept 2018 <br>"
                                                            "License: GPL3 <br> <br>"

                                                            "Copyright © Abdullah Radwan <br>"
                                                            "Icon by icons8.com");
}

MainWindow::~MainWindow(){ConfigEditor::writeConfig(pathOrbiter, dataAddons); delete ui;}
