#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <settingsops.h>

namespace Ui { class SettingsWindow; }

class SettingsWindow : public QMainWindow 
{
	Q_OBJECT
public:
	SettingsOps settingsOps;

	bool showAll;

	explicit SettingsWindow(QWidget *parent = nullptr);

	SettingsWindow(QWidget *parent, QString orbiterPath, QString backupDir, QStringList pathsList,
		QMap<QString, QStringList> dbMap, QMap<QString, QStringList> ignoredMap,
		QMap<QString, QStringList> overMap, bool moveTrash, bool showAll);

	~SettingsWindow();

protected:
	void showEvent(QShowEvent *ev);

private slots:
	void on_addPathButton_clicked();

	void on_removePathButton_clicked();

	void on_createButton_clicked();

	void on_importButton_clicked();

	void on_addEntryButton_clicked();

	void on_removeEntryButton_clicked();

	void on_editEntryButton_clicked();

	void on_addIgnButton_clicked();

	void on_removeIgnButton_clicked();

	void on_trashCheck_toggled(bool checked);

	void on_pathsCombo_currentTextChanged(const QString &arg1);

	void on_remOverButton_clicked();

signals:
	void closed();

private:
	Ui::SettingsWindow *ui;

	bool checkSnapshot(QStringList fileNames, QStringList fileTimes, QStringList snapFiles);

	void setDbTree();

	void setIgnTree();

	void setOverTree();

	QString scanDir(QString path);

	void closeEvent(QCloseEvent *bar);
};

#endif // SETTINGSWINDOW_H
