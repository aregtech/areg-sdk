#ifndef AREG_COMPONENT_EVENTDATASTREAM_HPP
#define AREG_COMPONENT_EVENTDATASTREAM_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/EventDataStream.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event data streaming object.
 *              This object is used to serialize and deserialize
 *              data when service event is passed.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/IOStream.hpp"

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Stack.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventDataStream class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Object used to serialize and deserialize event data. Every instance of Component and
 *          Proxy event contains data, which has a streaming object and which contains at least
 *          information of function parameters, attributes and states.
 **/
class AREG_API EventDataStream final : public IOStream
{
    //! The list of shared buffer list (stack).
    using SharedList    = Stack<SharedBuffer>;

//////////////////////////////////////////////////////////////////////////
// Internal constants and types public
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   EventDataStream::EventDataKind
     *          The type of Event Data.
     *          If internal, the Shared Buffer data will not be copied, but shared.
     *          If external, the Shared Buffer data will be copied.
     **/
    enum class EventDataKind : uint8_t
    {
          Internal  = 1 //!< Internal Data
        , External  = 2 //!< External Data
        , Empty     = 3 //!< An empty data
    };

    /**
     * \brief   Returns predefined Empty Data object. Can be used when event has no data to transfer
     *          and no data should write or read.
     **/
    [[nodiscard]]
    static const EventDataStream& empty_data() noexcept;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Sets event data type and optional name.
     *
     * \param   evetDataType    The type of event data. Either for internal or for external
     *                          communication
     * \param   name            The name for streaming object. Can be ignored.
     **/
    explicit EventDataStream( EventDataStream::EventDataKind evetDataType, const String & name = String::empty_string() );

    /**
     * \brief   Creates read only event data streaming object containing read only data of shared
     *          buffer.
     *
     * \param   buffer      The shared buffer to pass to streaming object.
     * \param   name        The name of streaming object.
     **/
    EventDataStream(const EventDataStream & buffer, const String & name);

    /**
     * \brief   Initializes object data from streaming object.
     **/
    EventDataStream(const InStream& stream);

    EventDataStream( const EventDataStream & src );

    EventDataStream( EventDataStream && src ) noexcept;

    virtual ~EventDataStream();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    EventDataStream & operator = ( const EventDataStream & src );

    EventDataStream & operator = ( EventDataStream && src ) noexcept;

/************************************************************************/
// Friend global operators to stream Event Data Stream Buffer
/************************************************************************/

    /**
     * \brief   Friend global operator declaration to read data from streaming object.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, EventDataStream & input );

    /**
     * \brief   Friend global operator declaration to write data to streaming object.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const EventDataStream & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if buffer is either empty or is invalid.
     **/
    [[nodiscard]]
    inline bool is_empty() const noexcept;

    /**
     * \brief   Returns true if buffer is saving for external data streaming.
     **/
    [[nodiscard]]
    inline bool is_external_stream() const noexcept;

    /**
     * \brief   Returns reference to the streaming object to read data.
     **/
    [[nodiscard]]
    inline const InStream & stream_for_read() const noexcept;

    /**
     * \brief   Returns reference to the streaming object to write data.
     **/
    [[nodiscard]]
    inline OutStream & stream_for_write() noexcept;

/************************************************************************/
// InStream interface overrides
/************************************************************************/

    /**
     * \brief   Resets cursor pointer and moves to the begin of data.
     **/
    void reset() const noexcept override;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// InStream interface overrides
/************************************************************************/

    /**
     * \brief   Reads data from input stream object, copies into given buffer and returns the size
     *          of copied data.
     *
     * \param   buffer      The pointer to buffer to copy data from input object
     * \param   size        The size in bytes of available buffer
     * \return  Returns the size in bytes of copied data.
     **/
    uint32_t read( uint8_t* buffer, uint32_t size ) const noexcept override;

    /**
     * \brief   Reads data from input stream object, copies into given Byte Buffer object and
     *          returns the size of copied data.
     *
     * \param   buffer      The instance of Byte Buffer object to stream data from Input Stream
     *                      object
     * \return  Returns the size in bytes of copied data.
     **/
    uint32_t read(SharedBuffer& buffer ) const override;

