#include <filesystem>
#include <iostream>
#include "storage.hpp"

void Storage::Get(const std::string path)
{
    std::filesystem::path p(path_ + path);
    if (std::filesystem::is_directory(p) && std::filesystem::exists(p))
    {
        //TODO how to get an object?
        std::cout << "Object " << p << " recieved from storage\n";
    }
    else
        std::cout << "STORAGE GET PATH NOT EXIST\n";
}

void Storage::Put(const std::string path)
{
    std::filesystem::path p(path_ + path);
    if (std::filesystem::is_directory(p) && std::filesystem::exists(p))
    {
        //TODO how to transmit an object and save it
        std::cout << "Object " << p << " saved into storage\n";
    }
}

void Storage::PutIfNonExist(const std::string path)
{
    std::filesystem::path p(path_ + path);
    if (std::filesystem::is_directory(p))
    {
        if (std::filesystem::exists(p))
        {
            std::cout << "Object " << p << " already exist. New object did not saved\n";
            return;
        }
        else
        {
            //TODO how to get an object?
            std::cout << "Object " << p << " saved into storage\n";
        }
    }
}

void Storage::List(const std::string path) const
{
    std::filesystem::path p(path_ + path);

    if (std::filesystem::is_directory(p) && std::filesystem::exists(p))
    {
        std::cout << "list of objects in storage:\n";
        for (const auto& it: std::filesystem::recursive_directory_iterator(p))
        {
            std::cout << it.path() << std::endl;
        }
    }
}
