#include "fileop.h"
#include <QDirIterator>

FileOp::FileOp(){}

// Scan the given directory and returns a list of files
QList<QString> FileOp::scanDirectory(QString path){

    QDirIterator it(path, QDirIterator::Subdirectories);

    QList<QString> files;

    while (it.hasNext()) {

        it.next();

        // To avoid adding directories
        if(QFileInfo(it.filePath()).isFile()){

            // Convert the path to relative path to the given directory
            QString relPath = QDir(path).relativeFilePath(it.filePath());

            files.append(relPath);

        }

    }

    return files;

}

// Check if the list of files exists in the given directory
bool FileOp::checkFiles(QString pathDir, QList<QString> listFiles){

    foreach(QString file, listFiles){

        // Avoid comparing with these folders as they don't always exists
        if(!file.startsWith("Orbitersdk", Qt::CaseInsensitive) && !file.startsWith("Scenarios", Qt::CaseInsensitive) &&
                !file.startsWith("Doc", Qt::CaseInsensitive) && !file.startsWith("Sources", Qt::CaseInsensitive)){

            QFileInfo fileInfo(pathDir + file);

            if(!fileInfo.exists()){return false;}

        }

    }

    return true;

}

// Check if the files of two list are the same
bool FileOp::checkFiles(QList<QString> listFiles1, QList<QString> listFiles2){

    QList<QString> tempList1 = listFiles1; tempList1.sort();

    QList<QString> tempList2 = listFiles2; tempList2.sort();

    return tempList1 == tempList2;

}
