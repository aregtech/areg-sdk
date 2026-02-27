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
namespace areg
{
    class InStream;
    class OutStream;
    class IOStream;
}

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg
{
    class String;
    class WideString;
    class ByteBuffer;
}
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
    friend inline const areg::InStream & operator >> (const areg::InStream & stream, data_type & input);                \
    friend inline areg::OutStream & operator << (areg::OutStream & stream, const data_type & output)

/**
 * \brief   Declares global (non-friend) stream operators for a data type.
 **/
#define AREG_GLOBAL_DECLARE_STREAMABLE(data_type)                                                               \
    const areg::InStream & operator >> (const areg::InStream & stream, data_type & input);                              \
    areg::OutStream & operator << (areg::OutStream & stream, const data_type & output)

/**
 * \brief   Implements stream operators for trivially-copyable types via raw memory read/write.
 *          Includes a static_assert to catch misuse with non-trivial types.
 **/
#define AREG_IMPLEMENT_STREAMABLE(data_type)                                                                    \
    inline const areg::InStream& operator >> (const areg::InStream& stream, data_type & input)                          \
    {   stream.read( reinterpret_cast<uint8_t *>(&input), sizeof(data_type) ); return stream; }           \
    inline areg::OutStream& operator << (areg::OutStream& stream, const data_type& output)                              \
    {   stream.write( reinterpret_cast<const uint8_t *>(&output), sizeof(data_type) ); return stream; }

/**
 * \brief   Declares friend stream operators with DLL export/import specifier.
 **/
#define AREG_DECLARE_STREAMABLE_EXPORT(data_type, ExportDef)                                                    \
    friend ExportDef const areg::InStream & operator >> (const areg::InStream & stream, data_type & input);             \
    friend ExportDef areg::OutStream & operator << (areg::OutStream & stream, const data_type & output);

#define AREG_GLOBAL_DECLARE_STREAMABLE_EXPORT(data_type, ExportDef)                                             \
    ExportDef const areg::InStream & operator >> (const areg::InStream & stream, data_type & input);                    \
    ExportDef areg::OutStream & operator << (areg::OutStream & stream, const data_type & output);

#define AREG_IMPLEMENT_READABLE_EXPORT(data_type, ExportDef)                                                    \
    ExportDef const areg::InStream & operator >> (const areg::InStream & stream, data_type & input)

#define AREG_IMPLEMENT_WRITABLE_EXPORT(data_type, ExportDef)                                                    \
    ExportDef areg::OutStream & operator << (areg::OutStream & stream, const data_type & output)                        \


namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // InStream class declaration: to read data from
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   This class is an interface and implementations of basic functions
     *          to support read from the data stream.
     **/
    class AREG_API InStream
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Protected constructor
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
         * \brief   Reads and returns 8-bit value from buffer
         **/
        virtual uint8_t read8Bits() const;

        /**
         * \brief   Reads and returns 16-bit value from buffer
         **/
        virtual uint16_t read16Bits() const;

        /**
         * \brief   Reads and returns 32-bit value from buffer
         **/
        virtual uint32_t read32Bits() const;

        /**
         * \brief   Reads and returns 64-bit value from buffer
         **/
        virtual uint64_t read64Bits() const;

    //////////////////////////////////////////////////////////////////////////
    // Overrides
    //////////////////////////////////////////////////////////////////////////
    public:
    /************************************************************************/
    // InStream interface overrides
    /************************************************************************/

        /**
         * \brief	Reads data from input stream object, copies into given buffer and
         *          returns the size in bytes of copied data.
         * \param	buffer	The pointer to buffer to copy data.
         * \param	size	The size in bytes of available buffer.
         * \return	Returns the size in bytes of copied data.
         **/
        virtual uint32_t read( uint8_t * buffer, uint32_t size ) const = 0;

        /**
         * \brief   Reads data from input stream object, copies into given Byte Buffer object
         *          and returns the size in bytes of copied data.
         * \param   buffer  The instance of Byte Buffer object to copy data.
         * \return	Returns the size in bytes of copied data.
         **/
        virtual uint32_t read( ByteBuffer & buffer ) const = 0;

        /**
         * \brief   Reads string data from input stream object, copies into given string object
         *          and returns the size in bytes of copied data.
         * \param   ascii   The instance of ASCII string object to copy data.
         * \return  Returns the size in bytes of copied string data.
         **/
        virtual uint32_t read( String & ascii ) const = 0;

        /**
         * \brief   Reads string data from input stream object, copies into given wide-string object
         *          and returns the size in bytes of copied data.
         * \param   wide    The instance of wide-string object to copy data.
         * \return  Returns the size in bytes of copied string data.
         **/
        virtual uint32_t read( WideString & wide ) const = 0;

        /**
         * \brief   Resets cursor position and moves to the begin of data.
         **/
        virtual void resetCursor() const = 0;

    protected:
        /**
         * \brief	Returns size in bytes of available data that can read, 
         *          i.e. remaining readable size. The returns value is less or equal to
         *          the size of streamable buffer.
         **/
        virtual uint32_t getSizeReadable() const = 0;

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
     * \brief   This class is an interface and implementations of basic functions
     *          to support write to the data stream.
     **/
    class AREG_API OutStream
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Protected constructor
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
         * \brief   Writes given 8-bit value into the stream and returns true if operation succeeded.
         * \param   value8Bit   The 8-bit value to write into the stream.
         * \return  Returns true if operation succeeded.
         **/
        virtual bool write8Bits( uint8_t value8Bit );

        /**
         * \brief   Writes given 16-bit value into the stream and returns true if operation succeeded.
         * \param   value16Bit  The 16-bit value to write into the stream.
         * \return  Returns true if operation succeeded.
         **/
        virtual bool write16Bits( uint16_t value16Bit );

        /**
         * \brief   Writes given 32-bit value into the stream and returns true if operation succeeded.
         * \param   value32Bit  The 32-bit value to write into the stream.
         * \return  Returns true if operation succeeded.
         **/
        virtual bool write32Bits( uint32_t value32Bit );

        /**
         * \brief   Writes given 64-bit value into the stream and returns true if operation succeeded.
         * \param   value64Bit  The 64-bit value to write into the stream.
         * \return  Returns true if operation succeeded.
         **/
        virtual bool write64Bits( uint64_t value64Bit );

    //////////////////////////////////////////////////////////////////////////
    // Overrides
    //////////////////////////////////////////////////////////////////////////
    public:
    /************************************************************************/
    // OutStream interface overrides
    /************************************************************************/

        /**
         * \brief	Writes data to output stream object from given buffer and 
         *         returns the size in bytes of written data.
         * \param	buffer	The pointer to buffer as a data source.
         * \param	size	The size in bytes of data buffer.
         * \return	Returns the size in bytes of written data.
         **/
        virtual uint32_t write( const uint8_t * buffer, uint32_t size ) = 0;

        /**
         * \brief	Writes data to output stream object from given byte-buffer object and
         *         returns the size in bytes of written data.
         * \param	buffer	The instance of byte-buffer object as a data source.
         * \return	Returns the size in bytes of written data.
         **/
        virtual uint32_t write( const ByteBuffer & buffer ) = 0;

        /**
         * \brief	Writes data to output stream object from given ASCII-string object and
         *         returns the size in bytes of written data.
         * \param	ascii	The instance of ASCII-string object as a data source.
         * \return	Returns the size in bytes of written data.
         **/
        virtual uint32_t write( const String & ascii )  = 0;

        /**
         * \brief	Writes data to output stream object from given wide-string object and
         *         returns the size in bytes of written data.
         * \param	wide 	The instance of wide-string object as a data source.
         * \return	Returns the size in bytes of written data.
         **/
        virtual uint32_t write( const WideString & wide ) = 0;

        /**
         * \brief	Flushes cached data to output stream object.
         **/
        virtual void flush() = 0;

    protected:
        /**
         * \brief	Returns the size in bytes of available space in the stream to write data, 
         *          i.e. remaining writable size.
         **/
        virtual uint32_t getSizeWritable() const = 0;

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
     * \brief       Input and Output streaming interface. 
     **/
    class AREG_API IOStream  : public InStream
                            , public OutStream
    {
    protected:
        /**
         * \brief   Protected constructor
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
         * \brief   Writes an ASCII string to the stream
         **/
        friend inline OutStream & operator << (OutStream & stream, const char * output);

        /**
         * \brief   Writes an wide string to the stream
         **/
        friend inline OutStream & operator << (OutStream & stream, const wchar_t * output);

        /**
         * \brief   Writes STL string into the stream.
         **/
        template<typename CharType>
        friend inline OutStream& operator << (OutStream& stream, const std::basic_string<CharType>& output);
        template<typename CharType>
        friend inline OutStream& operator << (OutStream& stream, const std::basic_string_view<CharType>& output);

        /**
         * \brief   Writes STL string into the stream.
         **/
        template<typename CharType>
        friend inline const InStream& operator >> (const InStream& stream, std::basic_string<CharType>& input);

        /**
         * \brief   Reads and writes std::deque object to the stream.
         *          Each element in the container must be streamable.
         **/
        template<typename ElemType>
        friend inline OutStream& operator << (OutStream& stream, const std::deque<ElemType>& output);
        template<typename ElemType>
        friend inline const InStream& operator >> (const InStream& stream, std::deque<ElemType>& input);

        /**
         * \brief   Reads and writes std::list object to the stream.
         *          Each element in the container must be streamable.
         **/
        template<typename ElemType>
        friend inline OutStream& operator << (OutStream& stream, const std::list<ElemType>& output);
        template<typename ElemType>
        friend inline const InStream& operator >> (const InStream& stream, std::list<ElemType>& input);

        /**
         * \brief   Reads and writes std::vector object to the stream.
         *          Each element in the container must be streamable. 
         **/
        template<typename ElemType>
        friend inline OutStream& operator << (OutStream& stream, const std::vector<ElemType>& output);
        template<typename ElemType>
        friend inline const InStream& operator >> (const InStream& stream, std::vector<ElemType>& input);

        /**
         * \brief   Reads and writes std::pair object to the stream.
         *          Each element in the container must be streamable.
         **/
        template<typename Key, typename Value>
        friend inline OutStream& operator << (OutStream& stream, const std::pair<Key, Value>& output);
        template<typename Key, typename Value>
        friend inline const InStream& operator >> (const InStream& stream, std::pair<Key, Value>& input);

        /**
         * \brief   Reads and writes std::map object to the stream.
         *          Each element in the container must be streamable.
         **/
        template<typename Key, typename Value>
        friend inline OutStream& operator << (OutStream& stream, const std::map<Key, Value>& output);
        template<typename Key, typename Value>
        friend inline const InStream& operator >> (const InStream& stream, std::map<Key, Value>& input);

        /**
         * \brief   Reads and writes std::unordered_map object to the stream.
         *          Each element in the container must be streamable.
         **/
        template<typename Key, typename Value>
        friend inline OutStream& operator << (OutStream& stream, const std::unordered_map<Key, Value>& output);
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

} // namespace areg
#endif  // AREG_BASE_IOSTREAM_HPP
