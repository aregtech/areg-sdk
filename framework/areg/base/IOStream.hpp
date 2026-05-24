#ifndef AREG_BASE_IOSTREAM_HPP
#define AREG_BASE_IOSTREAM_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/IOStream.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Input / Output stream interfaces
 *              Following interfaces are defined:
 *              InStream     - is an interface of stream to read data from.
 *              OutStream    - is an interface of stream to write data to.
 *              IOStream     - is an interface of reads and writes streaming.
 *              Implemented instances should provide read and write
 *              functionalities.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include <deque>
#include <list>
#include <map>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

/************************************************************************
 * Declared classes
 ************************************************************************/

/************************************************************************
 * \brief   Following classes are defined in following hierarchy.
 *          Description of every class see bellow.
 ************************************************************************/
namespace areg {
    class InStream;
    class OutStream;
    class IOStream;
} // namespace areg

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class String;
    class WideString;
    class SharedBuffer;
} // namespace areg

/************************************************************************
 * \brief   Predefined MACRO to support data read / write streaming
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Macros declaration: declare data as streamable, 
// i.e. available for read and write
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Declaration of streaming operator. Fine for object with primitives.
 *          Might be not proper for objects with multiple instance.
 **/
/**
 * \brief   Declares friend stream operators for a data type (use inside a class).
 **/
#define AREG_DECLARE_STREAMABLE(data_type)                                                                  \
    friend inline const areg::InStream& operator >> (const areg::InStream& stream, data_type& input);       \
    friend inline areg::OutStream& operator << (areg::OutStream& stream, const data_type& output)

/**
 * \brief   Declares global (non-friend) stream operators for a data type.
 **/
#define AREG_GLOBAL_DECLARE_STREAMABLE(data_type)                                                           \
    const areg::InStream& operator >> (const areg::InStream& stream, data_type& input);                     \
    areg::OutStream& operator << (areg::OutStream& stream, const data_type& output)

/**
 * \brief   Implements stream operators for trivially-copyable types via raw memory read/write.
 *          Includes a static_assert to catch misuse with non-trivial types.
 **/
#define AREG_IMPLEMENT_STREAMABLE(data_type)                                                                \
    inline const areg::InStream& operator >> (const areg::InStream& stream, data_type& input)               \
    {   stream.read( reinterpret_cast<uint8_t *>(&input), sizeof(data_type) ); return stream; }             \
    inline areg::OutStream& operator << (areg::OutStream& stream, const data_type& output)                  \
    {   stream.write( reinterpret_cast<const uint8_t *>(&output), sizeof(data_type) ); return stream; }

/**
 * \brief   Declares friend stream operators with DLL export/import specifier.
 **/
#define AREG_DECLARE_STREAMABLE_EXPORT(data_type, ExportDef)                                                \
    friend ExportDef const areg::InStream& operator >> (const areg::InStream& stream, data_type& input);    \
    friend ExportDef areg::OutStream& operator << (areg::OutStream& stream, const data_type& output);

#define AREG_GLOBAL_DECLARE_STREAMABLE_EXPORT(data_type, ExportDef)                                         \
    ExportDef const areg::InStream& operator >> (const areg::InStream& stream, data_type& input);           \
    ExportDef areg::OutStream& operator << (areg::OutStream& stream, const data_type& output);

#define AREG_IMPLEMENT_READABLE_EXPORT(data_type, ExportDef)                                                \
    ExportDef const areg::InStream& operator >> (const areg::InStream& stream, data_type& input)

#define AREG_IMPLEMENT_WRITABLE_EXPORT(data_type, ExportDef)                                                \
    ExportDef areg::OutStream& operator << (areg::OutStream& stream, const data_type& output)

namespace areg {

//////////////////////////////////////////////////////////////////////////
// InStream class declaration: to read data from
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface and base implementation for reading data from input streams.
 **/
class AREG_API InStream
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    InStream() noexcept = default;

