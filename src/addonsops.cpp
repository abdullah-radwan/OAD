#include "addonsops.h"
#include "zipthread.h"
#include <QDirIterator>
#include <QCoreApplication>
#include <QThread>

static QStringList orbiterFolders = { "config", "doc", "flights", "html", "images", "install", "meshes", "missions",
									 "modules", "orbitersdk", "scenarios", "script", "textures", "textures2", "utils" };

AddonsOps::AddonsOps() {}

AddonsOps::AddonsOps(QString orbiterPath, QString backupDir, QMap<QString, QStringList> dbMap,
	QMap<QString, QStringList> ignoredMap, QMap<QString, QStringList> overMap,
	bool moveTrash, bool showAll)
{
	this->orbiterPath = orbiterPath;

	this->dbMap = dbMap;

	this->ignoredMap = ignoredMap;

	this->overMap = overMap;

	this->moveToTrash = moveTrash;

	this->showAll = showAll;

	this->backupDir = backupDir;

	QDir checkBackupDir(backupDir);

	if (!checkBackupDir.exists()) checkBackupDir.mkpath(".");
}

AddonsOps::addonsLists AddonsOps::scanAddons()
{
	installedMap.clear();

	if (showAll) return { dbMap.keys(), {} };

	QStringList enabledAddons, disabledAddons;

	foreach(QString addon, dbMap.keys())
	{
		bool enabledExists = false;

		bool disabledExists = false;

		foreach(QString file, dbMap.value(addon))
		{
			if (!checkIgn(addon, file))
			{
				if (QFileInfo::exists(orbiterPath + file)) enabledExists = true;

				else if (QFileInfo::exists(orbiterPath + file + ".oad")) disabledExists = true;

				// Go to the next add-on if a file doesn't exists
				else { enabledExists = false; disabledExists = false; break; }
			}

			QCoreApplication::processEvents();
		}

		if (enabledExists && !enabledAddons.contains(addon, Qt::CaseInsensitive))
		{
			enabledAddons.append(addon);
			installedMap.insert(addon, dbMap.value(addon));
		}
		else if (disabledExists && !disabledAddons.contains(addon, Qt::CaseInsensitive))
		{
			disabledAddons.append(addon);
			installedMap.insert(addon, dbMap.value(addon));
		}
	}

	return { enabledAddons, disabledAddons };
}

void AddonsOps::enableAddon(QString name)
{
	QStringList overNames;

	bool needRemove = false;

	if (overMap.contains(name)) { overNames = overMap.value(name); needRemove = true; }
	else {
		foreach(QString addon, overMap.keys())
			if (overMap.value(addon).contains(name)) { overNames = overMap.value(addon); break; }
	}

	if (!overNames.isEmpty())
	{
		// Enable each file from the overrider add-on
		foreach(QString overName, overNames)
		{
			foreach(QString dbFile, dbMap.value(overName))
			{
				QString absPath = orbiterPath + dbFile + ".oad";

				QFile::rename(absPath, orbiterPath + dbFile);

				QCoreApplication::processEvents();
			}
		}
	}

	if (needRemove)
	{
		foreach(QString file, dbMap.value(name))
		{
			QString absPath = orbiterPath + file + ".oad";

			if (QFileInfo(absPath).exists()) QFile::rename(absPath, orbiterPath + file);

			QCoreApplication::processEvents();
		}
	}
}

void AddonsOps::disableAddon(QString addonName)
{
	QStringList overNames;

	bool needRemove = false;

	if (overMap.contains(addonName)) { overNames = overMap.value(addonName); needRemove = true; }
	else
	{
		foreach(QString addon, overMap.keys())
		{
			if (overMap.value(addon).contains(addonName)) { overNames = overMap.value(addon); break; }
			QCoreApplication::processEvents();
		}
	}

	if (!overNames.isEmpty())
	{
		// Enable each file from the overrider add-on
		foreach(QString overName, overNames)
		{
			foreach(QString dbFile, dbMap.value(overName))
			{
				QString absPath = orbiterPath + dbFile;

				if (!checkIgn(addonName, dbFile)) QFile::rename(absPath, absPath + ".oad");

				QCoreApplication::processEvents();
			}
		}
	}

	if (needRemove)
	{
		foreach(QString file, dbMap.value(addonName))
		{
			if (!checkIgn(addonName, file))
			{
				QFileInfo absPath(orbiterPath + file);

				// If the file isn't already disabled
				if (absPath.exists()) { QFile::rename(absPath.filePath(), absPath.filePath() + ".oad"); }
			}

			QCoreApplication::processEvents();
		}
	}
}

