#ifndef ABSTRACTFILESYSTEM_H
#define ABSTRACTFILESYSTEM_H
#include <QString>
/**
 * The abstract class of all sub filesystem classes. Defines the outer scope of functions needed
 * for a filesystem
 * @brief The abstractFilesystem class parent class of all filesystems
 */
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
