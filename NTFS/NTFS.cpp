#include "NTFS.h"

vector<int> fileID;
vector<int> parentID;
vector<string> nameFile;
bool check = false; // Kiểm tra file có phải là file text không

// Đọc sector từ ổ đĩa "drive" từ vị trí "readPoint" và lưu vào "sector"
bool readSector(LPCWSTR drive, int readPoint, BYTE *&sector)
{
    DWORD bytesRead;      // lưu số bytes đọc được từ ổ đĩa
    HANDLE device = NULL; // lưu handle của ổ đĩa cần đọc

    // Mở ổ đĩa
    device = CreateFileW(drive,                              // Drive
                         GENERIC_READ,                       // Trạng thái truy cập
                         FILE_SHARE_READ | FILE_SHARE_WRITE, // Share Mode
                         NULL,                               // Security Descriptor (default)
                         OPEN_EXISTING,                      // cách tạo file
                         0,                                  // thuộc tính file
                         NULL);                              // xử lý file mẫu

    if (device == INVALID_HANDLE_VALUE) // Nếu không mở được file
    {
        cerr << "CreateFile : " << GetLastError() << endl; // In ra lỗi và thoát chương trình
        return 0;
    }

    // Đặt vị trí con trỏ file tại vị trí "readPoint" truyền vào bằng 0
    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);

    // Đọc sector
    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        cerr << "ReadFile : " << GetLastError() << endl;
        return 0;
    }
    // Đóng file
    else
    {
        cout << "Thanh cong doc sector" << endl;
        cout << endl;
        return 1;
    }
}

// Đọc "nsect" sector từ ổ đĩa "disk" từ vị trí "readPoint" và lưu vào "DATA"
void readSector2(LPCWSTR disk, BYTE *&DATA, unsigned int _nsect)
{
    DWORD dwBytesRead(0);

    HANDLE hDisk = NULL;
    hDisk = CreateFileW(disk, // Floppy drive cần mở
                        GENERIC_READ,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL);

    if (hDisk != NULL)
    {
        LARGE_INTEGER li;
        li.QuadPart = _nsect * 512;

        // Đặt vị trí con trỏ file tại vị trí "li"
        SetFilePointerEx(hDisk, li, 0, FILE_BEGIN);

        // Đọc boot sector
        if (!ReadFile(hDisk, DATA, 512, &dwBytesRead, NULL))
        {
            cerr << "Xay ra loi khi doc sector" << endl;
        }

        CloseHandle(hDisk);
    }
}

// Lấy number bytes từ vị trí offset
int64_t getBytes(BYTE *sector, int offset, int number)
{
    int64_t extractedBytesToInteger = 0;
    memcpy(&extractedBytesToInteger, sector + offset, number);
    return extractedBytesToInteger;
}

// Chuyển số lượng bytes Data từ vị trí offset thành string
string numToString(BYTE *DATA, int offset, int number)
{
    // tạo mảng temp để lưu kí tự
    char *temp = new char[number + 1];
    memcpy(temp, DATA + offset, number);
    string result = "";

    // chuyển mảng temp thành string
    for (int i = 0; i < number; ++i)
        if (temp[i] != 0x00)
            result += temp[i];

    delete[] temp;
    return result;
}

// Chuyển hệ 10 sang hệ 2
string decimalToBinary(int number)
{
    string result = "";
    while (number > 0)
    {
        result = to_string(number % 2) + result;
        number /= 2;
    }
    return result;
}

