#ifndef ADDONSOPS_H
#define ADDONSOPS_H

#include <QMap>

class AddonsOps{

public:
    QString orbiterPath, backupDir;

    QList<QString> ignoredList;

    QMap<QString, QList<QString>> dbMap;

    QMap<QString, QString> overMap;

    struct addonsLists{QList<QString> enabledAddons, disabledAddons;};

    AddonsOps();

    AddonsOps(QString orbiterPath, QString backupDir, QList<QString> ignoredList, QMap<QString,
              QList<QString>> dbMap, QMap<QString, QString> overMap);

    addonsLists scanAddons();

    void enableAddon(QString name);

    void disableAddon(QString name);

    void installAddon(QString name, QString path, bool installSources, bool removeAddonDir);

    QString uninstallAddon(QString name);

private:
    QList<QString> scanDirectory(QString dirPath);

    void setOverrider(QString addonName);

    void setDbMap(QString addonName, QList<QString> addonFiles);

};

#endif // ADDONSOPS_H