    /**
     * \brief   Reads string data from Input Stream object and copies into given ASCII String.
     *
     * \param   ascii       The buffer of ASCII String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    uint32_t read( String & ascii ) const override;

    /**
     * \brief   Reads string data from Input Stream object and copies into given Wide String.
     *
     * \param   wide    The buffer of Wide String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    uint32_t read( WideString & wide ) const override;

/************************************************************************/
// OutStream interface overrides
/************************************************************************/

    /**
     * \brief   Writes data to output stream object from given buffer and returns the size of
     *          written data.
     *
     * \param   buffer      The pointer to buffer to read data and copy to output stream object
     * \param   size        The size in bytes of data buffer
     * \return  Returns the size in bytes of written data.
     **/
    uint32_t write( const uint8_t* buffer, uint32_t size ) override;

    /**
     * \brief   Writes Binary data from Byte Buffer object to Output Stream object and returns the
     *          size of written data.
     *
     * \param   buffer      The instance of Byte Buffer object containing data to stream to Output
     *                      Stream.
     * \return  Returns the size in bytes of written data.
     **/
    uint32_t write( const SharedBuffer& buffer ) override;

    /**
     * \brief   Writes string data from given ASCII String object to output stream object.
     *
     * \param   ascii       The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    uint32_t write( const String & ascii ) override;

    /**
     * \brief   Writes string data from given wide-char String object to output stream object.
     *
     * \param   wide    The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    uint32_t write( const WideString & wide ) override;

    /**
     * \brief   Flushes cached data to output stream object.
     **/
    void flush() noexcept override;

protected:
    /**
     * \brief   Returns size in bytes of available data that can be read, i.e. remaining readable
     *          size. No necessarily that this size is equal to size of streamable buffer. For
     *          example, if the size of buffer is 'n' and 'x' bytes of data was already read from
     *          stream, the available readable size is 'n - x'.
     **/
    [[nodiscard]]
    uint32_t size_readable() const noexcept override;

    /**
     * \brief   Returns size in bytes of available space that can be written, i.e. remaining
     *          writable size. No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was already written
     *          to stream, the available writable size is 'n - x'.
     **/
    [[nodiscard]]
    uint32_t size_writable() const noexcept override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   The type of Event Data. Either internal or external.
     **/
    EventDataStream::EventDataKind mEventDataType;

    /**
     * \brief   The name of Event Data object.
     **/
    String                      mBufferName;

    /**
     * \brief   The Shared Buffer where the data is written / streamed.
     **/
    mutable SharedBuffer        mDataBuffer;

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   FIFO Stack of Shared Buffers. Used only if the data type is internal.
     *          All Shared Buffer objects instead of copying data, will be added to this list.
     **/
    mutable SharedList          mSharedList;

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER
};

AREG_IMPLEMENT_STREAMABLE(EventDataStream::EventDataKind)

//////////////////////////////////////////////////////////////////////////
// EventDataStream inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline bool EventDataStream::is_empty() const noexcept
{
    return (mDataBuffer.is_empty() && mSharedList.is_empty());
}

inline bool EventDataStream::is_external_stream() const noexcept
{
    return (mEventDataType != EventDataStream::EventDataKind::Internal);
}

inline const InStream & EventDataStream::stream_for_read() const noexcept
{
    return static_cast<const InStream &>(*this);
}

inline OutStream & EventDataStream::stream_for_write() noexcept
{
    return static_cast<OutStream &>(*this);
}

inline const InStream & operator >> ( const InStream & stream, EventDataStream & input )
{
    stream >> input.mEventDataType;
    stream >> input.mBufferName;
    stream >> input.mDataBuffer;
    return stream;
}

inline OutStream & operator << ( OutStream & stream, const EventDataStream & output )
{
    ASSERT(output.mEventDataType != EventDataStream::EventDataKind::Internal);
    stream << EventDataStream::EventDataKind::External;
    stream << output.mBufferName;
    stream << output.mDataBuffer;
    return stream;
}

} // namespace areg
#endif  // AREG_COMPONENT_EVENTDATASTREAM_HPP
