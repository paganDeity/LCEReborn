#ifndef MOD_MODVALUE_HPP
#define MOD_MODVALUE_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

namespace mod
{
class Value
{
public:
    enum EType
    {
        eType_Nil = 0,
        eType_Bool = 1,
        eType_Int = 2,
        eType_Float = 3,
        eType_String = 4,
        eType_Handle = 5,
        // values in order, such as the item stacks a call returns
        eType_List = 6
    };

    Value() : m_type(eType_Nil), m_int(0), m_float(0.0), m_kind(0) {}

    static Value FromBool(bool value);
    static Value FromInt(int64_t value);
    static Value FromFloat(double value);
    static Value FromString(const std::string& value);
    static Value FromHandle(unsigned int kind, uint32_t id);
    static Value FromList(const std::vector<Value>& items);

    EType Type() const { return m_type; }
    bool Bool() const { return m_type == eType_Bool ? m_int != 0 : false; }
    int64_t Int() const { return m_type == eType_Float ? static_cast<int64_t>(m_float) : m_int; }
    double Float() const { return m_type == eType_Float ? m_float : static_cast<double>(m_int); }
    const std::string& String() const { return m_text; }
    unsigned int Kind() const { return m_kind; }
    uint32_t Id() const { return static_cast<uint32_t>(m_int); }
    std::vector<Value> Items() const;

    static void Encode(const std::vector<Value>& values, std::vector<uint8_t>& out);
    static bool Decode(const uint8_t* data, size_t size, std::vector<Value>& values);
    static void EncodeOne(const Value& value, std::vector<uint8_t>& out);
    static bool DecodeOne(const uint8_t* data, size_t size, size_t* at, Value* value);

private:
    EType m_type;
    int64_t m_int;
    double m_float;
    unsigned int m_kind;
    std::string m_text;
};
}

#endif
