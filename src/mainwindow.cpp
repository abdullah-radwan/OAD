#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "configeditor.h"
#include "installdialog.h"
#include "opsdialog.h"
#include "addonsops.h"
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// Read config, data returned as struct
	auto config = ConfigEditor::readConfig();

	orbiterPath = config.orbiterPath;

	pathsList = config.pathsList;

	QString backupDir;

	if (!orbiterPath.isEmpty()) backupDir = QDir::currentPath() +
		"/OrbiterBackup " + QString::number(pathsList.indexOf(orbiterPath)) + "/";

	addonsOps = AddonsOps(orbiterPath, backupDir, config.dbMap,
		config.ignoredMap, config.overMap, config.moveTrash, false);

	updateAddonsList();

	setWidgets();
}

// Enable/Disable widgets based on Orbiter path
void MainWindow::setWidgets()
{
	if (orbiterPath.isEmpty()) { ui->centralWidget->setEnabled(false); return; }

	if (QFileInfo(orbiterPath + "orbiter.exe").isFile()) ui->centralWidget->setEnabled(true);

	else {
		ui->centralWidget->setEnabled(false);

		QMessageBox::warning(this, "Wrong Orbiter installation", "Please add a correct Orbiter installation");
	}
}

void MainWindow::updateAddonsList()
{
	ui->enabledAddonsList->clear();

	ui->disabledAddonsList->clear();

    OpsDialog opsDialog(this, "Scanning for add-ons", "Scanning for add-ons...");

    opsDialog.show();

    auto lists = addonsOps.scanAddons();

    opsDialog.hide();

    opsDialog.deleteLater();

	ui->enabledAddonsList->addItems(lists.enabledAddons);

	ui->disabledAddonsList->addItems(lists.disabledAddons);
}

void MainWindow::on_enableButton_clicked()
{
	// If nothing is selected
	if (ui->disabledAddonsList->selectedItems().isEmpty()) {
		QMessageBox::warning(this, "Select an add-on", "You must select an add-on to enable it");

		return;
	}

	QString addonName = ui->disabledAddonsList->currentItem()->text();

	OpsDialog opsDialog(this, "Enabling " + addonName,
		"Enabling " + addonName + "...");

	opsDialog.show();

	addonsOps.enableAddon(addonName);

	opsDialog.hide();

	opsDialog.deleteLater();

	updateAddonsList();
}

void MainWindow::on_disableButton_clicked()
{
	if (ui->enabledAddonsList->selectedItems().isEmpty())
	{
		QMessageBox::warning(this, "Select an add-on", "You must select an add-on to disable it");

		return;
	}

	QString addonName = ui->enabledAddonsList->currentItem()->text();

	OpsDialog opsDialog(this, "Disabling " + addonName,
		"Disabling " + addonName + "...");

	opsDialog.show();

	addonsOps.disableAddon(addonName);

	opsDialog.hide();

	opsDialog.deleteLater();

	updateAddonsList();
}

// Install button
void MainWindow::on_installButton_clicked()
{
	InstallDialog installDialog(this);

	installDialog.exec();

	// If user didn't press Install button in the dialog
	if (!installDialog.check) return;

	// If addon name or path is empty
	if (installDialog.addonName.isEmpty() || installDialog.addonPath.isEmpty())
	{
		QMessageBox::critical(this, "Enter the add-on's full information",
			"Add-on wasn't installed. You must enter the add-on name or/and path.");

		return;
	}

	OpsDialog opsDialog(this, "Installing " + installDialog.addonName,
		"Installing " + installDialog.addonName + "...");

	opsDialog.show();

	int res = addonsOps.installAddon(installDialog.addonName, installDialog.addonPath, installDialog.compChecked,
		installDialog.installSources, installDialog.removeAddonDir);

	opsDialog.deleteLater();

	switch (res)
	{
		// Failed
	case -1:
		QMessageBox::critical(this, "Add-on installation failed",
			installDialog.addonName + " installation failed");
		return;
		// Already exists
	case 0:
		QMessageBox::warning(this, "Add-on already installed",
			installDialog.addonName + " is already installed");
		return;
		// Sucess
	case 1:
		break;
	}

	updateAddonsList();

	QMessageBox::information(this, "Add-on installed successfully",
		installDialog.addonName + " installed successfully");
}

void MainWindow::on_uninstallButton_clicked()
{
	if (ui->enabledAddonsList->selectedItems().isEmpty())
	{
		QMessageBox::warning(this, "Select an add-on", "You must select an add-on to uninstall it");

		return;
	}

	QString addonName = ui->enabledAddonsList->currentItem()->text();

	QMessageBox::StandardButton response = QMessageBox::question(this, "Uninstallation confirm",
		"Are you sure you want to uninstall " + addonName + "?");

	if (response == QMessageBox::No) return;

	OpsDialog opsDialog(this, "Uninstalling " + addonName,
		"Uninstalling " + addonName + "...");

	opsDialog.show();

	QStringList result = addonsOps.uninstallAddon(addonName);

	if (!result.isEmpty())
	{
		if (result.first() == "OVERRIDE")
		{
			opsDialog.hide();

			opsDialog.deleteLater();

			QMessageBox::warning(this, "Can't uninstall " + addonName,
				"Can't uninstall " + addonName +
				", You must uninstall the following add-ons first:\n" + result.last());

			return;
		}
		else
		{
			opsDialog.hide();

			opsDialog.deleteLater();

			QMessageBox::critical(this, addonName + " Uninstallation failed",
				"Couldn't remove the following files:\n" + result.last());
		}
	}

	updateAddonsList();

	QMessageBox::information(this, "Add-on uninstalled successfully", addonName + " uninstalled successfully");
}

void MainWindow::on_actionSettings_triggered()
{
	SettingsWindow settingsWin(this, orbiterPath, addonsOps.backupDir, pathsList,
		addonsOps.dbMap, addonsOps.ignoredMap, addonsOps.overMap,
		addonsOps.moveToTrash, addonsOps.showAll);

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

	addonsOps.overMap = settingsWin.settingsOps.overMap;

	addonsOps.moveToTrash = settingsWin.settingsOps.moveTrash;

	addonsOps.showAll = settingsWin.showAll;

	updateAddonsList();
}

void MainWindow::on_actionRescan_triggered() { updateAddonsList(); }

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::about(this, "About Orbiter Addons Manager", "Orbiter Addons Manager <br>"
		"A tool to organize your Orbiter add-ons <br> <br>"

		"Version: 1.0.5 <br>"
		"Build date: Feb 2019 <br>"
		"Check for updates on <a href='https://github.com/abdullah-radwan/OAD/releases'>"
		"Github</a>. <br> <br>"

		"Copyright © Abdullah Radwan <br>"
		"Icon by icons8.com");
}

MainWindow::~MainWindow()
{
	ConfigEditor::writeConfig({ orbiterPath, pathsList, addonsOps.dbMap, addonsOps.ignoredMap,
							   addonsOps.overMap, addonsOps.moveToTrash });

	delete ui;
}
