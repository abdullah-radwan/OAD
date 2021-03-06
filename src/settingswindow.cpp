#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "addigndialog.h"
#include "adddbdialog.h"
#include "opsdialog.h"
#include <settingsops.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>

SettingsWindow::SettingsWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::SettingsWindow) { ui->setupUi(this); }

SettingsWindow::SettingsWindow(QWidget *parent, QString orbiterPath, QString backupDir, QStringList pathsList,
	QMap<QString, QStringList> dbMap, QMap<QString, QStringList> ignoredMap,
	QMap<QString, QStringList> overMap, bool moveTrash, bool showAll)
	: QMainWindow(parent), ui(new Ui::SettingsWindow)
{
	ui->setupUi(this);

	setWindowModality(Qt::ApplicationModal);

	settingsOps = SettingsOps(orbiterPath, backupDir, pathsList, dbMap, ignoredMap, overMap, moveTrash);

	this->showAll = showAll;
}

void SettingsWindow::showEvent(QShowEvent *ev)
{
	QMainWindow::showEvent(ev);

	if (settingsOps.pathsList.isEmpty())
	{
		ui->snapBox->setDisabled(true);

		ui->removePathButton->setDisabled(true);
	}

	QString path = settingsOps.orbiterPath;

	ui->pathsCombo->addItems(settingsOps.pathsList);

	settingsOps.orbiterPath = path;

	ui->pathsCombo->setCurrentIndex(settingsOps.pathsList.indexOf(settingsOps.orbiterPath));

#ifdef Q_OS_WIN
	ui->trashCheck->setChecked(settingsOps.moveTrash);

#else
	ui->trashCheck->setChecked(false);

	ui->trashCheck->setDisabled(true);
#endif

	ui->showCheck->setChecked(showAll);

	setDbTree();

	setIgnTree();

	setOverTree();
}

void SettingsWindow::on_pathsCombo_currentTextChanged(const QString &arg1)
{
	settingsOps.orbiterPath = arg1;

	settingsOps.backupDir = QDir::currentPath() + "/OrbiterBackup " +
		QString::number(settingsOps.pathsList.indexOf(arg1)) + "/";
}

void SettingsWindow::on_addPathButton_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, "Select Orbiter installation folder", settingsOps.orbiterPath);

	if (path.isEmpty()) return;

	path += "/";

	if (settingsOps.pathsList.contains(path))
	{
		QMessageBox::warning(this, "Installation already exists",
			"The installation you are trying to add is already added");

		return;
	}

	// Check if orbiter.exe exists in given path
	if (settingsOps.addPath(path))
	{
		ui->pathsCombo->addItem(path);

		ui->pathsCombo->setCurrentText(path);

		if (!ui->snapBox->isEnabled()) ui->snapBox->setEnabled(true);

		if (!ui->removePathButton->isEnabled()) ui->removePathButton->setEnabled(true);

	}
	else
	{
		QMessageBox::warning(this, "Orbiter folder is wrong", "Please select the correct Orbiter folder");

		on_addPathButton_clicked();
	}
}

void SettingsWindow::on_removePathButton_clicked()
{
	int index = ui->pathsCombo->currentIndex();

	ui->pathsCombo->removeItem(index);

	if (settingsOps.rmPath(index))
	{
		ui->snapBox->setDisabled(true);

		ui->removePathButton->setDisabled(true);
	}
}

void SettingsWindow::on_createButton_clicked()
{
	QMessageBox::information(this, "Snapshot created successfully", "Snapshot created successfully in "
		+ settingsOps.createSnapshot());
}

void SettingsWindow::on_importButton_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Select snapshot file", "", "Text file (*.txt)");

	if (fileName.isEmpty()) return;

	OpsDialog opsDialog(this, "Importing the snapshot",
		"Importing " + fileName + "...");

	opsDialog.show();

	auto snapInfo = settingsOps.setSnapshot(fileName);

	if (snapInfo.files.isEmpty())
	{
		opsDialog.hide();

		opsDialog.deleteLater();

		QMessageBox::warning(this, "Select a correct snapshot file", "Please selected a correct snapshot file.");

		return;
	}
	else if (snapInfo.files.first() == "NO_CHANGE")
	{
		opsDialog.hide();

		opsDialog.deleteLater();

		QMessageBox::information(this, "No changes were found", "No changes were made since the snapshot was taken");
	}

	QString res = settingsOps.importSnapshot(snapInfo.files, snapInfo.snapNames, snapInfo.snapTimes,
		checkSnapshot(snapInfo.files, snapInfo.snapNames, snapInfo.snapTimes));

	if (res == "NO_BACKUP")
	{
		opsDialog.hide();

		opsDialog.deleteLater();

		QMessageBox::warning(this, "Import failed",
			"Can't import the snapshot because it doesn't have a back up");

		return;
	}
	else if (res == "SUCCESS")
	{
		opsDialog.hide();

		opsDialog.deleteLater();

		QMessageBox::information(this, "Snapshot imported successfully", "Snapshot imported successfully");
	}
}