// Đọc thông tin của entry $INFORMATION
int readEntryInformation(BYTE *Entry, int start)
{
    // Byte thứ 0 đến 3, Type của attribute
    int status = getBytes(Entry, start + 56, 4);

    // chuyển status sang hệ 2
    string bin = decimalToBinary(status);

    // Kiểm tra các bits ở vị trí cuối cùng của status
    for (int i = bin.length() - 1; i >= 0; i--)
    {
        int n = bin.length();
        // Nếu bit cuối cùng = 1 thì kiểm tra các bit khác
        if (bin[i] == '1')
        {
            // Read Only
            if (i == n - 2)
            {
                // Hệ thống tập tin không thể đọc
                return -1;
            }
            if (i == n - 3)
            {
                // Hệ thống tập tin không thể ghi
                return -1;
            }
        }
    }
    cout << "Attribute $STANDARD_INFORMATION" << endl;

    // Byte thứ 4 đến 7, Kích thước của attribute
    int size = getBytes(Entry, start + 4, 4);
    cout << "\t- Do dai attribute (bao gom header): " << size << endl;
    cout << "\t- Trang thai attribute: " << bin << endl;
    for (int i = bin.length() - 1; i >= 0; i--)
    {
        int n = bin.length();
        if (bin[i] == '1')
        {
            if (i == n - 1)
                cout << "\t\t => Read Only" << endl;
            if (i == n - 4)
                cout << "\t\t => Vollabel" << endl;
            if (i == n - 5)
                cout << "\t\t => Directory" << endl;
            if (i == n - 6)
                cout << "\t\t => Archive" << endl;
        }
    }
    cout << endl;

    // Trả về size của attribute
    return size;
}

// Đọc thông tin của Attribute $FILE_NAME
int readEntryFileName(BYTE *Entry, int start, int ID)
{
    cout << "Attribute $FILE_NAME" << endl;
    int size = getBytes(Entry, start + 4, 4);
    cout << "\t- Do dai attribute (bao gom header): " << size << endl;
    int parent_file = getBytes(Entry, start + 24, 6);
    cout << "\t- File cha: " << parent_file << endl;

    parentID.push_back(parent_file);

    int lengthName = getBytes(Entry, start + 88, 1);
    cout << "\t- Do dai ten File: " << lengthName << endl;
    string name = numToString(Entry, start + 90, lengthName * 2);
    cout << "\t- Ten File: " << name << endl;

    // Lấy đuôi mở rộng
    string exts = "";
    for (int i = name.length() - 1; i >= name.length() - 5; i--)
    {
        if (name[i] == '.')
            break;
        exts += name[i];
    }
    reverse(exts.begin(), exts.end());

    // Hỗ trợ đọc file
    if (exts == "doc" || exts == "docx")
        cout << "\t\t\t Su dung Microsoft Office Word de mo!\n";
    if (exts == "ppt" || exts == "pptx")
        cout << "\t\t\t Su dung Microsoft Office PowerPoint de mo!\n";
    if (exts == "xls" || exts == "xlsx")
        cout << "\t\t\t Su dung Microsoft Office Excel de mo!\n";
    if (exts == "sln" || exts == "cpp" || exts == "java" || exts == "html" || exts == "css")
        cout << "\t\t\t Su dung Microsoft Visual Studio de mo!\n";
    if (exts == "pdf")
        cout << "\t\t\t Su dung Foxit PDF Reader hoac Trinh duyet Web (Edge, Chrome, ...) de mo!\n";

    if (exts == "txt")
        check = true;
    nameFile.push_back(name);
    cout << endl;

    return size;
}

// Đọc thông tin của Attribute $DATA
void readEntryData(BYTE *Entry, int start)
{
    cout << "Attribute $DATA" << endl;
    int size = getBytes(Entry, start + 4, 4);
    cout << "\t- Do dai attribute (bao gom header): " << size << endl;
    int sizeFile = getBytes(Entry, start + 16, 4);
    cout << "\t- Kich thuoc File: " << sizeFile << endl;

    int type = getBytes(Entry, start + 8, 1);
    if (type == 0 && check == true)
    {
        cout << "\t\t=> Resident" << endl;
        int cont_Size = getBytes(Entry, start + 16, 4);
        int cont_Start = getBytes(Entry, start + 20, 2);
        string content = numToString(Entry, start + cont_Start, cont_Size);
        cout << endl;
        cout << "Noi dung: " << content << endl;
    }
    else
        cout << "\t\t=> Non-resident" << endl;
    cout << endl;
}

