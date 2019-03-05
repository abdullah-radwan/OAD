#include "zipthread.h"
#include <JlCompress.h>

ZipThread::ZipThread(QString archivePath, QString extractPath, OpType opType)
{
    this->archivePath = archivePath;
    this->extractPath = extractPath;
    this->opType = opType;
}

void ZipThread::process()
{
    switch (opType) {
    case GETNAMES:
        filesList = JlCompress::getFileList(archivePath);
        break;
    case UNZIP:
        filesList = JlCompress::extractDir(archivePath, extractPath);
        break;
    }

    emit finished();
}
