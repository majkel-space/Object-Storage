#ifndef STORAGE
#define STORAGE

#include <span>
#include <string>
#include <vector>
#include "../protocols/IParser.hpp"

class Storage
{
  public:
    Storage(const std::string);

    void Get(Request&);
    std::size_t GetChunk(Request&, std::span<char>);
    void Put(Request&, std::span<const char>);
    void PutIfNonExist(Request&, std::span<const char>);
    void List(Request&) const;

  private:
    enum class OpenMode
    {
        Overwrite,
        CreateOnly,
    };

    bool StartWrite(Request&, OpenMode);
    void FinishWithError(Request&);
    void FinishComplete(Request&, const std::string_view);
    void Write(Request&, std::span<const char>, OpenMode);

    const std::string path_;
};

#endif //STORAGE
