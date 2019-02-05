#include "settingsops.h"
#include "addonsops.h"
#include <QDateTime>
#include <QDirIterator>
#include <QTextStream>
#include <QCoreApplication>

SettingsOps::SettingsOps(){}

SettingsOps::SettingsOps(QString orbiterPath, QString backupDir, QStringList pathsList,
                         QMap<QString, QStringList> dbMap, QMap<QString, QStringList> ignoredMap,
                         QMap<QString, QString> overMap, bool moveTrash){

    this->orbiterPath = orbiterPath;

    this->backupDir = backupDir;

    this->pathsList = pathsList;

    this->dbMap = dbMap;

    this->ignoredMap = ignoredMap;

    this->overMap = overMap;

    this->moveTrash = moveTrash;
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

    AddonsOps::moveTrash(path + QString::number(index), moveTrash);

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

        QCoreApplication::processEvents();

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

    QStringList files, snapNames, snapTimes, snapFiles;

    while(!in.atEnd()) {

        QString line = in.readLine();

        snapNames.append(line.split("=").first());

        snapTimes.append(line.split("=").last());

        snapFiles.append(line);

    }

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

        QCoreApplication::processEvents();

    }

    files.sort(); snapFiles.sort();

    if(files == snapFiles) return {{"NO_CHANGE"}, {"NO_CHANGE"}, {"NO_CHANGE"}};

    return {files, snapNames, snapTimes};
}

QString SettingsOps::importSnapshot(QStringList files, QStringList snapNames, QStringList snapTimes, bool checkRes){

    if(checkRes) return "NO_BACKUP";

    foreach(QString snapName, snapNames) {

        QFileInfo snapInfo(backupDir + snapName);

        if(snapInfo.isFile() && !QFileInfo(orbiterPath + snapName).isFile()){

            QDir(orbiterPath + snapName.remove(snapName.split("/").last())).mkpath(".");

            QFile::copy(snapInfo.filePath(), orbiterPath + QDir(backupDir).relativeFilePath(snapInfo.filePath()));

            AddonsOps::moveTrash(snapInfo.filePath(), moveTrash);

            while(snapInfo.dir().rmdir(".")) snapInfo.dir().cdUp();

        }

        QCoreApplication::processEvents();

    }

    foreach(QString file, files){

        QString fileName = file.split("=").first();

        QString fileTime = file.split("=").last();

        if(snapNames.contains(fileName, Qt::CaseInsensitive) && snapTimes.contains(fileTime)) {

        } else if(snapNames.contains(fileName, Qt::CaseInsensitive)) {

            AddonsOps::moveTrash(orbiterPath + fileName, moveTrash);

            QFile::copy(backupDir + fileName, orbiterPath + fileName);

            AddonsOps::moveTrash(backupDir + fileName, moveTrash);

            QDir parentDir(backupDir + fileName.left(fileName.lastIndexOf("/")));

            while(parentDir.rmdir(".")) parentDir.cdUp();

        } else if(fileName.endsWith(".oad") && snapNames.contains(fileName.split(".oad").first(), Qt::CaseInsensitive)) {

            QFile::rename(orbiterPath + fileName, orbiterPath + fileName + ".oad");

        } else if(snapNames.contains(fileName + ".oad", Qt::CaseInsensitive)) {

            QFile::rename(orbiterPath + fileName, orbiterPath + fileName.remove(".oad"));

        } else {

            AddonsOps::moveTrash(orbiterPath + fileName, moveTrash);

            QDir parentDir(orbiterPath + fileName.left(fileName.lastIndexOf("/")));

            while(parentDir.rmdir(".")) parentDir.cdUp();

        }

        QCoreApplication::processEvents();

    }

    return "SUCCESS";
}

void SettingsOps::addIgn(QString addonName, QString addonFiles){

    QStringList ignoredList;

    if(!ignoredMap.value(addonName).isEmpty()){

        ignoredList = ignoredMap.value(addonName);

        ignoredList.append(addonFiles.toLower().split(","));

    } else ignoredList = addonFiles.toLower().split(",");

    ignoredMap.insert(addonName, ignoredList);
}

bool SettingsOps::addEntry(QString addonName, QString addonPath, QString addonFiles,
                           bool folderChecked, bool fileChecked, bool removeDir){

    if(folderChecked) {
        AddonsOps::scanDirectory(addonPath);

        dbMap.insert(addonName, AddonsOps::scanDirectory(addonPath));

        if(removeDir) AddonsOps::moveTrash(addonPath, moveTrash);
    }

    else if(fileChecked)
    {
        QString oldPath = addonPath;

        QString res = AddonsOps::checkCompFile(addonPath);

        if(res.isEmpty()){

            QString tempPath = QDir::currentPath() + "/" + addonName;

            QDir(tempPath).mkdir(".");

            if(!AddonsOps::extract(addonPath, tempPath)) return false;

            AddonsOps::scanDirectory(tempPath);

            dbMap.insert(addonName, AddonsOps::scanDirectory(tempPath));

            QDir(tempPath).removeRecursively();

        } else {

            if(!AddonsOps::extract(addonPath, QDir::currentPath())) return false;

            addonPath = QDir::currentPath() + "/" + res;

            AddonsOps::scanDirectory(addonPath);

            dbMap.insert(addonName, AddonsOps::scanDirectory(addonPath));

        }

        if(removeDir) AddonsOps::moveTrash(oldPath, moveTrash);

    } else dbMap.insert(addonName, addonFiles.split(","));

    return true;
}
