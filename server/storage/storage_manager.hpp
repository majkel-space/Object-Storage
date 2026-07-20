#ifndef STORAGE_MANAGER
#define STORAGE_MANAGER

#include "storage.hpp"

class StorageManager
{
  public:
    enum class Operation
    {
        List,
        Put,
        PutX,
        Get,
        Delete,
        Unknown
    };

    void Execute(const std::string, const std::string);

  private:
    Operation GetOperation(const std::string_view, const std::string_view);

    Storage storage{};
};

#endif //STORAGE_MANAGER
