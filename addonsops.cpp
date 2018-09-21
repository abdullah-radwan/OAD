#include "addonsops.h"
#include <QDirIterator>

AddonsOps::AddonsOps(QString orbiterPath, QMap<QString, QList<QString>> dbMap, QMap<QString, QString> overMap){

    this->orbiterPath = orbiterPath;

    this->dbMap = dbMap;

    this->overMap = overMap;

    backupDir = QDir::currentPath() + "/OrbiterBackup/";

    QDir checkBackupDir(backupDir);

    if(!checkBackupDir.exists()) checkBackupDir.mkpath(".");
}

AddonsOps::addonsLists AddonsOps::scanAddons(){

    QList<QString> enabledAddons;

    QList<QString> disabledAddons;

    foreach(QString addon, dbMap.keys()){

        bool enabledExists = false;

        bool disabledExists = false;

        foreach(QString file, dbMap.value(addon)){

            // Avoid comparing with these folders as they don't always exists
            if(!file.startsWith("Orbitersdk", Qt::CaseInsensitive) && !file.startsWith("Scenarios", Qt::CaseInsensitive) &&
                    !file.startsWith("Sources", Qt::CaseInsensitive) && !file.startsWith("Doc", Qt::CaseInsensitive)){

                if(QFileInfo (orbiterPath + file).exists()){enabledExists = true;}

                else if(QFileInfo (orbiterPath + file + ".oad").exists()) {disabledExists = true;}

                // Go to the next add-on if a file doesn't exists
                else {enabledExists = false; disabledExists = false; break;}

            }

        }

        if(enabledExists) {enabledAddons.append(addon);}

        if(disabledExists){disabledAddons.append(addon);}

    }

    QList<QString> tempList;

    tempList.append(enabledAddons);

    tempList.append(disabledAddons);

    foreach(QString name, tempList){

        foreach(QString file, dbMap.value(name)){

            foreach(QString addon, dbMap.keys()){

                if(name == addon) continue;

                if(overMap.key(name).isEmpty() && overMap.value(name).isEmpty()){

                    foreach(QString dbFile, dbMap.value(addon)){

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

void AddonsOps::disableAddon(QString name){

    foreach(QString file, dbMap.value(name)){

        if(!overMap.key(name).isEmpty() || !overMap.value(name).isEmpty()){

            QString overName;

            if(!overMap.key(name).isEmpty()){overName = overMap.key(name);}

            else if(!overMap.value(name).isEmpty()){overName = overMap.value(name);}

            foreach(QString dbFile, dbMap.value(overName)){

                QString absPath = orbiterPath + dbFile;

                if(!dbFile.startsWith("Orbitersdk", Qt::CaseInsensitive) && !dbFile.startsWith("Sources", Qt::CaseInsensitive)
                        && !dbFile.startsWith("Doc", Qt::CaseInsensitive)){

                    QFile::rename(absPath, absPath + ".oad");

                }

            }

        }

        if(!file.startsWith("Orbitersdk", Qt::CaseInsensitive) && !file.startsWith("Sources", Qt::CaseInsensitive)
                && !file.startsWith("Doc", Qt::CaseInsensitive)){

            QFileInfo absPath(orbiterPath + file);

            // If the file isn't already disabled
            if(absPath.exists()){QFile::rename(absPath.filePath(), absPath.filePath() + ".oad");}

        }

    }

}

void AddonsOps::addAddon(QString name, QString path, bool install, bool installSdk, bool removeAddonDir){

    QList<QString> files;

    QDirIterator it(path, QDirIterator::Subdirectories);

    while (it.hasNext()) {

        it.next();

        QFileInfo fileInfo(it.filePath());

        // Avoid adding directories
        if(fileInfo.isFile()){

            // Convert the path to relative path from the given directory
            QString relPath = QDir(path).relativeFilePath(it.filePath());

            files.append(relPath);

        }

    }

    // If install checkbox is checked
    if(install){

        foreach(QString file, files){

            if(file.startsWith("Orbitersdk", Qt::CaseInsensitive) && !installSdk){continue;}

            QString filePath = orbiterPath + file;

            QFileInfo fileInfo(filePath);

            // Create the parent directory to the addon if not exists
            if(!fileInfo.dir().exists()){fileInfo.dir().mkpath(".");}

            // If a file in the addon exists in Orbiter
            if(fileInfo.exists()){

                foreach(QString addon, dbMap.keys()){

                    if(addon != name){

                        foreach(QString dbFile, dbMap.value(addon)){

                            if(file == dbFile){overMap.insert(addon, name); break;}

                        }

                    }
                }

                // Set the parent directory to the file
                QDir parentBackDir(backupDir + QFileInfo(file).dir().path());

                // Create the parent directory if not exists in the backup path
                if(!parentBackDir.exists()) parentBackDir.mkpath(".");

                QFile::copy(filePath, backupDir + file);

                QFile::remove(filePath);

                QFile::copy(path + file, filePath);

            } else if(QFileInfo(filePath + ".oad").exists()){

                foreach(QString addon, dbMap.keys()){

                    if(name == addon) continue;

                    if(overMap.key(name).isEmpty() && overMap.value(name).isEmpty()){

                        foreach(QString dbFile, dbMap.value(addon)){

                            if(file == dbFile){overMap.insert(addon, name); break;}

                        }

                    }

                }

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

        if(removeAddonDir){QDir(path).removeRecursively();}

    }

    bool exists = false;

    foreach(QString addon, dbMap.keys()){

        QList<QString> dbFiles;

        foreach(QString file, dbMap.value(addon)){

            if(!file.startsWith("Orbitersdk", Qt::CaseInsensitive) && !file.startsWith("Scenarios", Qt::CaseInsensitive) &&
                    !file.startsWith("Sources", Qt::CaseInsensitive) && !file.startsWith("Doc", Qt::CaseInsensitive)){

                dbFiles.append(file);

            }

        }

        QList<QString> compFiles;

        foreach(QString file, files){

            if(!file.startsWith("Orbitersdk", Qt::CaseInsensitive) && !file.startsWith("Scenarios", Qt::CaseInsensitive) &&
                    !file.startsWith("Sources", Qt::CaseInsensitive)  && !file.startsWith("Doc", Qt::CaseInsensitive)){

                compFiles.append(file);

            }

        }

        dbFiles.sort();

        compFiles.sort();

        if(compFiles == dbFiles){

            // Rename the database add-on name to the entered name
            if(addon != name){

                QList<QString> tempList = dbMap.value(addon);

                dbMap.remove(addon);

                dbMap.insert(name, tempList);

            }

            exists = true;

            break;

        }

    }

    if(!exists){dbMap.insert(name, files);}

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

            if(!overMap.key(name).isEmpty()){dbMap.remove(name); continue;}

            QFile::remove(filePath);

            fileDir = QFileInfo(filePath).dir();

        }

        // If the given add-on exists in the overrider map as a base add-on, remove it
        if(!overMap.key(name).isEmpty()){overMap.remove(overMap.key(name));}

        // Remove any folder as long as it empty (rmdir removes empty directories only and return false when remove is failed)
        while(fileDir.rmdir(fileDir.absolutePath())){fileDir.cdUp();}
    }

    return "";

}