bool SettingsWindow::checkSnapshot(QStringList files, QStringList snapNames, QStringList snapTimes)
{
	foreach(QString file, files)
	{
		QString fileName = file.split("=").first();

		QString fileTime = file.split("=").last();

		if ((snapNames.contains(fileName, Qt::CaseInsensitive) && snapTimes.contains(fileTime)) ||
			snapNames.contains(fileName + ".oad", Qt::CaseInsensitive) ||
			(fileName.endsWith(".oad") && snapNames.contains(fileName.split(".oad").first(), Qt::CaseInsensitive)))
		{
		}
		else if (snapNames.contains(fileName, Qt::CaseInsensitive))
		{
			if (!QFileInfo(settingsOps.backupDir + fileName).isFile())
			{
				QMessageBox::StandardButtons response = QMessageBox::question(this, "Missing backup file",
					fileName + " doesn't have a backup file."
					" Do you want to skip it?");

				if (response == QMessageBox::No) return true;
			}
		}

		QCoreApplication::processEvents();
	}

	return false;
}

void SettingsWindow::on_addIgnButton_clicked()
{
	AddIgnDialog addDialog(this);

	addDialog.addons = settingsOps.dbMap.keys();

	addDialog.exec();

	if (!addDialog.check) return;

	if (addDialog.addonFiles.isEmpty())
	{
		QMessageBox::warning(this, "Enter the ignored file/folder", "You must enter a file/folder to add it");

		return;
	}

	settingsOps.addIgn(addDialog.addonName, addDialog.addonFiles);

	setIgnTree();
}

void SettingsWindow::on_removeIgnButton_clicked()
{
	if (ui->ignoredTree->selectedItems().isEmpty())
	{
		QMessageBox::warning(this, "Select an add-on", "You must select a file/folder to remove it");

		return;
	}

	QTreeWidgetItem *selectedItem = ui->ignoredTree->currentItem();

	if (selectedItem->childCount() == 0)
	{
		QStringList ignoredList = settingsOps.ignoredMap.value(selectedItem->parent()->text(0));

		ignoredList.removeOne(selectedItem->text(0));

		settingsOps.ignoredMap.insert(selectedItem->parent()->text(0), ignoredList);
	}
	else settingsOps.ignoredMap.remove(selectedItem->text(0));

	setIgnTree();
}

void SettingsWindow::setIgnTree()
{
	ui->ignoredTree->clear();

	int index = 0;

	foreach(QString addon, settingsOps.ignoredMap.keys())
	{
		QTreeWidgetItem *addonItem = new QTreeWidgetItem();

		addonItem->setText(0, addon);

		ui->ignoredTree->insertTopLevelItem(index, addonItem);

		QStringList filesList = settingsOps.ignoredMap.value(addon);

		QList<QTreeWidgetItem*> items;

		foreach(QString file, filesList) items.append(new QTreeWidgetItem(addonItem, file.split(",")));

		ui->ignoredTree->insertTopLevelItems(index, items);

		index++;

	}
}

void SettingsWindow::on_trashCheck_toggled(bool checked) { settingsOps.moveTrash = checked; }

void SettingsWindow::on_addEntryButton_clicked()
{
	AddDbDialog addDialog(this);

	addDialog.folderChecked = true;

	addDialog.exec();

	if (!addDialog.check) return;

	if (addDialog.addonName.isEmpty() || (addDialog.addonFiles.isEmpty() && addDialog.addonPath.isEmpty()))
	{
		QMessageBox::critical(this, "Enter the full add-on information",
			"The add-on wasn't added. You must enter the add-on full information.");

		return;
	}

	bool res = settingsOps.addEntry(addDialog.addonName, addDialog.addonPath, addDialog.addonFiles,
		addDialog.folderChecked, addDialog.fileChecked, addDialog.removeDir);

	if (res) QMessageBox::information(this, addDialog.addonName + " added successfully",
		addDialog.addonName + " added successfully to the database.");

	else QMessageBox::critical(this, addDialog.addonName + " addition failed",
		addDialog.addonName + " addition failed.");

	setDbTree();
}

