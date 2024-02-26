#include "BootSector.h"

// Close disk
BootSector::~BootSector()
{
    CloseHandle(hDevice);
}

// Read boot sector
void BootSector::ReadBootSector(string path)
{
    string diskPath = "\\\\.\\";
    diskPath += path + ":";

    // Convert diskPath to wide character string
    wstring wdPath(diskPath.begin(), diskPath.end());
    hDevice = CreateFileW(wdPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        cerr << "Failed to open disk device" << endl;
        exit(1);
    }

    bResult = ReadFile(hDevice, bBootSector, sizeof(bBootSector), &dwBytesRead, NULL);
    if (!bResult || dwBytesRead != sizeof(bBootSector))
    {
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
void BootSector::DisplayBootSector()
{
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

// Read FAT
std::vector<uint32_t> BootSector::ReadFAT()
{
    uint64_t fatOffset = SectorBeforeFatInt * BytesPerSectorInt;
    cout << "FAT Offset: " << fatOffset << endl;
    // Seek to the beginning of the FAT area
    LARGE_INTEGER liOffset;
    liOffset.QuadPart = fatOffset;
    if (!SetFilePointerEx(hDevice, liOffset, NULL, FILE_BEGIN))
    {
        cerr << "Failed to seek to FAT area" << endl;
        exit(1);
    }

    // Read the FAT entries
    DWORD bytesToRead = NumberOfFATsInt * SectorsPerFATInt * BytesPerSectorInt;
    std::vector<uint8_t> fatData(bytesToRead);
    DWORD bytesRead;
    if (!ReadFile(hDevice, fatData.data(), bytesToRead, &bytesRead, NULL) || bytesRead != bytesToRead)
    {
        cerr << "Failed to read FAT" << endl;
        exit(1);
    }

    // Process FAT entries
    vector<uint32_t> fatEntries;
    for (size_t i = 0; i < fatData.size(); i += 4)
    {
        uint32_t fatEntry = *reinterpret_cast<uint32_t *>(&fatData[i]);
        fatEntries.push_back(fatEntry);
        // cout << "FAT Entry[" << i / 4 << "]: " << std::hex << std::setw(8) << std::setfill('0') << fatEntry << endl;
        // cout << std::dec;
    }

    return fatEntries;
}

// Read RDET
vector<uint8_t> BootSector::ReadRDET()
{
    // Calculate the byte offset to the beginning of the RDET area
    uint64_t rdetOffset = (SectorBeforeFatInt + NumberOfFATsInt * SectorsPerFATInt) * BytesPerSectorInt;
    cout << "RDET Offset: " << rdetOffset << endl;
    if (RootClusterInt != 0)
    {
        uint64_t rootClusterOffset = (RootClusterInt - 2) * SectorsPerClusterInt * BytesPerSectorInt;
        rdetOffset += rootClusterOffset;
    }

    // Seek to the beginning of the RDET area
    LARGE_INTEGER liOffset;
    liOffset.QuadPart = rdetOffset;
    if (!SetFilePointerEx(hDevice, liOffset, NULL, FILE_BEGIN))
    {
        cerr << "Failed to seek to RDET area" << endl;
        exit(1);
    }

    // Count Entries in RDET
    std::vector<uint8_t> tmp(512);
    DWORD bytesRead;
    bool out = false;
    size_t numEntries = 0;
    while (!out)
    {
        if (!ReadFile(hDevice, tmp.data(), 512, &bytesRead, NULL) || bytesRead != 512)
        {
            cerr << "Failed to read RDET" << endl;
            exit(1);
        }
        for (int i = 0; i < 512; i += 32)
        {
            if (static_cast<unsigned int>(tmp[i]) == 0x00)
            {
                out = true;
                break;
            }
            numEntries++;
        }
    }

    // Seek to the beginning of the RDET area
    if (!SetFilePointerEx(hDevice, liOffset, NULL, FILE_BEGIN))
    {
        cerr << "Failed to seek to RDET area" << endl;
        exit(1);
    }

    DWORD bytesToRead = numEntries * 32;
    if (bytesToRead < 512)
        bytesToRead = 512;
    else if (bytesToRead % 512 != 0)
        bytesToRead = (bytesToRead / 512 + 1) * 512;

    // Read the RDET entries
    std::vector<uint8_t> Rdet(bytesToRead);
    if (!ReadFile(hDevice, Rdet.data(), bytesToRead, &bytesRead, NULL) || bytesRead != bytesToRead)
    {
        cerr << "Failed to read RDET" << endl;
        exit(1);
    }

    // Trans rdet to a vector of unsign int for easier calculator
    vector<uint8_t> rdet;
    for (int i = 0; i < bytesToRead; ++i)
    {
        if (i % 32 == 0)
        {
            if (Rdet[i] == 0x00)
                break;
        }
        rdet.push_back(Rdet[i]);
    }
    return rdet;
}



// find the N sub entry, add N entry name to vecto then reverse it
string getNameWithSubEntry(vector<uint8_t>& data, uint32_t idx){
    vector<string> result;
    int tmp = idx;

    // find the idx of entry Nth
    while (tmp - 32 >= 0 && data[tmp - 32 + 11] == 0x0F)
        tmp -= 32;

    // getName from N and push to vecto result
    for (int i = tmp; i < idx; i += 32){
        string s = "";
        if (data[i] == 0x00 || data[i] == 0xE5)
            return s;
        bool flag = true;
        vector <uint8_t> nameZone;
        
        // the next 3 for loop: get name 
        for (int j = 0; j < 10; ++j){
            if (data[i + j + 1] == 0xFF){
                flag = false;
                break;
            } else if (data[i + j + 1] != 0x00){
                nameZone.push_back(data[i + j + 1]);
            }
        }

        for (int j = 0; flag && j < 12; ++j){
            if (data[i + j + 14] == 0xFF){
                flag = false;
                break;
            } else if (data[i + j + 14] == 0x00){
                continue;
            } else {
                nameZone.push_back(data[i + j + 14]);
            }
        }

        for (int j = 0; flag && j < 4; ++j){
            if (data[i + j + 28] == 0xFF){
                flag = false;
                break;
            } else if (data[i + j + 28] != 0x00){
                nameZone.push_back(data[i + j + 28]);
            }
        }
        
        // turn decimal to character
        for (auto j : nameZone){
            s += (char)j;
        }
        nameZone.clear();
        result.push_back(s);
    }

    string s = "";
    // reversed name in vecto
    for (int j = result.size() - 1 ; j >=0; --j){
        s += result[j];
    }
    return s;
}

int getSize(vector<uint8_t>& data, int idx){
    return data[idx + 31] + data[idx + 30] * pow(16,2) + data[idx + 29] * pow(16,4) + data[idx + 28] * pow(16,6);
}

int getNextSector(vector<uint8_t>& data, int idx, int SectorBeforeFat, int SectorPerFat, int NumberOfFats, int SectorPerCluster, int RootCluster){
    int k = data[idx + 26] + data[idx + 27] * pow(16,2) + data[idx + 21] * pow(16,4) + data[idx + 20] * pow(16,6);
    int result = SectorBeforeFat + SectorPerFat * NumberOfFats + (k - RootCluster) * SectorPerCluster;
    return result; 
}



// later change from void to folder* to add to composite
void BootSector::TransRdet(vector<uint8_t>& rdet){
    for (int i = 0; i < rdet.size(); i += 32){
        if (rdet[i] == 0xE5 || rdet[i + 11] == 0x0F)   // if this entry is deleted or sub entry
            continue;
        if (rdet[i + 11 - 32] == 0x0F){      // if this main entry has sub entry
            string Name = ""; 
            Name = getNameWithSubEntry(rdet, i);     // start read name from the latest sub entry   
            int status = rdet[i + 11];
            bool isFolder = (1 & (status >> 4));        //true: folder, false: file
            bool isSystemFolder = (1 & (status >> 2));
            bool isHidden = (1 & (status >> 1));
            int size = 0;
            int sectorOfChild = getNextSector(rdet, i, SectorBeforeFatInt, SectorsPerFATInt, NumberOfFATsInt, SectorsPerClusterInt, RootClusterInt);
            if (!isFolder){
                size = getSize(rdet, i);    // if folder -> size = 0, we need calculate size in composite
                // get Data here
            }
            else {
                //get to child
            }
            cout << Name << " " << isFolder << " " << isSystemFolder << " " << isHidden << '\n';            
        }
    }
}

// Read Sector
vector<uint8_t> BootSector::ReadSector(int sector , int numSector){
    uint64_t sectorOffset = sector * BytesPerSectorInt;

    // Seek to the beginning of the sector
    LARGE_INTEGER liOffset;
    liOffset.QuadPart = sectorOffset;
    if (!SetFilePointerEx(hDevice, liOffset, NULL, FILE_BEGIN))
    {
        cerr << "Failed to seek to sector" << endl;
        exit(1);
    }

    // Read the sector
    std::vector<uint8_t> sectorData(BytesPerSectorInt  * numSector);
    DWORD bytesRead;
    if (!ReadFile(hDevice, sectorData.data(), BytesPerSectorInt * numSector , &bytesRead, NULL) || bytesRead != BytesPerSectorInt * numSector)
    {
        cerr << "Failed to read sector" << endl;
        exit(1);
    }
    return sectorData;
}