    virtual ~InStream() noexcept = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// InStream convenience read helpers (non-virtual, inline)
/************************************************************************/
    /**
     * \brief   Reads and returns an 8-bit value from the stream.
     **/
    inline uint8_t read8_bits() const noexcept;

    /**
     * \brief   Reads and returns a 16-bit value from the stream.
     **/
    inline uint16_t read16_bits() const noexcept;

    /**
     * \brief   Reads and returns a 32-bit value from the stream.
     **/
    inline uint32_t read32_bits() const noexcept;

    /**
     * \brief   Reads and returns a 64-bit value from the stream.
     **/
    inline uint64_t read64_bits() const noexcept;

    /**
     * \brief   Reads a 32-bit floating-point value from the stream.
     **/
    inline float read32_real() const noexcept;

    /**
     * \brief   Reads a 64-bit floating-point value from the stream.
     **/
    inline double read64_real() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// InStream interface overrides
/************************************************************************/

    /**
     * \brief   Reads data from stream into buffer. Returns the number of bytes read.
     *
     * \param[out] buffer   A buffer to receive the data.
     * \param   size        Maximum number of bytes to read.
     * \return  The number of bytes read.
     **/
    virtual uint32_t read( uint8_t * buffer, uint32_t size ) const noexcept = 0;

    /**
     * \brief   Reads data from stream into a SharedBuffer. Returns the number of bytes read.
     *
     * \param[out] buffer   A SharedBuffer object to receive the data.
     * \return  The number of bytes read.
     **/
    virtual uint32_t read(SharedBuffer& buffer ) const = 0;

    /**
     * \brief   Reads string data from stream into a String. Returns the number of bytes read.
     *
     * \param[out] ascii       String to receive the data.
     * \return  The number of bytes read.
     **/
    virtual uint32_t read( String& ascii ) const = 0;

    /**
     * \brief   Reads wide-string data from stream into a WideString. Returns the number of bytes
     *          read.
     *
     * \param[out] wide    WideString to receive the data.
     * \return  The number of bytes read.
     **/
    virtual uint32_t read( WideString& wide ) const = 0;

    /**
     * \brief   Resets the cursor to the beginning of the stream.
     **/
    virtual void reset() const noexcept = 0;

protected:
    /**
     * \brief   Returns the number of bytes available to read.
     **/
    [[nodiscard]]
    virtual uint32_t size_readable() const noexcept = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( InStream );
}; 


//////////////////////////////////////////////////////////////////////////
// OutStream class declaration: to write data to
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface and base implementation for writing data to output streams.
 **/
class AREG_API OutStream
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    OutStream() noexcept = default;

    virtual ~OutStream() noexcept = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// OutStream convenience write helpers (non-virtual, inline)
/************************************************************************/
    /**
     * \brief   Writes an 8-bit value to the stream. Returns true if successful.
     **/
    inline bool write8_bits( uint8_t value );

    /**
     * \brief   Writes a 16-bit value to the stream. Returns true if successful.
     **/
    inline bool write16_bits( uint16_t value );

    /**
     * \brief   Writes a 32-bit value to the stream. Returns true if successful.
     **/
    inline bool write32_bits( uint32_t value );

    /**
     * \brief   Writes a 64-bit value to the stream. Returns true if successful.
     **/
    inline bool write64_bits( uint64_t value );

    /**
     * \brief   Writes a 32-bit floating-point value to the stream. Returns true if successful.
     **/
    inline bool write32_real( float value );

    /**
     * \brief   Writes a 64-bit floating-point value to the stream. Returns true if successful.
     **/
    inline bool write64_real( double value );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// OutStream interface overrides
