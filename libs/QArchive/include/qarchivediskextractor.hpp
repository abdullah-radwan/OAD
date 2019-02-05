#ifndef QARCHIVE_DISK_EXTRACTOR_HPP_INCLUDED
#define QARCHIVE_DISK_EXTRACTOR_HPP_INCLUDED
#include <QScopedPointer>
#include <QFile>
#include <QString>
#include <QObject>
#include <QThread>
#include <QJsonObject>

namespace QArchive
{
class DiskExtractorPrivate;
class DiskExtractor : public QObject
{
    Q_OBJECT
public:
    DiskExtractor(QObject *parent = nullptr, bool singleThreaded = true);
    DiskExtractor(QFile*, QObject *parent = nullptr, bool singleThreaded = true);
    DiskExtractor(const QString&, QObject *parent = nullptr,  bool singleThreaded = true);
    DiskExtractor(const QString&, const QString&, QObject *parent = nullptr, bool singleThreaded = true );
    ~DiskExtractor();
public Q_SLOTS:
    void setArchive(QFile*);
    void setArchive(const QString&);
    void setArchive(const QString&, const QString&);
    void setBlockSize(int);
    void setCalculateProgress(bool);
    void setOutputDirectory(const QString&);
    void setPassword(const QString&);
    void addFilter(const QString&);
    void addFilter(const QStringList&);
    void clear();

    void getInfo();

    void start();
    void cancel();
    void pause();
    void resume();

Q_SIGNALS:
    void started();
    void canceled();
    void paused();
    void resumed();
    void finished();
    void filesFinished();
    void error(short , QString);
    void progress(QString, int, int, int);
    void getInfoRequirePassword(int);
    void extractionRequirePassword(int);
    void info(QJsonObject);

private:
    QScopedPointer<DiskExtractorPrivate> m_Extractor;
    QScopedPointer<QThread> m_Thread;
};
}
#endif // QARCHIVE_DISK_EXTRACTOR_HPP_INCLUDED
