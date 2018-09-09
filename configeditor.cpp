#include "configeditor.h"
#include <QSettings>

ConfigEditor::ConfigEditor(){}

ConfigEditor::Config ConfigEditor::readConfig(){

    // Read the config file from config.cfg file in main program directory
    QSettings settings("config.cfg", QSettings::IniFormat);

    QString pathOrbiter;

    settings.beginGroup("General");

    pathOrbiter = settings.value("OrbiterPath").toString();

    settings.endGroup();

    QMap<QString, QList<QString>> dataAddons;

    settings.beginGroup("AddonData");

    QStringList keys = settings.childKeys();

    foreach (QString key, keys) {dataAddons.insert(key, settings.value(key).toStringList());}

    settings.endGroup();

    // Return data as config struct
    return {pathOrbiter, dataAddons};

}

void ConfigEditor::writeConfig(QString pathOrbiter, QMap<QString, QList<QString>> dataAddons){

    QSettings settings("config.cfg", QSettings::IniFormat);

    settings.beginGroup("General");

    settings.setValue("OrbiterPath", pathOrbiter);

    settings.endGroup();

    settings.beginGroup("AddonData");

    QMap<QString, QList<QString>>::const_iterator i = dataAddons.constBegin();

    while (i != dataAddons.constEnd()) {settings.setValue(i.key(), QVariant(i.value())); ++i;}

    settings.endGroup();

}

