#pragma once
#include "BootSector.h"

struct Volume{
    BootSector bootSector;
    std::vector<uint32_t> fatData;
    std::vector<uint8_t> RDETData;

    void read_fatData();
    void read_RDETData();
    void DisplayFatData();
    void DisplayRDETData();
};

void Volume::read_fatData() {
    fatData = bootSector.ReadFAT();
}

void Volume::read_RDETData() {
    RDETData = bootSector.ReadRDET();
}

void Volume::DisplayFatData() {
    cout << "FAT Entries:" << endl;
    for (size_t i = 0; i < 30; ++i) {
        cout << "  FAT Entry[" << i << "]: " << fatData[i] << endl;
    }
}

void Volume::DisplayRDETData() {
    cout << "RDET Entries:" << endl;
    for (size_t i = 0; i < RDETData.size(); ++i) {
        cout << std::hex << std::setw(2) << std::setfill('0') << (int)RDETData[i] << " ";
        if ((i + 1) % 16 == 0) {
            cout << endl;
        }
    }
}