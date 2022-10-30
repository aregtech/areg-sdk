#ifndef AREG_BASE_IEIOSTREAM_HPP
#define AREG_BASE_IEIOSTREAM_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/IEIOStream.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Input / Output stream interfaces
 *              Following interfaces are defined:
 *              IEInStream     - is an interface of stream to read data from.
 *              IEOutStream    - is an interface of stream to write data to.
 *              IEIOStream     - is an interface of reads and writes streaming.
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
class IEInStream;
class IEOutStream;
    class IEIOStream;

/************************************************************************
 * Dependencies
 ************************************************************************/
class String;
class WideString;
class IEByteBuffer;
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
#define DECLARE_STREAMABLE(data_type)                                                                           \
    /* \brief   Reading from stream operators. It reads data from stream and                            */      \
    /*          initializes input object.                                                               */      \
    friend inline const IEInStream & operator >> (const IEInStream & stream, data_type & input);                \
    /* \brief   Writing to stream operator. It reads data from output instance                          */      \
    /*          and writes data to stream object.                                                       */      \
    friend inline IEOutStream & operator << (IEOutStream & stream, const data_type & output);                   \

#define GLOBAL_DECLARE_STREAMABLE(data_type)                                                                    \
    /* \brief   Reading from stream operators. It reads data from stream and                            */      \
    /*          initializes input object.                                                               */      \
    const IEInStream & operator >> (const IEInStream & stream, data_type & input);                              \
    /* \brief   Writing to stream operator. It reads data from output instance                          */      \
    /*          and writes data to stream object.                                                       */      \
    IEOutStream & operator << (IEOutStream & stream, const data_type & output);                                 \


/**
 * \brief   Implementation of streaming operators. Fine for object with primitives.
 *          Might be not proper for objects with multiple instance.
 **/
#define IMPLEMENT_STREAMABLE(data_type)                                                                         \
    /* \brief   Read data from stream and initialize input object.                                      */      \
    inline const IEInStream& operator >> (const IEInStream& stream, data_type & input)                          \
    {   stream.read( reinterpret_cast<unsigned char *>(&input), sizeof(data_type) ); return stream; }           \
                                                                                                                \
    /* \brief   Write output data object to stream instance.                                            */      \
    inline IEOutStream& operator << (IEOutStream& stream, const data_type& output)                              \
    {   stream.write( reinterpret_cast<const unsigned char *>(&output), sizeof(data_type) ); return stream; }   \


/**
 * \brief   Declare operator with library compiler switches.
 **/
#define DECLARE_STREAMABLE_EXPORT(data_type, ExportDef)                                                         \
    friend ExportDef const IEInStream & operator >> (const IEInStream & stream, data_type & input);             \
    friend ExportDef IEOutStream & operator << (IEOutStream & stream, const data_type & output);                \

#define GLOBAL_DECLARE_STREAMABLE_EXPORT(data_type, ExportDef)                                                  \
    ExportDef const IEInStream & operator >> (const IEInStream & stream, data_type & input);                    \
    ExportDef IEOutStream & operator << (IEOutStream & stream, const data_type & output);                       \

#define IMPLEMENT_READABLE_EXPORT(data_type, ExportDef)                                                         \
    ExportDef const IEInStream & operator >> (const IEInStream & stream, data_type & input)                     \

#define IMPLEMENT_WRITABLE_EXPORT(data_type, ExportDef)                                                         \
    ExportDef IEOutStream & operator << (IEOutStream & stream, const data_type & output)                        \


//////////////////////////////////////////////////////////////////////////
// IEInStream class declaration: to read data from
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This class is an interface and implementations of basic functions
 *          to support read from the data stream.
 **/
