#pragma once
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include <iomanip>
#include <sstream>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

// Đọc sector từ ổ đĩa "drive" từ vị trí "readPoint" và lưu vào "sector"
bool readSector(LPCWSTR drive, int readPoint, BYTE *&sector);

// Đọc "nsect" sector từ ổ đĩa "disk" từ vị trí "readPoint" và lưu vào "DATA"
void readSect2(LPCWSTR disk, BYTE *&DATA, unsigned int _nsect);

// Đọc "number" bytes từ vị trí "offset" của "sector" và lưu vào "DATA"
int64_t getBytes(BYTE *sector, int offset, int number);

// Đọc Bios Parasmeter Block
void readBPB(BYTE *sector, LPCWSTR disk);

// Chuyển "number" bytes DATA từ vị trí "offset" thành string
string numToString(BYTE *DATA, int offset, int number);

// Chuyển hệ 10 sang hệ 2
string decimalToBinary(int n);

// Đọc thông tin của entry $INFORMATION
int readEntryInformation(BYTE *Entry, int start);

// Đọc thông tin của Attribute $FILE_NAME
int readEntryfileName(BYTE *Entry, int start, int ID);

// Đọc thông tin của Attribute $DATA
void readEntryData(BYTE *Entry, int start);

// Hàm in "tab" lần /t
void printTab(int tab);

// lấy tên file trong mảng nameFile có ID file là "id"
string getNameFile(int id);

// Hàm đệ quy in ra cây thư mục
void printFolderTree(int a, int tab, int vt);

// Đọc Bios Parameter Block
void readBPB(BYTE *sector, LPCWSTR disk);

// Đọc $MFT Entry
void readMFT(unsigned int MFTStart, unsigned int sectors_per_cluster, LPCWSTR disk);

// xử lí cây thư mục
void folderTree(unsigned int len_MFT, unsigned int MFTStart, LPCWSTR disk);

// In bảng ra sector
void printSector(BYTE *sector);