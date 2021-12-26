#pragma once

#include <istream>
#include <chrono>
#include <cmath>
#include "xson/trace.hpp"

namespace xson::fast {

class decoder
{
public:

    decoder(std::istream& is) : m_is{is}
    {}

    void decode(std::uint8_t&);

    void decode(std::uint32_t&);

    void decode(std::int32_t&);

    void decode(std::uint64_t&);

    void decode(std::int64_t&);

    void decode(std::string&);

    void decode(std::double_t&);

    void decode(bool&);

    void decode(std::chrono::system_clock::time_point&);

    template<typename T> requires std::is_enum_v<T>
    void decode(T& e)
    {
        std::uint8_t byte;
        decode(byte);
        e = static_cast<T>(byte);
        TRACE(e);
    }

protected:

    std::istream& m_is;
};

inline void decoder::decode(std::uint8_t& byte)
{
    byte = m_is.get();
    TRACE(byte);
}

inline void decoder::decode(std::uint32_t& i)
{
    i = 0u;
    for(std::uint8_t byte = m_is.get(); m_is; byte = m_is.get())
    {
        i = (i << 7) | (byte & 0x7f);
        if(byte & 0x80)
            break;
    }
    TRACE(i);
}

inline void decoder::decode(std::int32_t& i)
{
    i = (m_is.peek() & 0x40) ? -1 : 0;
    for(std::uint8_t byte = m_is.get(); m_is; byte = m_is.get())
    {
        i = (i << 7) | (byte & 0x7f);
        if(byte & 0x80)
            break;
    }
    TRACE(i);
}

inline void decoder::decode(std::uint64_t& i)
{
    i = 0ull;
    for(std::uint8_t byte = m_is.get(); m_is; byte = m_is.get())
    {
        i = (i << 7) | (byte & 0x7f);
        if(byte & 0x80)
            break;
    }
    TRACE(i);
}

inline void decoder::decode(std::int64_t& i)
{
    i = (m_is.peek() & 0x40) ? -1ll : 0ll;
    for(std::uint8_t byte = m_is.get(); m_is; byte = m_is.get())
    {
        i = (i << 7) | (byte & 0x7f);
        if(byte & 0x80)
            break;
    }
    TRACE(i);
}

inline void decoder::decode(std::string& str)
{
    str.clear();
    for(char byte = m_is.get(); m_is; byte = m_is.get())
    {
        str.push_back(byte & 0x7f);
        if(byte & 0x80)
            break;
    }
    TRACE(str);
}

inline void decoder::decode(std::double_t& d)
{
    union {
        std::uint64_t i64;
        std::double_t d64;
    } i2d;
    decode(i2d.i64);
    d = i2d.d64;
    TRACE(d);
}

inline void decoder::decode(bool& b)
{
    std::uint8_t byte;
    decode(byte);
    b = static_cast<std::uint8_t>(byte);
    TRACE(b);
}

inline void decoder::decode(std::chrono::system_clock::time_point& dt)
{
    using namespace std::chrono;
    std::uint64_t i64;
    decode(i64);
    dt = system_clock::time_point{milliseconds{i64}};
    TRACE(i64);
}

} // namespace xson::fast
