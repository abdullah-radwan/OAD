#ifndef ADDONSOPS_H
#define ADDONSOPS_H

#include <QMap>

class AddonsOps{

public:
    QString orbiterPath, backupDir;

    QMap<QString, QList<QString>> ignoredMap, dbMap;

    QMap<QString, QString> overMap;

    struct addonsLists{QList<QString> enabledAddons, disabledAddons;};

    AddonsOps();

    AddonsOps(QString orbiterPath, QString backupDir, QMap<QString, QList<QString>> ignoredMap, QMap<QString,
              QList<QString>> dbMap, QMap<QString, QString> overMap);

    addonsLists scanAddons();

    void enableAddon(QString name);

    void disableAddon(QString name);

    void installAddon(QString name, QString path, bool compChecked, bool installSources, bool removeAddonDir);

    QString uninstallAddon(QString name);

    static QList<QString> scanDirectory(QString dirPath);

    static QString checkCompFile(QString path);

private:
    void setOverrider(QString addonName);

    void setDbMap(QString addonName, QList<QString> addonFiles);

    bool checkIgn(QString addonName, QString file);

};

#endif // ADDONSOPS_H
