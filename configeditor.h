#ifndef CONFIGEDITOR_H
#define CONFIGEDITOR_H

#include <QList>
#include <QMap>
#include <QString>

class ConfigEditor
{
public:
    struct Config{
        QString orbiterPath;
        QMap<QString, QList<QString>> dbMap;
        QMap<QString, QString> overMap;
    };
    ConfigEditor();
    static Config readConfig();
    static void writeConfig(QString orbiterPath, QMap<QString, QList<QString>> dbMap,
                            QMap<QString, QString> overMap);
};

#endif // CONFIGEDITOR_H
