#include "addonsops.h"
#include <QDirIterator>
#include <quazip/JlCompress.h>

static QList<QString> orbiterFolders = {"config", "doc", "flights", "html", "images", "install", "meshes", "missions",
                                        "modules", "orbitersdk", "scenarios", "script", "textures", "textures2", "utils"};

AddonsOps::AddonsOps(){}

AddonsOps::AddonsOps(QString orbiterPath, QString backupDir, QMap<QString, QList<QString>> ignoredMap,
                     QMap<QString, QList<QString>> dbMap, QMap<QString, QString> overMap){

    this->orbiterPath = orbiterPath;

    this->ignoredMap = ignoredMap;

    this->dbMap = dbMap;

    this->overMap = overMap;

    QDir checkBackupDir(backupDir);

    if(!checkBackupDir.exists()) checkBackupDir.mkpath(".");
}

AddonsOps::addonsLists AddonsOps::scanAddons(){

    QMap<QString, QList<QString>> addons;

    QList<QString> enabledAddons, disabledAddons;

    foreach(QString addon, dbMap.keys()){

        bool enabledExists = false;

        bool disabledExists = false;

        foreach(QString file, dbMap.value(addon)){

            if(!checkIgn(addon, file)){

                if(QFileInfo (orbiterPath + file).exists()){enabledExists = true;}

                else if(QFileInfo (orbiterPath + file + ".oad").exists()) {disabledExists = true;}

                // Go to the next add-on if a file doesn't exists
                else {enabledExists = false; disabledExists = false; break;}

            }

        }

        if(enabledExists) {enabledAddons.append(addon); addons.insert(addon, dbMap.value(addon));}

        if(disabledExists){disabledAddons.append(addon); addons.insert(addon, dbMap.value(addon));}

    }

    QList<QString> tempList;

    tempList.append(enabledAddons);

    tempList.append(disabledAddons);

    foreach(QString name, tempList){

        foreach(QString file, addons.value(name)){

            foreach(QString addon, addons.keys()){

                if(name == addon) continue;

                if(overMap.key(name).isEmpty() && overMap.value(name).isEmpty()){

                    foreach(QString dbFile, addons.value(addon)){

                        if(file == dbFile){

                            overMap.insert(addon, name);

                            break;

                        }

                    }

                }

            }

        }

    }

    return {enabledAddons, disabledAddons};

}

void AddonsOps::enableAddon(QString name){

    foreach(QString file, dbMap.value(name)){

        // If the add-on exists in the overrider map
        if(!overMap.key(name).isEmpty() || !overMap.value(name).isEmpty()){

            QString overName;

            if(!overMap.key(name).isEmpty()){overName = overMap.key(name);}

            else if(!overMap.value(name).isEmpty()){overName = overMap.value(name);}

            // Enable each file from the overrider add-on
            foreach(QString dbFile, dbMap.value(overName)){

                QString absPath = orbiterPath + dbFile + ".oad";

                QFile::rename(absPath, orbiterPath + dbFile);

            }

        }

        QString absPath = orbiterPath + file + ".oad";

        if(QFileInfo(absPath).exists()){QFile::rename(absPath, orbiterPath + file);}

    }

}

void AddonsOps::disableAddon(QString addonName){

    foreach(QString file, dbMap.value(addonName)){

        if(!overMap.key(addonName).isEmpty() || !overMap.value(addonName).isEmpty()){

            QString overName;

            if(!overMap.key(addonName).isEmpty()){overName = overMap.key(addonName);}

            else if(!overMap.value(addonName).isEmpty()){overName = overMap.value(addonName);}

            foreach(QString dbFile, dbMap.value(overName)){

                QString absPath = orbiterPath + dbFile;

                if(!checkIgn(addonName, file)) QFile::rename(absPath, absPath + ".oad");

            }

        }

        if(!checkIgn(addonName, file)){

            QFileInfo absPath(orbiterPath + file);

            // If the file isn't already disabled
            if(absPath.exists()){QFile::rename(absPath.filePath(), absPath.filePath() + ".oad");}

        }

    }

}