class AREG_API IEInStream
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor
     **/
    IEInStream( void ) = default;

    /**
     * \brief   Destructor
     **/
    virtual ~IEInStream( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEInStream interface data read overrides
/************************************************************************/
    /**
     * \brief   Reads and returns 8-bit value from buffer
     **/
    virtual uint8_t read8Bits( void ) const;

    /**
     * \brief   Reads and returns 16-bit value from buffer
     **/
    virtual uint16_t read16Bits( void ) const;

    /**
     * \brief   Reads and returns 32-bit value from buffer
     **/
    virtual uint32_t read32Bits( void ) const;

    /**
     * \brief   Reads and returns 64-bit value from buffer
     **/
    virtual uint64_t read64Bits( void ) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEInStream interface overrides
/************************************************************************/

    /**
     * \brief	Reads data from input stream object, copies into given buffer and
     *          returns the size in bytes of copied data.
     * \param	buffer	The pointer to buffer to copy data.
     * \param	size	The size in bytes of available buffer.
     * \return	Returns the size in bytes of copied data.
     **/
    virtual unsigned int read( unsigned char * buffer, unsigned int size ) const = 0;

    /**
     * \brief   Reads data from input stream object, copies into given Byte Buffer object
     *          and returns the size in bytes of copied data.
     * \param   buffer  The instance of Byte Buffer object to copy data.
     * \return	Returns the size in bytes of copied data.
     **/
    virtual unsigned int read( IEByteBuffer & buffer ) const = 0;

    /**
     * \brief   Reads string data from input stream object, copies into given string object
     *          and returns the size in bytes of copied data.
     * \param   ascii   The instance of ASCII string object to copy data.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( String & ascii ) const = 0;

    /**
     * \brief   Reads string data from input stream object, copies into given wide-string object
     *          and returns the size in bytes of copied data.
     * \param   ascii   The instance of wide-string object to copy data.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( WideString & wideString ) const = 0;

    /**
     * \brief   Resets cursor position and moves to the begin of data.
     **/
    virtual void resetCursor( void ) const = 0;

protected:
    /**
     * \brief	Returns size in bytes of available data that can read, 
     *          i.e. remaining readable size. The returns value is less or equal to
     *          the size of streamable buffer.
     **/
    virtual unsigned int getSizeReadable( void ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEInStream );
}; 


//////////////////////////////////////////////////////////////////////////
// IEOutStream class declaration: to write data to
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This class is an interface and implementations of basic functions
 *          to support write to the data stream.
 **/
class AREG_API IEOutStream
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor
     **/
    IEOutStream( void ) = default;

    /**
     * \brief   Destructor
     **/
    virtual ~IEOutStream( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEOutStream interface data write overrides
/************************************************************************/
    /**
     * \brief   Writes given 8-bit value into the stream and returns true if operation succeeded.
     * \param   value8Bit   The 8-bit value to write into the stream.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool write8Bits( uint8_t value8Bit );

    /**
     * \brief   Writes given 16-bit value into the stream and returns true if operation succeeded.
     * \param   value8Bit   The 16-bit value to write into the stream.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool write16Bits( uint16_t value16Bit );

    /**
     * \brief   Writes given 32-bit value into the stream and returns true if operation succeeded.
     * \param   value8Bit   The 32-bit value to write into the stream.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool write32Bits( uint32_t value32Bit );

    /**
     * \brief   Writes given 64-bit value into the stream and returns true if operation succeeded.
     * \param   value8Bit   The 64-bit value to write into the stream.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool write64Bits( uint64_t value64Bit );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEOutStream interface overrides
/************************************************************************/

     /**
      * \brief	Writes data to output stream object from given buffer and 
      *         returns the size in bytes of written data.
      * \param	buffer	The pointer to buffer as a data source.
      * \param	size	The size in bytes of data buffer.
      * \return	Returns the size in bytes of written data.
      **/
    virtual unsigned int write( const unsigned char * buffer, unsigned int size ) = 0;

     /**
      * \brief	Writes data to output stream object from given byte-buffer object and
      *         returns the size in bytes of written data.
      * \param	buffer	The instance of byte-buffer object as a data source.
      * \return	Returns the size in bytes of written data.
      **/
    virtual unsigned int write( const IEByteBuffer & buffer ) = 0;

     /**
      * \brief	Writes data to output stream object from given ASCII-string object and
      *         returns the size in bytes of written data.
      * \param	ascii	The instance of ASCII-string object as a data source.
      * \return	Returns the size in bytes of written data.
      **/
    virtual unsigned int write( const String & ascii )  = 0;

    /**
     * \brief	Writes data to output stream object from given wide-string object and
     *         returns the size in bytes of written data.
     * \param	ascii	The instance of wide-string object as a data source.
     * \return	Returns the size in bytes of written data.
     **/
    virtual unsigned int write( const WideString & wideString ) = 0;

    /**
     * \brief	Flushes cached data to output stream object.
     **/
    virtual void flush( void ) = 0;

protected:
    /**
     * \brief	Returns the size in bytes of available space in the stream to write data, 
     *          i.e. remaining writable size.
     **/
    virtual unsigned int getSizeWritable( void ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEOutStream );
};

//////////////////////////////////////////////////////////////////////////
// IEIOStream class declaration: to read / write data
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Input and Output streaming interface. 
 **/
