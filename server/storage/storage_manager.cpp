#include <iostream>
#include "storage_manager.hpp"

void StorageManager::Execute(Request& request)
{
    if (operation_ == Operation::None)
        operation_  = GetOperation(request.method, request.path);
    switch (operation_)
    {
        case Operation::List:
            storage.List(request.path);
            break;
        case Operation::Get:
            storage.Get(request.path, status_);
            break;
        case Operation::Put:
            storage.Put(request.path, request.content_length, request.body, status_);
            break;
        case Operation::PutX:
            storage.PutIfNonExist(request.path, request.content_length, request.body, status_);
            break;
    }
    if (status_ == StorageStatus::Complete)
        operation_ = Operation::None;
}

std::size_t StorageManager::Read(Request& request, std::span<char> read_buffer)
{
    std::size_t bytes = storage.GetChunk(request.path, read_buffer, status_);
    if (bytes == 0)
        status_ = StorageStatus::Complete;
    return bytes;
}

void StorageManager::Append(Request& request, std::span<const char> data)
{
    std::cout << "APPEND path " << request.path << " leng " << request.content_length << std::endl;
    for (const auto& it: data)
        std::cout << it;
    std::cout << std::endl;
    storage.Put(request.path, request.content_length, data, status_);
}

StorageManager::Operation StorageManager::GetOperation(const std::string_view method, const std::string_view path)
{
    if (method == "GET")
    {
        if (path == "/")
            return Operation::List;
        else
            return Operation::Get;
    }
    else if (method == "KEYS")
        return Operation::List;
    else if (method == "PUT")
        return Operation::Put;
    else if (method == "SETNX")
        return Operation::PutX;
    return Operation::Unknown;
}
