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

/**
 * @brief fatFileSystem::createNode creates a node and inits it with null
 * @param index
 * @return returns a pointer to the new node
 */
fatFileSystem::Node * fatFileSystem::createNode(int index){
    Node* node = new Node();
    node->nextNode = nullptr;
    node->lastNode = nullptr;
    node->index = index;
    return node;
}
/**
 * @brief fatFileSystem::appendNode appends a node or creates a node if its the first node of the doubly linked list
 * @param file
 * @param index
 */
void fatFileSystem::appendNode(struct BsFile* file, int index){
    // Create new node, if it's the first node let head point to this new node
    Node* newNode = createNode(index);
    if(file->head == nullptr){
        file->head = newNode;
        return;
    }
    // If it's not the first node, create a temp node, loop from start of array (head) to last node in list
    Node* tempNode;
    tempNode = file->head;
    while(tempNode->nextNode != nullptr){
        tempNode = tempNode->nextNode;
    }
    // Now tempNode contains the address of the last element in the list, because .nextNode points to NULL
    tempNode->nextNode = newNode;
    newNode->lastNode = tempNode;
}

/**
 * @brief fatFileSystem::createBsFat creates a fat table containing blocksizes, drivvesizes etc
 * @param driveSize the size of the drive
 * @param blockSize the size of each block
 * @return returns a pointer to the created fat
 */
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

/**
 * @brief fatFileSystem::getFreeDiskSpace counts free blocks on the disk
 * @return returns the free space counter
 */
int fatFileSystem::getFreeDiskSpace(){
    int freeBlocks = 0;
    for(int i = 0; i < pFat->amountOfBlocks; i++){
        if(pFat->listOfStati[i] == FREE){
            freeBlocks++;
        }
    }
    return freeBlocks;
}

/**
 * Creates a file on the disk. Checks if the file fits and the size is legal. Creates a node containing a random free index on the disk for each needed block.
 * @brief fatFileSystem::createFile creates a file on the disk
 * @param szFile
 * @param name
 * @param systemFlag
 */
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

/**
 * Loops through the files nodes
 * @brief fatFileSystem::deleteFile delets the file with a given name
 * @param fileName the name of the file
 */
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
/**
 * @brief fatFileSystem::showFat prints the underlying array readable for a human
 */
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

/**
 * @brief fatFileSystem::getLargestEmptySpace returns the largest space of free blocks
 * @param arr the pointer to the array disk
 * @param size size of the array
 * @param largestSpaceIndex
 * @param largestSpace
 */
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
/**
 * @brief fatFileSystem::getFragmentation calculates the fragmentation of the disk
 * @return returns the fragmenation percentage
 */
float fatFileSystem::getFragmentation(){
    int freeBlocks = getFreeDiskSpace();

    // get the biggest chunk of free space
    int biggestFreeChunk;
    int nA;
    getLargestEmptySpace(pFat->listOfStati, pFat->blockSize, &nA, &biggestFreeChunk);

    return (((float)freeBlocks - biggestFreeChunk)/(float)freeBlocks)*100;
}
/**
 * @brief fatFileSystem::defrag defrags the disk
 */
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
/**
 * Checks if a given name fits the fat convetion for a filename (8.3 - 1-8 letters followed by an optional 1-3 letters extension. No special chars
 * @brief fatFileSystem::checkName Checks if name fits fat convention
 * @param fileName
 * @return returns true or false wether the name fits
 */
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
/**
 * @brief fatFileSystem::getFileSize returns the filesize for a given filename
 * @param fileName the file name
 * @return returns file size
 */
long fatFileSystem::getFileSize(QString fileName)
{
    for(int i = 0; i < pFat->listOfFiles.size(); i++){
        if(pFat->listOfFiles.at(i)->name == fileName){
            return pFat->listOfFiles.at(i)->fileLength;
        }
    }
    return 0;

}
