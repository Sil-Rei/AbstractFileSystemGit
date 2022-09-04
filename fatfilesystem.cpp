//
// Created by Silas on 25.08.2022.
//

#include "fatfilesystem.h"
#include "statusTypes.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <ctime>
#include <iostream>
#include <QDebug>
#include <QMessageBox>

using namespace std;

fatFileSystem::fatFileSystem(Disk* disk) {
    pFat = createBsFat(disk->getDiskSize()*1000, disk->getBlockSize());
    m_disk = disk;
}

fatFileSystem::Node * fatFileSystem::createNode(int index){
    Node* node = new Node();
    node->nextNode = nullptr;
    node->lastNode = nullptr;
    node->index = index;
    return node;
}

void fatFileSystem::appendNode(struct BsFile* file, int index){
    // Create new node, if it's the first node let head point to this new node
    Node* newNode = createNode(index);
    if(file->head == nullptr){
        file->head = newNode;
        return;
    }
    // If it's not the first node, create a temp node, loop from start of array (head) to last node in list
    Node* tempNode = new Node();
    tempNode = file->head;
    while(tempNode->nextNode != nullptr){
        tempNode = tempNode->nextNode;
    }
    // Now tempNode contains the address of the last element in the list, because .nextNode points to NULL
    tempNode->nextNode = newNode;
    newNode->lastNode = tempNode;
}

void fatFileSystem::printList(struct BsFile* file){
    struct Node* printingNode = file->head;
    printf("Printing list: ");
    while(printingNode != nullptr){
        printf("%d ", printingNode->index);
        printingNode = printingNode->nextNode;
    }
    printf("\n");
}

fatFileSystem::BsFat* fatFileSystem::createBsFat(unsigned int driveSize, unsigned int blockSize){
    BsFat* fat = new BsFat();
    fat->blockSize = blockSize;
    fat->driveSize = driveSize;
    fat->amountOfBlocks = (int)ceil((double)driveSize/blockSize);
    printf("Amount of blocks in drive: %d\n", fat->amountOfBlocks);
    fat->listOfStati = new unsigned char[fat->amountOfBlocks];
    for(int i = 0; i < fat->amountOfBlocks; i++){
        fat->listOfStati[i] = FREE;
    }
    fat->listOfStati[0] = RESERVED;
    fat->listOfStati[1] = OCCUPIED;

    return fat;
}


int fatFileSystem::getFreeDiskSpace(){
    int freeBlocks = 0;
    for(int i = 0; i < pFat->amountOfBlocks; i++){
        if(pFat->listOfStati[i] == FREE){
            freeBlocks++;
        }
    }
    return freeBlocks;
}


void fatFileSystem::createFile(int szFile, QString name, unsigned char systemFlag){
    if(szFile <= 0){
        fprintf(stderr, "Filesize cant be 0 or negative");
    }
    int blocksNeeded = ceil((double)szFile / pFat->blockSize);
    int freeDiskSpace = getFreeDiskSpace();

    if(blocksNeeded > freeDiskSpace){
        fprintf(stderr, "Error, FileSize is too large for existing disk space\n");
        exit(-1);
    }

    BsFile* file = new BsFile();
    // Set attributes
    file->name = name;
    file->systemFlag = systemFlag;
    file->fileLength = szFile;
    file->head = nullptr;

    // Get random space in Disk/array of stati for each needed block
    int randomPositionInStatiArray;
    for(int i = 0; i < blocksNeeded; i++){
        // Generate a random position in array, if its not free, next iteration
        do{
            randomPositionInStatiArray = rand() % (pFat->amountOfBlocks + 1);
            //printf("randd: %d\n", randomPositionInStatiArray);
        } while (pFat->listOfStati[randomPositionInStatiArray] != FREE);

        // We now have a random, free position to put the cluster in, first change free to occupied
        pFat->listOfStati[randomPositionInStatiArray] = OCCUPIED;
        m_disk->getPlate()[randomPositionInStatiArray] = OCCUPIED;

        // Then create a node representing the cluster with a new cluster struct with the index in it

        appendNode(file, randomPositionInStatiArray);
    }
    pFat->listOfFiles.append(file);

}

void fatFileSystem::deleteFile(QString fileName){
    for(int i = 0; i < pFat->listOfFiles.count(); i++){
        if(pFat->listOfFiles.at(i)->name == fileName){
            struct Node* tempNode = pFat->listOfFiles.at(i)->head;
            while(tempNode->nextNode != nullptr){
                tempNode = tempNode->nextNode;
            }
            while(tempNode != nullptr){
                pFat->listOfStati[tempNode->index] = FREE;
                m_disk->getPlate()[tempNode->index] = FREE;
                tempNode = tempNode->lastNode;

                // TODO nodes still existing in nirvana
            }
            pFat->listOfFiles.removeAt(i);
            return;
        }
    }
    cout << "No file found named " << fileName.toStdString() << endl;

}

void fatFileSystem::showFat(){
    for(int i = 0; i < pFat->amountOfBlocks; i++){
        if(pFat->listOfStati[i] == FREE){
            printf("|F");
        }else if(pFat->listOfStati[i] == RESERVED){
            printf("|R");
        }else if(pFat->listOfStati[i] == OCCUPIED){
            // Loop through array of files and their nodes to find the correct file index
            for(int x = 0; x < pFat->listOfFiles.count(); x++){
                struct Node* tempNode = pFat->listOfFiles.at(x)->head;
                while(tempNode != NULL){
                    if(tempNode->index == i){
                        printf("|%d", x);
                    }
                    tempNode = tempNode->nextNode;
                }
            }
        }else{
            printf("|x");
        }
    }
    printf("|\n");
}

