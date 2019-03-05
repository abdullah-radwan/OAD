#ifndef CONFIGEDITOR_H
#define CONFIGEDITOR_H

#include <QMap>

class ConfigEditor
{
public:
	struct Config
	{
		QString orbiterPath;
		QStringList pathsList;
		QMap<QString, QStringList> dbMap, ignoredMap, overMap;
		bool moveTrash;
	};

	ConfigEditor();

	static Config readConfig();

	static void writeConfig(Config config);
};

#endif // CONFIGEDITOR_H
