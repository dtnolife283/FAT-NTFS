#include "BootSector.h"

// Open disk
BootSector::BootSector() {
    hDevice = CreateFileW(L"\\\\.\\D:", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        cerr << "Failed to open disk device" << endl;
        exit(1);
    }
}

// Close disk
BootSector::~BootSector() {
    CloseHandle(hDevice);
}

// Read boot sector
void BootSector::ReadBootSector() {
    bResult = ReadFile(hDevice, bBootSector, sizeof(bBootSector), &dwBytesRead, NULL);
    if (!bResult || dwBytesRead != sizeof(bBootSector)) {
        cerr << "Failed to read boot sector" << endl;
        exit(1);
    }
    memcpy(jmpBoot, bBootSector, 3);
    memcpy(OEMName, bBootSector + 3, 8);
    memcpy(BytesPerSector, bBootSector + 11, 2);
    BytesPerSectorInt = (BytesPerSector[1] << 8) + BytesPerSector[0];

    SectorsPerCluster = bBootSector[13];
    SectorsPerClusterInt = bBootSector[13];

    memcpy(SectorBeforeFat, bBootSector + 14, 2);
    SectorBeforeFatInt = (SectorBeforeFat[1] << 8) + SectorBeforeFat[0];

    NumberOfFATs = bBootSector[16];
    NumberOfFATsInt = bBootSector[16];

    memcpy(RootEntries, bBootSector + 17, 2);

    memcpy(TotalSectors16, bBootSector + 19, 2);
    MediaDescriptor = bBootSector[21];

    memcpy(SectorsPerFAT16, bBootSector + 22, 2);

    memcpy(SectorsPerTrack, bBootSector + 24, 2);
    SectorsPerTrackInt = (SectorsPerTrack[1] << 8) + SectorsPerTrack[0];

    memcpy(NumberOfHeads, bBootSector + 26, 2);
    NumberOfHeadsInt = (NumberOfHeads[1] << 8) + NumberOfHeads[0];

    memcpy(Distance, bBootSector + 28, 4);

    memcpy(TotalSectors32, bBootSector + 32, 4);
    TotalSectorsInt = (TotalSectors32[3] << 24) + (TotalSectors32[2] << 16) + (TotalSectors32[1] << 8) + TotalSectors32[0];
    
    memcpy(SectorsPerFAT32, bBootSector + 36, 4);
    SectorsPerFATInt = (SectorsPerFAT32[3] << 24) + (SectorsPerFAT32[2] << 16) + (SectorsPerFAT32[1] << 8) + SectorsPerFAT32[0];

    memcpy(ExtFlag, bBootSector + 40, 2);
    memcpy(FSVersion, bBootSector + 42, 2);

    memcpy(RootCluster, bBootSector + 44, 4);
    RootClusterInt = (RootCluster[3] << 24) + (RootCluster[2] << 16) + (RootCluster[1] << 8) + RootCluster[0];
    
    memcpy(FSInfo, bBootSector + 48, 2);
    FSInfoInt = (FSInfo[1] << 8) + FSInfo[0];

    memcpy(BackupBootSector, bBootSector + 50, 2);
    BackupBootSectorInt = (BackupBootSector[1] << 8) + BackupBootSector[0];

    memcpy(Reserved, bBootSector + 52, 12);

    PhysicalDriveNumber = bBootSector[64];
    PhysicalDriveNumberInt = bBootSector[64];

    Reserved2 = bBootSector[65];
    ExtendedBootSignature = bBootSector[66];

    memcpy(VolumeSerialNumber, bBootSector + 67, 4);
    memcpy(VolumeLabel, bBootSector + 71, 11);
    memcpy(FileSystemType, bBootSector + 82, 8);
    memcpy(BootCode, bBootSector + 90, 420);
    memcpy(BootSig, bBootSector + 510, 2);
}

// Display boot sector in decimal
void BootSector::DisplayBootSector(){
    cout << "Bytes per sector: " << BytesPerSectorInt << endl;
    cout << "Sectors per cluster: " << SectorsPerClusterInt << endl;
    cout << "Sector before FAT: " << SectorBeforeFatInt << endl;
    cout << "Number of FATs: " << NumberOfFATsInt << endl;
    cout << "Sectors per track: " << SectorsPerTrackInt << endl;
    cout << "Number of heads: " << NumberOfHeadsInt << endl;
    cout << "Total sectors (if FAT32): " << TotalSectorsInt << endl;
    cout << "Sectors per FAT: " << SectorsPerFATInt << endl;
    cout << "Root cluster: " << RootClusterInt << endl;
    cout << "FSInfo: " << FSInfoInt << endl;
    cout << "Backup boot sector: " << BackupBootSectorInt << endl;
    cout << "Physical drive number: " << PhysicalDriveNumberInt << endl;    
}