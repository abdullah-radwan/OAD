#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "configeditor.h"
#include "adddialog.h"
#include "addonsops.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QDirIterator>
#include <QFileDialog>

AddonsOps createAddonsOps() {

    // Read config, data returned as struct
    auto config = ConfigEditor::readConfig();

    QString orbiterPath = config.orbiterPath;

    return AddonsOps(orbiterPath, config.dbMap, config.overMap);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), addonsOps(createAddonsOps()){

    ui->setupUi(this);

    orbiterPath = addonsOps.orbiterPath;

    updateAddonsList();

    // Enable/Disable widgets based on Orbiter path
    setWidgets();
}

// Enable/Disable widgets based on Orbiter path
void MainWindow::setWidgets(){

    if(orbiterPath.isEmpty()){setWidgets(false); return;}

    if(QFileInfo(orbiterPath + "orbiter.exe").isFile()){

        setWidgets(true);

    } else {

        setWidgets(false);

        QMessageBox::warning(this, "Orbiter folder is wrong", "Please select the correct Orbiter folder");

        on_actionPath_triggered();

    }

}

void MainWindow::setWidgets(bool stat){

    ui->enabledAddonsList->setEnabled(stat);

    ui->disabledAddonsList->setEnabled(stat);

    ui->enableButton->setEnabled(stat);

    ui->disableButton->setEnabled(stat);

    ui->addButton->setEnabled(stat);

    ui->uninstallButton->setEnabled(stat);

}

void MainWindow::updateAddonsList(){

    ui->enabledAddonsList->clear();

    ui->disabledAddonsList->clear();

    auto lists = addonsOps.scanAddons();

    ui->enabledAddonsList->addItems(lists.enabledAddons);

    ui->disabledAddonsList->addItems(lists.disabledAddons);

}

void MainWindow::on_enableButton_clicked()
{
    // If nothing is selected
    if(ui->disabledAddonsList->selectedItems().isEmpty()){

        QMessageBox::warning(this, "Select an add-on", "You must select an add-on to enable it");

        return;

    }

    addonsOps.enableAddon(ui->disabledAddonsList->currentItem()->text());

    updateAddonsList();
}

void MainWindow::on_disableButton_clicked()
{
    if(ui->enabledAddonsList->selectedItems().isEmpty()){

        QMessageBox::warning(this, "Select an add-on", "You must select an add-on to disable it");

        return;

    }

    addonsOps.disableAddon(ui->enabledAddonsList->currentItem()->text());

    updateAddonsList();
}

// Install/Add button
void MainWindow::on_addButton_clicked()
{
    AddDialog addDialog;

    addDialog.exec();

    // If user didn't press Ok button in the dialog
    if(!addDialog.check) return;

    // If addon name or path is empty
    if(addDialog.name.isEmpty() || addDialog.path.isEmpty()){

        QMessageBox::critical(this, "Add-on hasn't installed/added",
                              "Add-on hasn't installed/added, you must enter the Add-on name or/and folder");

        return;

    }

    addonsOps.addAddon(addDialog.name, addDialog.path, addDialog.install, addDialog.installSdk, addDialog.removeAddonDir);

    updateAddonsList();

    QMessageBox::information(this, "Add-on installed/added successfully", addDialog.name + " installed/added successfully");
}

void MainWindow::on_uninstallButton_clicked()
{
    if(ui->enabledAddonsList->selectedItems().isEmpty()){

        QMessageBox::warning(this, "Select an add-on", "You must select an add-on to uninstall it");

        return;

    }

    QString addonName = ui->enabledAddonsList->currentItem()->text();

    QMessageBox::StandardButton response = QMessageBox::question(this, "Uninstallation confirm",
                                                                 "Are you sure you want to uninstall " + addonName + "?");

    if(response == QMessageBox::No) return;

    QString result = addonsOps.uninstallAddon(addonName);

    if(!result.isEmpty()){

        QMessageBox::warning(this, "Can't uninstall " + addonName,
                             "Can't uninstall " + addonName + ", You must uninstall " + result + " first");

        return;

    }

    updateAddonsList();

    QMessageBox::information(this, "Add-on uninstalled successfully", addonName + " uninstalled successfully");
}

void MainWindow::on_actionPath_triggered()
{
    QString path = QFileDialog::getExistingDirectory(this, "Select Orbiter installation folder", orbiterPath);

    if(path.isEmpty()) {return;};

    // Check if orbiter.exe exists in given path
    if(QFileInfo(path + "/orbiter.exe").exists()){

        orbiterPath = path + "/";

        setWidgets();

        addonsOps.orbiterPath = orbiterPath;

        updateAddonsList();

        ui->statusBar->showMessage("Orbiter folder is set to " + orbiterPath, 5000);

    }

    // Open file dialog again (Repeat this function)
    else {

        QMessageBox::warning(this, "Orbiter folder is wrong", "Please select the correct Orbiter folder");

        on_actionPath_triggered();

    }
}

void MainWindow::on_actionRescan_triggered(){updateAddonsList();}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About Orbiter Addons Manager","Orbiter Addons Manager <br>"
                                                            "A tool to organize your Orbiter add-ons <br> <br>"

                                                            "Version: 1.0.1 <br>"
                                                            "Build date: Sept 2018 <br>"
                                                            "Check for updates here: "
                                                            "<a href='http://bit.ly/2QKVXqV'>http://bit.ly/2QKVXqV</a> <br> <br>"

                                                            "Copyright © Abdullah Radwan <br>"
                                                            "Icon by icons8.com");
}

MainWindow::~MainWindow(){ConfigEditor::writeConfig(orbiterPath, addonsOps.dbMap, addonsOps.overMap); delete ui;}
