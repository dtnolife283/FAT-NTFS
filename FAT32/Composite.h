#pragma once

#include <iostream>
#include <vector>
#include <stdint.h>
#include <string>

using namespace std;

class Item
{
protected:
    std::string m_name;
    int size;
    int startSector;
    bool isSystem;
    bool isHidden;

public:
    Item(string name, uint32_t _size, uint32_t _startSector, bool _isSystem, bool _isHidden);
    std::string getName();
    virtual int getSize() = 0;
    virtual void print(int depth) = 0;
    virtual bool isFolder() = 0;
    virtual ~Item() = 0;
    virtual bool removeByName(std::string name, bool& isFolder) = 0;
    virtual Item *findByName(std::string name) = 0;
};

class File : public Item
{
private:
    string data;
public:
    File(string name, uint32_t _size, uint32_t _startSector, bool _isSystem, bool _isHidden, string _data);
    int getSize();
    void print(int depth);
    bool isFolder();
    bool removeByName(std::string name, bool& isFolder);
    Item *findByName(std::string name);
};

class Folder : public Item
{
private:
    std::vector<Item *> items;

public:
    Folder(string name, uint32_t _startSector, bool _isSystem, bool _isHidden);
    int getSize();
    void print(int depth);
    void addItem(Item *item);
    bool removeByName(std::string name, bool& isFolder);
    Item *findByName(std::string name);
    bool isFolder();
    ~Folder();
};
