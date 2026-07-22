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
    void Get(const std::string);
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

    std::string path_ = "../output/";
    std::ofstream file_;
    std::filesystem::path current_path_;
};

#endif //STORAGE
