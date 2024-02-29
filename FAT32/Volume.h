#pragma once
#include "BootSector.h"
#include "Composite.h"
#include <cmath>

struct Volume{
    BootSector bootSector;
    std::vector<uint32_t> fatData;
    std::vector<uint8_t> RDETData;

    vector<Item*> TransRdet(vector<uint8_t>& data, bool flag, uint32_t startOffset, uint32_t& startCluster);
    vector<uint8_t> ReadSector(int sector , int numSector);
    void read_fatData();
    void read_RDETData();
};