// Hàm in "tab" lần /t
void printTab(int tab)
{
    for (int i = 0; i < tab; ++i)
        cout << "\t-";
}

// lấy tên file trong mảng nameFile có ID file là "id"
string getNameFile(int id)
{
    string res = "";
    // Xác định index id trong fileID
    int pos = -1;
    for (int i = 0; i < fileID.size(); ++i)
        if (fileID[i] == id)
        {
            pos = i;
            break;
        }

    if (pos != -1)
        res = nameFile[pos];
    return res;
}

// Đọc Bios Parameter Block
void readBPB(BYTE *sector, LPCWSTR disk)
{
    unsigned int bytesPerSector = getBytes(sector, 0x0B, 2);    // Bytes Per Sector
    unsigned int sectorsPerCluster = getBytes(sector, 0x0D, 1); // Sectors Per Cluster
    unsigned int sectorsPerTrack = getBytes(sector, 0x18, 2);   // Sectors Per Track
    unsigned int totalSectors = getBytes(sector, 0x28, 8);      // Total Sectors
    unsigned int MFTStart = getBytes(sector, 0x30, 8);          // Cluster start of MFT
    unsigned int MFTMirrorStart = getBytes(sector, 0x38, 8);    // Cluster start of MFTMirror

    cout << "\nSo Bytes moi Sector : " << bytesPerSector << endl;
    cout << "So Sectors moi Cluster : " << sectorsPerCluster << endl;
    cout << "So Sectors moi Track : " << sectorsPerTrack << endl;
    cout << "Tong so Sectors : " << totalSectors << endl;
    cout << "Cluster bat dau cua MFT : " << MFTStart << endl;
    cout << "Cluster bat dau cua MFTMirror : " << MFTMirrorStart << endl;
    cout << endl;

    // Đọc $MFT Entry
    readMFT(MFTStart, sectorsPerCluster, disk);
}

// Đọc $MFT Entry
void readMFT(unsigned int MFTStart, unsigned int sectors_per_cluster, LPCWSTR disk)
{
    BYTE *MFT = new BYTE[512];
    MFTStart *= sectors_per_cluster;
    readSector2(disk, MFT, MFTStart);

    // INFORMATION
    int Entry_in4 = getBytes(MFT, 0x014, 2);
    cout << "Attribute $INFORMATION Entry bat dau tai: " << Entry_in4 << endl;

    int len_in4 = getBytes(MFT, 0x048, 4);
    cout << "Do dai cua Infomation Entry: " << len_in4 << endl;

    // FILE NAME
    int Entry_Name = Entry_in4 + len_in4;
    cout << "Attribute $FILE NAME Entry bat dau tai: " << Entry_Name << endl;

    int len_Name = getBytes(MFT, 0x09C, 4);
    cout << "Do dai cua $FILE NAME Entry: " << len_Name << endl;

    // DATA
    int tmp = getBytes(MFT, 0x108, 4);
    int Entry_Data = 0;
    if (tmp == 64)
    {
        Entry_Data = Entry_Name + len_Name + getBytes(MFT, 0x10C, 4);
        cout << "Attribute $DATA Entry bat dau tai: " << Entry_Data << endl;
        int len_data = getBytes(MFT, 0x134, 4);
        cout << "Do dai cua Data Entry: " << len_data << endl;
    }
    else
    {
        Entry_Data = Entry_Name + len_Name;
        cout << "Attribute $DATA Entry bat dau tai: " << Entry_Data << endl;
        int len_data = getBytes(MFT, 0x10C, 4);
        cout << "Do dai cua Data Entry: " << len_data << endl;
    }

    // main DATA
    unsigned int len_MFT = MFTStart + (getBytes(MFT, Entry_Data + 24, 8) + 1) * 8;
    cout << "So luong sector trong MFT: " << len_MFT - MFTStart << endl;
    cout << endl;

    // xử lí cây thư mục
    folderTree(len_MFT, MFTStart, disk);

    delete[] MFT;
}

