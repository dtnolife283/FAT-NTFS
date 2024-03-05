#include "NTFS.h"

int main()
{
    // Người dùng nhập vào tên địa chỉ ổ đĩa cần đọc (ví dụ: C, D, E, F, ...)
    while (1)
    {
        wstring disk_name;
        cout << "Nhap ten o dia (-1 de thoat): ";
        wcin >> disk_name;
        if (disk_name == L"-1")
        {
            cout << "Thoat chuong trinh! Cam on ban da su dung chuong trinh!";
            break;
        }
        disk_name = L"\\\\.\\" + disk_name + L":";
        LPCWSTR drive = disk_name.c_str();

        // Tạo mảng lưu sector
        BYTE *sector = new BYTE[512];

        // Đọc sector 0 từ ổ đĩa "drive" và lưu vào "sector"
        int flag = readSector(drive, 0, sector);

        // Kiểm tra xem có đọc được không
        if (flag)
        {
            // In ra sector 0
            printSector(sector);

            // Đọc Bios Parameter Block
            readBPB(sector, drive);

            // Xóa mảng lưu sector
            delete[] sector;
        }
        else
        {
            cout << "Dia chi nhap khong hop le hoac khong ton tai" << endl;
        }
    }
}