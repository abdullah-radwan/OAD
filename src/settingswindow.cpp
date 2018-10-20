#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "addigndialog.h"
#include "adddbdialog.h"
#include <settingsops.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>

SettingsWindow::SettingsWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::SettingsWindow){ui->setupUi(this);}

SettingsWindow::SettingsWindow(QWidget *parent, QString orbiterPath, QString backupDir, QList<QString> pathsList,
                               QMap<QString, QList<QString>> dbMap, QMap<QString, QList<QString>> ignoredMap)

    : QMainWindow(parent), ui(new Ui::SettingsWindow){

    ui->setupUi(this);

    setWindowModality(Qt::ApplicationModal);

    settingsOps = SettingsOps(orbiterPath, backupDir, pathsList, dbMap, ignoredMap);
}

void SettingsWindow::showEvent(QShowEvent *ev){

     QMainWindow::showEvent(ev);

     ui->pathsCombo->addItems(settingsOps.pathsList);

     ui->pathsCombo->setCurrentText(settingsOps.orbiterPath);

     if(settingsOps.pathsList.isEmpty()){

         ui->snapBox->setDisabled(true);

         ui->removePathButton->setDisabled(true);

     }

     setDbTree();

     setIgnTree();

}

void SettingsWindow::on_pathsCombo_currentIndexChanged(const QString &arg1){

    settingsOps.orbiterPath = arg1;

    settingsOps.backupDir = QDir::currentPath() + "/OrbiterBackup " +
            QString::number(settingsOps.pathsList.indexOf(settingsOps.orbiterPath)) + "/";

}

void SettingsWindow::on_addPathButton_clicked(){

    QString path = QFileDialog::getExistingDirectory(this, "Select Orbiter installation folder", settingsOps.orbiterPath);

    if(path.isEmpty()) {return;};

    path += "/";

    if(settingsOps.pathsList.contains(path)){

        QMessageBox::warning(this, "Installation already exists",
                             "The installation you are trying to add is already added");

        return;

    }

    // Check if orbiter.exe exists in given path
    if(settingsOps.addPath(path)){

        ui->pathsCombo->addItem(path);

        ui->pathsCombo->setCurrentText(path);

        if(!ui->snapBox->isEnabled()) ui->snapBox->setEnabled(true);

        if(!ui->removePathButton->isEnabled()) ui->removePathButton->setEnabled(true);

    } else {

        QMessageBox::warning(this, "Orbiter folder is wrong", "Please select the correct Orbiter folder");

        on_addPathButton_clicked();

    }
}

void SettingsWindow::on_removePathButton_clicked(){

    int index = ui->pathsCombo->currentIndex();

    ui->pathsCombo->removeItem(index);

    if(settingsOps.rmPath(index)){

        ui->snapBox->setDisabled(true);

        ui->removePathButton->setDisabled(true);

    }

}

void SettingsWindow::on_createButton_clicked(){

    QMessageBox::information(this, "Snapshot created successfully", "Snapshot created successfully in "
                             + settingsOps.createSnapshot());

}

void SettingsWindow::on_importButton_clicked(){

    QString fileName = QFileDialog::getOpenFileName(this, "Select snapshot file", "", "Text file (*.txt)");

    if(fileName.isEmpty()) return;

    auto snapInfo = settingsOps.setSnapshot(fileName);

    if(snapInfo.files.isEmpty()){

        QMessageBox::warning(this, "Select a correct snapshot file", "Please selected a correct snapshot file.");

        return;
    }

    QString res = settingsOps.importSnapshot(snapInfo.files, snapInfo.snapfiles,
                                             checkSnapshot(snapInfo.files, snapInfo.snapfiles));

    if(res == "NO_BACKUP"){

        QMessageBox::warning(this, "Import failed",
                             "Can't import the snapshot because it doesn't have a back up");

        return;

    } else if(res == "SUCCESS"){

        QMessageBox::information(this, "Snapshot imported successfully", "Snapshot imported successfully");

    } else QMessageBox::information(this, "No changes were found", "No changes were made since the snapshot was taken");

}

bool SettingsWindow::checkSnapshot(QList<QString> snapFiles, QList<QString> files){

    foreach(QString snapFile, snapFiles){

        QString snapFileName = snapFile.split("=").first();

        QString snapFileTime = snapFile.split("=").last();

        foreach(QString file, files){

            QString fileName = file.split("=").first();

            QString fileTime = file.split("=").last();

            if(file == snapFile || file + ".oad" == snapFile || file == snapFile + ".oad") break;

            else if(fileName == snapFileName){

                if(!QFileInfo(settingsOps.backupDir + fileName).isFile()){

                    QMessageBox::StandardButtons response = QMessageBox::question(this, "Missing backup file",
                                                                                  fileName + " doesn't have a backup file."
                                                                                             " Do you want to skip it?");

                    if(response == QMessageBox::No) return true;

                }

            }

        }

    }

    return false;

}

void SettingsWindow::on_addIgnButton_clicked(){

    AddIgnDialog addDialog(this);

    addDialog.addons = settingsOps.dbMap.keys();

    addDialog.exec();

    if(!addDialog.check) return;

    if(addDialog.addonFiles.isEmpty()){

        QMessageBox::warning(this, "Enter the ignored file/folder", "You must enter a file/folder to add it");

        return;

    }

    settingsOps.addIgn(addDialog.addonName, addDialog.addonFiles);

    setIgnTree();
}

