#ifndef STORAGE_MANAGER
#define STORAGE_MANAGER

#include <span>
#include "storage.hpp"
#include "../protocols/IParser.hpp"

class StorageManager
{
  public:
    enum class Operation
    {
        None,
        List,
        Put,
        PutX,
        Get,
        Delete,
        Unknown
    };

    void Execute(Request&);
    void Append(Request&, std::span<const char>);
    std::size_t Read(Request&, std::span<char>);
    StorageStatus GetStatus() const { return status_; }

  private:
    Operation GetOperation(const std::string_view, const std::string_view);

    Storage storage{};
    Operation operation_ = Operation::None;
    StorageStatus status_ = StorageStatus::NotStarted;
};

#endif //STORAGE_MANAGER
