#ifndef AREG_COMPONENT_EVENTDATASTREAM_HPP
#define AREG_COMPONENT_EVENTDATASTREAM_HPP
/************************************************************************
 * \file        areg/component/EventDataStream.hpp
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

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/TEStack.hpp"

//////////////////////////////////////////////////////////////////////////
// EventDataStream class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       This object is used to serialize and de-serialize event
 *              data. Every instance of Component and Proxy event
 *              contains data, which has a streaming object and which
 *              contains at least information of function parameters,
 *              attributes and states.
 **/
class AREG_API EventDataStream : public IEIOStream
{
//////////////////////////////////////////////////////////////////////////
// Internal constants and types public
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   EventDataStream::EmptyData
     *          Predefined Empty Data object. Can be used when event has
     *          no data to transfer and no data should be write or read,
     *          because it is instance of Invalid Buffer and any 
     *          read / write will fail.
     **/
    static const EventDataStream  EmptyData;

    /**
     * \brief   EventDataStream::eEventData
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
    EventDataStream( EventDataStream::eEventData evetDataType, const char* name = NULL );

    /**
     * \brief	Constructor. Creates read only event data streaming object containing read only data of shared buffer.
     * \param	buffer	The shared buffer to pass to streaming object.
     * \param	name	The name of streaming object. Can be ignored.
     **/
    EventDataStream(const EventDataStream & buffer, const char* name );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    EventDataStream( const EventDataStream & src );

    /**
     * \brief   Initialization constructor.
     *          Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    EventDataStream( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~EventDataStream( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies Event Data from given source.
     * \param   src     The source of event data to copy.
     **/
    const EventDataStream & operator = ( const EventDataStream & src );


/************************************************************************/
// Friend global operators to stream Event Data Stream Buffer
/************************************************************************/

    /**
     * \brief	Friend global operator declaration to read data from streaming object
     * \param	stream	The data streaming object to read data
     * \param	input	The Event Data Stream Buffer object to write data
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, EventDataStream & input );
    /**
     * \brief	Friend global operator declaration to write data to streaming object
     * \param	stream	The data streaming object to write data
     * \param	output	The Event Data Stream Buffer object containing data
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const EventDataStream & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if buffer is either empty or is invalid.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief   Returns true if buffer is saving for external data streaming.
     **/
    inline bool isExternalDataStream( void ) const;

    /**
     * \brief   Returns reference to the streaming object to read data
     **/
    inline const IEInStream & getStreamForRead( void ) const;

    /**
     * \brief   Returns reference to the streaming object to write data
     **/
    inline IEOutStream & getStreamForWrite( void );

/************************************************************************/
// IEInStream interface overrides
/************************************************************************/

    /**
     * \brief   Resets cursor pointer and moves to the begin of data.
     *          Implement the function if stream has pointer reset mechanism
     **/
    virtual void resetCursor( void ) const;

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
    virtual unsigned int read( unsigned char* buffer, unsigned int size ) const;

    /**
     * \brief   Reads data from input stream object, copies into give Byte Buffer object
     *          and returns the size of copied data. Overwrite this method if copy behavior
     *          changed for certain buffer. For other buffers it should have simple behavior
     *          as copying to raw buffer
     * \param   buffer  The instance of Byte Buffer object to stream data from Input Stream object
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int read( IEByteBuffer & buffer ) const;

    /**
     * \brief   Reads string data from Input Stream object and copies into given ASCII String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of ASCII String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( String & asciiString ) const;

    /**
     * \brief   Reads string data from Input Stream object and copies into given Wide String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString      The buffer of Wide String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( WideString & wideString ) const;

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
    virtual unsigned int write( const unsigned char* buffer, unsigned int size );

    /**
     * \brief	Writes Binary data from Byte Buffer object to Output Stream object
     *          and returns the size of written data. Overwrite this method if need 
     *          to change behavior of streaming buffer.
     * \param	buffer	The instance of Byte Buffer object containing data to stream to Output Stream.
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int write( const IEByteBuffer & buffer );

    /**
    * \brief   Writes string data from given ASCII String object to output stream object.
    *          Overwrite method if need to change behavior of streaming string.
    * \param   asciiString     The buffer of String containing data to stream to Output Stream.
    * \return  Returns the size in bytes of copied string data.
    **/
    virtual unsigned int write( const String & asciiString );

    /**
    * \brief   Writes string data from given wide-char String object to output stream object.
    *          Overwrite method if need to change behavior of streaming string.
    * \param   wideString  The buffer of String containing data to stream to Output Stream.
    * \return  Returns the size in bytes of copied string data.
    **/
    virtual unsigned int write( const WideString & wideString );

    /**
     * \brief	Flushes cached data to output stream object.
     *          Implement the function is device has caching mechanism
     **/
    virtual void flush( void );

protected:
    /**
     * \brief	Returns size in bytes of available data that can be read, 
     *          i.e. remaining readable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already read from stream, the available readable size is 'n - x'.
     **/
    virtual unsigned int getSizeReadable( void ) const;

    /**
     * \brief	Returns size in bytes of available space that can be written, 
     *          i.e. remaining writable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already written to stream, the available writable size is 'n - x'.
     **/
    virtual unsigned int getSizeWritable( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    
    /**
     * \brief   The type of Event Data. Either internal or external.
     **/
    EventDataStream::eEventData mEventDataType;

    /**
     * \brief   The name of Event Data object.
     **/
    String                      mBufferName;

    /**
     * \brief   The Shared Buffer where the data is written / streamed.
     **/
    mutable SharedBuffer        mDataBuffer;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   FIFO Stack of Shared Buffers. Used only if the data type is internal.
     *          All Shared Buffers instead of copying data, will be added to this list.
     **/
    mutable TENolockStack<SharedBuffer, const SharedBuffer &>  mSharedList;
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER
};

IMPLEMENT_STREAMABLE(EventDataStream::eEventData)

//////////////////////////////////////////////////////////////////////////
// EventDataStream inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline bool EventDataStream::isEmpty( void ) const
{
    return (mDataBuffer.isEmpty() && mSharedList.isEmpty());
}

inline bool EventDataStream::isExternalDataStream( void ) const
{
    return (mEventDataType == EventDataStream::EventDataExternal);
}

inline const IEInStream & EventDataStream::getStreamForRead( void ) const
{
    return static_cast<const IEInStream &>(*this);
}

inline IEOutStream & EventDataStream::getStreamForWrite( void )
{
    return static_cast<IEOutStream &>(*this);
}

#endif  // AREG_COMPONENT_EVENTDATASTREAM_HPP
