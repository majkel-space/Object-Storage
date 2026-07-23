#ifndef STORAGE
#define STORAGE

#include <fstream>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

enum class StorageStatus
{
    NotStarted,
    Receiving,
    Sending,
    Complete,
    Error,
};

class Storage
{
  public:
    void Get(const std::string, StorageStatus&);
    std::size_t GetChunk(const std::string&, std::span<char>, StorageStatus&);
    void Put(const std::string&, std::size_t&, std::span<const char>, StorageStatus&);
    void PutIfNonExist(const std::string&, std::size_t&, std::span<const char>, StorageStatus&);
    void List(const std::string) const;

  private:
    enum class OpenMode
    {
        Overwrite,
        CreateOnly,
    };

    bool StartWrite(const std::filesystem::path&, StorageStatus&, OpenMode);
    void FinishWithError(StorageStatus&);
    void FinishComplete(StorageStatus&, const std::string_view);
    void Write(const std::string&, std::size_t&, std::span<const char>, StorageStatus&, OpenMode);

    std::ofstream file_;
    const std::string path_ = "../output/";
    std::filesystem::path write_path_;
    std::filesystem::path read_path_;
    std::ifstream read_file_;
};

#endif //STORAGE
