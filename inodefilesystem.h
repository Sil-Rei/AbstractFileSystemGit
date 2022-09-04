#ifndef INODEFILESYSTEM_H
#define INODEFILESYSTEM_H
#include <QString>
#include "abstractfilesystem.h"
#include "disk.h"
#include <ctime>
// based on ext2-fileSystem
class inodefilesystem : public abstractFilesystem
{
public:

       //test
    //contains the 12 direct Blockptrs
    struct iNode {
        unsigned int simplePtrs[12];
        unsigned int* singleptrs;
        unsigned int* doubleptrs;
        unsigned int* tripleptrs;
        unsigned int iNumb;
        unsigned int fileSize;
        unsigned int ownerUID;
        QString author;
        QString fileName;
        char sizeFlag;

    };
    struct inodeFile {
      iNode* referediNode;

    };
    void addToInodeArr(iNode* x, int index) {
        m_inodeArr[index] = *x;
    };
    iNode* createInode(QString author, unsigned int fileSize, unsigned int ownerUID);

    unsigned int* locateFile(iNode* x);

    inodefilesystem(Disk* disk);

    virtual void createFile(int szFile, QString name, unsigned char systemFlag);
    virtual void deleteFile(QString fileName);
    virtual void defrag();
    virtual bool checkName(QString fileName);
    virtual long getFileSize(QString fileName);

private:
    Disk* m_disk;
    map<QString, unsigned int> m_inodeTable;
    unsigned int m_iNumb;
    iNode* m_inodeArr;
};

#endif // INODEFILESYSTEM_H