class AREG_API IEIOStream  : public IEInStream
                           , public IEOutStream
{
protected:
    /**
     * \brief   Protected constructor
     **/
    IEIOStream( void ) = default;

public:
    /**
     * \brief   Destructor
     **/
    virtual ~IEIOStream( void ) = default;

public:
/************************************************************************
 * \brief   Support streaming of primitives
 ************************************************************************/
    DECLARE_STREAMABLE(bool)            //!< Declare primitive type bool as streamable
    DECLARE_STREAMABLE(char)            //!< Declare primitive type char as streamable
    DECLARE_STREAMABLE(wchar_t)         //!< Declare primitive type wchar_t as streamable
    DECLARE_STREAMABLE(unsigned char)   //!< Declare primitive type unsigned char as streamable
    DECLARE_STREAMABLE(short)           //!< Declare primitive type short as streamable
    DECLARE_STREAMABLE(unsigned short)  //!< Declare primitive type unsigned short as streamable
    DECLARE_STREAMABLE(int)             //!< Declare primitive type int as streamable
    DECLARE_STREAMABLE(unsigned int)    //!< Declare primitive type unsigned int as streamable
    DECLARE_STREAMABLE(int64_t)         //!< Declare primitive type int64_t as streamable
    DECLARE_STREAMABLE(uint64_t)        //!< Declare primitive type uint64_t as streamable
    DECLARE_STREAMABLE(float)           //!< Declare primitive type float as streamable
    DECLARE_STREAMABLE(double)          //!< Declare primitive type double as streamable

    /**
     * \brief   Writes an ASCII string to the stream
     **/
    friend inline IEOutStream & operator << (IEOutStream & stream, const char * output);

    /**
     * \brief   Writes an wide string to the stream
     **/
    friend inline IEOutStream & operator << (IEOutStream & stream, const wchar_t * output);

    /**
     * \brief   Writes STL string into the stream.
     **/
    template<typename CharType>
    friend inline IEOutStream& operator << (IEOutStream& stream, const std::basic_string<CharType>& output);
    template<typename CharType>
    friend inline IEOutStream& operator << (IEOutStream& stream, const std::basic_string_view<CharType>& output);

    /**
     * \brief   Writes STL string into the stream.
     **/
    template<typename CharType>
    friend inline const IEInStream& operator >> (const IEInStream& stream, std::basic_string<CharType>& input);

    /**
     * \brief   Reads and writes std::deque object to the stream.
     *          Each element in the container must be streamable.
     **/
    template<typename ElemType>
    friend inline IEOutStream& operator << (IEOutStream& stream, const std::deque<ElemType>& output);
    template<typename ElemType>
    friend inline const IEInStream& operator >> (const IEInStream& stream, std::deque<ElemType>& input);

    /**
     * \brief   Reads and writes std::list object to the stream.
     *          Each element in the container must be streamable.
     **/
    template<typename ElemType>
    friend inline IEOutStream& operator << (IEOutStream& stream, const std::list<ElemType>& output);
    template<typename ElemType>
    friend inline const IEInStream& operator >> (const IEInStream& stream, std::list<ElemType>& input);

    /**
     * \brief   Reads and writes std::vector object to the stream.
     *          Each element in the container must be streamable. 
     **/
    template<typename ElemType>
    friend inline IEOutStream& operator << (IEOutStream& stream, const std::vector<ElemType>& output);
    template<typename ElemType>
    friend inline const IEInStream& operator >> (const IEInStream& stream, std::vector<ElemType>& input);

    /**
     * \brief   Reads and writes std::pair object to the stream.
     *          Each element in the container must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline IEOutStream& operator << (IEOutStream& stream, const std::pair<Key, Value>& output);
    template<typename Key, typename Value>
    friend inline const IEInStream& operator >> (const IEInStream& stream, std::pair<Key, Value>& input);

    /**
     * \brief   Reads and writes std::map object to the stream.
     *          Each element in the container must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline IEOutStream& operator << (IEOutStream& stream, const std::map<Key, Value>& output);
    template<typename Key, typename Value>
    friend inline const IEInStream& operator >> (const IEInStream& stream, std::map<Key, Value>& input);

    /**
     * \brief   Reads and writes std::unordered_map object to the stream.
     *          Each element in the container must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline IEOutStream& operator << (IEOutStream& stream, const std::unordered_map<Key, Value>& output);
    template<typename Key, typename Value>
    friend inline const IEInStream& operator >> (const IEInStream& stream, std::unordered_map<Key, Value>& input);


//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEIOStream );
};

//////////////////////////////////////////////////////////////////////////
// Inline function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MACRO make primitives streamable
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_STREAMABLE(bool)
IMPLEMENT_STREAMABLE(char)
IMPLEMENT_STREAMABLE(wchar_t)
IMPLEMENT_STREAMABLE(unsigned char)
IMPLEMENT_STREAMABLE(int16_t)
IMPLEMENT_STREAMABLE(uint16_t)
IMPLEMENT_STREAMABLE(int32_t)
IMPLEMENT_STREAMABLE(uint32_t)
IMPLEMENT_STREAMABLE(int64_t)
IMPLEMENT_STREAMABLE(uint64_t)
IMPLEMENT_STREAMABLE(float)
IMPLEMENT_STREAMABLE(double)

inline IEOutStream & operator << (IEOutStream & stream, const char * output)
{
    if (output != nullptr)
    {
        constexpr unsigned int single = static_cast<unsigned int>(sizeof(char));
        unsigned int length = 0;
        const char * src = output;
        while (*src ++ != '\0')
        {
            ++ length;
        }
        
        stream.write(reinterpret_cast<const unsigned char *>(output), (length + 1) * single);
    }

    return stream;
}

inline IEOutStream & operator << (IEOutStream & stream, const wchar_t * output)
{
    if (output != nullptr)
    {
        constexpr unsigned int single = static_cast<unsigned int>(sizeof(wchar_t));
        unsigned int length = 0;
        const wchar_t * src = output;
        while (*src ++ != L'\0')
        {
            ++ length;
        }

        stream.write(reinterpret_cast<const unsigned char *>(output), (length + 1) * single);
    }

    return stream;
}

template<typename CharType>
inline IEOutStream& operator << (IEOutStream& stream, const std::basic_string<CharType>& output)
{
    constexpr unsigned int single = sizeof(CharType);
    stream.write(reinterpret_cast<const unsigned char*>(output.c_str()), (output.length() + 1) * single);
    return stream;
}

template<typename CharType>
inline IEOutStream& operator << (IEOutStream& stream, const std::basic_string_view<CharType>& output)
{
    constexpr unsigned int single = sizeof(CharType);
    stream.write(reinterpret_cast<const unsigned char*>(output.data()), (output.length() + 1) * single);
    return stream;
}

template<typename CharType>
inline const IEInStream& operator >> (const IEInStream& stream, std::basic_string<CharType>& input)
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
inline IEOutStream& operator << (IEOutStream& stream, const std::deque<ElemType>& output)
{
    stream << static_cast<unsigned int>(output.size());
    for (const auto& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename ElemType>
inline const IEInStream& operator >> (const IEInStream& stream, std::deque<ElemType>& input)
{
    input.clear();

    unsigned int size = 0;
    stream >> size;
    input.resize(size);
    for (auto& elem : input)
    {
        stream >> elem;
    }

    return stream;
}

template<typename ElemType>
inline IEOutStream& operator << (IEOutStream& stream, const std::list<ElemType>& output)
{
    stream << static_cast<unsigned int>(output.size());
    for (const auto& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename ElemType>
inline const IEInStream& operator >> (const IEInStream& stream, std::list<ElemType>& input)
{
    input.clear();

    unsigned int size = 0;
    stream >> size;
    input.resize(size);
    for (auto& elem : input)
    {
        stream >> elem;
    }

    return stream;
}

template<typename ElemType>
inline IEOutStream& operator << (IEOutStream& stream, const std::vector<ElemType>& output)
{
    stream << static_cast<unsigned int>(output.size());
    for (const auto& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename ElemType>
inline const IEInStream& operator >> (const IEInStream& stream, std::vector<ElemType>& input)
{
    input.clear();

    unsigned int size = 0;
    stream >> size;
    input.resize(size);
    for (auto& elem : input)
    {
        stream >> elem;
    }

    return stream;
}

template<typename Key, typename Value>
inline IEOutStream& operator << (IEOutStream& stream, const std::pair<Key, Value>& output)
{
    stream << output.first;
    stream << output.second;
    return stream;
}

template<typename Key, typename Value>
inline const IEInStream& operator >> (const IEInStream& stream, std::pair<Key, Value>& input)
{
    stream >> input.first;
    stream >> input.second;
    return stream;
}

template<typename Key, typename Value>
inline IEOutStream& operator << (IEOutStream& stream, const std::map<Key, Value>& output)
{
    stream << static_cast<unsigned int>(output.size());
    for (const std::pair<Key, Value> & elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename Key, typename Value>
inline const IEInStream& operator >> (const IEInStream& stream, std::map<Key, Value>& input)
{
    input.clear();

    unsigned int size = 0;
    stream >> size;
    for (unsigned int i = 0; i < size; ++i)
    {
        std::pair<Key, Value> elem;
        stream >> elem;
        input[elem.first] = elem.second;
    }

    return stream;
}

template<typename Key, typename Value>
inline IEOutStream& operator << (IEOutStream& stream, const std::unordered_map<Key, Value>& output)
{
    stream << static_cast<unsigned int>(output.size());
    for (const std::pair<Key, Value>& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename Key, typename Value>
inline const IEInStream& operator >> (const IEInStream& stream, std::unordered_map<Key, Value>& input)
{
    input.clear();

    unsigned int size = 0;
    stream >> size;
    for (unsigned int i = 0; i < size; ++i)
    {
        std::pair<Key, Value> elem;
        stream >> elem;
        input[elem.first] = elem.second;
    }

    return stream;
}

#endif  // AREG_BASE_IEIOSTREAM_HPP
