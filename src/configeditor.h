#ifndef CONFIGEDITOR_H
#define CONFIGEDITOR_H

#include <QMap>

class ConfigEditor{

public:
    struct Config{
        QString orbiterPath;
        QStringList pathsList;
        QMap<QString, QStringList> dbMap, ignoredMap;
        QMap<QString, QString> overMap;
        bool moveTrash;
    };

    ConfigEditor();

    static Config readConfig();

    static void writeConfig(QString orbiterPath, QStringList pathsList, QMap<QString, QStringList> dbMap,
                            QMap<QString, QStringList> ignoredMap , QMap<QString, QString> overMap, bool moveTrash);

};

#endif // CONFIGEDITOR_H
