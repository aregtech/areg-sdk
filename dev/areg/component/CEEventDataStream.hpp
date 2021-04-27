#ifndef AREG_COMPONENT_CEEVENTDATASTREAM_HPP
#define AREG_COMPONENT_CEEVENTDATASTREAM_HPP
/************************************************************************
 * \file        areg/component/CEEventDataStream.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event data streaming object.
 *              This object is used to serialize and de-serialize
 *              data when service event is passed.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEIOStream.hpp"

#include "areg/base/CESharedBuffer.hpp"
#include "areg/base/CEString.hpp"
#include "areg/base/TEStack.hpp"

//////////////////////////////////////////////////////////////////////////
// CEEventDataStream class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       This object is used to serialize and de-serialize event
 *              data. Every instance of Component and Proxy event
 *              contains data, which has a streaming object and which
 *              contains at least information of function parameters,
 *              attributes and states.
 * 
 * \details     This object is an instance of shared buffer. The actual
 *              data in Byte Buffer will not be copied when event is
 *              instantiated multiple time. For all event dispatchers
 *              the data saved in this streaming object is accessible
 *              for read only operation, which guaranties that all
 *              no internal data will be updated when event is processed
 *              in different threads.
 *
 **/
class AREG_API CEEventDataStream : public IEIOStream
{
//////////////////////////////////////////////////////////////////////////
// Internal constants and types public
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   CEEventDataStream::EmptyData
     *          Predefined Empty Data object. Can be used when event has
     *          no data to transfer and no data should be write or read,
     *          because it is instance of Invalid Buffer and any 
     *          read / write will fail.
     **/
    static const CEEventDataStream  EmptyData;

    /**
     * \brief   CEEventDataStream::eEventData
     *          The type of Event Data.
     *          If internal, the Shared Buffer data will not be copied, but shared.
     *          If external, the Shared Buffer data will be copied.
     **/
    typedef enum E_EventData
    {
          EventDataInternal //!< Internal Data
        , EventDataExternal //!< External Data

    } eEventData;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Constructor. Sets event data type and optional name.
     * \param   evetDataType    The type of event data. Either for internal or for external communication
     * \param   name            The name for streaming object. Can be ignored.
     **/
    CEEventDataStream( CEEventDataStream::eEventData evetDataType, const char* name = NULL );

    /**
     * \brief	Constructor. Creates read only event data streaming object containing read only data of shared buffer.
     * \param	buffer	The shared buffer to pass to streaming object.
     * \param	name	The name of streaming object. Can be ignored.
     **/
    CEEventDataStream(const CEEventDataStream & buffer, const char* name );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    CEEventDataStream( const CEEventDataStream & src );

    /**
     * \brief   Initialization constructor.
     *          Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    CEEventDataStream( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEEventDataStream( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies Event Data from given source.
     * \param   src     The source of event data to copy.
     **/
    const CEEventDataStream & operator = ( const CEEventDataStream & src );


/************************************************************************/
// Friend global operators to stream Event Data Stream Buffer
/************************************************************************/

    /**
     * \brief	Friend global operator declaration to read data from streaming object
     * \param	stream	The data streaming object to read data
     * \param	input	The Event Data Stream Buffer object to write data
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEEventDataStream & input );
    /**
     * \brief	Friend global operator declaration to write data to streaming object
     * \param	stream	The data streaming object to write data
     * \param	output	The Event Data Stream Buffer object containing data
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEEventDataStream & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if buffer is either empty or is invalid.
     **/
    inline bool IsEmpty( void ) const;

    /**
     * \brief   Returns true if buffer is saving for external data streaming.
     **/
    inline bool IsExternalDataStream( void ) const;

    /**
     * \brief   Returns reference to the streaming object to read data
     **/
    inline const IEInStream & GetStreamForRead( void ) const;

    /**
     * \brief   Returns reference to the streaming object to write data
     **/
    inline IEOutStream & GetStreamForWrite( void );

/************************************************************************/
// IEInStream interface overrides
/************************************************************************/