int AddonsOps::installAddon(QString name, QString path, bool compChecked, bool installSources, bool removeAddonDir)
{
	bool installFail = false;

	QStringList installedList;

	QString compPath = path;

	if (compChecked)
	{
		QString checkRes = checkCompFile(path);

		if (checkRes.isEmpty())
		{
			QString tempPath = QDir::currentPath() + "/" + name;

			QDir(tempPath).mkdir(".");

			if (!extract(path, tempPath)) return -1;

			path = tempPath + "/";
		}
		else
		{
			if (!extract(path, QDir::currentPath())) return -1;

			path = QDir::currentPath() + "/" + checkRes;
		}
	}

	QStringList files = scanDirectory(path);

	files.sort();

	if (installedMap.values().contains(files) || installedMap.keys().contains(name)) return 0;

	foreach(QString file, files)
	{
		if ((file.startsWith("Orbitersdk", Qt::CaseInsensitive) || file.startsWith("Sources", Qt::CaseInsensitive))
			&& !installSources) continue;

		QString filePath = orbiterPath + file;

		QFileInfo fileInfo(filePath);

		// Create the parent directory to the addon if not exists
		if (!fileInfo.dir().exists()) fileInfo.dir().mkpath(".");

		// If a file in the addon exists in Orbiter
		if (fileInfo.isFile())
		{
			setOverrider(name, file);

			if (file.lastIndexOf("/") != -1)
			{
				// Set the parent directory to the file
				QDir parentBackDir(backupDir + file.left(file.lastIndexOf("/")));

				// Create the parent directory if not exists in the backup path
				if (!parentBackDir.exists()) parentBackDir.mkpath(".");
			}

			if (QFile::copy(filePath, backupDir + file)) moveTrash(filePath, moveToTrash);

			if (!QFile::copy(path + file, filePath)) { removeAddonDir = false; installFail = true; break; }

			else installedList.append(file);

		}
		else if (QFileInfo(filePath + ".oad").isFile())
		{
			setOverrider(name, file);

			// Set the parent directory to the file
			QDir parentBackDir(backupDir + file.left(file.lastIndexOf("/")));

			// Create the parent directory if not exists in the backup path
			if (!parentBackDir.exists()) parentBackDir.mkpath(".");

			filePath += ".oad";

			if (QFile::copy(filePath, backupDir + file + ".oad")) moveTrash(filePath, moveToTrash);

			if (!QFile::copy(path + file, filePath)) { removeAddonDir = false; installFail = true; break; }

			else installedList.append(file);
		}
		else
		{
			if (!QFile::copy(path + file, filePath)) { removeAddonDir = false; installFail = true; break; }

			else installedList.append(file);
		}

		QCoreApplication::processEvents();
	}

	if (compChecked) QDir(path).removeRecursively();

	if (installFail)
	{
		foreach(QString file, installedList) moveTrash(orbiterPath + file, moveToTrash);

		return -1;
	}

	if (compChecked && removeAddonDir) moveTrash(compPath, moveToTrash);

	else if (removeAddonDir) moveTrash(path, moveToTrash);

	setDbMap(name, files);

	return 1;
}

bool AddonsOps::extract(QString archivePath, QString extractPath)
{
    QThread* thread = new QThread;
    ZipThread* zipThread = new ZipThread(archivePath, extractPath, UNZIP);
    QEventLoop loop;

    QObject::connect(thread, SIGNAL(started()), zipThread, SLOT(process()));
    QObject::connect(zipThread, SIGNAL(finished()), thread, SLOT(quit()));
    QObject::connect(thread, SIGNAL(finished()), &loop, SLOT(quit()));

    zipThread->moveToThread(thread);
    thread->start();

    loop.exec();

    return !(zipThread->filesList.isEmpty());
}

QString AddonsOps::checkCompFile(QString path)
{
    QStringList filesList;

    QThread* thread = new QThread;
    ZipThread* zipThread = new ZipThread(path, "", GETNAMES);
    QEventLoop loop;

    QObject::connect(thread, SIGNAL(started()), zipThread, SLOT(process()));
    QObject::connect(zipThread, SIGNAL(finished()), thread, SLOT(quit()));
    QObject::connect(thread, SIGNAL(finished()), &loop, SLOT(quit()));

    zipThread->moveToThread(thread);
    thread->start();

    loop.exec();

    filesList = zipThread->filesList;

	QStringList compList;

	foreach(QString file, filesList) compList.append(file.split("/").first().toLower());

	int index = 99999;

	QString indexFile;

	foreach(QString folder, orbiterFolders)
	{
		if (compList.contains(folder, Qt::CaseInsensitive)) return "";

		foreach(QString file, filesList)
		{
			QStringList listFile = file.toLower().split("/");

			if (listFile.contains(folder, Qt::CaseInsensitive))
			{
				if (listFile.indexOf(folder, Qt::CaseInsensitive) < index)
				{
					indexFile = "";

					index = listFile.indexOf(folder, Qt::CaseInsensitive);

					for (int i = 0; i < index; i++) indexFile += listFile.at(i) + "/";
				}
			}

			QCoreApplication::processEvents();
		}
	}

	if (index != 99999) return indexFile;

	return "";
}

