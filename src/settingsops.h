#ifndef SETTINGSOPS_H
#define SETTINGSOPS_H

#include <QList>

class SettingsOps{

public:
    struct snapInfo{QList<QString> files, snapfiles;};

    QString orbiterPath, backupDir;

    QList<QString> pathsList;

    QMap<QString, QList<QString>> dbMap, ignoredMap;

    SettingsOps();

    SettingsOps(QString orbiterPath, QString backupDir, QList<QString> pathsList,
                QMap<QString, QList<QString>> dbMap, QMap<QString, QList<QString>> ignoredMap);

    QString createSnapshot();

    snapInfo setSnapshot(QString fileName);

    QString importSnapshot(QList<QString> files, QList<QString> snapFiles, bool checkRes);

    bool addPath(QString path);

    bool rmPath(int index);

    void addIgn(QString addonName, QString addonFiles);

    void addEntry(QString addonName, QString addonPath, QString addonFiles,
                  bool folderChecked, bool fileChecked, bool removeDir);
};

#endif // SETTINGSOPS_H