/************************************************************************/

    /**
     * \brief   Writes data from buffer to stream. Returns the number of bytes written.
     *
     * \param   buffer      Pointer to the data buffer.
     * \param   size        Number of bytes in the buffer.
     * \return  The number of bytes written.
     **/
    virtual uint32_t write( const uint8_t * buffer, uint32_t size ) = 0;

    /**
     * \brief   Writes data from a SharedBuffer to stream. Returns the number of bytes written.
     *
     * \param   buffer      The SharedBuffer to write.
     * \return  The number of bytes written.
     **/
    virtual uint32_t write( const SharedBuffer& buffer ) = 0;

    /**
     * \brief   Writes a String to stream. Returns the number of bytes written.
     *
     * \param   ascii       The ASCII string to write.
     * \return  The number of bytes written.
     **/
    virtual uint32_t write( const String& ascii )  = 0;

    /**
     * \brief   Writes a WideString to stream. Returns the number of bytes written.
     *
     * \param   wide    The wide string to write.
     * \return  The number of bytes written.
     **/
    virtual uint32_t write( const WideString& wide ) = 0;

    /**
     * \brief   Flushes cached data to the output stream.
     **/
    virtual void flush() noexcept = 0;

    /**
     * \brief   Reserve and ensure additional size to the existing. If the free space of the stream to write
     *          is enough, no changes should be done. If the size of the stream is not enough, it should allocate
     *          additional space and should not loose existing data.
     * \param   addSize     The size to add if required.
     * \return  Returns true if the stream has enough space to write the data.
     **/
    virtual bool ensure_size(uint32_t addSize) = 0;

protected:
    /**
     * \brief   Returns the available space in bytes remaining to write.
     **/
    [[nodiscard]]
    virtual uint32_t size_writable() const noexcept = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( OutStream );
};

//////////////////////////////////////////////////////////////////////////
// IOStream class declaration: to read / write data
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Input and output streaming interface.
 **/
class AREG_API IOStream : public InStream
                        , public OutStream
{
protected:
    IOStream() noexcept = default;

public:
    virtual ~IOStream() noexcept = default;

public:
    /**
     * \brief   Writes an ASCII string to the stream.
     **/
    friend inline OutStream& operator << (OutStream& stream, const char * output);

    /**
     * \brief   Writes a wide string to the stream.
     **/
    friend inline OutStream& operator << (OutStream& stream, const wchar_t * output);

    /**
     * \brief   Writes an STL string to the stream.
     **/
    template<typename CharType>
    friend inline OutStream& operator << (OutStream& stream, const std::basic_string<CharType>& output);
    /**
     * \brief   Writes an STL string_view to the stream.
     **/
    template<typename CharType>
    friend inline OutStream& operator << (OutStream& stream, const std::basic_string_view<CharType>& output);

    /**
     * \brief   Reads an STL string from the stream.
     **/
    template<typename CharType>
    friend inline const InStream& operator >> (const InStream& stream, std::basic_string<CharType>& input);

    /**
     * \brief   Writes an std::deque to the stream. Each element must be streamable.
     **/
    template<typename ElemType>
    friend inline OutStream& operator << (OutStream& stream, const std::deque<ElemType>& output);
    /**
     * \brief   Reads an std::deque from the stream. Each element must be streamable.
     **/
    template<typename ElemType>
    friend inline const InStream& operator >> (const InStream& stream, std::deque<ElemType>& input);

    /**
     * \brief   Writes an std::list to the stream. Each element must be streamable.
     **/
    template<typename ElemType>
    friend inline OutStream& operator << (OutStream& stream, const std::list<ElemType>& output);
    /**
     * \brief   Reads an std::list from the stream. Each element must be streamable.
     **/
    template<typename ElemType>
    friend inline const InStream& operator >> (const InStream& stream, std::list<ElemType>& input);

    /**
     * \brief   Writes an std::vector to the stream. Each element must be streamable.
     **/
    template<typename ElemType>
    friend inline OutStream& operator << (OutStream& stream, const std::vector<ElemType>& output);
    /**
     * \brief   Reads an std::vector from the stream. Each element must be streamable.
     **/
    template<typename ElemType>
    friend inline const InStream& operator >> (const InStream& stream, std::vector<ElemType>& input);

    /**
     * \brief   Writes an std::pair to the stream. Each element must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline OutStream& operator << (OutStream& stream, const std::pair<Key, Value>& output);
    /**
     * \brief   Reads an std::pair from the stream. Each element must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline const InStream& operator >> (const InStream& stream, std::pair<Key, Value>& input);

    /**
     * \brief   Writes an std::map to the stream. Each element must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline OutStream& operator << (OutStream& stream, const std::map<Key, Value>& output);
    /**
     * \brief   Reads an std::map from the stream. Each element must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline const InStream& operator >> (const InStream& stream, std::map<Key, Value>& input);

    /**
     * \brief   Writes an std::unordered_map to the stream. Each element must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline OutStream& operator << (OutStream& stream, const std::unordered_map<Key, Value>& output);
    /**
     * \brief   Reads an std::unordered_map from the stream. Each element must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline const InStream& operator >> (const InStream& stream, std::unordered_map<Key, Value>& input);


//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( IOStream );
};

//////////////////////////////////////////////////////////////////////////
// Inline function implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   InStream convenience helpers — inline implementations
 ************************************************************************/

inline uint8_t areg::InStream::read8_bits() const noexcept
{
    uint8_t result{ 0u };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(uint8_t));
    return result;
}

