#ifndef AREG_BASE_REMOTEMESSAGE_HPP
#define AREG_BASE_REMOTEMESSAGE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/RemoteMessage.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Remote Shared Buffer class. This Buffer is used for 
 *              Read and Write of data during remote communication
 *              between connected nodes.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/SharedBuffer.hpp"

//////////////////////////////////////////////////////////////////////////
// RemoteMessage class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote message is used for IPC communication, which is an
 *          instance of shared buffer. Unlike internal communication message,
 *          the remote message contains additional information to deliver
 *          messages to target application for further processing.
 **/
class AREG_API RemoteMessage  : public    SharedBuffer
{
//////////////////////////////////////////////////////////////////////////
// Friend objects
//////////////////////////////////////////////////////////////////////////
    friend class Socket;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the object with default values and specified or default block size.
     *
     * \param   blockSize       The size of minimum block size to increase on resize. It is aligned
     *                          to NEMemory::BLOCK_SIZE (minimum size)
     **/
    explicit RemoteMessage(uint32_t blockSize = NEMemory::BLOCK_SIZE);

    /**
     * \brief   Reserves space for byte buffer object.
     *
     * \param   reserveSize     Size in bytes to reserve
     * \param   blockSize       The size of minimum block size to increase on resize. It is aligned
     *                          to NEMemory::BLOCK_SIZE (minimum size)
     **/
    RemoteMessage( uint32_t reserveSize, uint32_t blockSize );

    /**
     * \brief   Initializes and writes given data into byte buffer.
     *
     * \param   buffer          The data to initialize byte buffer.
     * \param   size            The length in bytes of data.
     * \param   blockSize       The size of minimum block size to increase on resize. It is aligned
     *                          to NEMemory::BLOCK_SIZE (minimum size).
     **/
    RemoteMessage(const uint8_t * buffer, uint32_t size, uint32_t blockSize = NEMemory::BLOCK_SIZE);

    /**
     * \brief   Copy constructor. Does not make hard copy of data from given source; refers to the
     *          same shared byte buffer object and increases reference counter by one.
     *
     * \param   src     The source of shared buffer object instance.
     **/
    RemoteMessage( const RemoteMessage & src ) = default;

    /**
     * \brief   Move constructor. Moves data from given source.
     *
     * \param   src     The source of data.
     **/
    RemoteMessage( RemoteMessage && src ) noexcept = default;

    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteMessage() = default;

//////////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copy assignment operator. Does not copy source data; increases byte buffer reference
     *          counter by one.
     *
     * \param   src     Reference to source remote buffer data.
     **/
    RemoteMessage & operator = ( const RemoteMessage & src ) = default;

    /**
     * \brief   Move assignment operator. Moves data from given source.
     *
     * \param   src     The source of data.
     **/
    RemoteMessage & operator = ( RemoteMessage && src ) noexcept = default;

    /************************************************************************/
// Friend global operators to stream Shared Buffer
/************************************************************************/

    /**
     * \brief   Extracts data from streaming object and writes it to remote message.
     *
     * \param   stream      The streaming object to read data
     * \param[in,out] input       The Remote Message object to write data
     * \return  Reference to streaming object.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, RemoteMessage & input );

    /**
     * \brief   Writes data from remote message to streaming object.
     *
     * \param[in,out] stream      The streaming object to write data
     * \param   output      The Remote Message object to read data
     * \return  Reference to streaming object.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const RemoteMessage & output );

//////////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns remote message structure data.
     **/
    inline const NEMemory::RawMessage * remote_message() const;

    /**
     * \brief   Returns checksum value of Remote Message. The value is calculated by calling
     *          buffer_completion_fix().
     **/
    inline uint32_t checksum() const;

    /**
     * \brief   Returns the ID of remote source set in Remote Message header.
     **/
    inline const ITEM_ID & source() const;

    /**
     * \brief   Sets the ID of source in Remote Message.
     *
     * \param   idSource    The ID of source to set in Remote Message
     **/
    inline void set_source(const ITEM_ID & idSource);

    /**
     * \brief   Returns the ID of remote target set in Remote Message header.
     **/
    inline const ITEM_ID & target() const;

    /**
     * \brief   Sets the ID of target in Remote Message.
     *
     * \param   idTarget    The ID of target to set in Remote Message
     **/
    inline void set_target(const ITEM_ID & idTarget);

    /**
     * \brief   Returns the message ID value set in remote message.
     **/
    inline uint32_t message_id() const;

    /**
     * \brief   Sets new message ID value in Remote Message.
     *
     * \param   newMessageId    New Message ID value to set in Remote Message
     **/
    inline void set_message_id( uint32_t newMessageId );

    /**
     * \brief   Returns result of processed message.
     **/
    inline uint32_t result() const;

    /**
     * \brief   Sets result of processed message.
     *
     * \param   newResult       The result value to set
     **/
    inline void set_result( uint32_t newResult );

    /**
     * \brief   Returns Sequence number value set in Remote Message.
     **/
    inline const SequenceNumber & sequence_nr() const;

    /**
     * \brief   Sets new Sequence number value in Remote Message.
     *
     * \param   newSequenceNr       New Sequence number value to set in Remote Message
     **/
    inline void set_sequence_nr(const SequenceNumber & newSequenceNr );

    /**
     * \brief   Returns true if marked checksum value is valid; false otherwise.
     **/
    bool is_checksum_valid() const;

    /**
     * \brief   Completes the buffer by fixing length and checksum. Call when done modifying buffer
     *          and before transferring to remote target.
     **/
    void buffer_completion_fix() const;

