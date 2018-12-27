#ifndef _STRING_PIECE_H
#define _STRING_PIECE_H
#include <string.h>
#include <string>

class StringArg
{
public:
    StringArg(const char* str)
    :   str_(str),
        len_(strlen(str))
    { }

    StringArg(const std::string& str)
    :   str_(str.c_str()),
        len_(str.size())
    { }

    StringArg(const char* str, size_t len)
    :   str_(str),
        len_(len)
    { }

    const char* c_str() const { return str_; }
    const size_t length() const { return len_; }

private:
    const char* str_;
    size_t len_;
};



#endif
