#include "addonsops.h"
#include <QDirIterator>
#include <QCoreApplication>
#include "qarchivediskextractor.hpp"
#include <QDebug>

using QArchive::DiskExtractor;

static QStringList orbiterFolders = {"config", "doc", "flights", "html", "images", "install", "meshes", "missions",
                                     "modules", "orbitersdk", "scenarios", "script", "textures", "textures2", "utils"};

AddonsOps::AddonsOps(){}

AddonsOps::AddonsOps(QString orbiterPath, QString backupDir, QMap<QString, QStringList> dbMap,
                     QMap<QString, QStringList> ignoredMap, QMap<QString, QString> overMap,
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

    if(!checkBackupDir.exists()) checkBackupDir.mkpath(".");
}

AddonsOps::addonsLists AddonsOps::scanAddons()
{
    if(showAll) return {dbMap.keys(), {}};

    QStringList enabledAddons, disabledAddons;

    foreach(QString addon, dbMap.keys()){

        bool enabledExists = false;

        bool disabledExists = false;

        foreach(QString file, dbMap.value(addon)){

            if(!checkIgn(addon, file)){

                if(QFileInfo::exists(orbiterPath + file)) enabledExists = true;

                else if(QFileInfo::exists(orbiterPath + file + ".oad")) disabledExists = true;

                // Go to the next add-on if a file doesn't exists
                else {enabledExists = false; disabledExists = false; break;}

            }

            QCoreApplication::processEvents();

        }

        if(enabledExists && !enabledAddons.contains(addon, Qt::CaseInsensitive)) {
            enabledAddons.append(addon);
//            installedMap.insert(addon, dbMap.value(addon));
        }

        else if(disabledExists && !disabledAddons.contains(addon, Qt::CaseInsensitive)) {
            disabledAddons.append(addon);
//            installedMap.insert(addon, dbMap.value(addon));
        }

    }

    return {enabledAddons, disabledAddons};
}

void AddonsOps::enableAddon(QString name)
{
    foreach(QString file, dbMap.value(name))
    {
//        bool exists = false;
//        foreach(QString addon, overMap.keys())
//        {
//            if(overMap.value(addon).contains(name)) {exists = true; break;}
//        }
//        if(!overMap.value(name).empty() || overMap.contains(name))

        // If the add-on exists in the overrider map
        if(!overMap.key(name).isEmpty() || !overMap.value(name).isEmpty()){

            QString overName;

            if(!overMap.key(name).isEmpty()){overName = overMap.key(name);}

            else if(!overMap.value(name).isEmpty()){overName = overMap.value(name);}

            // Enable each file from the overrider add-on
            foreach(QString dbFile, dbMap.value(overName)){

                QString absPath = orbiterPath + dbFile + ".oad";

                QFile::rename(absPath, orbiterPath + dbFile);

                QCoreApplication::processEvents();

            }

        }

        QString absPath = orbiterPath + file + ".oad";

        if(QFileInfo(absPath).exists()) QFile::rename(absPath, orbiterPath + file);

        QCoreApplication::processEvents();

    }
}

void AddonsOps::disableAddon(QString addonName)
{
    foreach(QString file, dbMap.value(addonName)){

        if(!overMap.key(addonName).isEmpty() || !overMap.value(addonName).isEmpty()){

            QString overName;

            if(!overMap.key(addonName).isEmpty()){overName = overMap.key(addonName);}

            else if(!overMap.value(addonName).isEmpty()){overName = overMap.value(addonName);}

            foreach(QString dbFile, dbMap.value(overName)){

                QString absPath = orbiterPath + dbFile;

                if(!checkIgn(addonName, file)) QFile::rename(absPath, absPath + ".oad");

                QCoreApplication::processEvents();

            }

        }

        if(!checkIgn(addonName, file)){

            QFileInfo absPath(orbiterPath + file);

            // If the file isn't already disabled
            if(absPath.exists()){QFile::rename(absPath.filePath(), absPath.filePath() + ".oad");}

        }

        QCoreApplication::processEvents();

    }
}

