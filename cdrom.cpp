#include "cdrom.h"
#include <math.h>
#include "statusTypes.h"

CDROM::CDROM()
{
    m_amountOfBlocks = ceil((double)driveSize / blockSize);
    m_arr = new char[m_amountOfBlocks]();
    for(int i = 0; i < m_amountOfBlocks; i++){
        m_arr[i] = FREE;
    }

}

char *CDROM::getPlate()
{

    return m_arr;

}

int CDROM::getFreeDiskSpaceInBlocks(){
    int counter = 0;

    for(int i = 0; i < m_amountOfBlocks; i++){
        if(m_arr[i] == FREE){
            counter++;
        }
    }
    return counter;
}

int CDROM::getAmountOfBlocks() const
{
    return m_amountOfBlocks;
}

void CDROM::pushFileToCd(QString fileName, long fileSize)
{
    int blocksNeeded = ceil((double)fileSize / blockSize);
    for(int i = 0; i < blocksNeeded; i++){
        m_arr[i] = RESERVED;
    }
}

void CDROM::clear()
{
    for(int i = 0; i < m_amountOfBlocks; i++){
        m_arr[i] = FREE;

    }
}
