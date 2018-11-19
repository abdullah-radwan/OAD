#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "configeditor.h"
#include "installdialog.h"
#include "addonsops.h"
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){

    ui->setupUi(this);

    // Read config, data returned as struct
    auto config = ConfigEditor::readConfig();

    orbiterPath = config.orbiterPath;

    pathsList = config.pathsList;

    QString backupDir;

    if(!orbiterPath.isEmpty()) backupDir = QDir::currentPath() +
            "/OrbiterBackup " + QString::number(pathsList.indexOf(orbiterPath)) + "/";

    addonsOps = AddonsOps(orbiterPath, backupDir, config.dbMap,
                          config.ignoredMap, config.overMap, config.moveTrash);

    updateAddonsList();

    setWidgets();
}

// Enable/Disable widgets based on Orbiter path
void MainWindow::setWidgets(){

    if(orbiterPath.isEmpty()){setWidgets(false); return;}

    if(QFileInfo(orbiterPath + "orbiter.exe").isFile()){

        setWidgets(true);

    } else {

        setWidgets(false);

        QMessageBox::warning(this, "Wrong Orbiter installation", "Please add a correct Orbiter installation");

    }

}

void MainWindow::setWidgets(bool stat){

    ui->enabledGroup->setEnabled(stat);

    ui->disabledGroup->setEnabled(stat);

    ui->installButton->setEnabled(stat);

    ui->uninstallButton->setEnabled(stat);

}

void MainWindow::updateAddonsList(){

    ui->enabledAddonsList->clear();

    ui->disabledAddonsList->clear();

    auto lists = addonsOps.scanAddons();

    ui->enabledAddonsList->addItems(lists.enabledAddons);

    ui->disabledAddonsList->addItems(lists.disabledAddons);

}

void MainWindow::on_enableButton_clicked(){

    // If nothing is selected
    if(ui->disabledAddonsList->selectedItems().isEmpty()){

        QMessageBox::warning(this, "Select an add-on", "You must select an add-on to enable it");

        return;

    }

    addonsOps.enableAddon(ui->disabledAddonsList->currentItem()->text());

    updateAddonsList();
}

void MainWindow::on_disableButton_clicked(){

    if(ui->enabledAddonsList->selectedItems().isEmpty()){

        QMessageBox::warning(this, "Select an add-on", "You must select an add-on to disable it");

        return;

    }

    addonsOps.disableAddon(ui->enabledAddonsList->currentItem()->text());

    updateAddonsList();
}

// Install button
void MainWindow::on_installButton_clicked(){

    InstallDialog installDialog(this);

    installDialog.exec();

    // If user didn't press Install button in the dialog
    if(!installDialog.check) return;

    // If addon name or path is empty
    if(installDialog.addonName.isEmpty() || installDialog.addonPath.isEmpty()){

        QMessageBox::critical(this, "Enter the add-on's full information",
                              "Add-on wasn't installed. You must enter the add-on name or/and path.");

        return;

    }

    bool res = addonsOps.installAddon(installDialog.addonName, installDialog.addonPath, installDialog.compChecked,
                       installDialog.installSources, installDialog.removeAddonDir);

    if(!res) {

        QMessageBox::warning(this, "Add-on installation failed",
                                 installDialog.addonName + " installation failed");

        return;

    }

    updateAddonsList();

    QMessageBox::information(this, "Add-on installed successfully",
                             installDialog.addonName + " installed successfully");
}

void MainWindow::on_uninstallButton_clicked(){

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

void MainWindow::on_actionSettings_triggered(){

    SettingsWindow settingsWin(this, orbiterPath, addonsOps.backupDir, pathsList,
                               addonsOps.dbMap, addonsOps.ignoredMap, addonsOps.overMap, addonsOps.moveToTrash);

    settingsWin.show();

    QEventLoop loop;

    connect(&settingsWin, SIGNAL(closed()), &loop, SLOT(quit()));

    loop.exec();

    orbiterPath = settingsWin.settingsOps.orbiterPath;

    setWidgets();

    pathsList = settingsWin.settingsOps.pathsList;

    addonsOps.orbiterPath = orbiterPath;

    addonsOps.dbMap = settingsWin.settingsOps.dbMap;

    addonsOps.backupDir = settingsWin.settingsOps.backupDir;

    addonsOps.ignoredMap = settingsWin.settingsOps.ignoredMap;

    addonsOps.overMap = settingsWin.overMap;

    addonsOps.moveToTrash = settingsWin.settingsOps.moveTrash;

    updateAddonsList();

}

void MainWindow::on_actionRescan_triggered(){updateAddonsList();}

void MainWindow::on_actionAbout_triggered(){

    QMessageBox::about(this, "About Orbiter Addons Manager","Orbiter Addons Manager <br>"
                                                            "A tool to organize your Orbiter add-ons <br> <br>"

                                                            "Version: 1.0.4 <br>"
                                                            "Build date: Nov 2018 <br>"
                                                            "Check for updates here: "
                                                            "<a href='http://bit.ly/2QKVXqV'>http://bit.ly/2QKVXqV</a> <br> <br>"

                                                            "Copyright © Abdullah Radwan <br>"
                                                            "Icon by icons8.com");
}

MainWindow::~MainWindow(){

    ConfigEditor::writeConfig(orbiterPath, pathsList, addonsOps.dbMap, addonsOps.ignoredMap,
                              addonsOps.overMap, addonsOps.moveToTrash);

    delete ui;
}
