#include <iostream>
#include "storage.hpp"

Storage::Storage(const std::string path) : path_{path} {}

void Storage::Get(Request& request)
{
    request.path = std::filesystem::path(path_ + request.path_str);
    if (request.msg_status == MessageStatus::NotStarted)
    {
        request.msg_status = MessageStatus::Sending;
        if (!std::filesystem::exists(request.path) || std::filesystem::is_directory(request.path))
        {
            request.final_response = "Error: Object " + request.path.string() + " does not exist in Storage\n";
            request.msg_status = MessageStatus::Error;
            return;
        }

        if (request.path.extension() == ".tmp")
        {
            request.final_response = "Error: Object " + request.path.string() + " is temporary. Can't download temporary object\n";
            return;
        }

        if (request.read_file.is_open())
        {
            request.read_file.close();
        }

        request.read_file.open(request.path, std::ios::binary);
        if (!request.read_file.is_open())
        {
            request.final_response = "Storage GET ErrorOpening\n";
            return;
        }
    }
}

std::size_t Storage::GetChunk(Request& request, std::span<char> read_buffer)
{
    const std::filesystem::path full_path = std::filesystem::path(path_ + request.path_str);
    if (request.msg_status == MessageStatus::NotStarted)
        Get(request);

    if (request.msg_status != MessageStatus::Sending || full_path != request.path)
    {
        request.final_response = "Error path mismatch req path " + request.path.string() + " chunk path " + full_path.string() + "\n";
        request.msg_status = MessageStatus::Error;
        return 0;
    }

    request.read_file.read(read_buffer.data(), static_cast<std::streamsize>(read_buffer.size()));
    const std::size_t bytes_read = static_cast<std::size_t>(request.read_file.gcount());

    if (request.read_file.bad())
    {
        request.read_file.close();
        request.path.clear();
        request.final_response = "ifstream bad() Error\n";
        request.msg_status = MessageStatus::Error;
        return 0;
    }

    if (bytes_read == 0)
    {
        request.final_response = "Object " + request.path.string() + " downloaded from Storage\n";
        request.read_file.close();
        request.path.clear();
        request.msg_status = MessageStatus::FinalResponse;
        return 0;
    }

    if (request.read_file.eof() || request.read_file.peek() == std::char_traits<char>::eof())
    {
        request.final_response = "Object " + request.path.string() + " downloaded from Storage\n";
        request.read_file.close();
        request.path.clear();
        request.msg_status = MessageStatus::FinalResponse;
    }

    return bytes_read;
}

void Storage::Put(Request& request, std::span<const char> data)
{
    Write(request, data, OpenMode::Overwrite);
}

void Storage::PutIfNonExist(Request& request, std::span<const char> data)
{
    Write(request, data, OpenMode::CreateOnly);
}

void Storage::List(Request& request) const
{
    request.operation = Operation::List;
    request.msg_status = MessageStatus::Sending;
    request.path = (request.path_str == "*" or request.path_str == "/")
    ? std::filesystem::path(path_)
    : std::filesystem::path(path_ + request.path_str);

    if (std::filesystem::is_directory(request.path ) && std::filesystem::exists(request.path))
    {
        std::ostringstream out;
        for (const auto& it: std::filesystem::recursive_directory_iterator(request.path))
        {
            out << it.path().lexically_relative(request.path).string() << '\n';
        }
        request.final_response = out.str();
    }
    request.msg_status == MessageStatus::FinalResponse;
}

bool Storage::StartWrite(Request& request,
                         OpenMode open_mode)
{
    if (request.msg_status == MessageStatus::NotStarted)
    {
        std::filesystem::create_directories(request.path.parent_path());

        if (open_mode == OpenMode::CreateOnly && std::filesystem::exists(request.path))
        {
            request.final_response = "Object " + request.path.string() + " already exists. New object was not saved\n";
            request.msg_status = MessageStatus::Error;
            return false;
        }

        request.tmp_path = request.path;
        request.tmp_path += ".tmp";
        request.write_file.open(request.tmp_path, std::ios::binary | std::ios::trunc);
        if (!request.write_file.is_open())
        {
            request.final_response = "Can't opent file " + request.path.string() + "\n";
            request.msg_status = MessageStatus::Error;
            return false;
        }

        request.msg_status = MessageStatus::Receiving;
        return true;
    }

    if (request.msg_status != MessageStatus::Receiving)
    {
        request.final_response = "Error: Wrong Messsage Status while reading\n";
        request.msg_status = MessageStatus::Error;
        return false;
    }

    return true;
}

void Storage::FinishWithError(Request& request)
{
    if (request.write_file.is_open())
        request.write_file.close();

    if (!request.tmp_path.empty())
    {
        std::error_code ec;
        std::filesystem::remove(request.tmp_path, ec);
    }

    request.path.clear();
    request.tmp_path.clear();
    request.final_response = "Path " + request.tmp_path.string() + " emtpy\n";
    request.msg_status = MessageStatus::Error;
}

void Storage::FinishComplete(Request& request, const std::string_view message)
{
    request.write_file.flush();
    request.write_file.close();
    std::error_code ec;
    std::filesystem::rename(request.tmp_path, request.path, ec);

    if (ec)
    {
        std::filesystem::remove(request.tmp_path);
        request.msg_status = MessageStatus::Error;
    }
    else
        request.msg_status = MessageStatus::FinalResponse;

    request.final_response = "Object " + request.path.string() + " created in Storage\n";

    request.path.clear();
    request.tmp_path.clear();
}

void Storage::Write(Request& request,
               std::span<const char> data,
               OpenMode open_mode)
{
    request.path = std::filesystem::path(path_ + request.path_str);
    if (request.content_length < data.size())
    {
        FinishWithError(request);
        return;
    }

    if (!StartWrite(request, open_mode))
    {
        return;
    }

    if (!data.empty())
    {
        request.write_file.write(data.data(), static_cast<std::streamsize>(data.size()));
    }

    if (!request.write_file)
    {
        request.final_response = "Not file Error\n";
        FinishWithError(request);
        return;
    }

    request.content_length -= data.size();

    if (request.content_length == 0)
    {
        FinishComplete(request, open_mode == OpenMode::Overwrite
            ? "PUT complete"
            : "PUT_IF_NON_EXIST complete");
    }
}
