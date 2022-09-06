#include "disk.h"
#include "statusTypes.h"
#include <math.h>
#include <QDebug>
using namespace std;
/**
 * Disk size gets converted in kilobytes and amountOfBlocks is calculated using the disksize in byte
 * divided by the blocksize
 * @brief Disk::Disk Constructs a disk object
 * @param diskSize how large the disk is in byte
 * @param blockSize how large a block of the disk is in byte
 */
Disk::Disk(int diskSize, int blockSize){

    m_diskSize = diskSize;
    int diskSizeInByte = diskSize*1000;
    m_blockSize = blockSize;
    m_amountOfBlocks = ceil(diskSizeInByte / static_cast<double>(blockSize));
    m_arr = new char[m_amountOfBlocks]();
    for(int i = 0; i < m_amountOfBlocks; i++){
        m_arr[i] = FREE;
    }
    m_arr[0] = RESERVED;
    m_arr[1] = OCCUPIED;

}
/**
 * @brief Disk::getDiskSize returns the disk size
 * @return disk size of object
 */

int Disk::getDiskSize(){
    return m_diskSize;
}

/**
 * @brief Disk::getBlockSize getter of blocksize attribute
 * @return block size of object
 */
int Disk::getBlockSize(){
    return m_blockSize;
}

/**
 *
 * @brief Disk::getAmountOfBlocks getter of block amount
 * @return returns the amount of blocks
 */
int Disk::getAmountOfBlocks(){
    return m_amountOfBlocks;
}

/**
 * @brief Disk::setDiskSize setter for the disk size
 * @param newDiskSize the new disksize
 */
void Disk::setDiskSize(int newDiskSize)
{
    m_diskSize = newDiskSize;
}

/**
 * @brief Disk::setBlockSize setter for the blocksize
 * @param newBlockSize the new blocksize
 */
void Disk::setBlockSize(int newBlockSize)
{
    m_blockSize = newBlockSize;
}

/**
 * @brief Disk::getPlate getter for the char array simulating the drive
 * @return returns the pointer to the char array
 */
char *Disk::getPlate()
{
    return m_arr;
}

/**
 * @brief Disk::getFreeDiskSpaceInBlocks loops over array and counts free blocks
 * @return returns the amount of free blocks in the char array
 */
int Disk::getFreeDiskSpaceInBlocks(){
    int counter = 0;

    for(int i = 0; i < m_amountOfBlocks; i++){
        if(m_arr[i] == FREE){
            counter++;
        }
    }
    return counter;
}

/**
 * @brief Disk::getLargestEmptySpace counts the biggest free block of free spaces of the char array
 * @return returns the int value of biggest space
 */
int Disk::getLargestEmptySpace(){
    int currentStreak = 0;
    int largestSpaceLocal = 0;

    int currentLargestSpaceIndex = -1;
    int largestSpaceIndexLocal = 0;

    for(int i = 0; i < m_amountOfBlocks; i++){
        if(m_arr[i] == FREE){
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
    return largestSpaceLocal;
}
/**
 * @brief Disk::getFragmentation calculates the fragmentation following the wikipedia formula
 * @return returns the fragmentation in percentage
 */
float Disk::getFragmentation(){
    int freeBlocks = getFreeDiskSpaceInBlocks();

    // get the biggest chunk of free space

    int largestEmptySpace = getLargestEmptySpace();

    return (((float)freeBlocks - largestEmptySpace)/(float)freeBlocks)*100;
}
/**
 * @brief Disk::~Disk destructor
 */
Disk::~Disk(){
    delete[] m_arr;
}


