#include "NTFS.h"

int main()
{
    // xử lí nhập tên ổ đĩa
    wstring disk_name;
    cout << "Nhap ten o dia: ";
    wcin >> disk_name;
    disk_name = L"\\\\.\\" + disk_name + L":";
    LPCWSTR drive = disk_name.c_str();

    // xử lí đọc
    BYTE *sector = new BYTE[512];
    int flag = readSector(drive, 0, sector);

    // nếu đọc thành công
    if (flag == 1)
    {
        // in ra bảng sector vừa đọc
        printSector(sector);

        // Đọc Bios Parameter Block
        readBPB(sector, drive);

        // giải phóng bộ nhớ
        delete[] sector;
    }
    else
    {
        cout << "Khong doc duoc" << endl;
        return 0;
    }
}