int AddonsOps::installAddon(QString name, QString path, bool compChecked, bool installSources, bool removeAddonDir)
{
    bool installFail = false;

    QStringList installedList;

    QString compPath = path;

    if(compChecked){

        QString checkRes = checkCompFile(path);

        if(checkRes.isEmpty()){

            QString tempPath = QDir::currentPath() + "/" + name;

            QDir(tempPath).mkdir(".");

            if(!extract(path, tempPath)) return -1;

            path = tempPath + "/";

        } else {

            if(!extract(path, QDir::currentPath())) return -1;

            path = QDir::currentPath() + "/" + checkRes;

        }

    }

    QStringList files = scanDirectory(path);

    files = scanDirectory(path);

//    if(installedMap.values().contains(files)) return 0; TODO

    foreach(QString file, files){

        if((file.startsWith("Orbitersdk", Qt::CaseInsensitive) || file.startsWith("Sources", Qt::CaseInsensitive))
                && !installSources) continue;

        QString filePath = orbiterPath + file;

        QFileInfo fileInfo(filePath);

        // Create the parent directory to the addon if not exists
        if(!fileInfo.dir().exists()) fileInfo.dir().mkpath(".");

        // If a file in the addon exists in Orbiter
        if(fileInfo.isFile()){

            setOverrider(name, file);

            if(file.lastIndexOf("/") != -1) {

                // Set the parent directory to the file
                QDir parentBackDir(backupDir + file.left(file.lastIndexOf("/")));

                // Create the parent directory if not exists in the backup path
                if(!parentBackDir.exists()) parentBackDir.mkpath(".");
            }

            if(QFile::copy(filePath, backupDir + file)) moveTrash(filePath, moveToTrash);

            if(!QFile::copy(path + file, filePath)) {removeAddonDir = false; installFail = true;}

            else installedList.append(file);

         } else if(QFileInfo(filePath + ".oad").isFile()){

            setOverrider(name, file);

            // Set the parent directory to the file
            QDir parentBackDir(backupDir + file.left(file.lastIndexOf("/")));

            // Create the parent directory if not exists in the backup path
            if(!parentBackDir.exists()) parentBackDir.mkpath(".");

            filePath += ".oad";

            if(QFile::copy(filePath, backupDir + file + ".oad")) moveTrash(filePath, moveToTrash);

            if(!QFile::copy(path + file, filePath)) {removeAddonDir = false; installFail = true;}

            else installedList.append(file);

        } else {

            if(!QFile::copy(path + file, filePath)) {removeAddonDir = false; installFail = true;}

            else installedList.append(file);
        }

        QCoreApplication::processEvents();

    }

    if(compChecked) QDir(path).removeRecursively();

    if(compChecked && removeAddonDir) moveTrash(compPath, moveToTrash);

    else if(removeAddonDir) moveTrash(path, moveToTrash);

    if(installFail) {

        foreach(QString file, installedList) moveTrash(orbiterPath + file, moveToTrash);

        return -1;
    }

    setDbMap(name, files);

    return 1;
}

bool AddonsOps::extract(QString archivePath, QString extractPath)
{
    DiskExtractor extractor(archivePath, extractPath);

    QObject::connect(&extractor, &DiskExtractor::error, [&](short code, QString archive){
        return false;
    });

    extractor.start();

    QEventLoop loop;

    loop.connect(&extractor, SIGNAL(finished()), &loop, SLOT(quit()));

    loop.exec();

    return true;
}

QString AddonsOps::checkCompFile(QString path)
{
    DiskExtractor extractor;

    extractor.setArchive(path);

    QStringList filesList;

    QObject::connect(&extractor , &DiskExtractor::info, [&](QJsonObject information){
        filesList = information.keys();
    });

    extractor.getInfo();

    QEventLoop loop;

    loop.connect(&extractor, SIGNAL(filesFinished()), &loop, SLOT(quit()));

    loop.exec();

    QStringList compList;

    foreach(QString file, filesList) compList.append(file.split("/").first().toLower());

    int index = 99999;

    QString indexFile;

    foreach(QString folder, orbiterFolders){

        if(compList.contains(folder, Qt::CaseInsensitive)) return "";

        foreach(QString file, filesList){

            QStringList listFile = file.toLower().split("/");

            if(listFile.contains(folder, Qt::CaseInsensitive)){

                if(listFile.indexOf(folder, Qt::CaseInsensitive) < index) {

                    indexFile = "";

                    index = listFile.indexOf(folder, Qt::CaseInsensitive);

                    for(int i = 0; i < index; i++) indexFile += listFile.at(i) + "/";

                }

            }

        }

    }

    if(index != 99999) return indexFile;

    return "";
}

