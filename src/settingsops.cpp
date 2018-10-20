#include "settingsops.h"
#include "addonsops.h"
#include <quazip/JlCompress.h>
#include <QDateTime>
#include <QDirIterator>
#include <QTextStream>

SettingsOps::SettingsOps(){}

SettingsOps::SettingsOps(QString orbiterPath, QString backupDir, QList<QString> pathsList,
                         QMap<QString, QList<QString>> dbMap, QMap<QString, QList<QString>> ignoredMap){

    this->orbiterPath = orbiterPath;

    this->backupDir = backupDir;

    this->pathsList = pathsList;

    this->dbMap = dbMap;

    this->ignoredMap = ignoredMap;
}

bool SettingsOps::addPath(QString path){

    if(QFileInfo(path + "orbiter.exe").isFile()){

        pathsList.append(path);

        QDir checkBackupDir(QDir::currentPath() + "/OrbiterBackup " + QString::number(pathsList.indexOf(path)) + "/");

        if(!checkBackupDir.exists()) checkBackupDir.mkpath(".");

        return true;

    }

    return false;
}

bool SettingsOps::rmPath(int index){

    QString path = QDir::currentPath() + "/OrbiterBackup ";

    QDir(path + QString::number(index)).removeRecursively();

    for(int counter = 1; counter < pathsList.count(); counter++){

        if(counter < index) continue;

        QDir(path + QString::number(counter)).rename(".", path + QString::number(counter - 1));

    }

    pathsList.removeAt(index);

    if(pathsList.isEmpty()) return true;

    return false;

}

QString SettingsOps::createSnapshot(){

    QString fileName = "snapshot-" + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss") + ".txt";

    QFile file(QDir::currentPath() + "/" + fileName);

    file.open(QIODevice::WriteOnly);

    QTextStream out(&file);

    out << "SNAPSHOT_OAD" << "\n";

    QDirIterator it(orbiterPath, QDirIterator::Subdirectories);

    // Scan Orbiter installation
    while (it.hasNext()) {

        it.next();

        QFileInfo fileInfo(it.filePath());

        if(fileInfo.isFile()){

            QString relPath = QDir(orbiterPath).relativeFilePath(it.filePath());

            out << relPath + "=" + QString::number(QDateTime(fileInfo.lastModified()).toSecsSinceEpoch()) << "\n";

        }

    }

    file.flush();

    file.close();

    return fileName;

}

SettingsOps::snapInfo SettingsOps::setSnapshot(QString fileName){

    QFile file(fileName);

    file.open(QIODevice::ReadOnly);

    QTextStream in(&file);

    if(in.readLine() != "SNAPSHOT_OAD") return {};

    QList<QString> files, snapFiles;

    while(!in.atEnd()){snapFiles.append(in.readLine());}

    QDirIterator it(orbiterPath, QDirIterator::Subdirectories);

    while (it.hasNext()) {

        it.next();

        QFileInfo fileInfo(it.filePath());

        // Avoid adding directories
        if(fileInfo.isFile()){

            // Convert the path to relative path from the given directory
            QString relPath = QDir(orbiterPath).relativeFilePath(it.filePath());

            files.append(relPath + "=" + QString::number(QDateTime(fileInfo.lastModified()).toSecsSinceEpoch()));

        }

    }

    files.sort(); snapFiles.sort();

    return {files, snapFiles};

}

QString SettingsOps::importSnapshot(QList<QString> files, QList<QString> snapFiles, bool checkRes){

    if(files != snapFiles){

        if(checkRes) return "NO_BACKUP";

        foreach(QString snapFile, snapFiles){

            QString snapFileName = snapFile.split("=").first();

            QString snapFileTime = snapFile.split("=").last();

            foreach(QString file, files){

                QString fileName = file.split("=").first();

                QString fileTime = file.split("=").last();

                if(file == snapFile){break;}

                else if(fileName == snapFileName){

                    QFile::remove(orbiterPath + fileName);

                    QFile::copy(backupDir + fileName, orbiterPath + fileName);

                    QFile::remove(backupDir + fileName);

                }

                else if(fileName + ".oad" == snapFileName){

                    QFile::rename(orbiterPath + fileName, orbiterPath + fileName + ".oad");

                }

                else if(fileName == snapFileName + ".oad"){

                    QFile::rename(orbiterPath + fileName, orbiterPath + fileName.remove(".oad"));

                }

                else{

                    QFile::remove(orbiterPath + fileName);

                    QDir fileDir = QFileInfo(orbiterPath + fileName).dir();

                    while(fileDir.rmdir(".")){fileDir.cdUp();}

                }

            }

        }

        return "SUCCESS";

    } else return "NO_CHANGES";

}

void SettingsOps::addIgn(QString addonName, QString addonFiles){

    QList<QString> ignoredList;

    if(!ignoredMap.value(addonName).isEmpty()){

        ignoredList = ignoredMap.value(addonName);

        ignoredList.append(addonFiles.toLower().split(","));

    } else ignoredList = addonFiles.toLower().split(",");

    ignoredMap.insert(addonName, ignoredList);

}

void SettingsOps::addEntry(QString addonName, QString addonPath, QString addonFiles,
                           bool folderChecked, bool fileChecked, bool removeDir){

    if(folderChecked) dbMap.insert(addonName, AddonsOps::scanDirectory(addonPath));

    else if(fileChecked){

        QString oldPath = addonPath;

        QString res = AddonsOps::checkCompFile(addonPath);

        if(res.isEmpty()){

            QString tempPath = QDir::currentPath() + "/" + addonName;

            QDir(tempPath).mkdir(".");

            JlCompress::extractDir(addonPath, tempPath);

            AddonsOps::scanDirectory(tempPath);

            dbMap.insert(addonName, AddonsOps::scanDirectory(tempPath));

            QDir(tempPath).removeRecursively();

        } else {

            JlCompress::extractDir(addonPath, QDir::currentPath());

            addonPath = QDir::currentPath() + "/" + res;

            AddonsOps::scanDirectory(addonPath);

            dbMap.insert(addonName, AddonsOps::scanDirectory(addonPath));

            QDir(addonPath).removeRecursively();

        }

        if(removeDir) QFile::remove(oldPath);

    } else dbMap.insert(addonName, addonFiles.split(","));

}
