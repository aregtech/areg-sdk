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
#include "areg/base/areg_global.h"
#include "areg/base/SharedBuffer.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// RemoteMessage class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote message is used for IPC communication, which is an
 *          instance of shared buffer. Unlike internal communication message,
 *          the remote message contains additional information to deliver
 *          messages to target application for further processing.
 **/
class AREG_API RemoteMessage final : public SharedBuffer
{
//////////////////////////////////////////////////////////////////////////
// Friend objects
//////////////////////////////////////////////////////////////////////////
    friend class Socket;

//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
    static const ITEM_ID        _INVALID_COOKIE;
    static const SequenceNumber _INVALID_SEQUENCE;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the object with default values and specified or default block size.
     *
     * \param   blockSize       The size of minimum block size to increase on resize. It is aligned
     *                          to areg::BLOCK_SIZE (minimum size)
     **/
    RemoteMessage(uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Initializes the object with default values and specified or default block size.
     * 
     * \param   init        Create RemoteMessage initialized
     * \param   blockSize   The size of minimum block size to increase on resize. It is aligned
     *                      to areg::BLOCK_SIZE (minimum size)
     **/
    explicit RemoteMessage(bool init, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Reserves space for byte buffer object.
     *
     * \param   reserveSize     Size in bytes to reserve
     * \param   blockSize       The size of minimum block size to increase on resize. It is aligned
     *                          to areg::BLOCK_SIZE (minimum size)
     **/
    RemoteMessage( uint32_t reserveSize, uint32_t blockSize );

    /**
     * \brief   Initializes and writes given data into byte buffer.
     *
     * \param   buffer          The data to initialize byte buffer.
     * \param   size            The length in bytes of data.
     * \param   blockSize       The size of minimum block size to increase on resize. It is aligned
     *                          to areg::BLOCK_SIZE (minimum size).
     **/
    RemoteMessage(const uint8_t * buffer, uint32_t size, uint32_t blockSize = areg::BLOCK_SIZE);

    RemoteMessage(const areg::MessageHeader& rmHeader, uint32_t reserve, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Does not make hard copy of data from given source; refers to the
     *          same shared byte buffer object and increases reference counter by one.
     **/
    RemoteMessage( const RemoteMessage & src ) = default;

    RemoteMessage( RemoteMessage && src ) noexcept = default;

    virtual ~RemoteMessage() noexcept = default;

//////////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Does not copy source data; increases byte buffer reference counter by one.
     **/
    RemoteMessage & operator = ( const RemoteMessage & src ) = default;

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
    [[nodiscard]]
    inline const areg::RawMessage * remote_message() const noexcept;

    /**
     * \brief   Returns checksum value of Remote Message. The value is calculated by calling
     *          buffer_completion_fix().
     **/
    [[nodiscard]]
    inline uint32_t checksum() const noexcept;

    /**
     * \brief   Returns the ID of remote source set in Remote Message header.
     **/
    [[nodiscard]]
    inline const ITEM_ID & source() const noexcept;

    /**
     * \brief   Sets the ID of source in Remote Message.
     *
     * \param   idSource    The ID of source to set in Remote Message
     **/
    inline void set_source(const ITEM_ID & idSource) noexcept;

    /**
     * \brief   Returns the ID of remote target set in Remote Message header.
     **/
    [[nodiscard]]
    inline const ITEM_ID & target() const noexcept;

    /**
     * \brief   Sets the ID of target in Remote Message.
     *
     * \param   idTarget    The ID of target to set in Remote Message
     **/
    inline void set_target(const ITEM_ID & idTarget) noexcept;

    /**
     * \brief   Returns the message ID value set in remote message.
     **/
    [[nodiscard]]
    inline uint32_t message_id() const noexcept;

    /**
     * \brief   Sets new message ID value in Remote Message.
     *
     * \param   newMessageId    New Message ID value to set in Remote Message
     **/
    inline void set_message_id( uint32_t newMessageId ) noexcept;

    /**
     * \brief   Returns result of processed message.
     **/
    [[nodiscard]]
    inline uint32_t result() const noexcept;

    /**
     * \brief   Sets result of processed message.
     *
     * \param   newResult       The result value to set
     **/
    inline void set_result( uint32_t newResult ) noexcept;

    /**
     * \brief   Returns Sequence number value set in Remote Message.
     **/
    [[nodiscard]]
    inline const SequenceNumber & sequence() const noexcept;

    /**
     * \brief   Sets new Sequence number value in Remote Message.
     *
     * \param   newSequenceNr       New Sequence number value to set in Remote Message
     **/
    inline void set_sequence(const SequenceNumber & newSequenceNr ) noexcept;

    /**
     * \brief   Returns true if marked checksum value is valid; false otherwise.
     **/
    [[nodiscard]]
    inline bool is_checksum_valid() const noexcept;

    /**
     * \brief   Returns true if checksum value is set to be ignored; false otherwise.
     **/
    [[nodiscard]]
    inline bool is_checksum_ignore() const noexcept;

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
    uint8_t * init_message( const areg::MessageHeader & rmHeader, uint32_t reserve = 0 );

    /**
     * \brief   Creates a copy of the message buffer with independent data, optionally setting
     *          source and target IDs.
     *
     * \param   source      The ID of the source to set. Ignored if 0
     * \param   target      The ID of the target to set. Ignored if 0
     * \return  Returns the cloned message buffer.
     **/
    [[nodiscard]]
    RemoteMessage clone(const ITEM_ID & source = 0, const ITEM_ID & target = 0) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// SharedBuffer Attributes and operations
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
    uint32_t init_buffer(uint8_t * newBuffer, uint32_t bufLength, bool makeCopy) const noexcept final;

    /**
     * \brief   Returns the offset value from the beginning of byte buffer.
     **/
    [[nodiscard]]
    inline uint32_t data_offset() const noexcept final;

    /**
     * \brief   Returns the size of message header structure to allocate.
     **/
    [[nodiscard]]
    inline uint32_t header_size() const noexcept final;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns const reference to message header.
     **/
    [[nodiscard]]
    inline const areg::MessageHeader & _header() const;
    /**
     * \brief   Returns mutable reference to message header. Allows modification of the returned
     *          value.
     * \note    Overload. Const variant returns const reference.
     **/
    [[nodiscard]]
    inline areg::MessageHeader & _header();

    /**
     * \brief   Returns const reference to raw message structure.
     **/
    [[nodiscard]]
    inline const areg::RawMessage & _raw_message() const noexcept;
    /**
     * \brief   Returns mutable reference to raw message structure. Allows modification of the
     *          returned value.
     * \note    Overload. Const variant returns const reference.
     **/
    [[nodiscard]]
    inline areg::RawMessage & _raw_message() noexcept;

    /**
     * \brief   Calculates and returns the checksum value of given remote message.
     *
     * \param   remoteMessage       The remote message to calculate checksum for
     **/
    [[nodiscard]]
    static uint32_t _checksum_calculate( const areg::RawMessage & remoteMessage ) noexcept;
};

//////////////////////////////////////////////////////////////////////////
// RemoteMessage class inline functions
//////////////////////////////////////////////////////////////////////////
inline const areg::MessageHeader & RemoteMessage::_header() const
{
    return reinterpret_cast<const areg::MessageHeader &>(*byte_buffer());
}

inline areg::MessageHeader & RemoteMessage::_header()
{
    ASSERT(mByteBuffer.get() != nullptr);
    return reinterpret_cast<areg::MessageHeader &>(*(mByteBuffer.get()));
}

inline const areg::RawMessage & RemoteMessage::_raw_message() const noexcept
{
    ASSERT(mByteBuffer.get() != nullptr);
    return reinterpret_cast<const areg::RawMessage&>(*mByteBuffer.get());
}

inline areg::RawMessage & RemoteMessage::_raw_message() noexcept
{
    ASSERT( mByteBuffer.get( ) != nullptr );
    return reinterpret_cast<areg::RawMessage &>(*mByteBuffer.get());
}

inline const areg::RawMessage * RemoteMessage::remote_message() const noexcept
{
    return reinterpret_cast<const areg::RawMessage *>(mByteBuffer.get());
}

inline uint32_t RemoteMessage::checksum() const noexcept
{
    const areg::RawMessage* msg{ remote_message() };
    return (msg != nullptr ? msg->rbHeader.rbhChecksum : areg::CHECKSUM_INVALID);
}

inline const ITEM_ID & RemoteMessage::source() const noexcept
{
    const areg::RawMessage* msg{ remote_message() };
    return (msg != nullptr ? msg->rbHeader.rbhSource : RemoteMessage::_INVALID_COOKIE);
}

inline void RemoteMessage::set_source(const ITEM_ID & idSource ) noexcept
{
    areg::RawMessage* msg{ reinterpret_cast<areg::RawMessage*>(byte_buffer()) };
    if (msg != nullptr)
    {
        msg->rbHeader.rbhSource = idSource;
    }
}

inline const ITEM_ID & RemoteMessage::target() const noexcept
{
    const areg::RawMessage* msg{ remote_message() };
    return (msg != nullptr ? msg->rbHeader.rbhTarget : RemoteMessage::_INVALID_COOKIE);
}

inline void RemoteMessage::set_target(const ITEM_ID & idTarget ) noexcept
{
    areg::RawMessage* msg{ reinterpret_cast<areg::RawMessage*>(byte_buffer()) };
    if (msg != nullptr)
    {
        msg->rbHeader.rbhTarget = idTarget;
    }
}

inline uint32_t RemoteMessage::message_id() const noexcept
{
    const areg::RawMessage* msg{ remote_message() };
    return (msg != nullptr ? msg->rbHeader.rbhMessageId : areg::INVALID_MESSAGE_ID);
}

inline void RemoteMessage::set_message_id( uint32_t newMessageId ) noexcept
{
    areg::RawMessage* msg{ reinterpret_cast<areg::RawMessage*>(byte_buffer()) };
    if (msg != nullptr)
    {
        msg->rbHeader.rbhMessageId = newMessageId;
    }
}
inline uint32_t RemoteMessage::result() const noexcept
{
    const areg::RawMessage* msg{ remote_message() };
    return (msg != nullptr ? msg->rbHeader.rbhResult : areg::INVALID_VALUE);
}

inline void RemoteMessage::set_result( uint32_t newResult ) noexcept
{
    areg::RawMessage* msg{ reinterpret_cast<areg::RawMessage*>(byte_buffer()) };
    if (msg != nullptr)
    {
        msg->rbHeader.rbhResult = newResult;
    }
}

inline const SequenceNumber & RemoteMessage::sequence() const noexcept
{
    const areg::RawMessage* msg{ remote_message() };
    return (msg != nullptr ? msg->rbHeader.rbhSequenceNr : RemoteMessage::_INVALID_SEQUENCE);
}

inline void RemoteMessage::set_sequence(const SequenceNumber & newSequenceNr ) noexcept
{
    areg::RawMessage* msg{ reinterpret_cast<areg::RawMessage*>(byte_buffer()) };
    if (msg != nullptr)
    {
        msg->rbHeader.rbhSequenceNr = newSequenceNr;
    }
}

inline bool RemoteMessage::is_checksum_valid() const noexcept
{
    const areg::RawMessage* msg{ remote_message() };
    return (msg != nullptr) && (msg->rbHeader.rbhChecksum != areg::CHECKSUM_INVALID);
}

inline bool RemoteMessage::is_checksum_ignore() const noexcept
{
    return checksum() == areg::CHECKSUM_IGNORE;
}

inline uint32_t RemoteMessage::data_offset() const noexcept
{
    return offsetof(areg::RawMessage, rbData);
}

inline uint32_t RemoteMessage::header_size() const noexcept
{
    return sizeof(areg::RawMessage);
}

/************************************************************************/
// Friend streamable operators
/************************************************************************/

inline const InStream & operator >> (const InStream & stream, RemoteMessage & input)
{
    if ( static_cast<const InStream *>(&stream) == static_cast<const InStream *>(&input) )
        return stream;

    areg::MessageHeader msgHdr{};
    if (stream.read(reinterpret_cast<uint8_t *>(&msgHdr), sizeof(areg::MessageHeader)) != sizeof(areg::MessageHeader))
        return stream;

    uint8_t* dst = input.init_message(msgHdr);
    if (dst != nullptr)
    {
        uint32_t read = stream.read(dst, msgHdr.rbhBufHeader.biUsed);
        input.set_size_used(read);
        input.move_to_begin();
    }

    return stream;
}

inline OutStream & operator << (OutStream & stream, const RemoteMessage & output)
{
    if ( (static_cast<const OutStream *>(&stream)) == (static_cast<const OutStream *>(&output)) )
        return stream;


    const areg::RawMessage* msg = reinterpret_cast<const areg::RawMessage*>(output.mByteBuffer.get());
    if (msg != nullptr)
    {
        stream.write(reinterpret_cast<const uint8_t*>(&msg->rbHeader), sizeof(areg::MessageHeader));
        stream.write(msg->rbData, msg->rbHeader.rbhBufHeader.biUsed);
    }
    else
    {
        areg::MessageHeader hdr{};
        stream.write(reinterpret_cast<const uint8_t*>(&hdr), sizeof(areg::MessageHeader));
    }

    return stream;
}

} // namespace areg

#endif  // AREG_BASE_REMOTEMESSAGE_HPP