inline uint16_t areg::InStream::read16_bits() const noexcept
{
    uint16_t result{ 0u };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(uint16_t));
    return result;
}

inline uint32_t areg::InStream::read32_bits() const noexcept
{
    uint32_t result{ 0u };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(uint32_t));
    return result;
}

inline uint64_t areg::InStream::read64_bits() const noexcept
{
    uint64_t result{ 0u };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(uint64_t));
    return result;
}

inline float areg::InStream::read32_real() const noexcept
{
    float result{ 0.0f };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(float));
    return result;
}

inline double areg::InStream::read64_real() const noexcept
{
    double result{ 0.0 };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(double));
    return result;
}

/************************************************************************
 * \brief   OutStream convenience helpers — inline implementations
 ************************************************************************/

inline bool areg::OutStream::write8_bits( uint8_t value )
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(uint8_t)) == sizeof(uint8_t);
}

inline bool areg::OutStream::write16_bits( uint16_t value )
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(uint16_t)) == sizeof(uint16_t);
}

inline bool areg::OutStream::write32_bits( uint32_t value )
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(uint32_t)) == sizeof(uint32_t);
}

inline bool areg::OutStream::write64_bits( uint64_t value )
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(uint64_t)) == sizeof(uint64_t);
}

inline bool areg::OutStream::write32_real( float value )
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(float)) == sizeof(float);
}

inline bool areg::OutStream::write64_real( double value )
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(double)) == sizeof(double);
}

