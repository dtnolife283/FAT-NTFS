#include <BootSector.h>

class Item
{
protected:
    std::string m_name;
    int status;
    int size;
    int StartCluster;
    bool isSystem;
    bool isHidden;

public:
    Item();
    std::string getName();
    void setName(std::string name) { m_name = name; }
    void setSize(int size) { this->size = size; }
    virtual int getSize() = 0;
    virtual void print(int depth, bool isHidden) = 0;
    virtual bool isArchive() = 0;
    virtual ~Item() = 0;
};

class File : public Item
{
public:
    File();
    int getSize();
    void print(int depth, bool isHidden);
    bool isArchive();
};

class Folder : public Item
{
private:
    std::vector<Item *> items;

public:
    Folder();
    int getSize();
    void print(int depth, bool isHidden);
    void addItem(Item *item);
    Item *removeByName(std::string name);
    Item *findByName(std::string name);
    bool isArchive();
    ~Folder();
};

Item::Item()
{
    m_name = "";
    status = 0;
    size = 0;
    StartCluster = 0;
}

std::string Item::getName()
{
    return this->m_name;
}

Item::~Item() {}

File::File() {}

int File::getSize()
{
    return this->size;
}

bool File::isArchive()
{
    return true;
}

void File::print(int depth, bool isHidden)
{
    if (isHidden && this->isHidden)
    {
        return;
    }
    for (int i = 0; i < depth; i++)
    {
        std::cout << "+------"
                  << "\t";
    }
    std::cout << this->m_name << " " << this->size << std::endl;
}

Folder::Folder() {}

int Folder::getSize()
{
    int size = 0;
    for (int i = 0; i < this->items.size(); i++)
        size += this->items[i]->getSize();

    return size;
}

void Folder::print(int depth, bool isHidden)
{
    if (isHidden && this->isHidden)
    {
        return;
    }
    for (int i = 0; i < depth; i++)
    {
        std::cout << "+------"
                  << "\t";
    }
    std::cout << this->m_name << " " << this->getSize() << std::endl;

    for (int i = 0; i < this->items.size(); i++)
    {
        this->items[i]->print(depth + 1, isHidden);
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

bool Folder::isArchive()
{
    return false;
}

Folder::~Folder()
{
    for (int i = 0; i < this->items.size(); i++)
    {
        delete this->items[i];
    }
}