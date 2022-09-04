#ifndef ABSTRACTFILESYSTEM_H
#define ABSTRACTFILESYSTEM_H
#include <QString>

class abstractFilesystem
{
public:
    virtual void createFile(int szFile, QString name, unsigned char systemFlag) = 0;
    virtual void deleteFile(QString fileName) = 0;
    virtual void defrag() = 0;
    virtual bool checkName(QString fileName) = 0;
    virtual long getFileSize(QString fileName) = 0;

};

#endif // ABSTRACTFILESYSTEM_H
