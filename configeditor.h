#ifndef CONFIGEDITOR_H
#define CONFIGEDITOR_H

#include <QMap>

class ConfigEditor{

public:
    struct Config{
        QString orbiterPath;
        QList<QString> pathsList, ignoredList;
        QMap<QString, QList<QString>> dbMap;
        QMap<QString, QString> overMap;
    };

    ConfigEditor();

    static Config readConfig();

    static void writeConfig(QString orbiterPath, QList<QString> pathsList, QList<QString> ignoredList,
                            QMap<QString, QList<QString>> dbMap, QMap<QString, QString> overMap);

};

#endif // CONFIGEDITOR_H