QStringList AddonsOps::scanDirectory(QString dirPath)
{
	QStringList files;

	QDirIterator it(dirPath, QDirIterator::Subdirectories);

	bool needRescan = false;

	while (it.hasNext())
	{
		it.next();

		QFileInfo fileInfo(it.filePath());

		QString relPath = QDir(dirPath).relativeFilePath(it.filePath());

		// Avoid adding directories
		if (fileInfo.isFile())
		{
			// Convert the path to relative path from the given directory
			files.append(relPath);
		}
		else
		{
			if (relPath.startsWith("Add-on Docs", Qt::CaseInsensitive))
			{
				QDir(it.filePath()).rename(".", dirPath + "/Doc");

				needRescan = true;
			}
		}

		QCoreApplication::processEvents();
	}

	files.sort();

	if (needRescan) return scanDirectory(dirPath);

	return files;
}

void AddonsOps::setOverrider(QString addonName, QString addonFile)
{
	QStringList overAddons;

	foreach(QString addon, installedMap.keys())
	{
		if (installedMap.value(addon).contains(addonFile)) overAddons.append(addon);

		QCoreApplication::processEvents();
	}

	foreach(QString addon, overAddons)
	{
		QStringList addons = overMap.value(addon);

		QCoreApplication::processEvents();

		if (addons.contains(addonName)) continue;

		addons.append(addonName);

		overMap.insert(addon, addons);
	}
}

void AddonsOps::setDbMap(QString addonName, QStringList addonFiles)
{
	if (dbMap.keys().contains(addonName) && dbMap.values().contains(addonFiles)) return;

	else if (dbMap.keys().contains(addonName)) dbMap.insert(addonName + " [1]", addonFiles);

	else if (dbMap.values().contains(addonFiles))
	{
		dbMap.remove(dbMap.key(addonFiles));

		dbMap.insert(addonName, addonFiles);
	}
	else dbMap.insert(addonName, addonFiles);
}

bool AddonsOps::checkIgn(QString addonName, QString file)
{
	if (ignoredMap.value(addonName).contains(file.split("/").first().toLower())
		|| ignoredMap.value(addonName).contains(file.split("/").last().toLower())
		|| ignoredMap.value("All").contains(file.split("/").first().toLower())
		|| ignoredMap.value("All").contains(file.split("/").last().toLower())) return true;

	return false;
}

QStringList AddonsOps::uninstallAddon(QString name)
{
	QStringList result;

	if (!overMap.value(name).isEmpty())
	{
		result.insert(0, "OVERRIDE");

		QString addons;

		foreach(QString addon, overMap.value(name)) { addons += addon + "\n"; QCoreApplication::processEvents(); }

		result.insert(1, addons);

		return result;
	}

	result.insert(0, "FILES");

	QString files;

	foreach(QString file, dbMap.value(name))
	{
		QString filePath = orbiterPath + file;

		// Check if the file exists in the backup directory to restore it
		QFileInfo backFileInfo(backupDir + file);

		QDir fileDir;

		if (backFileInfo.isFile())
		{
			moveTrash(filePath, moveToTrash);

			if (!QFile::copy(backFileInfo.filePath(), filePath)) files += file + "\n";

			moveTrash(backFileInfo.filePath(), moveToTrash);

			fileDir = backFileInfo.dir();
		}
		else if (QFileInfo(backupDir + file + ".oad").isFile())
		{
			QString backFilePath = backupDir + file + ".oad";

			QFile::rename(backFilePath, backupDir + file);

			moveTrash(filePath, moveToTrash);

			if (!QFile::copy(backFileInfo.filePath(), filePath)) files += file + "\n";

			moveTrash(backFileInfo.filePath(), moveToTrash);

			fileDir = backFileInfo.dir();
		}
		else
		{
			moveTrash(filePath, moveToTrash);

			fileDir = QFileInfo(filePath).dir();
		}

		QCoreApplication::processEvents();

		// Remove any folder as long as it empty (rmdir removes empty directories only and return false when remove is failed)
		while (fileDir.rmdir(".")) fileDir.cdUp();
	}

	// If the given add-on exists in the overrider map as a base add-on, remove it
	foreach(QString addon, overMap.keys())
	{
		if (overMap.value(addon).contains(name))
		{
			QStringList addons = overMap.value(addon);

			addons.removeOne(name);

			overMap.insert(addon, addons);
		}

		if (overMap.value(addon).isEmpty()) overMap.remove(addon);

		QCoreApplication::processEvents();
	}

	result.append(files);

	return result;
}
#ifdef Q_OS_WIN

#include <windows.h>

void AddonsOps::moveTrash(QString file, bool moveTrash)
{
	if (!moveTrash)
	{
		if (!QFile::remove(file)) QDir(file).removeRecursively();

		return;
	}

	QFileInfo fileinfo(file);

	WCHAR from[MAX_PATH];

	memset(from, 0, sizeof(from));

	int l = fileinfo.absoluteFilePath().toWCharArray(from);

	Q_ASSERT(0 <= l && l < MAX_PATH);

	from[l] = '\0';

	SHFILEOPSTRUCT fileop;

	memset(&fileop, 0, sizeof(fileop));

	fileop.wFunc = FO_DELETE;

	fileop.pFrom = from;

	fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

	SHFileOperation(&fileop);
}

#else
void AddonsOps::moveTrash(QString file, bool moveTrash) { if (!QFile::remove(file)) QDir(file).removeRecursively(); }

#endif
