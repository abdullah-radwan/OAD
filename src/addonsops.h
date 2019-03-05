#ifndef ADDONSOPS_H
#define ADDONSOPS_H

#include <QMap>

class AddonsOps
{
public:
	QString orbiterPath, backupDir;

	QMap<QString, QStringList> dbMap, ignoredMap, installedMap, overMap;

	bool moveToTrash, showAll;

	struct addonsLists { QStringList enabledAddons, disabledAddons; };

	AddonsOps();

	AddonsOps(QString orbiterPath, QString backupDir, QMap<QString, QStringList> dbMap,
		QMap<QString, QStringList> ignoredMap, QMap<QString, QStringList> overMap,
		bool moveTrash, bool showAll);

	addonsLists scanAddons();

	void enableAddon(QString name);

	void disableAddon(QString name);

	int installAddon(QString name, QString path, bool compChecked, bool installSources, bool removeAddonDir);

	QStringList uninstallAddon(QString name);

	static QStringList scanDirectory(QString dirPath);

	static QString checkCompFile(QString path);

	static void moveTrash(QString file, bool moveTrash);

	static bool extract(QString archivePath, QString extractPath);

private:
	void setOverrider(QString addonName, QString addonFile);

	void setDbMap(QString addonName, QStringList addonFiles);

	bool checkIgn(QString addonName, QString file);

	bool checkInstalled(QString addonName, QStringList addonFiles);
};

#endif // ADDONSOPS_H
