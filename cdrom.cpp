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
    int currentIndex = m_amountOfBlocks - getFreeDiskSpaceInBlocks();
    filesToBeBurned.append(fileName);
    int blocksNeeded = ceil((double)fileSize / blockSize);
    for(int i = 0; i < blocksNeeded; i++){
        //qDebug() << "index is: " << currentIndex+i;
        m_arr[currentIndex+i] = RESERVED;
    }
}

void CDROM::clear()
{
    for(int i = 0; i < m_amountOfBlocks; i++){
        m_arr[i] = FREE;
    }
    filesToBeBurned.clear();
}

void CDROM::burnCD(QString filepath){
    for(int i = 0; i < m_amountOfBlocks; i++){
        if(m_arr[i] == RESERVED){
            m_arr[i] = OCCUPIED;
        }
    }
    qDebug()<< filesToBeBurned.count();
    for(int i = 0; i < filesToBeBurned.count(); i++){
        //qDebug() << filepath + "cd/" + filesToBeBurned[i];
        QFile file(filepath + "cd/" + filesToBeBurned[i]);
        file.open(QIODevice::ReadWrite);
    }
}

const QList<QString> &CDROM::getFilesToBeBurned() const
{
    return filesToBeBurned;
}

