#include "configeditor.h"
#include <QSettings>

ConfigEditor::ConfigEditor(){}

ConfigEditor::Config ConfigEditor::readConfig(){

    // Read the config file from config.cfg file in main program directory
    QSettings settings("config.cfg", QSettings::IniFormat);

    QString orbiterPath;

    QList<QString> pathsList;

    settings.beginGroup("General");

    orbiterPath = settings.value("OrbiterPath").toString();

    pathsList = settings.value("PathsList").toStringList();

    settings.endGroup();

    QMap<QString, QList<QString>> ignoredMap, dbMap;

    QMap<QString, QString> overMap;

    settings.beginGroup("IgnoredMap");

    QStringList ignKeys = settings.childKeys();

    foreach (QString key, ignKeys) {ignoredMap.insert(key, settings.value(key).toStringList());}

    settings.endGroup();

    settings.beginGroup("DatabaseMap");

    QStringList dbKeys = settings.childKeys();

    foreach (QString key, dbKeys) {dbMap.insert(key, settings.value(key).toStringList());}

    settings.endGroup();

    settings.beginGroup("OverriderMap");

    QStringList overKeys = settings.childKeys();

    foreach (QString key, overKeys) {overMap.insert(key, settings.value(key).toString());}

    settings.endGroup();

    // Return data as config struct
    return {orbiterPath, pathsList, ignoredMap, dbMap, overMap};

}

void ConfigEditor::writeConfig(QString orbiterPath, QList<QString> pathsList, QMap<QString, QList<QString>> ignoredMap,
                               QMap<QString, QList<QString>> dbMap, QMap<QString, QString> overMap){

    QSettings settings("config.cfg", QSettings::IniFormat);

    settings.clear();

    settings.beginGroup("General");

    settings.setValue("OrbiterPath", orbiterPath);

    settings.setValue("PathsList", QVariant(pathsList));

    settings.endGroup();

    settings.beginGroup("IgnoredMap");

    QMap<QString, QList<QString>>::const_iterator ignIterator = ignoredMap.constBegin();

    while (ignIterator != ignoredMap.constEnd()) {

        settings.setValue(ignIterator.key(), QVariant(ignIterator.value()));

        ++ignIterator;

    }

    settings.endGroup();

    settings.beginGroup("DatabaseMap");

    QMap<QString, QList<QString>>::const_iterator dbIterator = dbMap.constBegin();

    while (dbIterator != dbMap.constEnd()) {

        settings.setValue(dbIterator.key(), QVariant(dbIterator.value()));

        ++dbIterator;

    }

    settings.endGroup();

    settings.beginGroup("OverriderMap");

    QMap<QString, QString>::const_iterator overIterator = overMap.constBegin();

    while (overIterator != overMap.constEnd()) {

        settings.setValue(overIterator.key(), QVariant(overIterator.value()));

        ++overIterator;

    }

    settings.endGroup();

}

