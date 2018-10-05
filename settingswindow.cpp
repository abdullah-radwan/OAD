#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "addigndialog.h"
#include "adddbdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDirIterator>
#include <QDateTime>
#include <QTextStream>
#include <QCloseEvent>

SettingsWindow::SettingsWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::SettingsWindow){

    ui->setupUi(this);

    setWindowModality(Qt::ApplicationModal);
}

void SettingsWindow::showEvent(QShowEvent *ev){

     QMainWindow::showEvent(ev);

     ui->pathsCombo->addItems(pathsList);

     ui->pathsCombo->setCurrentText(orbiterPath);

     if(pathsList.isEmpty()){

         ui->snapBox->setDisabled(true);

         ui->removePathButton->setDisabled(true);

     }

     setDbTree();

     setIgnList();

}

void SettingsWindow::on_pathsCombo_currentIndexChanged(const QString &arg1){

    orbiterPath = arg1;

    backupDir = QDir::currentPath() + "/OrbiterBackup " + QString::number(pathsList.indexOf(orbiterPath)) + "/";

}

void SettingsWindow::on_addPathButton_clicked(){

    QString path = QFileDialog::getExistingDirectory(this, "Select Orbiter installation folder", orbiterPath);

    if(path.isEmpty()) {return;};

    path += "/";

    if(pathsList.contains(path)){

        QMessageBox::warning(this, "Installation already exists",
                             "The installation you are trying to add is already added");

        return;

    }

    // Check if orbiter.exe exists in given path
    if(QFileInfo(path + "orbiter.exe").isFile()){

        pathsList.append(path);

        ui->pathsCombo->addItem(path);

        QDir checkBackupDir(QDir::currentPath() + "/OrbiterBackup " + QString::number(pathsList.indexOf(path)) + "/");

        if(!checkBackupDir.exists()) checkBackupDir.mkpath(".");

        ui->pathsCombo->setCurrentText(path);

        if(!ui->snapBox->isEnabled()) ui->snapBox->setEnabled(true);

        if(!ui->removePathButton->isEnabled()) ui->removePathButton->setEnabled(true);

    }

    else {

        QMessageBox::warning(this, "Orbiter folder is wrong", "Please select the correct Orbiter folder");

        on_addPathButton_clicked();

    }
}

void SettingsWindow::on_removePathButton_clicked(){

    int index = ui->pathsCombo->currentIndex();

    QString path = QDir::currentPath() + "/OrbiterBackup ";

    QDir(path + QString::number(index)).removeRecursively();

    for(int counter = 1; counter < pathsList.count(); counter++){

        if(counter < index) continue;

        QDir(path + QString::number(counter)).rename(".", path + QString::number(counter - 1));

    }

    pathsList.removeAt(index);

    ui->pathsCombo->removeItem(index);

    if(pathsList.isEmpty()){

        ui->snapBox->setDisabled(true);

        ui->removePathButton->setDisabled(true);

    }

}

void SettingsWindow::on_createButton_clicked(){

    QString fileName = "snapshot-" + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss") + ".txt";

    QFile file(QDir::currentPath() + "/" + fileName);

    file.open(QIODevice::WriteOnly);

    QTextStream out(&file);

    out << "SNAPSHOT_OAD" << "\n";

    QDirIterator it(orbiterPath, QDirIterator::Subdirectories);

    // Scan Orbiter installation
    while (it.hasNext()) {

        it.next();

        QFileInfo fileInfo(it.filePath());

        if(fileInfo.isFile()){

            QString relPath = QDir(orbiterPath).relativeFilePath(it.filePath());

            out << relPath + "=" + QString::number(QDateTime(fileInfo.lastModified()).toSecsSinceEpoch()) << "\n";

        }

    }

    file.flush();

    file.close();

    QMessageBox::information(this, "Snapshot created successfully", "Snapshot created successfully in " + fileName);

}

