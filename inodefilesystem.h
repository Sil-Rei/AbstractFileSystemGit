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
        std::vector<int> singleptrs{};
        std::vector<int> doubleptrs{};
        std::vector<int> tripleptrs{};
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
    void partialDefrag(vector<int> *indirectPtrs, int* globix );
    iNode* createInode(QString author, unsigned int fileSize, unsigned int ownerUID);

    vector<int> locateFile(QString name);
    void relocateBlock(iNode* inodeContainer, int ptrType, int index);

    inodefilesystem(Disk* disk);
    void findPos(iNode *inodeContainer, int i, int *ptrType, int *positionOfMember);
    virtual void createFile(int szFile, QString name, unsigned char systemFlag);
    virtual void deleteFile(QString fileName);
    virtual void defrag();
    virtual bool checkName(QString fileName);
    virtual long getFileSize(QString fileName);

private:
    Disk* m_disk;
    map<QString, unsigned int> m_inodeTable;
    unsigned int m_iNumb;
    QList<iNode> m_listOfFiles;

};

#endif // INODEFILESYSTEM_H
