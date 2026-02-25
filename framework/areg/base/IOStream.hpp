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
#include "areg/base/GEGlobal.h"

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
class InStream;
class OutStream;
    class IOStream;

/************************************************************************
 * Dependencies
 ************************************************************************/
class String;
class WideString;
class ByteBuffer;
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
#define AREG_DECLARE_STREAMABLE(data_type)                                                                      \
    friend inline const InStream & operator >> (const InStream & stream, data_type & input);                \
    friend inline OutStream & operator << (OutStream & stream, const data_type & output)

/**
 * \brief   Declares global (non-friend) stream operators for a data type.
 **/
#define AREG_GLOBAL_DECLARE_STREAMABLE(data_type)                                                               \
    const InStream & operator >> (const InStream & stream, data_type & input);                              \
    OutStream & operator << (OutStream & stream, const data_type & output)

/**
 * \brief   Implements stream operators for trivially-copyable types via raw memory read/write.
 *          Includes a static_assert to catch misuse with non-trivial types.
 **/
#define AREG_IMPLEMENT_STREAMABLE(data_type)                                                                    \
    inline const InStream& operator >> (const InStream& stream, data_type & input)                          \
    {   stream.read( reinterpret_cast<uint8_t *>(&input), sizeof(data_type) ); return stream; }           \
    inline OutStream& operator << (OutStream& stream, const data_type& output)                              \
    {   stream.write( reinterpret_cast<const uint8_t *>(&output), sizeof(data_type) ); return stream; }

/**
 * \brief   Declares friend stream operators with DLL export/import specifier.
 **/
#define AREG_DECLARE_STREAMABLE_EXPORT(data_type, ExportDef)                                                    \
    friend ExportDef const InStream & operator >> (const InStream & stream, data_type & input);             \
    friend ExportDef OutStream & operator << (OutStream & stream, const data_type & output);

#define AREG_GLOBAL_DECLARE_STREAMABLE_EXPORT(data_type, ExportDef)                                             \
    ExportDef const InStream & operator >> (const InStream & stream, data_type & input);                    \
    ExportDef OutStream & operator << (OutStream & stream, const data_type & output);

#define AREG_IMPLEMENT_READABLE_EXPORT(data_type, ExportDef)                                                    \
    ExportDef const InStream & operator >> (const InStream & stream, data_type & input)

#define AREG_IMPLEMENT_WRITABLE_EXPORT(data_type, ExportDef)                                                    \
    ExportDef OutStream & operator << (OutStream & stream, const data_type & output)                        \


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
    /**
     * \brief   Protected constructor.
     **/
    InStream() = default;

    /**
     * \brief   Destructor
     **/
    virtual ~InStream() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// InStream interface data read overrides
/************************************************************************/
    /**
     * \brief   Reads and returns an 8-bit value from the stream.
     **/
    virtual uint8_t read8_bits() const;

    /**
     * \brief   Reads and returns a 16-bit value from the stream.
     **/
    virtual uint16_t read16_bits() const;

    /**
     * \brief   Reads and returns a 32-bit value from the stream.
     **/
    virtual uint32_t read32_bits() const;

    /**
     * \brief   Reads and returns a 64-bit value from the stream.
     **/
    virtual uint64_t read64_bits() const;

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
     * \param[out] buffer      Buffer to receive the data.
     * \param   size        Maximum number of bytes to read.
     * \return  The number of bytes read.
     **/
    virtual uint32_t read( uint8_t * buffer, uint32_t size ) const = 0;

    /**
     * \brief   Reads data from stream into a ByteBuffer. Returns the number of bytes read.
     *
     * \param[out] buffer      ByteBuffer to receive the data.
     * \return  The number of bytes read.
     **/
    virtual uint32_t read( ByteBuffer & buffer ) const = 0;

    /**
     * \brief   Reads string data from stream into a String. Returns the number of bytes read.
     *
     * \param[out] ascii       String to receive the data.
     * \return  The number of bytes read.
     **/
    virtual uint32_t read( String & ascii ) const = 0;

    /**
     * \brief   Reads wide-string data from stream into a WideString. Returns the number of bytes
     *          read.
     *
     * \param[out] wide    WideString to receive the data.
     * \return  The number of bytes read.
     **/
    virtual uint32_t read( WideString & wide ) const = 0;

    /**
     * \brief   Resets the cursor to the beginning of the stream.
     **/
    virtual void reset() const = 0;

protected:
    /**
     * \brief   Returns the number of bytes available to read.
     **/
    virtual uint32_t size_readable() const = 0;

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
    /**
     * \brief   Protected constructor.
     **/
    OutStream() = default;

    /**
     * \brief   Destructor
     **/
    virtual ~OutStream() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// OutStream interface data write overrides
