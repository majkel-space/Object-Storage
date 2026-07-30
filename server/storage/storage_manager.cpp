#include <iostream>
#include "storage_manager.hpp"

StorageManager::StorageManager(const std::string storage_path) : storage_{storage_path} {}


void StorageManager::Execute(Request& request)
{
    if (request.operation == Operation::None)
        request.operation  = GetOperation(request.method, request.path_str);
    switch (request.operation)
    {
        case Operation::List:
            storage_.List(request);
            break;
        case Operation::Get:
            storage_.Get(request);
            break;
        case Operation::Put:
            storage_.Put(request, request.body);
            break;
        case Operation::PutX:
            storage_.PutIfNonExist(request, request.body);
            break;
    }
    if (request.msg_status == MessageStatus::FinalResponse)
        request.operation = Operation::None;
}

std::size_t StorageManager::Read(Request& request, std::span<char> read_buffer)
{
    std::size_t bytes = storage_.GetChunk(request, read_buffer);
    if (bytes == 0)
        request.msg_status = MessageStatus::FinalResponse;
    return bytes;
}

void StorageManager::Append(Request& request, std::span<const char> data)
{
    storage_.Put(request, data);
}

Operation StorageManager::GetOperation(const std::string_view method, const std::string_view path)
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
    else if (method == "PUT" or method == "SET")
        return Operation::Put;
    else if (method == "SETNX")
        return Operation::PutX;
    return Operation::Unknown;
}