void fatFileSystem::getLargestEmptySpace(unsigned char* arr, unsigned int size, int* largestSpaceIndex, int* largestSpace){
    int currentStreak = 0;
    int largestSpaceLocal = 0;

    int currentLargestSpaceIndex = -1;
    int largestSpaceIndexLocal = 0;

    for(int i = 0; i < size; i++){
        if(arr[i] == FREE){
            currentStreak++;
            if(currentStreak == 1){
                currentLargestSpaceIndex = i;
            }
        }else{
            if(currentStreak > largestSpaceLocal){
                largestSpaceLocal = currentStreak;
                largestSpaceIndexLocal = currentLargestSpaceIndex;
            }
            currentStreak = 0;
            currentLargestSpaceIndex = 0;
        }
    }
    *largestSpaceIndex = largestSpaceIndexLocal;
    *largestSpace = largestSpaceLocal;
}

float fatFileSystem::getFragmentation(){
    int freeBlocks = getFreeDiskSpace();

    // get the biggest chunk of free space
    int biggestFreeChunk;
    int nA;
    getLargestEmptySpace(pFat->listOfStati, pFat->blockSize, &nA, &biggestFreeChunk);

    return (((float)freeBlocks - biggestFreeChunk)/(float)freeBlocks)*100;
}

void fatFileSystem::defrag(){
    // Check if enough space for defrag is available
//    if((double)getFreeDiskSpace()/pFat->amountOfBlocks < 0.1){
//        fprintf(stderr, "Not enough space for defragmentation.\n");
//        exit(-1);
//    }

    //create new empty "plate"
    unsigned char* newArr = new unsigned char[pFat->amountOfBlocks];

    // Fill in potential defects
    for(int i = 0; i < pFat->amountOfBlocks; i++){
        if(pFat->listOfStati[i] == MALFUNCTION){
            newArr[i] = MALFUNCTION;
            continue;
        }
        newArr[i] = FREE;
        m_disk->getPlate()[i] = FREE;
    }
        m_disk->getPlate()[0] = RESERVED;
        m_disk->getPlate()[1] = OCCUPIED;
        newArr[0] = RESERVED;
        newArr[1] = OCCUPIED;

    for(int file = 0; file < pFat->listOfFiles.count(); file++){

        Node* tempNode = pFat->listOfFiles.at(file)->head;
        while(tempNode != NULL){
            for(int block = 0; block < pFat->amountOfBlocks; block++){
                if(newArr[block] == FREE){
                    newArr[block] = OCCUPIED;
                    m_disk->getPlate()[block] = OCCUPIED;
                    tempNode->index = block;
                    break;
                }
            }
            tempNode = tempNode->nextNode;
        }
    }
    pFat->listOfStati = newArr;
    for(int i = 0; i < pFat->amountOfBlocks; i++){
        printf("%d ", newArr[i]);
    }
    printf("\n");
}

bool fatFileSystem::checkName(QString fileName){
    if(fileName.contains(".")){
        if(fileName.size() > 12){
            QMessageBox box;
            box.setText("Filename ist nicht zwischen 1-8 Zeichen lang bzw. 1-12 mit optionaler Extension.");
            box.setIcon(QMessageBox::Warning);
            box.addButton("OK", QMessageBox::AcceptRole);
            box.exec();
            return false;
        }else if(fileName.split(".")[1].size() > 3 || fileName.split(".")[1].size() < 1){
            QMessageBox box;
            box.setText("Filename ist nicht zwischen 1-8 Zeichen lang bzw. 1-12 mit optionaler Extension.");
            box.setIcon(QMessageBox::Warning);
            box.addButton("OK", QMessageBox::AcceptRole);
            box.exec();
            return false;
        }
    }else{
        if(fileName.size() > 8 || fileName.size() < 1){
            QMessageBox box;
            box.setText("Filename ist nicht zwischen 1-8 Zeichen lang bzw. 1-12 mit optionaler Extension.");
            box.setIcon(QMessageBox::Warning);
            box.addButton("OK", QMessageBox::AcceptRole);
            box.exec();
            return false;
        }
    }
    // Check if name is already in use
    for(int i = 0; i < pFat->listOfFiles.count(); i++){
        if(pFat->listOfFiles.at(i)->name == fileName){
            QMessageBox box;
            box.setText("Filename existiert bereits.");
            box.setIcon(QMessageBox::Warning);
            box.addButton("OK", QMessageBox::AcceptRole);
            box.exec();
            return false;
        }
    }

    // Check if name contains forbidden chars
    for(int i = 0; i < fileName.size(); i++){
        if(!fileName[i].isLetterOrNumber() && fileName[i] != QString("_") && fileName[i] != QString(".")){
            QMessageBox box;
            box.setText("Filename enthält unzulässige Zeichen.");
            box.setIcon(QMessageBox::Warning);
            box.addButton("OK", QMessageBox::AcceptRole);
            box.exec();
            return false;
        }
    }

    return true;
}

long fatFileSystem::getFileSize(QString fileName)
{
    for(int i = 0; i < pFat->listOfFiles.size(); i++){
        if(pFat->listOfFiles.at(i)->name == fileName){
            return pFat->listOfFiles.at(i)->fileLength;
        }
    }
    return 0;

}
