#ifndef MOD_MODSERVICES_HPP
#define MOD_MODSERVICES_HPP

#include <stdint.h>

#include <string>
#include <vector>

#include "mod/ModValue.hpp"

namespace mod
{
class Services
{
public:
    virtual ~Services() {}

    virtual void Log(const std::string& mod, const std::string& text) = 0;
    virtual void Say(const std::string& text) = 0;
    virtual bool Run(const std::string& line, std::string* reply) = 0;
    virtual std::vector<std::string> Players() = 0;
    virtual int64_t TimeMs() = 0;
    virtual bool ReadGameFile(const std::string& relative, std::string* data) = 0;
    virtual bool Call(const std::string& mod, int function, const std::vector<Value>& arguments, Value* result) = 0;
    virtual void Unloaded(const std::string& mod) = 0;
};

}

#endif
