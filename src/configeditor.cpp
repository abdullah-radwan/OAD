#include "configeditor.h"
#include <QSettings>

ConfigEditor::ConfigEditor() {}

ConfigEditor::Config ConfigEditor::readConfig()
{
	// Read the config file from config.cfg file in main program directory
	QSettings settings("config.cfg", QSettings::IniFormat);

	settings.beginGroup("General");

	QString orbiterPath = settings.value("OrbiterPath").toString();

	QStringList pathsList = settings.value("PathsList").toStringList();

	bool moveTrash = settings.value("MoveTrash").toBool();

	settings.endGroup();

	QMap<QString, QStringList> ignoredMap, dbMap, overMap;

	settings.beginGroup("IgnoredMap");

	QStringList ignKeys = settings.childKeys();

	foreach(QString key, ignKeys) ignoredMap.insert(key, settings.value(key).toStringList());

	settings.endGroup();

	settings.beginGroup("DatabaseMap");

	QStringList dbKeys = settings.childKeys();

	foreach(QString key, dbKeys) dbMap.insert(key, settings.value(key).toStringList());

	settings.endGroup();

	settings.beginGroup("OverriderMap");

	QStringList overKeys = settings.childKeys();

	if (!overKeys.isEmpty())
	{
		if (settings.value(overKeys.first()).toStringList().isEmpty())
			foreach(QString key, overKeys) overMap.insert(key, QStringList(settings.value(key).toString()));
		else foreach(QString key, overKeys) overMap.insert(key, settings.value(key).toStringList());
	}

	settings.endGroup();

	// Return data as config struct
	return { orbiterPath, pathsList, dbMap, ignoredMap, overMap, moveTrash };
}

void ConfigEditor::writeConfig(Config config)
{
	QSettings settings("config.cfg", QSettings::IniFormat);

	settings.clear();

	settings.beginGroup("General");

	settings.setValue("OrbiterPath", config.orbiterPath);

	settings.setValue("PathsList", QVariant(config.pathsList));

	settings.setValue("MoveTrash", config.moveTrash);

	settings.endGroup();

	settings.beginGroup("IgnoredMap");

	QMap<QString, QStringList>::const_iterator ignIterator = config.ignoredMap.constBegin();

	while (ignIterator != config.ignoredMap.constEnd())
	{
		settings.setValue(ignIterator.key(), QVariant(ignIterator.value()));

		++ignIterator;
	}

	settings.endGroup();

	settings.beginGroup("DatabaseMap");

	QMap<QString, QStringList>::const_iterator dbIterator = config.dbMap.constBegin();

	while (dbIterator != config.dbMap.constEnd())
	{
		settings.setValue(dbIterator.key(), QVariant(dbIterator.value()));

		++dbIterator;
	}

	settings.endGroup();

	settings.beginGroup("OverriderMap");

	QMap<QString, QStringList>::const_iterator overIterator = config.overMap.constBegin();

	while (overIterator != config.overMap.constEnd())
	{
		settings.setValue(overIterator.key(), QVariant(overIterator.value()));

		++overIterator;
	}

	settings.endGroup();
}

