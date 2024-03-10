#include "Volume.h"

string getNameMainEntry(vector<uint8_t>& data, uint32_t idx, bool isFolder){
    string result;
    vector<uint8_t> name;
    
    // get the values of bytes that represent name
    for (int i = 0; i < 8; ++i){
        if (data[idx + i] == 0x20)
            continue;
        name.push_back(data[idx+i]);
    }

    // convert it to string
    string Name = "";
    for (int i  = 0; i < name.size(); ++i)
        Name += (char)name[i];
    

    // check its extension
    if (!isFolder){
        vector<uint8_t> extension;
        for (int i = 8; i < 11; ++i){
            extension.push_back(data[idx + i]);
        }
        
        string Extension = "";
        for (int i  = 0; i < extension.size(); ++i)
            Extension += (char)extension[i];
        result = Name + "." + Extension;
        return result;
    }

    return Name;
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
    return data[idx + 28] + data[idx + 29] * pow(16,2) + data[idx + 30] * pow(16,4) + data[idx + 31] * pow(16,6);
}


int getNextSector(vector<uint8_t>& data, int idx, int SectorBeforeFat, int SectorPerFat, int NumberOfFats, int SectorPerCluster, int RootCluster, uint32_t& cluster){
    cluster = data[idx + 26] + data[idx + 27] * pow(16,2) + data[idx + 21] * pow(16,6) + data[idx + 20] * pow(16,4) ;
    int result = SectorBeforeFat + SectorPerFat * NumberOfFats + (cluster - RootCluster) * SectorPerCluster;
    return result; 
}



string readData(vector<uint8_t>& data){
    string result = "";
    int tmp = 0;
    while (data[tmp] != 0x00){
        result += (char)data[tmp++];
    }
    return result;
}


// check extension
bool isTxt(string Name){
    string checkExtension = "";
    for (int i = 3; i >= 1; --i)
        checkExtension += Name[Name.length() - i];
    if (checkExtension == "txt" || checkExtension == "TXT") 
        return true;
    return false;
}

// Trans data in Rdet to vector
vector<Item* > Volume::TransRdet(vector<uint8_t>& data, bool flag, uint32_t startSector, uint32_t& startCluster){
    vector<Item*> result;
    int startIdx = 0;
    if (!flag)
        startIdx = 64;
    for (int i = startIdx; i < data.size(); i += 32){
        if (data[i] == 0xE5 || data[i + 11] == 0x0F)   // if this entry is deleted or sub entry
            continue;

        if (data[i] == 0x00)            // go through all entry
            break;
        
        // get offset
        uint32_t sector = startSector + (i + startIdx) / bootSector.BytesPerSectorInt;
        string Name = ""; 
        int status = data[i + 11];
        bool isFile = (1 & (status >> 5)); 
        bool isFolder = (1 & (status >> 4));        //true: folder, false: file
        bool isSystem = (1 & (status >> 2));
        bool isHidden = (1 & (status >> 1));


        // need to update, name in main entry will be all capitalize so when we find file or folder by name must consider this case
        if (data[i + 11 - 32] == 0x0F)  // if this entry has sub entry
            Name = getNameWithSubEntry(data, i);     // start read name from the latest sub entry   
        else 
            Name = getNameMainEntry(data, i, isFolder);   // else only read name in that entry
        
        int size = 0;
        int sectorOfChild = getNextSector(data, i, bootSector.SectorBeforeFatInt, bootSector.SectorsPerFATInt, bootSector.NumberOfFATsInt, bootSector.SectorsPerClusterInt, bootSector.RootClusterInt, startCluster);
        int numbCluster = 1;
        int tmp = startCluster;
        while (fatData[tmp++] != 0x0FFFFFFF)        // count number of cluster in that folder/file
            numbCluster++;
        
        if (isFile){
            size = getSize(data, i);    // if folder -> size = 0, we need calculate size in composite
            vector<uint8_t> Content = ReadSector(sectorOfChild ,numbCluster * bootSector.SectorsPerCluster);
            string data = "";
            if (!isTxt(Name))
                data = "Please use appropriate program to read this file!\n";
            else {
                data = readData(Content);
            }
            File* file = new File(Name, size, sector, isSystem, isHidden, data);
            result.push_back(file);
        } else {
            //get to child
            
            vector<uint8_t> sdet = ReadSector(sectorOfChild ,numbCluster * bootSector.SectorsPerCluster);
            
            vector<Item*> data = TransRdet(sdet, false, sectorOfChild, startCluster);
            Folder* folder = new Folder(Name, sector, isSystem, isHidden);
            for (auto i : data) 
                folder->addItem(i);
            result.push_back(folder);
        }
    }
    return result;
}



vector<uint8_t> Volume::ReadSector(int sector , int numSector){
    uint64_t sectorOffset = sector * bootSector.BytesPerSectorInt;

    // Seek to the beginning of the sector
    LARGE_INTEGER liOffset;
    liOffset.QuadPart = sectorOffset;
    if (!SetFilePointerEx(bootSector.hDevice, liOffset, NULL, FILE_BEGIN))
    {
        cerr << "Failed to seek to sector" << endl;
        exit(1);
    }

    // Read the sector
    std::vector<uint8_t> sectorData(bootSector.BytesPerSectorInt  * numSector);
    DWORD bytesRead;
    if (!ReadFile(bootSector.hDevice, sectorData.data(), bootSector.BytesPerSectorInt * numSector , &bytesRead, NULL) || bytesRead != bootSector.BytesPerSectorInt * numSector)
    {
        cerr << "Failed to read sector" << endl;
        exit(1);
    }

    for (int i = 0; i < sectorData.size(); ++i)
        sectorData[i] = static_cast<unsigned int> (sectorData[i]);
    
    return sectorData;

}




void Volume::read_fatData() {
    fatData = bootSector.ReadFAT();
}

void Volume::read_RDETData() {
    RDETData = bootSector.ReadRDET();
}