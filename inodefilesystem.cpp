#include "inodefilesystem.h"
#include <iostream>
#include <ctime>
#include <math.h>
#include "statusTypes.h"
#include "disk.h"
//p
using namespace std;


inodefilesystem::inodefilesystem(Disk* disk)
{
    map<QString, inodefilesystem::iNode*> inodeTable;
    m_disk = disk;
    m_iNumb = 0;

}
// check if the files blocksize exceeds each of the PtrStorage
// Add the file to the inode-Table and
void inodefilesystem::createFile(int szFile, QString name, unsigned char systemFlag){

    int rndm;
    //Initialize Attributes of the inode
    /**
     * @brief currentiNode
     */
    inodefilesystem::iNode *currentiNode = createInode("user", name, 132 , 2);
    currentiNode->fileName = name;

    int blocksNeeded = ceil((double)szFile / m_disk->getBlockSize());

    if(!(blocksNeeded <= m_disk->getFreeDiskSpaceInBlocks())) {
      return;
    };
    do {
        rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
    } while(m_disk->getPlate()[rndm] != FREE);
    if(blocksNeeded <= 12) {
        for(int i = 0; i < blocksNeeded; i++) {
            currentiNode->simplePtrs[i] = i;
            m_disk->getPlate()[i] = OCCUPIED;
        }

    } else if(blocksNeeded > 12 && blocksNeeded <= 256) {
        inodefilesystem::singlePtrs* x = new inodefilesystem::singlePtrs();
        for(int i = 0; i < blocksNeeded; i++) {
            currentiNode->simplePtrs[i] = i;
            m_disk->getPlate()[i] = OCCUPIED;
        }

    } else if(blocksNeeded > 256 && blocksNeeded <= 65536) {
        inodefilesystem::singlePtrs* x = new inodefilesystem::singlePtrs();
        inodefilesystem::doublePtrs* y = new inodefilesystem::doublePtrs();
        for(int i = 0; i < blocksNeeded; i++) {
            currentiNode->simplePtrs[i] = i;
            m_disk->getPlate()[i] = OCCUPIED;
        }
    } else if(blocksNeeded > 65536 && blocksNeeded <= 16777216) {
        inodefilesystem::singlePtrs* x = new inodefilesystem::singlePtrs();
        inodefilesystem::doublePtrs* y = new inodefilesystem::doublePtrs();
        inodefilesystem::triplePtrs* z = new inodefilesystem::triplePtrs();
        for(int i = 0; i < blocksNeeded; i++) {
            currentiNode->simplePtrs[i] = i;
            m_disk->getPlate()[i] = OCCUPIED;
        }

    inodeTable.insert(pair<QString, iNode*>("hallo", currentiNode));

};
}

unsigned int* locateFile(inodefilesystem::iNode* inode, int blockSize) {
    unsigned int blocksInUse = ceil((double)inode->fileSize / blockSize);
    unsigned int *blockArray = new unsigned int[blocksInUse];
    char blockStatus;
    if(blocksInUse > 0 || blocksInUse <= 12)
    if(blocksInUse > 0 || blocksInUse <= 12) {
      for(int i = 0; i < blocksInUse; i++) {
          blockArray[i] = inode->simplePtrs[i];
      };
    };
    if(blocksInUse > 12 || blocksInUse <= 256) {
      for(int i = 0; i < blocksInUse; i++) {
          blockArray[i] = inode->simplePtrs[i];
      };
    };
    if(blocksInUse > 0 || blocksInUse <= 12) {
      for(int i = 0; i < blocksInUse; i++) {
          blockArray[i] = inode->simplePtrs[i];
      };
    };

    return blockArray;
}
void inodefilesystem::deleteFile(QString fileName){

}

void inodefilesystem::defrag(){
    cout << "Not implemented yet" << endl;
}

inodefilesystem::iNode* inodefilesystem::createInode(QString author, QString fileName, unsigned int fileSize, unsigned int ownerUID) {
    inodefilesystem::iNode* placeholderINode = new inodefilesystem::iNode();
    placeholderINode->author = author;
    placeholderINode->fileName = fileName;
    placeholderINode->iNumb = m_iNumb++;
    placeholderINode->fileSize = fileSize;
    placeholderINode->ownerUID = ownerUID;


    return placeholderINode;
}
bool inodefilesystem::checkName(QString fileName){
    return true;
}
