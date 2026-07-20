#include <iostream>
#include "storage_manager.hpp"

void StorageManager::Execute(const std::string method, const std::string path)
{
    const Operation operation = GetOperation(method, path);
    std::cout << "OPERATION " << (int)operation << std::endl;
    switch (operation)
    {
        case Operation::List:
            storage.List(path);
            break;
        case Operation::Get:
            storage.Get(path);
            break;
        case Operation::Put:
            storage.Put(path);
            break;
        case Operation::PutX:
            storage.PutIfNonExist(path);
            break;
    }
}

StorageManager::Operation StorageManager::GetOperation(const std::string_view operation, const std::string_view path)
{
    if (operation == "GET")
    {
        if (path == "/")
            return Operation::List;
        else
            return Operation::Get;
    }
    else if (operation == "KEYS *")
        return Operation::List;
    else if (operation == "PUT")
        return Operation::Put;
    else if (operation == "SETNX")
        return Operation::PutX;
    return Operation::Unknown;
}
