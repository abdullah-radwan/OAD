#ifndef CONFIGEDITOR_H
#define CONFIGEDITOR_H

#include <QList>
#include <QMap>
#include <QString>

class ConfigEditor
{
public:
    struct Config{
        QString pathOrbiter;
        QMap<QString, QList<QString>> dataAddons;
    };
    ConfigEditor();
    static Config readConfig();
    static void writeConfig(QString pathOrbiter, QMap<QString, QList<QString>> dataAddons);
};

#endif // CONFIGEDITOR_H
