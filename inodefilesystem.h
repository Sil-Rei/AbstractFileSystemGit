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

    struct fileSys {
      unsigned int driveSize;
      unsigned int iNodeTable; // Needs to be a name to value pair file to inode

    };

    struct singlePtrs {
        unsigned int* ptrs;
    };
    struct doublePtrs {
        unsigned int* ptrs;
    };
    struct triplePtrs {
        unsigned int* ptrs;
    };
    //contains the 12 direct Blockptrs
    struct iNode {
        unsigned int simplePtrs[12];
        singlePtrs* singleptrs;
        doublePtrs* doubleptrs;
        triplePtrs* tripleptrs;
        unsigned int iNumb;
        unsigned int fileSize;
        unsigned int ownerUID;
        QString author;
        QString fileName;
        time_t timeOfCreation = time(0);
    };
    struct inodeFile {
      iNode* referediNode;

    };

    iNode* createInode(QString author, QString fileName, unsigned int fileSize, unsigned int ownerUID);

    void addToDrive(iNode* x);
    unsigned int* locateFile(iNode* x);
    inodefilesystem(Disk* disk);

    virtual void createFile(int szFile, QString name, unsigned char systemFlag);
    virtual void deleteFile(QString fileName);
    virtual void defrag();
    virtual bool checkName(QString fileName);

private:
    Disk* m_disk;
    map<QString, iNode*> inodeTable;
    unsigned int m_iNumb;
};

#endif // INODEFILESYSTEM_H
