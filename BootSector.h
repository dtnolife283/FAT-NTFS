#pragma once
#include <iostream>
#include <Windows.h>

using namespace std;

class BootSector{
    public:
        BootSector();
        ~BootSector();
        void ReadBootSector();
        void DisplayBootSector();
    private:
        HANDLE hDevice;
        DWORD dwBytesRead;
        BOOL bResult;
        BYTE bBootSector[512];

        BYTE jmpBoot[3];
        BYTE OEMName[8];
        BYTE BytesPerSector[2];
        BYTE SectorsPerCluster;
        BYTE ReservedSectors[2];
        BYTE NumberOfFATs;
        BYTE RootEntries[2];
        BYTE TotalSectors16[2];
        BYTE MediaDescriptor;
        BYTE SectorsPerFAT[2];
        BYTE SectorsPerTrack[2];
        BYTE NumberOfHeads[2];
        BYTE HiddenSectors[4];
        BYTE TotalSectors32[4];
        BYTE PhysicalDriveNumber;
        BYTE Reserved;
        BYTE BootSig;
        BYTE VolumeID[4];
        BYTE VolumeLabel[11];
        BYTE FileSystemType[8];
        BYTE BootCode[448];

};