// xử lí cây thư mục
void folderTree(unsigned int len_MFT, unsigned int MFTStart, LPCWSTR disk)
{
    for (int i = 2; i < len_MFT - MFTStart; i += 2)
    {
        int currentSector = MFTStart + i;

        // Tạo mảng lưu sector
        BYTE *currentEntry = new BYTE[512];

        // Đọc sector từ ổ đĩa disk từ vị trí currentSector lưu vào currentEntry
        readSector2(disk, currentEntry, currentSector);
        if (numToString(currentEntry, 0x00, 4) == "FILE")
        {
            check = false;
            int ID = getBytes(currentEntry, 0x02C, 4);
            if (ID > 38)
            {
                cout << endl;
                cout << endl;
                cout << "ID File: " << ID << endl;
                int startInfor = getBytes(currentEntry, 0x014, 2);
                int sizeInfor = readEntryInformation(currentEntry, startInfor);
                if (sizeInfor == -1)
                {
                    delete[] currentEntry;
                    continue;
                }

                int startName = sizeInfor + 56;
                int sizeName = readEntryFileName(currentEntry, startName, ID);
                int startData = startName + sizeName;
                if (getBytes(currentEntry, startData, 4) == 64) // Nếu là $OBJECT
                {

                    int len_obj = getBytes(currentEntry, startData + 4, 4);
                    startData += len_obj;
                    readEntryData(currentEntry, startData);
                }
                else
                {
                    // Tìm sector dấu hiệu của DATA
                    while (getBytes(currentEntry, startData, 4) != 128)
                    {
                        startData += 4;
                    }

                    readEntryData(currentEntry, startData);
                }

                fileID.push_back(ID);
            }
        }

        delete[] currentEntry;
    }

    // in ra cây thư mục
    cout << "---------------------------------------------------------\n";
    cout << "\t \t \t CAY THU MUC: " << endl;
    for (int i = 0; i < fileID.size(); ++i)
        if (fileID[i] != -1 && parentID[i] != -1)
            printFolderTree(fileID[i], -1, i);
}

// In bảng sector
void printSector(BYTE *sector)
{
    int count = 0;
    int num = 0;

    cout << "         0  1  2  3  4  5  6  7    8  9  A  B  C  D  E  F" << endl;

    cout << "0x0" << num << "0  ";

    bool flag = false;
    for (int i = 0; i < 512; ++i)
    {
        count++;
        if (i % 8 == 0)
            cout << "  ";
        printf("%02X ", sector[i]);
        if (i == 255)
        {
            flag = true;
            num = 0;
        }

        if (i == 511)
            break;
        if (count == 16)
        {
            int index = i;

            cout << endl;

            if (!flag)
            {
                ++num;
                if (num < 10)
                    cout << "0x0" << num << "0  ";
                else
                {
                    char hexa = char(num - 10 + 'A');
                    cout << "0x0" << hexa << "0  ";
                }
            }
            else
            {
                if (num < 10)
                    cout << "0x1" << num << "0  ";
                else
                {
                    char hexa = char(num - 10 + 'A');
                    cout << "0x1" << hexa << "0  ";
                }
                ++num;
            }

            count = 0;
        }
    }

    cout << endl;
}

// In cây thư mục
void printFolderTree(int id, int tab, int pos)
{
    tab++;
    printTab(tab);
    cout << getNameFile(id) << endl;

    // Cho ID với parents = -1
    fileID[pos] = -1;
    parentID[pos] = -1;

    vector<int> child;
    vector<int> POS;
    // Tìm số lượng con
    for (int j = 0; j < fileID.size(); ++j)
        if (parentID[j] == id)
        {
            child.push_back(fileID[j]);
            POS.push_back(j);
        }

    if (child.size() == 0)
        return;

    // In con của từng phần tử
    for (int i = 0; i < child.size(); ++i)
        printFolderTree(child[i], tab, POS[i]);
}
