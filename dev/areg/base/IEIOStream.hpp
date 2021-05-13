#ifndef AREG_BASE_IEIOSTREAM_HPP
#define AREG_BASE_IEIOSTREAM_HPP
/************************************************************************
 * \file        areg/base/IEIOStream.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
    friend const IEInStream & operator >> (const IEInStream & stream, data_type & input);                       \
    /* \brief   Writing to stream operator. It reads data from output instance                          */      \
    /*          and writes data to stream object.                                                       */      \
    friend IEOutStream & operator << (IEOutStream & stream, const data_type & output);                          \

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
 * \brief       Pure virtual class to support data input streaming.
 *              This class supports read functionality to retrieve
 *              and instantiate data.
 * 
 * \details     Data streaming objects are used to transfer data between
 *              different threads and processes. The mechanism of data transfer
 *              depends on every individual streaming class implementation.
 *              Other classes, which are supporting serialization and can be
 *              instantiated by data streaming object, should overwrite
 *              read from stream operator (operator >> (const IEInStream& stream,...);
 *
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
    IEInStream( void );

public:
    /**
     * \brief   Destructor
     **/
    virtual ~IEInStream( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEOutStream interface data read overrides
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
     *          returns the size of copied data
     * \param	buffer	The pointer to buffer to copy data from input object
     * \param	size	The size in bytes of available buffer
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int read( unsigned char * buffer, unsigned int size ) const = 0;

    /**
     * \brief   Reads data from input stream object, copies into give Byte Buffer object
     *          and returns the size of copied data. Overwrite this method if copy behavior
     *          changed for certain buffer. For other buffers it should have simple behavior
     *          as copying to raw buffer
     * \param   buffer  The instance of Byte Buffer object to stream data from Input Stream object
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int read( IEByteBuffer & buffer ) const = 0;

    /**
     * \brief   Reads string data from Input Stream object and copies into given ASCII String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of ASCII String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( String & asciiString ) const = 0;

    /**
     * \brief   Reads string data from Input Stream object and copies into given Wide String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString      The buffer of Wide String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( WideString & wideString ) const = 0;

    /**
     * \brief   Resets cursor pointer and moves to the begin of data.
     *          Implement the function if stream has pointer reset mechanism
     **/
    virtual void resetCursor( void ) const = 0;

protected:
    /**
     * \brief	Returns size in bytes of available data that can be read, 
     *          i.e. remaining readable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already read from stream, the available readable size is 'n - x'.
     **/
    virtual unsigned int getSizeReadable( void ) const = 0;
}; 


//////////////////////////////////////////////////////////////////////////
// IEOutStream class declaration: to write data to
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Pure virtual class to support data output streaming.
 *              This class supports write functionality to serialize data.
 * 
 * \details     Data streaming objects are used to transfer data between
 *              different threads and processes. The mechanism of data transfer
 *              depends on every individual streaming class implementation.
 *              Other classes, which are supporting serialization and can
 *              transfer data by streaming object, should overwrite
 *              write to stream operator (operator << (IEOutStream& stream,...);
 *
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
    IEOutStream( void );

public:
    /**
     * \brief   Destructor
     **/
    virtual ~IEOutStream( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEOutStream interface data write overrides
/************************************************************************/
    /**
     * \brief   Writes given 8-bit value into the stream and returns true if operation succeeded.
     * \param   value8Bit       The 8-bit value to write into the stream.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool write8Bits( uint8_t value8Bit );

    /**
     * \brief   Writes given 16-bit value into the stream and returns true if operation succeeded.
     * \param   value8Bit       The 16-bit value to write into the stream.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool write16Bits( uint16_t value16Bit );

    /**
     * \brief   Writes given 32-bit value into the stream and returns true if operation succeeded.
     * \param   value8Bit       The 32-bit value to write into the stream.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool write32Bits( uint32_t value32Bit );

    /**
     * \brief   Writes given 64-bit value into the stream and returns true if operation succeeded.
     * \param   value8Bit       The 64-bit value to write into the stream.
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
     * \brief	Write data to output stream object from given buffer
     *          and returns the size of written data
     * \param	buffer	The pointer to buffer to read data and 
     *          copy to output stream object
     * \param	size	The size in bytes of data buffer
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int write( const unsigned char * buffer, unsigned int size ) = 0;

    /**
     * \brief	Writes Binary data from Byte Buffer object to Output Stream object
     *          and returns the size of written data. Overwrite this method if need 
     *          to change behavior of streaming buffer.
     * \param	buffer	The instance of Byte Buffer object containing data to stream to Output Stream.
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int write( const IEByteBuffer & buffer ) = 0;

    /**
     * \brief   Writes string data from given ASCII String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const String & asciiString )  = 0;

    /**
     * \brief   Writes string data from given wide-char String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString  The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const WideString & wideString ) = 0;

    /**
     * \brief	Flushes cached data to output stream object.
     *          Implement the function if device has caching mechanism
     **/
    virtual void flush( void ) = 0;

protected:
    /**
     * \brief	Returns size in bytes of available space that can be written, 
     *          i.e. remaining writable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already written to stream, the available writable size is 'n - x'.
     **/
    virtual unsigned int getSizeWritable( void ) const = 0;
};

//////////////////////////////////////////////////////////////////////////
// IEIOStream class declaration: to read / write data
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Input and Output streaming interface. 
 * 
 * \details     Classes implementing this interface are supporting
 *              data input and output functionalities.
 *
 **/
class AREG_API IEIOStream  : public IEInStream
                           , public IEOutStream
{
protected:
    /**
     * \brief   Protected constructor
     **/
    IEIOStream( void );

public:
    /**
     * \brief   Destructor
     **/
    virtual ~IEIOStream( void );
public:
/************************************************************************
 * \brief   Support streaming of primitives
 ************************************************************************/
    DECLARE_STREAMABLE(bool)                //!< Declare primitive type bool as streamable
    DECLARE_STREAMABLE(char)                //!< Declare primitive type char as streamable
    DECLARE_STREAMABLE(wchar_t)             //!< Declare primitive type wchar_t as streamable
    DECLARE_STREAMABLE(unsigned char)       //!< Declare primitive type unsigned char as streamable
    DECLARE_STREAMABLE(short)               //!< Declare primitive type short as streamable
    DECLARE_STREAMABLE(unsigned short)      //!< Declare primitive type unsigned short as streamable
    DECLARE_STREAMABLE(int)                 //!< Declare primitive type int as streamable
    DECLARE_STREAMABLE(unsigned int)        //!< Declare primitive type unsigned int as streamable
    DECLARE_STREAMABLE(int64_t)             //!< Declare primitive type int64_t as streamable
    DECLARE_STREAMABLE(uint64_t)            //!< Declare primitive type uint64_t as streamable
    DECLARE_STREAMABLE(float)               //!< Declare primitive type float as streamable
    DECLARE_STREAMABLE(double)              //!< Declare primitive type double as streamable
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

#endif  // AREG_BASE_IEIOSTREAM_HPP
