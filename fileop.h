#ifndef FILEOP_H
#define FILEOP_H

#include <QString>
#include <QList>

class FileOp
{
public:
    FileOp();
    static QList<QString> scanDirectory(QString path);
    static bool checkFiles(QString pathDir, QList<QString> listFiles);
    static bool checkFiles(QList<QString> listFiles1, QList<QString> listFiles2);
};

#endif // FILEOP_H
