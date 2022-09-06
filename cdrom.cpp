#include "cdrom.h"
#include <math.h>
#include "statusTypes.h"
#include <QDebug>
#include <QFile>

CDROM::CDROM()
{
    m_amountOfBlocks = ceil((double)driveSize / blockSize);
    m_arr = new char[m_amountOfBlocks]();
    for(int i = 0; i < m_amountOfBlocks; i++){
        m_arr[i] = FREE;
    }

}
/**
 * @brief CDROM::getPlate returns the array representing the disk space
 * @return char array pointer
 */
char *CDROM::getPlate()
{

    return m_arr;

}
/**
 * @brief CDROM::getFreeDiskSpaceInBlocks loops through char array and counts the free blocks
 * @return returns the amount of free blocks on the disk
 */
int CDROM::getFreeDiskSpaceInBlocks(){
    int counter = 0;

    for(int i = 0; i < m_amountOfBlocks; i++){
        if(m_arr[i] == FREE){
            counter++;
        }
    }
    return counter;
}

/**
 * @brief CDROM::getAmountOfBlocks getter of amount of blocks
 * @return returns the amount of blocks on cd
 */
int CDROM::getAmountOfBlocks() const
{
    return m_amountOfBlocks;
}

/**
 * @brief CDROM::pushFileToCd queues a file for burning to cd, sets the blocks to reserved
 * @param fileName name of the file queued
 * @param fileSize size of the file queued
 */
void CDROM::pushFileToCd(QString fileName, long fileSize)
{
    int currentIndex = m_amountOfBlocks - getFreeDiskSpaceInBlocks();
    filesToBeBurned.append(fileName);
    int blocksNeeded = ceil((double)fileSize / blockSize);
    for(int i = 0; i < blocksNeeded; i++){
        //qDebug() << "index is: " << currentIndex+i;
        m_arr[currentIndex+i] = RESERVED;
    }
}

/**
 * @brief CDROM::clear resets the cd
 */
void CDROM::clear()
{
    for(int i = 0; i < m_amountOfBlocks; i++){
        m_arr[i] = FREE;
    }
    filesToBeBurned.clear();
}

/**
 * Loops through the array and sets every reserved block to occupied. Then loops through all queued files and copys the
 * files to the cd folder
 * @brief CDROM::burnCD burns all queued files to the cd
 * @param filepath
 */
void CDROM::burnCD(QString filepath){
    for(int i = 0; i < m_amountOfBlocks; i++){
        if(m_arr[i] == RESERVED){
            m_arr[i] = OCCUPIED;
        }
    }
    //qDebug()<< filesToBeBurned.count();
    for(int i = 0; i < filesToBeBurned.count(); i++){
        //qDebug() << filepath + "cd/" + filesToBeBurned[i];
        QFile file(filepath + "cd/" + filesToBeBurned[i]);
        file.open(QIODevice::ReadWrite);
    }
}
/**
 * @brief CDROM::getFilesToBeBurned returns the list of queued filenames
 * @return returns a reference to the files
 */
const QList<QString> &CDROM::getFilesToBeBurned() const
{
    return filesToBeBurned;
}

