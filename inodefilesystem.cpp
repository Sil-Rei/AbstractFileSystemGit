#include "inodefilesystem.h"
#include <iostream>
#include <ctime>
#include <math.h>
#include "statusTypes.h"
#include "disk.h"
#include <QDebug>
//p
using namespace std;

/**
 * @brief inodefilesystem::inodefilesystem initiates new inode based filesystem
 * @param disk
 */
inodefilesystem::inodefilesystem(Disk* disk)
{
    map<QString,unsigned int> m_inodeTable;
    m_disk = disk;
    QList<iNode> m_listOfFiles;
    m_iNumb = 0;
}
/**
 * @brief inodefilesystem::createInode creates a struct iNode and sets the metadata of it based on the params
 * @param author
 * @param fileSize
 * @param ownerUID
 * @return returns an inode ptr to the createFile function
 */
inodefilesystem::iNode* inodefilesystem::createInode(QString author, unsigned int fileSize, unsigned int ownerUID) {
    inodefilesystem::iNode* placeholderINode = new inodefilesystem::iNode();
    placeholderINode->author = author;
    placeholderINode->iNumb = m_iNumb++;
    placeholderINode->fileSize = fileSize;
    placeholderINode->ownerUID = ownerUID;
    for(int i = 0; i < 12; i++) {
        placeholderINode->simplePtrs[i] = -1;
    }
    return placeholderINode;
}
/**
 * @brief inodefilesystem::createFile creates file and a belonging inode
 * @param szFile
 * @param name
 * @param systemFlag
 */
