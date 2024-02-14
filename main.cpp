#include "Program.h"

int main() {
    Program program;
    program.bootSector.ReadBootSector();
    program.bootSector.DisplayBootSector();
    return 0;
}