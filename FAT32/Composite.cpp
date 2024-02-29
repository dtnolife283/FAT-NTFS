#include "Composite.h"



Item::Item(string name, uint32_t _size, uint32_t _startSector, bool _isSystem, bool _isHidden)
{
    m_name = name;
    size = _size;
    startSector = _startSector;
    isSystem = _isSystem;
    isHidden = _isHidden;
}

std::string Item::getName()
{
    return this->m_name;
}

Item::~Item() {}

File::File(string name, uint32_t _size, uint32_t _startSector, bool _isSystem, bool _isHidden, string _data) : Item (name, _size, _startSector, _isSystem, _isHidden) {
    data = _data;
}

int File::getSize()
{
    return this->size;
}

bool File::isFolder()
{
    return false;
}

void File::print(int depth)
{
    for (int i = 0; i < depth; i++)
    {
        std::cout << "\t\t";
    }
    std::cout << "File: " << this->m_name << " size: " << this->size << " Sector: " << startSector << std::endl;   // hiển thị tên, trạng thái (file/folder, là System hay Hidden nữa)
    cout << "Data: " << data << "\n";   // này print thử test thôi nào sửa lại lúc tìm file thì mới print data
}

Folder::Folder(string name, uint32_t _startSector, bool _isSystem, bool _isHidden) : Item(name, 0, _startSector, _isSystem, _isHidden){}

int Folder::getSize()
{
    int size = 0;
    for (int i = 0; i < this->items.size(); i++)
        size += this->items[i]->getSize();

    return size;
}

void Folder::print(int depth)
{
    for (int i = 0; i < depth; i++)
    {
        std::cout << "\t\t";
    }
    std::cout << "Folder: " <<  this->m_name << " Size:" << this->getSize() << " Sector: " << startSector << std::endl;

    for (int i = 0; i < this->items.size(); i++)
    {
        this->items[i]->print(depth + 1);
    }
}

void Folder::addItem(Item *item)
{
    this->items.push_back(item);
}

Item *Folder::removeByName(std::string name)
{
    for (int i = 0; i < this->items.size(); i++)
    {
        if (this->items[i]->getName() == name)
        {
            Item *item = this->items[i];
            this->items.erase(this->items.begin() + i);
            return item;
        }
        else if (Folder *folder = dynamic_cast<Folder *>(this->items[i]))
        {
            Item *item = folder->removeByName(name);

            if (item != nullptr)
            {
                return item;
            }
        }
    }

    return nullptr;
}

//chưa test
Item *Folder::findByName(std::string name)
{
    for (int i = 0; i < this->items.size(); i++)
    {
        if (this->items[i]->getName() == name)
        {
            return this->items[i];
        }
        else if (Folder *folder = dynamic_cast<Folder *>(this->items[i]))
        {
            Item *item = folder->findByName(name);

            if (item != nullptr)
            {
                return item;
            }
        }
    }

    return nullptr;
}

bool Folder::isFolder()
{
    return true;
}

Folder::~Folder()
{
    for (int i = 0; i < this->items.size(); i++)
    {
        delete this->items[i];
    }
}