QStringList AddonsOps::scanDirectory(QString dirPath)
{
    QStringList files;

    QDirIterator it(dirPath, QDirIterator::Subdirectories);

    while (it.hasNext()) {

        it.next();

        QFileInfo fileInfo(it.filePath());

        QString relPath = QDir(dirPath).relativeFilePath(it.filePath());

        // Avoid adding directories
        if(fileInfo.isFile()){

            // Convert the path to relative path from the given directory
            files.append(relPath);

        } else {

            if(relPath.startsWith("Add-on Docs", Qt::CaseInsensitive)){

                QDir(it.filePath()).rename(".", dirPath + "/Doc");

            }

        }

        QCoreApplication::processEvents();

    }

    files.sort();

    return files;
}

void AddonsOps::setOverrider(QString addonName, QString addonFile)
{
   foreach(QString addon, dbMap.keys())
   {
       if(overMap.key(addonName).isEmpty() && overMap.value(addonName).isEmpty())
       {
           if(dbMap.value(addon).contains(addonFile, Qt::CaseInsensitive)) overMap.insert(addon, addonName);

        }
    }
}

void AddonsOps::setDbMap(QString addonName, QStringList addonFiles)
{
    bool updated = false;

    bool exists = false;

    addonFiles.sort();

    foreach(QString addon, dbMap.keys())
    {
        QStringList dbFiles = dbMap.value(addon);

        dbFiles.sort();

        foreach(QString dbFile, dbFiles)
        {
            if(addonFiles.contains(dbFile, Qt::CaseInsensitive)) exists = true;

            else {exists = false; break;}
        }

        if(exists)
        {
            dbMap.remove(addon);

            dbMap.insert(addonName, addonFiles);

            updated = true;

            break;
        } else if(addon == addonName)
        {
            dbMap.insert(addonName + " [1]", addonFiles);

            updated = true;

            break;

        } else updated = false;
    }

    if(!updated) dbMap.insert(addonName, addonFiles);
}

bool AddonsOps::checkIgn(QString addonName, QString file)
{
    if(ignoredMap.value(addonName).contains(file.split("/").first().toLower())
            || ignoredMap.value(addonName).contains(file.split("/").last().toLower())
            || ignoredMap.value("All").contains(file.split("/").first().toLower())
            || ignoredMap.value("All").contains(file.split("/").last().toLower())) return true;

    return false;
}

QString AddonsOps::uninstallAddon(QString name)
{
    if(!overMap.value(name).isEmpty()) return overMap.value(name);

    foreach(QString file, dbMap.value(name)){

        QString filePath = orbiterPath + file;

        // Check if the file exists in the backup directory to restore it
        QFileInfo backFileInfo(backupDir + file);

        QDir fileDir;

        if(backFileInfo.isFile())
        {
            moveTrash(filePath, moveToTrash);

            QFile::copy(backFileInfo.filePath(), filePath);

            moveTrash(backFileInfo.filePath(), moveToTrash);

            fileDir = backFileInfo.dir();
        } else if(QFileInfo(backupDir + file + ".oad").isFile())
        {
            QString backFilePath = backupDir + file + ".oad";

            QFile::rename(backFilePath, backupDir + file);

            moveTrash(filePath, moveToTrash);

            QFile::copy(backFileInfo.filePath(), filePath);

            moveTrash(backFileInfo.filePath(), moveToTrash);

            fileDir = backFileInfo.dir();
        } else
        {
            moveTrash(filePath, moveToTrash);

            fileDir = QFileInfo(filePath).dir();
        }

        QCoreApplication::processEvents();

        // If the given add-on exists in the overrider map as a base add-on, remove it
        if(!overMap.key(name).isEmpty()) overMap.remove(overMap.key(name));

        // Remove any folder as long as it empty (rmdir removes empty directories only and return false when remove is failed)
        while(fileDir.rmdir(".")) fileDir.cdUp();
    }

    return "";
}
#ifdef Q_OS_WIN

#include <windows.h>

void AddonsOps::moveTrash(QString file, bool moveTrash)
{
    if(!moveTrash)
    {
        if(!QFile::remove(file)) QDir(file).removeRecursively();

        return;
    }

    QFileInfo fileinfo(file);

    WCHAR from[MAX_PATH];

    memset(from, 0, sizeof(from));

    int l = fileinfo.absoluteFilePath().toWCharArray(from);

    Q_ASSERT(0 <= l && l < MAX_PATH);

    from[ l ] = '\0';

    SHFILEOPSTRUCT fileop;

    memset(&fileop, 0, sizeof(fileop));

    fileop.wFunc = FO_DELETE;

    fileop.pFrom = from;

    fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

    SHFileOperation(&fileop);
}

#else
void AddonsOps::moveTrash(QString file, bool moveTrash){if(!QFile::remove(file)) QDir(file).removeRecursively();}

#endif
