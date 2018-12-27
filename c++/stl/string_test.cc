#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>

class StringArg // copyable
{
 public:
  StringArg(const char* str)
    : str_(str)
  { }

  StringArg(const std::string& str)
    : str_(str.c_str())
  { }

  const char* c_str() const { return str_; }

 private:
  const char* str_;
};

int StringToInt(StringArg arg) 
{
    return atoi(arg.c_str());
}

int main(int argc, char *argv[])
{
    char id[128] = "123";
    std::string ids("987");

    std::cout << StringToInt(id)  <<  std::endl;
    std::cout << StringToInt(ids) <<  std::endl;

    return 0;
}