void SettingsWindow::on_importButton_clicked(){

    QString fileName = QFileDialog::getOpenFileName(this, "Select snapshot file");

    if(fileName.isEmpty()) return;

    QFile file(fileName);

    file.open(QIODevice::ReadOnly);

    QTextStream in(&file);

    if(in.readLine() != "SNAPSHOT_OAD"){

        QMessageBox::warning(this, "Select a correct snapshot file", "Please selected a correct snapshot file.");

        return;

    }

    QList<QString> files, snapFiles;

    while(!in.atEnd()){snapFiles.append(in.readLine());}

    QDirIterator it(orbiterPath, QDirIterator::Subdirectories);

    while (it.hasNext()) {

        it.next();

        QFileInfo fileInfo(it.filePath());

        // Avoid adding directories
        if(fileInfo.isFile()){

            // Convert the path to relative path from the given directory
            QString relPath = QDir(orbiterPath).relativeFilePath(it.filePath());

            files.append(relPath + "=" + QString::number(QDateTime(fileInfo.lastModified()).toSecsSinceEpoch()));

        }

    }

    files.sort(); snapFiles.sort();

    if(files != snapFiles){

        if(checkSnapshot(snapFiles, files)){

            QMessageBox::warning(this, "Import failed",
                                 "Can't import the snapshot because file " + fileName + " doesn't have a back up");

            return;

        }

        foreach(QString snapFile, snapFiles){

            QString snapFileName = snapFile.split("=").first();

            QString snapFileTime = snapFile.split("=").last();

            foreach(QString file, files){

                QString fileName = file.split("=").first();

                QString fileTime = file.split("=").last();

                if(file == snapFile){break;}

                else if(fileName == snapFileName){

                    QFile::remove(orbiterPath + fileName);

                    QFile::copy(backupDir + fileName, orbiterPath + fileName);

                    QFile::remove(backupDir + fileName);

                }

                else if(fileName + ".oad" == snapFileName){

                    QFile::rename(orbiterPath + fileName, orbiterPath + fileName + ".oad");

                }

                else if(fileName == snapFileName + ".oad"){

                    QFile::rename(orbiterPath + fileName, orbiterPath + fileName.remove(".oad"));

                }

                else{

                    QFile::remove(orbiterPath + fileName);

                    QDir fileDir = QFileInfo(orbiterPath + fileName).dir();

                    while(fileDir.rmdir(".")){fileDir.cdUp();}

                }

            }

        }

        QMessageBox::information(this, "Snapshot imported successfully", "Snapshot imported successfully");

    } else {QMessageBox::information(this, "No changes were found", "No changes were made since the snapshot was taken");}

}

bool SettingsWindow::checkSnapshot(QList<QString> snapFiles, QList<QString> files){

    foreach(QString snapFile, snapFiles){

        QString snapFileName = snapFile.split("=").first();

        QString snapFileTime = snapFile.split("=").last();

        foreach(QString file, files){

            QString fileName = file.split("=").first();

            QString fileTime = file.split("=").last();

            if(file == snapFile || file + ".oad" == snapFile || file == snapFile + ".oad"){break;}

            else if(fileName == snapFileName){if(!QFileInfo(backupDir + fileName).isFile()){return true;}}

        }

    }

    return false;

}

void SettingsWindow::on_addIgnButton_clicked(){

    AddIgnDialog addDialog;

    addDialog.exec();

    if(!addDialog.check) return;

    if(addDialog.addonName.isEmpty()){

        QMessageBox::warning(this, "Enter the ignored file/folder", "You must enter a file/folder to remove it");

        return;

    }

    ignoredList.append(addDialog.addonName.toLower());

    setIgnList();
}

void SettingsWindow::on_removeIgnButton_clicked(){

    if(ui->ignoredList->selectedItems().isEmpty()){

        QMessageBox::warning(this, "Select an add-on", "You must select an file/folder to remove it");

        return;

    }

    ignoredList.removeOne(ui->ignoredList->currentItem()->text());

    setIgnList();
}

void SettingsWindow::setIgnList(){

    ui->ignoredList->clear();

    ui->ignoredList->addItems(ignoredList);

}

