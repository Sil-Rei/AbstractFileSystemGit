#ifndef CDROM_H
#define CDROM_H
#include <QString>
#include <QList>

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
    void burnCD(QString filepath);

    const QList<QString> &getFilesToBeBurned() const;

protected:
    QList<QString> filesToBeBurned;
private:
    char* m_arr;
    int m_amountOfBlocks;
};

#endif // CDROM_H
