#pragma once
#include "BootSector.h"
#include "Volume.h"
#include <string>

void printCentered(const std::string &text, int width)
{
    int padding = (width - text.length()) / 2;
    std::cout << std::string(padding, ' ') << text << std::endl;
}

struct Program
{
    Volume volume;
    void run();
};

void Program::run()
{
    cout << "**************************************************" << endl;
    printCentered("*            Welcome to Our Program            *", 50);
    cout << "**************************************************" << endl;
    cout << endl;

    // Information about students
    string students[] = {"Tran Dang Tuan", "Nguyen Phuoc Sang", "Cao Minh Quang", "Ngo Tan Tai"};
    string studentIDs[] = {"22127438", "12345678", "22127353", "34567890"};
    string className = "22CLC2";

    // Display student information
    cout << "Students:" << endl;
    for (int i = 0; i < 4; ++i)
    {
        cout << "  ";
        printCentered("Name: " + students[i], 40);
        cout << "  ";
        printCentered("Student ID: " + studentIDs[i], 40);
        cout << "  ";
        printCentered("Class: " + className, 40);
        cout << endl;
    }

    while (true)
    {
        cout << "Enter the name of the volume (Ex: C, D,...): ";
        string path;
        cin >> path;

        cout << "Reading boot sector for volume " << path << "..." << endl
             << endl;
        volume.bootSector.ReadBootSector(path);
        cout << "Boot sector successfully read!" << endl;

        cout << "Displaying boot sector information..." << endl
             << endl;
        volume.bootSector.DisplayBootSector();

        volume.read_fatData();
        volume.read_RDETData();

        uint32_t RdetSector = volume.bootSector.SectorBeforeFatInt + volume.bootSector.NumberOfFATsInt * volume.bootSector.SectorsPerFATInt;
        uint32_t startCluster = volume.bootSector.RootClusterInt;
        vector<Item *> data = volume.TransRdet(volume.RDETData, true, RdetSector, startCluster); // chứa mọi data

        // này test thử
        for (auto i : data)
            i->print(0);

        cout << endl;
        cout << "============================" << endl;
        cout << "Volume processing complete." << endl;
        cout << "============================" << endl;

        // Option to find or delete file
        char opt;
        cout << "Do you want to find or delete a file/folder? (f/d/n): ";
        cin >> opt;
        do
        {
            if (opt == 'n' || opt == 'N')
                break;
            std::string name;
            if (opt != 'n' && opt != 'N')
            {
                std::cout << "Enter name: ";
                cin >> name;
                if (opt == 'f' || opt == 'F')
                {
                    Item *item = nullptr;
                    for (int i = 0; i < data.size(); i++)
                    {
                        // find folder
                        if (data[i]->getName() == name)
                        {
                            item = data[i];
                            break;
                        }
                        // find item in folder
                        item = data[i]->findByName(name);
                        if (item != nullptr)
                            break;
                    }
                    if (item != nullptr)
                    {
                        std::cout << "Item Found!" << std::endl;
                        item->print(0);
                    }
                    else
                        std::cout << "Item not found!" << std::endl;
                }
                else
                {
                    bool flag = false;
                    for (int i = 0; i < data.size(); i++)
                    {
                        // delete folder
                        if (data[i]->getName() == name)
                        {
                            delete data[i];
                            data.erase(data.begin() + i);
                            flag = true;
                            break;
                        }
                        // delete item in folder
                        if (data[i]->removeByName(name))
                        {
                            flag = true;
                            break;
                        }
                    }
                    if (flag)
                    {
                        std::cout << "Item deleted!" << std::endl;
                        for (auto i : data)
                            i->print(0);
                    }
                    else
                        std::cout << "Item not found!" << std::endl;
                }
            }
            std::cout << "Do you want to find or delete another file/folder? (f/d/n): ";
            cin >> opt;
        } while (opt != 'n' && opt != 'N');

        // Option to continue or exit
        cout << "Do you want to explore another volume? (y/n): ";
        char choice;
        cin >> choice;
        if (choice != 'y' && choice != 'Y')
        {
            cout << "Exiting Volume Explorer. Goodbye!" << endl;
            break;
        }
    }
}