void SettingsWindow::on_addEntryButton_clicked(){

    AddDbDialog addDialog(this);

    addDialog.exec();

    if(!addDialog.check) return;

    if(addDialog.addonName.isEmpty() || (addDialog.addonFiles.isEmpty() && addDialog.addonPath.isEmpty())){

        QMessageBox::critical(this, "Enter the full add-on information",
                              "The add-on wasn't added. You must enter the add-on full information.");

        return;

    }

    if(!addDialog.filesChecked){

        QList<QString> addonFiles;

        QDirIterator it(addDialog.addonPath, QDirIterator::Subdirectories);

        // Scan Orbiter installation
        while (it.hasNext()) {

            it.next();

            QFileInfo fileInfo(it.filePath());

            if(fileInfo.isFile()){

                QString relPath = QDir(addDialog.addonPath).relativeFilePath(it.filePath());

                addonFiles.append(relPath);

            }

        }

        dbMap.insert(addDialog.addonName, addonFiles);

    }

    else dbMap.insert(addDialog.addonName, addDialog.addonFiles.split(","));

    setDbTree();

}

void SettingsWindow::on_removeEntryButton_clicked(){

    if(ui->dbTree->selectedItems().isEmpty()){

        QMessageBox::warning(this, "Select an add-on", "You must select an add-on to remove it");

        return;

    }

    QString addonName;

    QTreeWidgetItem *selectedItem = ui->dbTree->currentItem();

    if(selectedItem->childCount() == 0) addonName = selectedItem->parent()->text(0);

    else addonName = selectedItem->text(0);

    QMessageBox::StandardButton response = QMessageBox::question(this, "Remove confirm",
                                            "Are you sure you want to remove " + addonName + " from the database?");

    if(response == QMessageBox::No) return;

    dbMap.remove(addonName);

    setDbTree();

}

void SettingsWindow::on_editEntryButton_clicked(){

    if(ui->dbTree->selectedItems().isEmpty()){

        QMessageBox::warning(this, "Select an add-on", "You must select an add-on to edit it");

        return;

    }

    AddDbDialog addDialog(this);

    QTreeWidgetItem *selectedItem = ui->dbTree->currentItem();

    // If the selected was a file in the list
    if(selectedItem->childCount() == 0) {

        addDialog.addonName = (selectedItem->parent()->text(0));

        for(int index = 0; index < selectedItem->parent()->childCount(); index++){

            if(index == selectedItem->parent()->childCount() - 1) {

                addDialog.addonFiles += selectedItem->parent()->child(index)->text(0);

                break;

            }

            addDialog.addonFiles += selectedItem->parent()->child(index)->text(0) + ",";

        }

    }

    else {

        addDialog.addonName = (selectedItem->text(0));

        for(int index = 0; index < selectedItem->childCount(); index++){

            if(index == selectedItem->childCount() - 1) {

                addDialog.addonFiles += selectedItem->child(index)->text(0);

                break;

            }

            addDialog.addonFiles += selectedItem->child(index)->text(0) + ",";

        }

    }

    addDialog.filesChecked = true;

    addDialog.exec();

    if(!addDialog.check) return;

    dbMap.insert(addDialog.addonName, addDialog.addonFiles.split(","));

    setDbTree();

}

void SettingsWindow::setDbTree(){

    ui->dbTree->clear();

    int index = 0;

    foreach(QString addon, dbMap.keys()){

        QTreeWidgetItem *addonItem = new QTreeWidgetItem();

        addonItem->setText(0, addon);

        ui->dbTree->insertTopLevelItem(index, addonItem);

        QList<QString> filesList = dbMap.value(addon);

        QList<QTreeWidgetItem*> items;

        foreach(QString file, filesList){items.append(new QTreeWidgetItem(addonItem, file.split(",")));}

        ui->dbTree->insertTopLevelItems(index, items);

        index++;

    }

}

void SettingsWindow::closeEvent(QCloseEvent *bar){emit closed(); bar->accept();}

SettingsWindow::~SettingsWindow(){delete ui;}