/************************************************************************/
    /**
     * \brief   Writes an 8-bit value to the stream. Returns true if successful.
     *
     * \param   value8Bit       The 8-bit value to write.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool write8_bits( uint8_t value8Bit );

    /**
     * \brief   Writes a 16-bit value to the stream. Returns true if successful.
     *
     * \param   value16Bit      The 16-bit value to write.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool write16_bits( uint16_t value16Bit );

    /**
     * \brief   Writes a 32-bit value to the stream. Returns true if successful.
     *
     * \param   value32Bit      The 32-bit value to write.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool write32_bits( uint32_t value32Bit );

    /**
     * \brief   Writes a 64-bit value to the stream. Returns true if successful.
     *
     * \param   value64Bit      The 64-bit value to write.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool write64_bits( uint64_t value64Bit );

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
     * \brief   Writes data from a ByteBuffer to stream. Returns the number of bytes written.
     *
     * \param   buffer      The ByteBuffer to write.
     * \return  The number of bytes written.
     **/
    virtual uint32_t write( const ByteBuffer & buffer ) = 0;

    /**
     * \brief   Writes a String to stream. Returns the number of bytes written.
     *
     * \param   ascii       The ASCII string to write.
     * \return  The number of bytes written.
     **/
    virtual uint32_t write( const String & ascii )  = 0;

    /**
     * \brief   Writes a WideString to stream. Returns the number of bytes written.
     *
     * \param   wide    The wide string to write.
     * \return  The number of bytes written.
     **/
    virtual uint32_t write( const WideString & wide ) = 0;

    /**
     * \brief   Flushes cached data to the output stream.
     **/
    virtual void flush() = 0;

