#ifndef DISK_H
#define DISK_H

#include <QObject>
using namespace std;
/**
 * The Disk class, represents the disk used by a filesystem. It has a char array where
 * each entry represents a block on the simulated harddrive.
 * @brief The Disk class, represents the disk used by a filesystem.
 *
 */
class Disk{
public:
    Disk(int diskSize, int blockSize);
    ~Disk();
    int getDiskSize();
    int getBlockSize();
    int getAmountOfBlocks();
    char *getPlate();

    void setDiskSize(int newDiskSize);

    void setBlockSize(int newBlockSize);

    int getFreeDiskSpaceInBlocks();

    int getLargestEmptySpace();

    float getFragmentation();


private:
    char* m_arr;
    int m_diskSize;
    int m_blockSize;
    int m_amountOfBlocks;
};

#endif // DISK_H
