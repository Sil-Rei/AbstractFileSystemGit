#ifndef CDROM_H
#define CDROM_H
#include <QString>

class CDROM
{
public:
    CDROM();
    const int blockSize = 2048;
    const int driveSize = 65000;

    char *getPlate();
    int getFreeDiskSpaceInBlocks();
    int getAmountOfBlocks() const;
    void pushFileToCd(QString fileName, long fileSize);
    void clear();

private:
    char* m_arr;
    int m_amountOfBlocks;
};

#endif // CDROM_H
