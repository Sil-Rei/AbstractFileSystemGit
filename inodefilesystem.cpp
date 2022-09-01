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
    map<QString,unsigned int> inodeTable;

    inodefilesystem::iNode* inodeArr;
    m_disk = disk;
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
// check if the files blocksize exceeds each of the PtrStorage
// Add the file to the inode-Table and
void inodefilesystem::createFile(int szFile, QString name, unsigned char systemFlag){

    int rndm;
    /*inodefilesystem::singlePtrs* x = new inodefilesystem::singlePtrs();
    inodefilesystem::doublePtrs* y = new inodefilesystem::doublePtrs();
    inodefilesystem::triplePtrs* z = new inodefilesystem::triplePtrs();*/
    inodefilesystem::iNode *currentiNode = createInode("user", 132 , 2);
    currentiNode->fileName = name;

    int blocksNeeded = ceil((double)szFile / m_disk->getBlockSize());
    cout << blocksNeeded;

    if(blocksNeeded >= m_disk->getFreeDiskSpaceInBlocks()) {
      return;
    };
    if(blocksNeeded > 0 && blocksNeeded < 12) {
        currentiNode->sizeFlag = 'a';
    } else if(blocksNeeded > 12 && blocksNeeded < 256) {
        currentiNode->sizeFlag = 'b';
    } else if(blocksNeeded > 256 && blocksNeeded < 65536) {
        currentiNode->sizeFlag = 'c';
    } else if(blocksNeeded > 65536 ) {
        currentiNode->sizeFlag = 'd';
    };

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
            std::cout << "hallo " << std::endl;
            break;
/*
    if(blocksNeeded <= 12) {
        for(int i = 0; i < blocksNeeded; i++) {
            currentiNode->simplePtrs[i] = rndm;
            m_disk->getPlate()[i] = OCCUPIED;

        }

    } else if(blocksNeeded > 12 && blocksNeeded <= 256) {
        x = new inodefilesystem::singlePtrs();
        for(int i = 0; i < blocksNeeded; i++) {
            if(i > 12) {
                x[i] = rndm;
                m_disk->getPlate()[i] = OCCUPIED;
            } else {
                currentiNode->simplePtrs[i] = i;
                m_disk->getPlate()[i] = OCCUPIED;
            }

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
*/
    inodefilesystem::inodeTable.insert(pair<QString, unsigned int>(currentiNode->fileName, currentiNode->iNumb));
    //inodefilesystem::inodeArr[currentiNode->iNumb] = &currentiNode;
}
};


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


bool inodefilesystem::checkName(QString fileName){
    return true;
}
