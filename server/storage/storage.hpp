#ifndef STORAGE
#define STORAGE

#include <string>

class Storage
{
  public:
    void Get(const std::string);
    void Put(const std::string);
    void PutIfNonExist(const std::string);
    void List(const std::string) const;

  private:
    std::string path_ = "../output/";
};

#endif //STORAGE
