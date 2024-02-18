#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>
#include <iomanip>
#include <stdint.h>
using namespace std;

class BootSector{
    public:
        ~BootSector();
        void ReadBootSector(string path);
        void DisplayBootSector();
        std::vector<uint32_t> ReadFAT();
        void ReadRDET();
    private:
        HANDLE hDevice;
        DWORD dwBytesRead;
        BOOL bResult;
        BYTE bBootSector[512];

        BYTE jmpBoot[3];
        BYTE OEMName[8];
        BYTE BytesPerSector[2];
        BYTE SectorsPerCluster;
        BYTE SectorBeforeFat[2];
        BYTE NumberOfFATs;
        BYTE RootEntries[2];
        BYTE TotalSectors16[2];
        BYTE MediaDescriptor;
        BYTE SectorsPerFAT16[2];
        BYTE SectorsPerTrack[2];
        BYTE NumberOfHeads[2];
        BYTE Distance[4];
        BYTE TotalSectors32[4];
        BYTE SectorsPerFAT32[4];
        BYTE ExtFlag[2];
        BYTE FSVersion[2]; 
        BYTE RootCluster[4]; //Cluster bat dau RDET
        BYTE FSInfo[2]; //Sector chua thong tin phu (cluster trong)
        BYTE BackupBootSector[2]; //Sector chua ban luu cua boot sector
        BYTE Reserved[12];
        BYTE PhysicalDriveNumber;
        BYTE Reserved2;
        BYTE ExtendedBootSignature;
        BYTE VolumeSerialNumber[4];
        BYTE VolumeLabel[11];
        BYTE FileSystemType[8];
        BYTE BootCode[420];
        BYTE BootSig[2];


        int BytesPerSectorInt;
        int SectorsPerClusterInt;
        int SectorBeforeFatInt;
        int NumberOfFATsInt;
        int SectorsPerTrackInt;
        int NumberOfHeadsInt;
        int TotalSectorsInt;
        int RootClusterInt;
        int FSInfoInt;
        int BackupBootSectorInt;
        int SectorsPerFATInt;
        int PhysicalDriveNumberInt;

};