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
    QList<iNode> m_listOfFiles;
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
            for(int i = 0; i < blocksNeeded; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->simplePtrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
     } else if(blocksNeeded > 12 && blocksNeeded < 268) {
            currentiNode->sizeFlag = 'b';
            for(int i = 0; i < 12; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->simplePtrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < 256; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->singleptrs.push_back(rndm);
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
     } else if(blocksNeeded > 268 && blocksNeeded < (65536+268)) {
            currentiNode->sizeFlag = 'c';
            for(int i = 0; i < 12; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->simplePtrs[i] = rndm;
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < 256; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->singleptrs.push_back(rndm);
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < 65636; i++) {
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
            for(int i = 0; i < 256; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->singleptrs.push_back(rndm);
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < 65636; i++) {
                do {
                    rndm = rand() % (m_disk->getAmountOfBlocks() + 1);
                } while(m_disk->getPlate()[rndm] != FREE);
                currentiNode->doubleptrs.push_back(rndm);
                m_disk->getPlate()[rndm] = OCCUPIED;
            }
            for(int i = 0; i < 65636; i++) {
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



vector<int> inodefilesystem::locateFile(QString fileName) {

    unsigned int iNumb = m_inodeTable.at(fileName);
    iNode ph = m_listOfFiles.at(iNumb);
    vector<int> blocks;
    int arrSize = *(&ph.simplePtrs + 1) - ph.simplePtrs;
    //int arrSize = sizeof(*ph.simplePtrs)/sizeof(ph.simplePtrs[0]);
    qDebug() << "arrSize:" << arrSize;
    for(int i = 0; i < arrSize; i++) {
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
void inodefilesystem::deleteFile(QString fileName){
    //map<QString,unsigned int> m_inodeTable;
    unsigned int iNumb = m_inodeTable.at(fileName);
    iNode ph = m_listOfFiles.at(iNumb);
    char sizeFlag = ph.sizeFlag;
    vector<int> blocks = locateFile(fileName);
    for(int i = 0; i < blocks.size(); i++) {
        m_disk->getPlate()[blocks[i]] = FREE;
    }

    m_listOfFiles.removeAt(iNumb);
    std::map<QString, unsigned int>::iterator it;
    it = m_inodeTable.find(fileName);
    m_inodeTable.erase(it);
    qDebug() << "Should have worked out";

}
void inodefilesystem::defrag(){
    cout << "Not implemented yet" << endl;
}


bool inodefilesystem::checkName(QString fileName){
    return true;
}
