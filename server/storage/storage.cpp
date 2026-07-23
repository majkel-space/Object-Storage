#include <iostream>
#include "storage.hpp"

void Storage::Get(const std::string path, StorageStatus& status)
{
    const std::filesystem::path full_path = std::filesystem::path(path_) / path;
    if (status == StorageStatus::NotStarted)
    {
        if (!std::filesystem::exists(full_path) || std::filesystem::is_directory(full_path))
        {
            status = StorageStatus::Error;
            return;
        }

        if (read_file_.is_open())
        {
            read_file_.close();
        }

        read_file_.open(full_path, std::ios::binary);
        if (!read_file_.is_open())
        {
            status = StorageStatus::Error;
            return;
        }

        read_path_ = full_path;
        status = StorageStatus::Sending;
    }
}

std::size_t Storage::GetChunk(const std::string& path, std::span<char> read_buffer, StorageStatus& status)
{
    const std::filesystem::path full_path = std::filesystem::path(path_) / path;
    if (status == StorageStatus::NotStarted)
        Get(path, status);

    if (status != StorageStatus::Sending || full_path != read_path_)
    {
        std::cout << "Error PATH MISMATCH\n";
        status = StorageStatus::Error;
        return 0;
    }

    read_file_.read(read_buffer.data(), static_cast<std::streamsize>(read_buffer.size()));
    const std::size_t bytes_read = static_cast<std::size_t>(read_file_.gcount());

    if (read_file_.bad())
    {
        read_file_.close();
        read_path_.clear();
        status = StorageStatus::Error;
        return 0;
    }

    if (bytes_read == 0)
    {
        read_file_.close();
        read_path_.clear();
        status = StorageStatus::Complete;
        return 0;
    }

    if (read_file_.eof() || read_file_.peek() == std::char_traits<char>::eof())
    {
        read_file_.close();
        read_path_.clear();
        status = StorageStatus::Complete;
        std::cout << "GET Complete\n";
    }

    return bytes_read;
}

void Storage::Put(const std::string& path, std::size_t& content_length, std::span<const char> data, StorageStatus& status)
{
    Write(path, content_length, data, status, OpenMode::Overwrite);
}


void Storage::PutIfNonExist(const std::string& path, std::size_t& content_length, std::span<const char> data, StorageStatus& status)
{
    Write(path, content_length, data, status, OpenMode::CreateOnly);
}

void Storage::List(const std::string path) const
{
    std::filesystem::path p = (path == "*" or path == "/")
    ? std::filesystem::path(path_)
    : std::filesystem::path(path_ + path);
    if (std::filesystem::is_directory(p) && std::filesystem::exists(p))
    {
        std::cout << "list of objects in storage:\n";
        for (const auto& it: std::filesystem::recursive_directory_iterator(p))
        {
            std::cout << it.path() << std::endl;
        }
    }
}

bool Storage::StartWrite(const std::filesystem::path& full_path,
                         StorageStatus& status,
                         OpenMode open_mode)
{
    if (status == StorageStatus::NotStarted)
    {
        std::filesystem::create_directories(full_path.parent_path());

        if (open_mode == OpenMode::CreateOnly && std::filesystem::exists(full_path))
        {
            std::cout << "Object " << full_path << " already exists. New object was not saved\n";
            status = StorageStatus::Error;
            return false;
        }

        file_.open(full_path, std::ios::binary | std::ios::trunc);
        if (!file_.is_open())
        {
            status = StorageStatus::Error;
            return false;
        }

        write_path_ = full_path;
        status = StorageStatus::Receiving;
        return true;
    }

    if (status != StorageStatus::Receiving || full_path != write_path_)
    {
        std::cout << "Error: path mismatch full " << full_path << " curr " << write_path_ << std::endl;
        status = StorageStatus::Error;
        return false;
    }

    return true;
}

void Storage::FinishWithError(StorageStatus& status)
{
    if (file_.is_open())
    {
        file_.close();
    }
    write_path_.clear();
    status = StorageStatus::Error;
}

void Storage::FinishComplete(StorageStatus& status, const std::string_view message)
{
    file_.flush();
    file_.close();
    write_path_.clear();
    std::cout << message << '\n';
    status = StorageStatus::Complete;
}

void Storage::Write(const std::string& path,
               std::size_t& content_length,
               std::span<const char> data,
               StorageStatus& status,
               OpenMode open_mode)
{
    const std::filesystem::path full_path = std::filesystem::path(path_) / path;
    std::cout << "Write Path " << path << std::endl;
    if (content_length < data.size())
    {
        std::cout << "Content length Error content " << content_length << " data " << (int)data.size() << std::endl;;
        FinishWithError(status);
        return;
    }

    if (!StartWrite(full_path, status, open_mode))
    {
        std::cout << "Not Start Error\n";
        return;
    }

    if (!data.empty())
    {
        std::cout << "Data empty Error\n";
        file_.write(data.data(), static_cast<std::streamsize>(data.size()));
    }

    if (!file_)
    {
        std::cout << "Not file Error\n";
        FinishWithError(status);
        return;
    }

    content_length -= data.size();

    if (content_length == 0)
    {
        std::cout << "Write finish\n";
        FinishComplete(status, open_mode == OpenMode::Overwrite
            ? "PUT complete"
            : "PUT_IF_NON_EXIST complete");
    }
}
