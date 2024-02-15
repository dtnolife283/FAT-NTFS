#pragma once
#include "BootSector.h"

struct Volume{
    BootSector bootSector;
    std::vector<uint32_t> fatData;
    std::vector<uint8_t> RDETData;
};


