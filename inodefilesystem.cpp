#include "inodefilesystem.h"
#include <iostream>
#include <ctime>
#include <math.h>
#include "statusTypes.h"
#include "disk.h"
#include <QDebug>
//p
using namespace std;


inodefilesystem::inodefilesystem(Disk* disk)
{
    map<QString,unsigned int> m_inodeTable;
    m_disk = disk;
    inodefilesystem::iNode* m_inodeArr = new inodefilesystem::iNode[disk->getAmountOfBlocks()-2]();
    m_iNumb = 0;

}

inodefilesystem::iNode* inodefilesystem::createInode(QString author, unsigned int fileSize, unsigned int ownerUID) {
    inodefilesystem::iNode* placeholderINode = new inodefilesystem::iNode();
    placeholderINode->author = author;
    placeholderINode->iNumb = m_iNumb++;
    placeholderINode->fileSize = fileSize;
    placeholderINode->ownerUID = ownerUID;


    return placeholderINode;
}

void inodefilesystem::createFile(int szFile, QString name, unsigned char systemFlag){

    int rndm;

    inodefilesystem::iNode *currentiNode = createInode("user", 132 , 2);
    currentiNode->fileName = name;


    int blocksNeeded = ceil((double)szFile / m_disk->getBlockSize());
    qDebug() << blocksNeeded;

    if(blocksNeeded >= m_disk->getFreeDiskSpaceInBlocks()) {
      return;
    };
    if(blocksNeeded > 0 && blocksNeeded < 12) {
        currentiNode->sizeFlag = 'a';
    } else if(blocksNeeded > 12 && blocksNeeded < 256) {
        currentiNode->sizeFlag = 'b';
        currentiNode->singleptrs = new unsigned int[256]();
    } else if(blocksNeeded > 268 && blocksNeeded < 65536) {
        currentiNode->sizeFlag = 'c';
        currentiNode->singleptrs = new unsigned int[256]();
        currentiNode->doubleptrs = new unsigned int[1000]();
    } else if(blocksNeeded > 65536 && blocksNeeded < 16777216) {
        currentiNode->sizeFlag = 'd';
        currentiNode->singleptrs = new unsigned int[256]();
        currentiNode->doubleptrs = new unsigned int[1000]();
        currentiNode->singleptrs = new unsigned int[100000]();
    };
    qDebug() << currentiNode->sizeFlag;
    switch(currentiNode->sizeFlag) {
        //Just 12 Blocks a needed so the simple ptrs are enough
        case 'a':
            for(int i = 0; i < blocksNeeded; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->simplePtrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            break;
        //the File needs 12-256 blocks
        case 'b':

            for(int i = 0; i < 12; i++) {
                qDebug() << "enterd simple ptr loop" << i;
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->simplePtrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < (blocksNeeded-12); i++) {
                qDebug() << "enterd loop" << i;
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                qDebug() << "crash vermutlich durch nullptr";
                currentiNode->singleptrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            break;
        //
        case 'c':
            for(int i = 0; i < 12; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->simplePtrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < blocksNeeded-12; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->singleptrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < blocksNeeded-268; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->doubleptrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            break;
        //
        case 'd':
            for(int i = 0; i < 12; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->simplePtrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < blocksNeeded-12; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->singleptrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < blocksNeeded-268; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->doubleptrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < blocksNeeded-(65536+268); i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->doubleptrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            break;
        default:
            qDebug() << "error occured";
            break;

    };

    //inodefilesystem::m_inodeTable.insert(pair<QString, unsigned int>(currentiNode->fileName, currentiNode->iNumb));
    //inodefilesystem::addToInodeArr(currentiNode, currentiNode->iNumb);

    //qDebug() << ""  << m_inodeArr[currentiNode->iNumb].fileName;

};



unsigned int* locateFile(inodefilesystem::iNode* inode, int blockSize) {
    unsigned int blocksInUse = ceil((double)inode->fileSize / blockSize);
    char sizeFlag = inode->sizeFlag;
    unsigned int *blockArray = new unsigned int[blocksInUse];
    switch(sizeFlag) {
        case 'a':
        break;
        case 'b':
        break;
        case 'c':
        break;
        case 'd':
        break;
        default:
        break;
    };


    return blockArray;
}
void inodefilesystem::deleteFile(QString fileName){

}

void inodefilesystem::defrag(){
    cout << "Not implemented yet" << endl;
}


bool inodefilesystem::checkName(QString fileName){
    return true;
}