void inodefilesystem::createFile(int szFile, QString name, unsigned char systemFlag){

    int rndm;

    inodefilesystem::iNode *currentiNode = createInode("user", szFile , 2);
    currentiNode->fileName = name;


    int blocksNeeded = ceil((double)szFile / m_disk->getBlockSize());
    qDebug() << blocksNeeded;

    if(blocksNeeded >= m_disk->getFreeDiskSpaceInBlocks()) {
      return;
    };
    //test
    if(blocksNeeded > 0 && blocksNeeded <= 12) {
            currentiNode->sizeFlag = 'a';
            for(int i = 0; i < blocksNeeded; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->simplePtrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }

     } else if(blocksNeeded > 12 && blocksNeeded <= 268) {
            currentiNode->sizeFlag = 'b';
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
                currentiNode->singleptrs.push_back(rndm);
                m_disk->getPlate()[rndm] = OCCUPIED;
            }

     } else if(blocksNeeded > 268 && blocksNeeded <= (65536)) {
            qDebug() << "Reached c";
            currentiNode->sizeFlag = 'c';
            for(int i = 0; i < 12; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->simplePtrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < blocksNeeded - 12; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->singleptrs.push_back(rndm);
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < blocksNeeded -12 - currentiNode->singleptrs.size(); i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->doubleptrs.push_back(rndm);
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
     } else if(blocksNeeded > 65536 && blocksNeeded < (16777216+65536)) {
            currentiNode->sizeFlag = 'd';
            for(int i = 0; i < 12; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->simplePtrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < blocksNeeded - 12; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->singleptrs.push_back(rndm);
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < blocksNeeded -12 - currentiNode->singleptrs.size(); i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->doubleptrs.push_back(rndm);
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < blocksNeeded -12 - currentiNode->singleptrs.size() - currentiNode->doubleptrs.size(); i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->tripleptrs.push_back(rndm);
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
        };

    qDebug() << currentiNode->sizeFlag;


    inodefilesystem::m_inodeTable.insert(pair<QString, unsigned int>(currentiNode->fileName, currentiNode->iNumb));
    m_listOfFiles.append(*currentiNode);

    qDebug() << m_listOfFiles.at(0).fileName;

};

/**
 * @brief inodefilesystem::locateFile helper function to find a file on m_disk
 * @param fileName
 * @return vector<int> stores the positions of the file on m_disk
 */

vector<int> inodefilesystem::locateFile(QString fileName) {

    unsigned int iNumb = m_inodeTable.at(fileName);
    iNode ph = m_listOfFiles.at(iNumb);
    vector<int> blocks;
    int arrSize = *(&ph.simplePtrs + 1) - ph.simplePtrs;
    //int arrSize = sizeof(*ph.simplePtrs)/sizeof(ph.simplePtrs[0]);
    qDebug() << "arrSize:" << arrSize;

    for(int i = 0; i < arrSize; i++) {
        if(ph.simplePtrs[i] == -1) {
            break;
        }
        blocks.push_back(ph.simplePtrs[i]);
    }
    for(int i = 0; i < ph.singleptrs.size(); i++) {
        blocks.push_back(ph.singleptrs[i]);
    }
    for(int i = 0; i < ph.doubleptrs.size(); i++) {
        blocks.push_back(ph.doubleptrs[i]);
    }
    for(int i = 0; i < ph.tripleptrs.size(); i++) {
        blocks.push_back(ph.tripleptrs[i]);
    }
    return blocks;
}
/**
 * @brief inodefilesystem::deleteFile removes a certain file off a m_disk
 * @param fileName
 */
void inodefilesystem::deleteFile(QString fileName){
    //map<QString,unsigned int> m_inodeTable;
    unsigned int iNumb = m_inodeTable.at(fileName);
    iNode ph = m_listOfFiles.at(iNumb);
    char sizeFlag = ph.sizeFlag;
    vector<int> blocks = locateFile(fileName);
    for(int i = 0; i < blocks.size(); i++) {
       qDebug() << blocks[i];
    }

    for(int i = 0; i < blocks.size(); i++) {
        m_disk->getPlate()[blocks[i]] = FREE;
    }

    m_listOfFiles.removeAt(iNumb);/*
    std::map<QString, unsigned int>::iterator it;
    it = m_inodeTable.find(fileName);
    m_inodeTable.erase(it);*/
    qDebug() << "Should have worked out";

}
// ptrType = 0 inside simplePtrs
// ptrType = 1 inside simplePtrs
// ptrType = 2 inside simplePtrs
// ptrType = 3 inside simplePtrs
void inodefilesystem::findPos(iNode *inodeContainer, int i,  int *ptrType, int *positionOfMember) {
    int plateLocation = i;
    for(iNode x: m_listOfFiles) {
        bool found = false;
            for(int y = 0; y < 12; y++) {
                if(x.simplePtrs[i] == 0) {
                    break;
                } else {
                    if(found) {
                        break;
                    } else {
                       if(x.simplePtrs[y] == i) {
                           *inodeContainer = x;
                           *positionOfMember = y;
                           *ptrType = 0;
                           found = true;
                       }
                    }
                }
            }
            for(int z = 0; z < x.singleptrs.size(); z++) {
                if(found) {
                    break;
                } else {
                   if(x.simplePtrs[z] == i) {
                       *inodeContainer = x;
                       *positionOfMember = z;
                       *ptrType = 1;
                       found = true;
                   }
            }

            }
            for(int a = 0; a < x.doubleptrs.size(); a++) {
                if(found) {
                    break;
                } else {
                   if(x.simplePtrs[a] == i) {
                       *inodeContainer = x;
                       *positionOfMember = a;
                       *ptrType = 2;
                       found = true;
                   }
            }

            }
            for(int b = 0; b < x.tripleptrs.size(); b++) {
                if(found) {
                    break;
                } else {
                   if(x.tripleptrs[b] == i) {
                       *inodeContainer = x;
                       *positionOfMember = b;
                       *ptrType = 3;
                       found = true;
                   }
            }

            }

}
}

void inodefilesystem::relocateBlock(iNode* inodeContainer, int ptrType, int index, int newLocation) {
    int rndm;
    do {
        rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
    } while(m_disk->getPlate()[rndm] != FREE);
    if(ptrType == 0) {
        inodeContainer->simplePtrs[index] = newLocation;
    } else if(ptrType == 1) {
        inodeContainer->singleptrs[index] = newLocation;
    } else if(ptrType == 2) {
        inodeContainer->doubleptrs[index] = newLocation;
    } else if(ptrType == 3) {
        inodeContainer->tripleptrs[index] = newLocation;
    }
    m_disk->getPlate()[rndm] = OCCUPIED;
}
/**
 * @brief inodefilesystem::defrag defragments m_disk due to looping through the entire simplePtrs and indirectPtrs to find the fitting positions
 *
 */

void inodefilesystem::defrag(){
    int rndm;
    int globalIndex;
    for(int i = 0; i < m_listOfFiles.size(); i++) {
        int fileSizeInBlocks = ceil((double)m_listOfFiles[i].fileSize  / m_disk->getBlockSize());

        for(int y )
    }
    for(int i = 0; i < m_disk->getAmountOfBlocks(); i++) {
        iNode inodeContainer;
        int positionOfMember;
        int next;
        int ptrType;

        if(m_disk->getPlate()[i]  == OCCUPIED) {
            //if Block is OCCUPIED find inode it belongs to
            findPos(&inodeContainer, m_disk->getPlate()[i], &ptrType, &positionOfMember);
            next = i+1;
            if(ptrType == 0) {
                if(next == inodeContainer.simplePtrs[positionOfMember+1]) {
                   continue;
                } else {
                    int fileSizeInBlocks = ceil((double)inodeContainer.fileSize / m_disk->getBlockSize());
                    //loop
                    for(int q = 0; q < fileSizeInBlocks; q++) {

                    }
                    iNode dBlockInode;
                    int pos;
                    int type;

                    //locate the disturbing block
                    findPos(&dBlockInode, next, &type, &pos);

                    //relocate file
                    relocateBlock(&dBlockInode, type, pos, rndm);



                    //FREE the block which is about to get moved
                    m_disk->getPlate()[next] = FREE;
                    inodeContainer.simplePtrs[positionOfMember] = next;
                }
            } else if(ptrType == 1) {
                if(m_disk->getPlate()[next] == FREE ||  next == inodeContainer.singleptrs[positionOfMember+1]) {
                   break;
                } else {
                    iNode dBlockInode;
                    int pos;
                    int type;
                    //FREE the block which is about to get moved
                    m_disk->getPlate()[next] = FREE;
                    inodeContainer.singleptrs[next] = next;
                    //locate the disturbing block
                    findPos(&inodeContainer, next, &ptrType, &positionOfMember);
                    do {
                        rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                    } while(m_disk->getPlate()[rndm] != FREE);
                    //relocate file
                    inodeContainer.singleptrs[positionOfMember] = rndm;
                    m_disk->getPlate()[rndm] = OCCUPIED;
                }
            } else if(ptrType == 2) {
                if(m_disk->getPlate()[next] == FREE ||  next == inodeContainer.doubleptrs[positionOfMember+1]) {
                   break;
                }
            } else if(ptrType == 3) {
                if(m_disk->getPlate()[next] == FREE ||  next == inodeContainer.doubleptrs[positionOfMember+1]) {
                   break;
                }
            }
            qDebug() << "file is in pos: " << positionOfMember << " was found in ptrType:" << ptrType;
            // while the following block on the disk is not already the second item of the file or is not already FREE
            if(m_disk->getPlate()[i+1] == FREE ||  i+1 == inodeContainer.simplePtrs[positionOfMember+1]) {
              break;
            } else {
                int ph = i+1;
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                //locate the disturbing block
                findPos(&inodeContainer, m_disk->getPlate()[i], &ptrType, &positionOfMember);
                m_disk->getPlate()[rndm] = OCCUPIED;
                // set next plateBlock to inodes next addressed block
                m_disk->getPlate()[i+1] = inodeContainer.simplePtrs[i+1];
            }
}
}
}
/**
 * @brief inodefilesystem::checkName checks if the fileName fulfills the restrictions
 * @param fileName
 * @return fileName passes the restrictions
 */
bool inodefilesystem::checkName(QString fileName){
    //All bytes except NUL ('\0') and '/'
    return true;
}
/**
 * @brief inodefilesystem::getFileSize loops through m_listOfFiles to find the correct inode and accesses the metadata fileSize
 * @param fileName
 * @return fileSize as long
 */
long inodefilesystem::getFileSize(QString fileName)
{
    for(int i = 0; i < m_listOfFiles.size(); i++){
        if(m_listOfFiles.at(i).fileName == fileName){
            return m_listOfFiles.at(i).fileSize;
        }
    }
    return 0;
}