void SettingsWindow::on_removeIgnButton_clicked(){

    if(ui->ignoredTree->selectedItems().isEmpty()){

        QMessageBox::warning(this, "Select an add-on", "You must select a file/folder to remove it");

        return;

    }

    QTreeWidgetItem *selectedItem = ui->ignoredTree->currentItem();

    if(selectedItem->childCount() == 0){

        QList<QString> ignoredList = settingsOps.ignoredMap.value(selectedItem->parent()->text(0));

        ignoredList.removeOne(selectedItem->text(0));

        settingsOps.ignoredMap.insert(selectedItem->parent()->text(0), ignoredList);

    } else settingsOps.ignoredMap.remove(selectedItem->text(0));

    setIgnTree();
}

void SettingsWindow::setIgnTree(){

    ui->ignoredTree->clear();

    int index = 0;

    foreach(QString addon, settingsOps.ignoredMap.keys()){

        QTreeWidgetItem *addonItem = new QTreeWidgetItem();

        addonItem->setText(0, addon);

        ui->ignoredTree->insertTopLevelItem(index, addonItem);

        QList<QString> filesList = settingsOps.ignoredMap.value(addon);

        QList<QTreeWidgetItem*> items;

        foreach(QString file, filesList){items.append(new QTreeWidgetItem(addonItem, file.split(",")));}

        ui->ignoredTree->insertTopLevelItems(index, items);

        index++;

    }

}

void SettingsWindow::on_addEntryButton_clicked(){

    AddDbDialog addDialog(this);

    addDialog.folderChecked = true;

    addDialog.exec();

    if(!addDialog.check) return;

    if(addDialog.addonName.isEmpty() || (addDialog.addonFiles.isEmpty() && addDialog.addonPath.isEmpty())){

        QMessageBox::critical(this, "Enter the full add-on information",
                              "The add-on wasn't added. You must enter the add-on full information.");

        return;

    }

    settingsOps.addEntry(addDialog.addonName, addDialog.addonPath, addDialog.addonFiles,
                         addDialog.folderChecked, addDialog.fileChecked, addDialog.removeDir);

    setDbTree();

}

void SettingsWindow::on_removeEntryButton_clicked(){

    if(ui->dbTree->selectedItems().isEmpty()){

        QMessageBox::warning(this, "Select an add-on", "You must select an add-on to remove it");

        return;

    }

    QMessageBox::StandardButton response = QMessageBox::question(this, "Removition confirm",
                                            "Are you sure you want to remove the selected item from the database?");

    if(response == QMessageBox::No) return;

    QTreeWidgetItem *selectedItem = ui->dbTree->currentItem();

    if(selectedItem->childCount() == 0){

        QList<QString> filesList = settingsOps.dbMap.value(selectedItem->parent()->text(0));

        filesList.removeOne(selectedItem->text(0));

        settingsOps.dbMap.insert(selectedItem->parent()->text(0), filesList);

    } else settingsOps.dbMap.remove(selectedItem->text(0));

    setDbTree();

}

void SettingsWindow::on_editEntryButton_clicked(){

    if(ui->dbTree->selectedItems().isEmpty()){

        QMessageBox::warning(this, "Select an add-on", "You must select an add-on to edit it");

        return;

    }

    QString oldName;

    AddDbDialog addDialog(this);

    QTreeWidgetItem *selectedItem = ui->dbTree->currentItem();

    // If the selected was a file in the list
    if(selectedItem->childCount() == 0) {

        addDialog.addonName = selectedItem->parent()->text(0);

        oldName = selectedItem->parent()->text(0);

        for(int index = 0; index < selectedItem->parent()->childCount(); index++){

            if(index == selectedItem->parent()->childCount() - 1) {

                addDialog.addonFiles += selectedItem->parent()->child(index)->text(0);

                break;

            }

            addDialog.addonFiles += selectedItem->parent()->child(index)->text(0) + ",";

        }

    }

    else {

        addDialog.addonName = selectedItem->text(0);

        oldName = selectedItem->text(0);

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

    settingsOps.dbMap.remove(oldName);

    settingsOps.ignoredMap.insert(addDialog.addonName, settingsOps.ignoredMap.value(oldName));

    if(addDialog.addonName != oldName) settingsOps.ignoredMap.remove(oldName);

    settingsOps.dbMap.insert(addDialog.addonName, addDialog.addonFiles.split(","));

    setDbTree();

}

void SettingsWindow::setDbTree(){

    ui->dbTree->clear();

    int index = 0;

    foreach(QString addon, settingsOps.dbMap.keys()){

        QTreeWidgetItem *addonItem = new QTreeWidgetItem();

        addonItem->setText(0, addon);

        ui->dbTree->insertTopLevelItem(index, addonItem);

        QList<QString> filesList = settingsOps.dbMap.value(addon);

        QList<QTreeWidgetItem*> items;

        foreach(QString file, filesList){items.append(new QTreeWidgetItem(addonItem, file.split(",")));}

        ui->dbTree->insertTopLevelItems(index, items);

        index++;

    }

}

void SettingsWindow::closeEvent(QCloseEvent *bar){

    emit closed();

    bar->accept();

}

SettingsWindow::~SettingsWindow(){delete ui;}
