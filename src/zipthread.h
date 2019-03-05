#ifndef ZIPTHREAD_H
#define ZIPTHREAD_H

#include <QObject>

typedef enum {UNZIP, GETNAMES} OpType;

class ZipThread : public QObject
{
    Q_OBJECT
public:
    ZipThread(QString archivePath, QString extractPath, OpType opType);
    QStringList filesList;
public slots:
    void process();
signals:
    void finished();
private:
    QString archivePath, extractPath;
    OpType opType;
};

#endif // ZIPTHREAD_H
