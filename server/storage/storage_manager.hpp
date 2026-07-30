#ifndef STORAGE_MANAGER
#define STORAGE_MANAGER

#include <span>
#include "storage.hpp"

class StorageManager
{
  public:
    StorageManager(const std::string);

    void Execute(Request&);
    void Append(Request&, std::span<const char>);
    std::size_t Read(Request&, std::span<char>);

  private:
    Operation GetOperation(const std::string_view, const std::string_view);

    Storage storage_;
};

#endif //STORAGE_MANAGER