    /**
     * \brief   Initializes new buffer based on given Message Header data. Returns pointer to
     *          allocated buffer for manual data copy.
     *
     * \param   rmHeader    Instance of Message Header containing buffer information.
     * \param   reserve     The size in bytes to reserve in the buffer
     * \return  Returns pointer to allocated data buffer to copy data.
     **/
    uint8_t * init_message( const NEMemory::MessageHeader & rmHeader, uint32_t reserve = 0 );

    /**
     * \brief   Creates a copy of the message buffer with independent data, optionally setting
     *          source and target IDs.
     *
     * \param   source      The ID of the source to set. Ignored if 0
     * \param   target      The ID of the target to set. Ignored if 0
     * \return  Returns the cloned message buffer.
     **/
    RemoteMessage clone(const ITEM_ID & source = 0, const ITEM_ID & target = 0) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// ByteBuffer Attributes and operations
/************************************************************************/
    /**
     * \brief   Initializes passed buffer as byte buffer and returns the current writing position.
     *          Optionally copies existing data.
     *
     * \param[in,out] newBuffer       The buffer to initialize.
     * \param   bufLength       The length in bytes of the entire buffer object.
     * \param   makeCopy        If 'true' it will make copy of existing buffer
     * \return  Returns the current writing position in initialized buffer. If buffer is invalid,
     *          returns INVALID_CURSOR_POSITION.
     **/
    uint32_t init_buffer(uint8_t * newBuffer, uint32_t bufLength, bool makeCopy) const override;

    /**
     * \brief   Returns the offset value from the beginning of byte buffer.
     **/
    uint32_t data_offset() const override;

    /**
     * \brief   Returns the size of message header structure to allocate.
     **/
    uint32_t header_size() const override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns const reference to message header.
     **/
    inline const NEMemory::MessageHeader & _header() const;
    /**
     * \brief   Returns mutable reference to message header. Allows modification of the returned
     *          value.
     * \note    Overload. Const variant returns const reference.
     **/
    inline NEMemory::MessageHeader & _header();

    /**
     * \brief   Returns const reference to raw message structure.
     **/
    inline const NEMemory::RawMessage & _remote_message() const;
    /**
     * \brief   Returns mutable reference to raw message structure. Allows modification of the
     *          returned value.
     * \note    Overload. Const variant returns const reference.
     **/
    inline NEMemory::RawMessage & _remote_message();

    /**
     * \brief   Calculates and returns the checksum value of given remote message.
     *
     * \param   remoteMessage       The remote message to calculate checksum for
     **/
    static uint32_t _checksum_calculate( const NEMemory::RawMessage & remoteMessage );
};

//////////////////////////////////////////////////////////////////////////
// RemoteMessage class inline functions
//////////////////////////////////////////////////////////////////////////
inline const NEMemory::MessageHeader & RemoteMessage::_header() const
{
    return reinterpret_cast<const NEMemory::MessageHeader &>(*byte_buffer());
}

inline NEMemory::MessageHeader & RemoteMessage::_header()
{
    ASSERT(mByteBuffer.get() != nullptr);
    return reinterpret_cast<NEMemory::MessageHeader &>(*(mByteBuffer.get()));
}

inline const NEMemory::RawMessage & RemoteMessage::_remote_message() const
{
    return reinterpret_cast<const NEMemory::RawMessage &>(*byte_buffer());
}

inline NEMemory::RawMessage & RemoteMessage::_remote_message()
{
    ASSERT( mByteBuffer.get( ) != nullptr );
    return reinterpret_cast<NEMemory::RawMessage &>(*mByteBuffer.get());
}

inline const NEMemory::RawMessage * RemoteMessage::remote_message() const
{
    return reinterpret_cast<const NEMemory::RawMessage *>(byte_buffer());
}

inline uint32_t RemoteMessage::checksum() const
{
    return _header().rbhChecksum;
}

inline const ITEM_ID & RemoteMessage::source() const
{
    return _header().rbhSource;
}

inline void RemoteMessage::set_source(const ITEM_ID & idSource )
{
    if (is_valid())
    {
        _header().rbhSource = idSource;
    }
}

inline const ITEM_ID & RemoteMessage::target() const
{
    return _header().rbhTarget;
}

inline void RemoteMessage::set_target(const ITEM_ID & idTarget )
{
    if (is_valid())
    {
        _header().rbhTarget = idTarget;
    }
}

inline uint32_t RemoteMessage::message_id() const
{
    return _header().rbhMessageId;
}

inline void RemoteMessage::set_message_id( uint32_t newMessageId )
{
    if (is_valid())
    {
        _header().rbhMessageId = newMessageId;
    }
}

inline uint32_t RemoteMessage::result() const
{
    return _header().rbhResult;
}

inline void RemoteMessage::set_result( uint32_t newResult )
{
    if (is_valid())
    {
        _header().rbhResult = newResult;
    }
}

inline const SequenceNumber & RemoteMessage::sequence_nr() const
{
    return _header().rbhSequenceNr;
}

inline void RemoteMessage::set_sequence_nr(const SequenceNumber & newSequenceNr )
{
    if ( is_valid() )
    {
        _header().rbhSequenceNr = newSequenceNr;
    }
}

/************************************************************************/
// Friend streamable operators
/************************************************************************/

inline const InStream & operator >> (const InStream & stream, RemoteMessage & input)
{
    if ( static_cast<const InStream *>(&stream) != static_cast<const InStream *>(&input) )
    {
        stream.read(input);
        input.move_to_begin();
    }

    return stream;
}

inline OutStream & operator << (OutStream & stream, const RemoteMessage & output)
{
    if ( (static_cast<const OutStream *>(&stream)) != (static_cast<const OutStream *>(&output)) )
    {
        stream.write( output );
    }
    
    return stream;
}

#endif  // AREG_BASE_REMOTEMESSAGE_HPP
