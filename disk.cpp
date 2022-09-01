#include "disk.h"
#include "statusTypes.h"
#include <math.h>
#include <QDebug>
using namespace std;

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

int Disk::getDiskSize(){
    return m_diskSize;
}
int Disk::getBlockSize(){
    return m_blockSize;
}
int Disk::getAmountOfBlocks(){
    return m_amountOfBlocks;
}

void Disk::setDiskSize(int newDiskSize)
{
    m_diskSize = newDiskSize;
}

void Disk::setBlockSize(int newBlockSize)
{
    m_blockSize = newBlockSize;
}

void Disk::setAmountOfBlocks(int newAmountOfBlocks)
{
    m_amountOfBlocks = newAmountOfBlocks;
}

char *Disk::getPlate()
{
    return m_arr;
}

int Disk::getFreeDiskSpaceInBlocks(){
    int counter = 0;

    for(int i = 0; i < m_amountOfBlocks; i++){
        if(m_arr[i] == FREE){
            counter++;
        }
    }
    return counter;
}

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

float Disk::getFragmentation(){
    int freeBlocks = getFreeDiskSpaceInBlocks();

    // get the biggest chunk of free space

    int largestEmptySpace = getLargestEmptySpace();

    return (((float)freeBlocks - largestEmptySpace)/(float)freeBlocks)*100;
}

Disk::~Disk(){
    delete[] m_arr;
}


