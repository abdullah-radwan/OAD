#ifndef SETTINGSOPS_H
#define SETTINGSOPS_H

#include <QList>

class SettingsOps{

public:
    struct snapInfo{QStringList files, snapNames, snapTimes;};

    QString orbiterPath, backupDir;

    QStringList pathsList;

    QMap<QString, QStringList> dbMap, ignoredMap;

    bool moveTrash;

    SettingsOps();

    SettingsOps(QString orbiterPath, QString backupDir, QStringList pathsList,
                QMap<QString, QStringList> dbMap, QMap<QString, QStringList> ignoredMap, bool moveTrash);

    QString createSnapshot();

    snapInfo setSnapshot(QString fileName);

    QString importSnapshot(QStringList files, QStringList snapNames, QStringList snapTimes, bool checkRes);

    bool addPath(QString path);

    bool rmPath(int index);

    void addIgn(QString addonName, QString addonFiles);

    void addEntry(QString addonName, QString addonPath, QString addonFiles,
                  bool folderChecked, bool fileChecked, bool removeDir);
};

#endif // SETTINGSOPS_H
