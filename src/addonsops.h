#ifndef ADDONSOPS_H
#define ADDONSOPS_H

#include <QMap>

class AddonsOps{

public:
    QString orbiterPath, backupDir;

    QMap<QString, QStringList> dbMap, ignoredMap;

    QMap<QString, QString> overMap;

    bool moveToTrash;

    struct addonsLists{QStringList enabledAddons, disabledAddons;};

    AddonsOps();

    AddonsOps(QString orbiterPath, QString backupDir, QMap<QString, QStringList> dbMap,
              QMap<QString, QStringList> ignoredMap, QMap<QString, QString> overMap, bool moveTrash);

    addonsLists scanAddons();

    void enableAddon(QString name);

    void disableAddon(QString name);

    bool installAddon(QString name, QString path, bool compChecked, bool installSources, bool removeAddonDir);

    QString uninstallAddon(QString name);

    static QStringList scanDirectory(QString dirPath);

    static QString checkCompFile(QString path);

    static void moveTrash(QString file, bool moveTrash);

private:
    void setOverrider(QString addonName, QString addonFile);

    void setDbMap(QString addonName, QStringList addonFiles);

    bool checkIgn(QString addonName, QString file);

};

#endif // ADDONSOPS_H