void AddonsOps::installAddon(QString name, QString path, bool compChecked, bool installSources, bool removeAddonDir){

    QString compPath = path;

    if(compChecked){

        QString checkRes = checkCompFile(path);

        if(checkRes.isEmpty()){

            QString tempPath = QDir::currentPath() + "/" + name;

            QDir(tempPath).mkdir(".");

            JlCompress::extractDir(path, tempPath);

            path = tempPath + "/";

        } else {

            JlCompress::extractDir(path, QDir::currentPath());

            path = QDir::currentPath() + "/" + checkRes;

        }

    }

    QList<QString> files = scanDirectory(path);

    files = scanDirectory(path);

    foreach(QString file, files){

        if((file.startsWith("Orbitersdk", Qt::CaseInsensitive) || file.startsWith("Sources", Qt::CaseInsensitive))
                && !installSources) continue;

        QString filePath = orbiterPath + file;

        QFileInfo fileInfo(filePath);

        // Create the parent directory to the addon if not exists
        if(!fileInfo.dir().exists()){fileInfo.dir().mkpath(".");}

        // If a file in the addon exists in Orbiter
        if(fileInfo.exists()){

            setOverrider(name);

            // Set the parent directory to the file
            QDir parentBackDir(backupDir + QFileInfo(file).dir().path());

            // Create the parent directory if not exists in the backup path
            if(!parentBackDir.exists()) parentBackDir.mkpath(".");

            QFile::copy(filePath, backupDir + file);

            QFile::remove(filePath);

            QFile::copy(path + file, filePath);

         } else if(QFileInfo(filePath + ".oad").exists()){

            setOverrider(name);

            // Set the parent directory to the file
            QDir parentBackDir(backupDir + QFileInfo(file).dir().path());

            // Create the parent directory if not exists in the backup path
            if(!parentBackDir.exists()) parentBackDir.mkpath(".");

            filePath += ".oad";

            QFile::copy(filePath, backupDir + file + ".oad");

            QFile::remove(filePath);

            QFile::copy(path + file, filePath);

        } else if(!overMap.key(name).isEmpty()){QFile::copy(path + file, filePath + ".oad");}

        else {QFile::copy(path + file, filePath);}

    }

    if(compChecked){QDir(path).removeRecursively();}

    if(compChecked && removeAddonDir){QFile::remove(compPath);}

    else if(removeAddonDir){QDir(path).removeRecursively();}

    setDbMap(name, files);

}

QString AddonsOps::checkCompFile(QString path){

    QList<QString> filesList = JlCompress::getFileList(path);

    QList<QString> compList;

    foreach(QString file, filesList){compList.append(file.split("/").first().toLower());}

    foreach(QString folder, orbiterFolders){

        if(compList.contains(folder)) return "";

        foreach(QString file, filesList){

            if(file.toLower().split("/").contains(folder)){

                return file.left(file.lastIndexOf(folder, -1, Qt::CaseInsensitive));

            }

        }

    }

    return "";

}

QList<QString> AddonsOps::scanDirectory(QString dirPath){

    QList<QString> files;

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

    }

    files.sort();

    return files;

}

void AddonsOps::setOverrider(QString addonName){

    foreach(QString addon, dbMap.keys()){

       if(addonName == addon) continue;

       // If the addon doesn't exists in the overrider map, add it
       if(overMap.key(addonName).isEmpty() && overMap.value(addonName).isEmpty()){

           foreach(QString dbFile, dbMap.value(addon)){

               if(addonName == dbFile){overMap.insert(addon, addonName); break;}

           }

       }

    }

}

void AddonsOps::setDbMap(QString addonName, QList<QString> addonFiles){

    bool exists = false;

    foreach(QString addon, dbMap.keys()){

        QList<QString> dbFiles, compFiles;

        foreach(QString file, dbMap.value(addon)) if(!checkIgn(addonName, file)) dbFiles.append(file);

        foreach(QString file, addonFiles) if(!checkIgn(addonName, file)) compFiles.append(file);

        dbFiles.sort(); compFiles.sort();

        if(compFiles == dbFiles){

            // Rename the database add-on name to the entered name
            if(addon != addonName){

                QList<QString> tempList = dbMap.value(addon);

                dbMap.remove(addon);

                dbMap.insert(addonName, tempList);

            }

            exists = true;

            break;

        }

    }

    if(!exists){dbMap.insert(addonName, addonFiles);}

}

bool AddonsOps::checkIgn(QString addonName, QString file){

    if(ignoredMap.value(addonName).contains(file.split("/").first().toLower())
            && ignoredMap.value(addonName).contains(file.split("/").last().toLower())
            && ignoredMap.value("All").contains(file.split("/").first().toLower())
            && ignoredMap.value("All").contains(file.split("/").last().toLower())) return true;

    return false;

}

QString AddonsOps::uninstallAddon(QString name){

    foreach(QString file, dbMap.value(name)){

        if(!overMap.value(name).isEmpty()){return overMap.value(name);}

        QString filePath = orbiterPath + file;

        // Check if the file exists in the backup directory to restore it
        QFileInfo backFileInfo(backupDir + file);

        QDir fileDir;

        if(backFileInfo.exists()){

            QFile::remove(filePath);

            QFile::copy(backFileInfo.filePath(), filePath);

            QFile::remove(backFileInfo.filePath());

            fileDir = backFileInfo.dir();

        } else if(QFileInfo(backupDir + file + ".oad").exists()){

            QString backFilePath = backupDir + file + ".oad";

            QFile::rename(backFilePath, backupDir + file);

            QFile::remove(filePath);

            QFile::copy(backFileInfo.filePath(), filePath);

            QFile::remove(backFileInfo.filePath());

            fileDir = backFileInfo.dir();

        }

        else {

            QFile::remove(filePath);

            fileDir = QFileInfo(filePath).dir();

        }

        // If the given add-on exists in the overrider map as a base add-on, remove it
        if(!overMap.key(name).isEmpty()){overMap.remove(overMap.key(name));}

        // Remove any folder as long as it empty (rmdir removes empty directories only and return false when remove is failed)
        while(fileDir.rmdir(".")){fileDir.cdUp();}
    }

    return "";

}
