#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "addonsops.h"
#include "settingswindow.h"
#include <QMainWindow>

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow();

private slots:
	void on_actionSettings_triggered();

	void on_actionRescan_triggered();

	void on_actionAbout_triggered();

	void on_installButton_clicked();

	void on_uninstallButton_clicked();

	void on_disableButton_clicked();

	void on_enableButton_clicked();

private:
	Ui::MainWindow *ui;

	QString orbiterPath;

	QStringList pathsList;

	AddonsOps addonsOps;

	void updateAddonsList();

	void setWidgets();
};

#endif // MAINWINDOW_H