    /**
     * \brief   Resets cursor pointer and moves to the begin of data.
     *          Implement the function if stream has pointer reset mechanism
     **/
    virtual void ResetCursor( void ) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
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
    virtual unsigned int Read( unsigned char* buffer, unsigned int size ) const;

    /**
     * \brief   Reads data from input stream object, copies into give Byte Buffer object
     *          and returns the size of copied data. Overwrite this method if copy behavior
     *          changed for certain buffer. For other buffers it should have simple behavior
     *          as copying to raw buffer
     * \param   buffer  The instance of Byte Buffer object to stream data from Input Stream object
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int Read( IEByteBuffer & buffer ) const;

    /**
     * \brief   Reads string data from Input Stream object and copies into given ASCII String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of ASCII String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int Read( CEString & asciiString ) const;

    /**
     * \brief   Reads string data from Input Stream object and copies into given Wide String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString      The buffer of Wide String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int Read( CEWideString & wideString ) const;

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
    virtual unsigned int Write( const unsigned char* buffer, unsigned int size );

    /**
     * \brief	Writes Binary data from Byte Buffer object to Output Stream object
     *          and returns the size of written data. Overwrite this method if need 
     *          to change behavior of streaming buffer.
     * \param	buffer	The instance of Byte Buffer object containing data to stream to Output Stream.
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int Write( const IEByteBuffer & buffer );

    /**
    * \brief   Writes string data from given ASCII String object to output stream object.
    *          Overwrite method if need to change behavior of streaming string.
    * \param   asciiString     The buffer of String containing data to stream to Output Stream.
    * \return  Returns the size in bytes of copied string data.
    **/
    virtual unsigned int Write( const CEString & asciiString );

    /**
    * \brief   Writes string data from given wide-char String object to output stream object.
    *          Overwrite method if need to change behavior of streaming string.
    * \param   wideString  The buffer of String containing data to stream to Output Stream.
    * \return  Returns the size in bytes of copied string data.
    **/
    virtual unsigned int Write( const CEWideString & wideString );

    /**
     * \brief	Flushes cached data to output stream object.
     *          Implement the function is device has caching mechanism
     **/
    virtual void Flush( void );

protected:
    /**
     * \brief	Returns size in bytes of available data that can be read, 
     *          i.e. remaining readable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already read from stream, the available readable size is 'n - x'.
     **/
    virtual unsigned int GetReadableSize( void ) const;

    /**
     * \brief	Returns size in bytes of available space that can be written, 
     *          i.e. remaining writable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already written to stream, the available writable size is 'n - x'.
     **/
    virtual unsigned int GetWritableSize( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    
    /**
     * \brief   The type of Event Data. Either internal or external.
     **/
    CEEventDataStream::eEventData   mEventDataType;

    /**
     * \brief   The name of Event Data object.
     **/
    CEString                        mBufferName;

    /**
     * \brief   The Shared Buffer where the data is written / streamed.
     **/
    mutable CESharedBuffer  mDataBuffer;

#if _MSC_VER
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   FIFO Stack of Shared Buffers. Used only if the data type is internal.
     *          All Shared Buffers instead of copying data, will be added to this list.
     **/
    mutable TENolockStack<CESharedBuffer, const CESharedBuffer &>  mSharedList;
#if _MSC_VER
    #pragma warning(default: 4251)
#endif  // _MSC_VER
};

IMPLEMENT_STREAMABLE(CEEventDataStream::eEventData)

//////////////////////////////////////////////////////////////////////////
// CEEventDataStream inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline bool CEEventDataStream::IsEmpty( void ) const
{   return (mDataBuffer.IsEmpty() && mSharedList.IsEmpty());        }

inline bool CEEventDataStream::IsExternalDataStream( void ) const
{   return mEventDataType == CEEventDataStream::EventDataExternal;  }

inline const IEInStream & CEEventDataStream::GetStreamForRead( void ) const
{   return static_cast<const IEInStream &>(*this);                 }

inline IEOutStream & CEEventDataStream::GetStreamForWrite( void )
{   return static_cast<IEOutStream &>(*this);                      }

#endif  // AREG_COMPONENT_CEEVENTDATASTREAM_HPP
