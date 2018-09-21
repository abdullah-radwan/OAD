#include "configeditor.h"
#include <QSettings>

ConfigEditor::ConfigEditor(){}

ConfigEditor::Config ConfigEditor::readConfig(){

    // Read the config file from config.cfg file in main program directory
    QSettings settings("config.cfg", QSettings::IniFormat);

    QString orbiterPath;

    settings.beginGroup("General");

    orbiterPath = settings.value("OrbiterPath").toString();

    settings.endGroup();

    QMap<QString, QList<QString>> dbMap;

    QMap<QString, QString> overMap;

    settings.beginGroup("DatabaseMap");

    QStringList dbKeys = settings.childKeys();

    foreach (QString key, dbKeys) {dbMap.insert(key, settings.value(key).toStringList());}

    settings.endGroup();

    settings.beginGroup("OverriderMap");

    QStringList overKeys = settings.childKeys();

    foreach (QString key, overKeys) {overMap.insert(key, settings.value(key).toString());}

    settings.endGroup();

    // Return data as config struct
    return {orbiterPath, dbMap, overMap};

}

void ConfigEditor::writeConfig(QString orbiterPath, QMap<QString, QList<QString>> dbMap,
                               QMap<QString, QString> overMap){

    QSettings settings("config.cfg", QSettings::IniFormat);

    settings.beginGroup("General");

    settings.setValue("OrbiterPath", orbiterPath);

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