void SettingsWindow::on_removeEntryButton_clicked()
{
	if (ui->dbTree->selectedItems().isEmpty())
	{
		QMessageBox::warning(this, "Select an add-on", "You must select an add-on to remove it");

		return;
	}

	QMessageBox::StandardButton response = QMessageBox::question(this, "Removition confirm",
		"Are you sure you want to remove the selected item from the database?");

	if (response == QMessageBox::No) return;

	QTreeWidgetItem *selectedItem = ui->dbTree->currentItem();

	if (selectedItem->childCount() == 0)
	{
		QStringList filesList = settingsOps.dbMap.value(selectedItem->parent()->text(0));

		filesList.removeOne(selectedItem->text(0));

		settingsOps.dbMap.insert(selectedItem->parent()->text(0), filesList);
	}
	else settingsOps.dbMap.remove(selectedItem->text(0));

	setDbTree();
}

void SettingsWindow::on_editEntryButton_clicked()
{
	if (ui->dbTree->selectedItems().isEmpty())
	{
		QMessageBox::warning(this, "Select an add-on", "You must select an add-on to edit it");

		return;
	}

	QString oldName;

	AddDbDialog addDialog(this);

	QTreeWidgetItem *selectedItem = ui->dbTree->currentItem();

	// If the selected was a file in the list
	if (selectedItem->childCount() == 0)
	{
		addDialog.addonName = selectedItem->parent()->text(0);

		oldName = selectedItem->parent()->text(0);

		for (int index = 0; index < selectedItem->parent()->childCount(); index++)
		{
			if (index == selectedItem->parent()->childCount() - 1)
			{
				addDialog.addonFiles += selectedItem->parent()->child(index)->text(0);

				break;
			}

			addDialog.addonFiles += selectedItem->parent()->child(index)->text(0) + ",";

		}
	}
	else
	{

		addDialog.addonName = selectedItem->text(0);

		oldName = selectedItem->text(0);

		for (int index = 0; index < selectedItem->childCount(); index++)
		{
			if (index == selectedItem->childCount() - 1)
			{
				addDialog.addonFiles += selectedItem->child(index)->text(0);

				break;
			}

			addDialog.addonFiles += selectedItem->child(index)->text(0) + ",";
		}
	}

	addDialog.filesChecked = true;

	addDialog.exec();

	if (!addDialog.check) return;

	settingsOps.editEntry(addDialog.addonName, oldName);

	settingsOps.dbMap.insert(addDialog.addonName, addDialog.addonFiles.split(","));

	setDbTree();
}

void SettingsWindow::setDbTree()
{
	ui->dbTree->clear();

	int index = 0;

	foreach(QString addon, settingsOps.dbMap.keys())
	{
		QTreeWidgetItem *addonItem = new QTreeWidgetItem();

		addonItem->setText(0, addon);

		ui->dbTree->insertTopLevelItem(index, addonItem);

		QStringList filesList = settingsOps.dbMap.value(addon);

		QList<QTreeWidgetItem*> items;

		foreach(QString file, filesList) items.append(new QTreeWidgetItem(addonItem, file.split(",")));

		ui->dbTree->insertTopLevelItems(index, items);

		index++;
	}
}

void SettingsWindow::on_remOverButton_clicked()
{
	if (ui->overTree->selectedItems().isEmpty())
	{
		QMessageBox::warning(this, "Select an add-on", "You must select an add-on to remove it");

		return;
	}

	QMessageBox::StandardButton response = QMessageBox::question(this, "Removition confirm",
		"Are you sure you want to remove the selected item from the database?");

	if (response == QMessageBox::No) return;

	QTreeWidgetItem *selectedItem = ui->overTree->currentItem();

	if (selectedItem->childCount() == 0)
	{
		QStringList addonsList = settingsOps.overMap.value(selectedItem->parent()->text(0));

		addonsList.removeOne(selectedItem->text(0));

		settingsOps.overMap.insert(selectedItem->parent()->text(0), addonsList);
	}
	else settingsOps.overMap.remove(selectedItem->text(0));

	setOverTree();
}

void SettingsWindow::setOverTree()
{
	ui->overTree->clear();

	int index = 0;

	foreach(QString addon, settingsOps.overMap.keys())
	{
		QTreeWidgetItem *addonItem = new QTreeWidgetItem();

		addonItem->setText(0, addon);

		ui->overTree->insertTopLevelItem(index, addonItem);

		QStringList addonsList = settingsOps.overMap.value(addon);

		QList<QTreeWidgetItem*> items;

		foreach(QString overAddon, addonsList) items.append(new QTreeWidgetItem(addonItem, overAddon.split(",")));

		ui->overTree->insertTopLevelItems(index, items);

		index++;
	}
}
void SettingsWindow::closeEvent(QCloseEvent *bar)
{
	showAll = ui->showCheck->isChecked();

	emit closed();

	bar->accept();
}

SettingsWindow::~SettingsWindow() { delete ui; }