/************************************************************************
 * \brief   Support streaming of primitives
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// MACRO make primitives streamable
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_STREAMABLE(bool)
AREG_IMPLEMENT_STREAMABLE(char)
AREG_IMPLEMENT_STREAMABLE(wchar_t)
AREG_IMPLEMENT_STREAMABLE(uint8_t)
AREG_IMPLEMENT_STREAMABLE(int16_t)
AREG_IMPLEMENT_STREAMABLE(uint16_t)
AREG_IMPLEMENT_STREAMABLE(int32_t)
AREG_IMPLEMENT_STREAMABLE(uint32_t)
AREG_IMPLEMENT_STREAMABLE(int64_t)
AREG_IMPLEMENT_STREAMABLE(uint64_t)
AREG_IMPLEMENT_STREAMABLE(float)
AREG_IMPLEMENT_STREAMABLE(double)

inline areg::OutStream& operator << (areg::OutStream& stream, const char * output)
{
    if (output != nullptr)
    {
        constexpr uint32_t single = static_cast<uint32_t>(sizeof(char));
        uint32_t length = 0;
        const char * src = output;
        while (*src ++ != '\0')
        {
            ++ length;
        }
        
        stream.write(reinterpret_cast<const uint8_t *>(output), (length + 1) * single);
    }

    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const wchar_t * output)
{
    if (output != nullptr)
    {
        constexpr uint32_t single = static_cast<uint32_t>(sizeof(wchar_t));
        uint32_t length = 0;
        const wchar_t * src = output;
        while (*src ++ != L'\0')
        {
            ++ length;
        }

        stream.write(reinterpret_cast<const uint8_t *>(output), (length + 1) * single);
    }

    return stream;
}

template<typename CharType>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::basic_string<CharType>& output)
{
    constexpr uint32_t single = static_cast<uint32_t>(sizeof(CharType));
    stream.write(reinterpret_cast<const uint8_t*>(output.c_str()), static_cast<uint32_t>(output.length() + 1) * single);
    return stream;
}

template<typename CharType>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::basic_string_view<CharType>& output)
{
    constexpr uint32_t single = static_cast<uint32_t>(sizeof(CharType));
    // string_view does not guarantee a NUL at data()[length()], so write the payload
    // and then a separate NUL terminator to preserve the NUL-terminated wire format.
    stream.write(reinterpret_cast<const uint8_t*>(output.data()), static_cast<uint32_t>(output.length()) * single);
    const CharType terminator = static_cast<CharType>('\0');
    stream.write(reinterpret_cast<const uint8_t*>(&terminator), single);
    return stream;
}

template<typename CharType>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::basic_string<CharType>& input)
{
    input.clear();

    CharType ch;
    stream >> ch;
    while (ch != static_cast<CharType>('\0'))
    {
        input += ch;
        stream >> ch;
    }

    return stream;
}

template<typename ElemType>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::deque<ElemType>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const auto& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename ElemType>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::deque<ElemType>& input)
{
    input.clear();

    uint32_t size = 0;
    stream >> size;
    input.resize(size);
    for (auto& elem : input)
    {
        stream >> elem;
    }

    return stream;
}

template<typename ElemType>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::list<ElemType>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const auto& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename ElemType>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::list<ElemType>& input)
{
    input.clear();

    uint32_t size = 0;
    stream >> size;
    input.resize(size);
    for (auto& elem : input)
    {
        stream >> elem;
    }

    return stream;
}

template<typename ElemType>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::vector<ElemType>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const auto& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename ElemType>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::vector<ElemType>& input)
{
    input.clear();

    uint32_t size = 0;
    stream >> size;
    input.resize(size);
    for (auto& elem : input)
    {
        stream >> elem;
    }

    return stream;
}

template<typename Key, typename Value>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::pair<Key, Value>& output)
{
    stream << output.first;
    stream << output.second;
    return stream;
}

template<typename Key, typename Value>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::pair<Key, Value>& input)
{
    stream >> input.first;
    stream >> input.second;
    return stream;
}

template<typename Key, typename Value>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::map<Key, Value>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const std::pair<const Key, Value>& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename Key, typename Value>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::map<Key, Value>& input)
{
    input.clear();

    uint32_t size = 0;
    stream >> size;
    for (uint32_t i = 0; i < size; ++i)
    {
        std::pair<Key, Value> elem;
        stream >> elem;
        input[elem.first] = elem.second;
    }

    return stream;
}

template<typename Key, typename Value>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::unordered_map<Key, Value>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const std::pair<const Key, Value>& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename Key, typename Value>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::unordered_map<Key, Value>& input)
{
    input.clear();

    uint32_t size = 0;
    stream >> size;
    for (uint32_t i = 0; i < size; ++i)
    {
        std::pair<Key, Value> elem;
        stream >> elem;
        input[elem.first] = elem.second;
    }

    return stream;
}

} // namespace areg

#endif  // AREG_BASE_IOSTREAM_HPP