protected:
    /**
     * \brief   Returns the available space in bytes remaining to write.
     **/
    virtual uint32_t size_writable() const = 0;

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
class AREG_API IOStream  : public InStream
                           , public OutStream
{
protected:
    /**
     * \brief
     **/
    IOStream() = default;

public:
    /**
     * \brief   Destructor
     **/
    virtual ~IOStream() = default;

public:
/************************************************************************
 * \brief   Support streaming of primitives
 ************************************************************************/
    AREG_DECLARE_STREAMABLE(bool);           //!< Declare primitive type bool as streamable
    AREG_DECLARE_STREAMABLE(char);           //!< Declare primitive type char as streamable
    AREG_DECLARE_STREAMABLE(wchar_t);        //!< Declare primitive type wchar_t as streamable
    AREG_DECLARE_STREAMABLE(uint8_t);  //!< Declare primitive type uint8_t as streamable
    AREG_DECLARE_STREAMABLE(int16_t);          //!< Declare primitive type int16_t as streamable
    AREG_DECLARE_STREAMABLE(uint16_t); //!< Declare primitive type uint16_t as streamable
    AREG_DECLARE_STREAMABLE(int32_t);            //!< Declare primitive type int32_t as streamable
    AREG_DECLARE_STREAMABLE(uint32_t);   //!< Declare primitive type uint32_t as streamable
    AREG_DECLARE_STREAMABLE(int64_t);        //!< Declare primitive type int64_t as streamable
    AREG_DECLARE_STREAMABLE(uint64_t);       //!< Declare primitive type uint64_t as streamable
    AREG_DECLARE_STREAMABLE(float);          //!< Declare primitive type float as streamable
    AREG_DECLARE_STREAMABLE(double);         //!< Declare primitive type double as streamable

    /**
     * \brief   Writes an ASCII string to the stream.
     **/
    friend inline OutStream & operator << (OutStream & stream, const char * output);

    /**
     * \brief   Writes a wide string to the stream.
     **/
    friend inline OutStream & operator << (OutStream & stream, const wchar_t * output);

    /**
     * \brief   Writes STL string into the stream.
     **/
    template<typename CharType>
    /**
     * \brief   Writes an STL string to the stream.
     **/
    friend inline OutStream& operator << (OutStream& stream, const std::basic_string<CharType>& output);
    template<typename CharType>
    /**
     * \brief   Writes an STL string_view to the stream.
     **/
    friend inline OutStream& operator << (OutStream& stream, const std::basic_string_view<CharType>& output);

    /**
     * \brief   Writes STL string into the stream.
     **/
    template<typename CharType>
    /**
     * \brief   Reads an STL string from the stream.
     **/
    friend inline const InStream& operator >> (const InStream& stream, std::basic_string<CharType>& input);

    /**
     * \brief   Reads and writes std::deque object to the stream.
     *          Each element in the container must be streamable.
     **/
    template<typename ElemType>
    /**
     * \brief   Writes an std::deque to the stream. Each element must be streamable.
     **/
    friend inline OutStream& operator << (OutStream& stream, const std::deque<ElemType>& output);
    template<typename ElemType>
    /**
     * \brief   Reads an std::deque from the stream. Each element must be streamable.
     **/
    friend inline const InStream& operator >> (const InStream& stream, std::deque<ElemType>& input);

    /**
     * \brief   Reads and writes std::list object to the stream.
     *          Each element in the container must be streamable.
     **/
    template<typename ElemType>
    /**
     * \brief   Writes an std::list to the stream. Each element must be streamable.
     **/
    friend inline OutStream& operator << (OutStream& stream, const std::list<ElemType>& output);
    template<typename ElemType>
    /**
     * \brief   Reads an std::list from the stream. Each element must be streamable.
     **/
    friend inline const InStream& operator >> (const InStream& stream, std::list<ElemType>& input);

    /**
     * \brief   Reads and writes std::vector object to the stream.
     *          Each element in the container must be streamable. 
     **/
    template<typename ElemType>
    /**
     * \brief   Writes an std::vector to the stream. Each element must be streamable.
     **/
    friend inline OutStream& operator << (OutStream& stream, const std::vector<ElemType>& output);
    template<typename ElemType>
    /**
     * \brief   Reads an std::vector from the stream. Each element must be streamable.
     **/
    friend inline const InStream& operator >> (const InStream& stream, std::vector<ElemType>& input);

    /**
     * \brief   Reads and writes std::pair object to the stream.
     *          Each element in the container must be streamable.
     **/
    template<typename Key, typename Value>
    /**
     * \brief   Writes an std::pair to the stream. Each element must be streamable.
     **/
    friend inline OutStream& operator << (OutStream& stream, const std::pair<Key, Value>& output);
    template<typename Key, typename Value>
    /**
     * \brief   Reads an std::pair from the stream. Each element must be streamable.
     **/
    friend inline const InStream& operator >> (const InStream& stream, std::pair<Key, Value>& input);

    /**
     * \brief   Reads and writes std::map object to the stream.
     *          Each element in the container must be streamable.
     **/
    template<typename Key, typename Value>
    /**
     * \brief   Writes an std::map to the stream. Each element must be streamable.
     **/
    friend inline OutStream& operator << (OutStream& stream, const std::map<Key, Value>& output);
    template<typename Key, typename Value>
    /**
     * \brief   Reads an std::map from the stream. Each element must be streamable.
     **/
    friend inline const InStream& operator >> (const InStream& stream, std::map<Key, Value>& input);

    /**
     * \brief   Reads and writes std::unordered_map object to the stream.
     *          Each element in the container must be streamable.
     **/
    template<typename Key, typename Value>
    /**
     * \brief   Writes an std::unordered_map to the stream. Each element must be streamable.
     **/
    friend inline OutStream& operator << (OutStream& stream, const std::unordered_map<Key, Value>& output);
    template<typename Key, typename Value>
    /**
     * \brief   Reads an std::unordered_map from the stream. Each element must be streamable.
     **/
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

inline OutStream & operator << (OutStream & stream, const char * output)
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

inline OutStream & operator << (OutStream & stream, const wchar_t * output)
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
inline OutStream& operator << (OutStream& stream, const std::basic_string<CharType>& output)
{
    constexpr uint32_t single = static_cast<uint32_t>(sizeof(CharType));
    stream.write(reinterpret_cast<const uint8_t*>(output.c_str()), static_cast<uint32_t>(output.length() + 1) * single);
    return stream;
}

template<typename CharType>
inline OutStream& operator << (OutStream& stream, const std::basic_string_view<CharType>& output)
{
    constexpr uint32_t single = static_cast<uint32_t>(sizeof(CharType));
    stream.write(reinterpret_cast<const uint8_t*>(output.data()), static_cast<uint32_t>(output.length() + 1) * single);
    return stream;
}

template<typename CharType>
inline const InStream& operator >> (const InStream& stream, std::basic_string<CharType>& input)
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
inline OutStream& operator << (OutStream& stream, const std::deque<ElemType>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const auto& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename ElemType>
inline const InStream& operator >> (const InStream& stream, std::deque<ElemType>& input)
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
inline OutStream& operator << (OutStream& stream, const std::list<ElemType>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const auto& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename ElemType>
inline const InStream& operator >> (const InStream& stream, std::list<ElemType>& input)
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
inline OutStream& operator << (OutStream& stream, const std::vector<ElemType>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const auto& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename ElemType>
inline const InStream& operator >> (const InStream& stream, std::vector<ElemType>& input)
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
inline OutStream& operator << (OutStream& stream, const std::pair<Key, Value>& output)
{
    stream << output.first;
    stream << output.second;
    return stream;
}

template<typename Key, typename Value>
inline const InStream& operator >> (const InStream& stream, std::pair<Key, Value>& input)
{
    stream >> input.first;
    stream >> input.second;
    return stream;
}

template<typename Key, typename Value>
inline OutStream& operator << (OutStream& stream, const std::map<Key, Value>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const std::pair<Key, Value> & elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename Key, typename Value>
inline const InStream& operator >> (const InStream& stream, std::map<Key, Value>& input)
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
inline OutStream& operator << (OutStream& stream, const std::unordered_map<Key, Value>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const std::pair<Key, Value>& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename Key, typename Value>
inline const InStream& operator >> (const InStream& stream, std::unordered_map<Key, Value>& input)
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

#endif  // AREG_BASE_IOSTREAM_HPP
