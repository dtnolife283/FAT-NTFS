#include "BootSector.h"

//Open disk
BootSector::BootSector() {
    hDevice = CreateFileW(L"\\\\.\\D:", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        cerr << "Failed to open disk device" << endl;
        exit(1);
    }
}

//Close disk
BootSector::~BootSector() {
    CloseHandle(hDevice);
}

//Read boot sector
void BootSector::ReadBootSector() {
    bResult = ReadFile(hDevice, bBootSector, sizeof(bBootSector), &dwBytesRead, NULL);
    if (!bResult || dwBytesRead != sizeof(bBootSector)) {
        cerr << "Failed to read boot sector" << endl;
        exit(1);
    }
    memcpy(jmpBoot, bBootSector, 3);
    memcpy(OEMName, bBootSector + 3, 8);
    memcpy(BytesPerSector, bBootSector + 11, 2);
    SectorsPerCluster = bBootSector[13];
    memcpy(ReservedSectors, bBootSector + 14, 2);
    NumberOfFATs = bBootSector[16];
    memcpy(RootEntries, bBootSector + 17, 2);
    memcpy(TotalSectors16, bBootSector + 19, 2);
    MediaDescriptor = bBootSector[21];
    memcpy(SectorsPerFAT, bBootSector + 22, 2);
    memcpy(SectorsPerTrack, bBootSector + 24, 2);
    memcpy(NumberOfHeads, bBootSector + 26, 2);
    memcpy(HiddenSectors, bBootSector + 28, 4);
    memcpy(TotalSectors32, bBootSector + 32, 4);
    PhysicalDriveNumber = bBootSector[36];
    Reserved = bBootSector[37];
    BootSig = bBootSector[38];
    memcpy(VolumeID, bBootSector + 39, 4);
    memcpy(VolumeLabel, bBootSector + 43, 11);
    memcpy(FileSystemType, bBootSector + 54, 8);
    memcpy(BootCode, bBootSector + 62, 448);
}

//Display boot sector in decimal
void BootSector::DisplayBootSector(){
    cout << "Bytes per sector: " << (BytesPerSector[1] << 8) + BytesPerSector[0] << endl;
    cout << "Sectors per cluster: " << (int)SectorsPerCluster << endl;
    cout << "Reserved sectors: " << (ReservedSectors[1] << 8) + ReservedSectors[0] << endl;
    cout << "Number of FATs: " << (int)NumberOfFATs << endl;
    cout << "Root entries: " << (RootEntries[1] << 8) + RootEntries[0] << endl;
    cout << "Total sectors (if FAT16): " << (TotalSectors16[1] << 8) + TotalSectors16[0] << endl;
    cout << "Total sectors (if FAT32): " << (TotalSectors32[3] << 24) + (TotalSectors32[2] << 16) + (TotalSectors32[1] << 8) + TotalSectors32[0] << endl;
    cout << "Sector per track: " << (SectorsPerTrack[1] << 8) + SectorsPerTrack[0] << endl;
    cout << "Number of heads: " << (NumberOfHeads[1] << 8) + NumberOfHeads[0] << endl;
    cout << "Hidden sectors: " << (HiddenSectors[3] << 24) + (HiddenSectors[2] << 16) + (HiddenSectors[1] << 8) + HiddenSectors[0] << endl;
    cout << "Physical drive number: " << (int)PhysicalDriveNumber << endl;
}