//
// Created by Silas on 25.08.2022.
//

#ifndef BETRIEBSSYSTEME_FATFILESYSTEM_H
#define BETRIEBSSYSTEME_FATFILESYSTEM_H
#include "disk.h"
#include "abstractfilesystem.h"
#include <string>
#include <QList>
using namespace std;
/**
 * The fatfilesystem class is a child class of abstractfilesystem and implements a simulation of the FAT Filesystem used by windows e.g.
 * @brief The fatFileSystem class implements a simulation of a fat file system
 */
class fatFileSystem : public abstractFilesystem{
public:

    struct Node{
        Node* lastNode;
        Node* nextNode;
        int index;
    };

    struct BsFile{
        fatFileSystem::Node* head;
        QString name;
        unsigned char systemFlag;
        unsigned int fileLength;
    };

    struct BsFat{
        unsigned int driveSize;
        unsigned int blockSize;
        unsigned int amountOfBlocks;
        unsigned char* listOfStati;

        QList<BsFile*> listOfFiles;
    };


    BsFat* pFat;
    fatFileSystem(Disk* disk);
    Node* createNode(int index);
    void appendNode(struct BsFile* file, int index);
    BsFat* createBsFat(unsigned int driveSize, unsigned int blockSize);
    int getFreeDiskSpace();
    void showFat();
    void getLargestEmptySpace(unsigned char* arr, unsigned int size, int* largestSpaceIndex, int* largestSpace);
    float getFragmentation();

    virtual void createFile(int szFile, QString name, unsigned char systemFlag);
    virtual void deleteFile(QString fileName);
    virtual void defrag();
    virtual bool checkName(QString fileName);
    virtual long getFileSize(QString fileName);


private:
    Disk* m_disk;

};

#endif //BETRIEBSSYSTEME_FATFILESYSTEM_H
