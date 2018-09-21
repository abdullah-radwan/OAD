#ifndef ADDONSOPS_H
#define ADDONSOPS_H

#include <QString>
#include <QList>
#include <QMap>

class AddonsOps
{
public:
    QString orbiterPath;
    QMap<QString, QList<QString>> dbMap;
    QMap<QString, QString> overMap;
    struct addonsLists{
        QList<QString> enabledAddons;
        QList<QString> disabledAddons;
    };
    AddonsOps(QString orbiterPath, QMap<QString, QList<QString>> dbMap, QMap<QString, QString> overMap);
    addonsLists scanAddons();
    void enableAddon(QString name);
    void disableAddon(QString name);
    void addAddon(QString name, QString path, bool install, bool installSdk, bool removeAddonDir);
    QString uninstallAddon(QString name);
private:
    QMap<QString, QList<QString>> disabledMap;
    QString backupDir;
};

#endif // FILEOP_H
