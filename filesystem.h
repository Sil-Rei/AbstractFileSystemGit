#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QString>

class filesystem {
public:
    virtual void createFile(int szFile, QString name, unsigned char systemFlag) = 0;
    virtual void deleteFile(QString fileName) = 0;
    virtual void defrag() = 0;
    virtual bool checkName(QString fileName) = 0;

};


#endif // FILESYSTEM_H
