#ifndef AREG_IPC_PRIVATE_CONNECTIONDEFS_HPP
#define AREG_IPC_PRIVATE_CONNECTIONDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/ipc/private/ConnectionDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Connection constants and the send queue gate.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/SocketDefs.hpp"

#include <atomic>
#include <string_view>

//////////////////////////////////////////////////////////////////////////
// Connection namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The connection namespace with shared objects and helper methods
 *          to create fixed communication messages.
 **/
namespace areg {
    /**
     * \brief   areg::CLIENT_SEND_MESSAGE_THREAD
     *          Fixed name of client message sender thread
     **/
    constexpr std::string_view  CLIENT_SEND_MESSAGE_THREAD      { "CLIENT_SEND_MESSAGE_THREAD" };
    /**
     * \brief   areg::CLIENT_RECEIVE_MESSAGE_THREAD
     *          Fixed name of client message receiver thread
     **/
    constexpr std::string_view  CLIENT_RECEIVE_MESSAGE_THREAD   { "CLIENT_RECEIVE_MESSAGE_THREAD" };
    /**
     * \brief   areg::CLIENT_DISPATCH_MESSAGE_THREAD
     *          Fixed name of client message dispatcher thread
     **/
    constexpr std::string_view  CLIENT_DISPATCH_MESSAGE_THREAD  { "CLIENT_DISPATCH_MESSAGE_THREAD" };
    /**
     * \brief   areg::SERVER_SEND_MESSAGE_THREAD
     *          Fixed name of server message sender thread
     **/
    constexpr std::string_view  SERVER_SEND_MESSAGE_THREAD      { "SERVER_SEND_MESSAGE_THREAD" };
    /**
     * \brief   areg::SERVER_RECEIVE_MESSAGE_THREAD
     *          Fixed name of server message receiver thread
     **/
    constexpr std::string_view  SERVER_RECEIVE_MESSAGE_THREAD   { "SERVER_RECEIVE_MESSAGE_THREAD" };
    /**
     * \brief   areg::SERVER_DISPATCH_MESSAGE_THREAD
     *          Fixed name of server message dispatcher thread
     **/
    constexpr std::string_view  SERVER_DISPATCH_MESSAGE_THREAD  { "SERVER_DISPATCH_MESSAGE_THREAD" };
    /**
     * \brief   areg::CLIENT_CONNECT_TIMER_NAME
     *          Fixed name of client connection retry timer name
     **/
    constexpr std::string_view  CLIENT_CONNECT_TIMER_NAME       { "CLIENT_CONNECT_TIMER_NAME" };
    /**
     * \brief   areg::SERVER_CONNECT_TIMER_NAME
     *          Fixed name of server connection retry timer name
     **/
    constexpr std::string_view  SERVER_CONNECT_TIMER_NAME       { "SERVER_CONNECT_TIMER_NAME" };
    /**
     * \brief   areg::DEFAULT_RETRY_CONNECT_TIMEOUT
     *          Default connect retry timer timeout value in milliseconds
     **/
    constexpr uint32_t      DEFAULT_RETRY_CONNECT_TIMEOUT       { TIMEOUT_500_MS };  // 500 ms
    /**
     * \brief   areg::SEND_QUEUE_FLUSH_TIMEOUT
     *          The timeout in milliseconds to wait for a graceful disconnect
     **/
    constexpr uint32_t      SEND_QUEUE_FLUSH_TIMEOUT            { TIMEOUT_1_SEC };   // 1 sec

    /**
     * \brief   The largest message, in bytes on the wire, that a producer thread may write into
     *          the socket itself instead of handing it to the send thread. A bigger message is
     *          always queued, because writing it may have to wait for the reader on the other
     *          side.
     **/
    constexpr uint32_t      INLINE_SEND_MAX_BYTES               { 64u * 1024u };     // 64 KB

//////////////////////////////////////////////////////////////////////////
// areg::SendQueueGate class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The gate of one send queue: it tells whether that queue still owes a message to the
 *          socket. A producer calls enter() before it puts a message into the queue, the send
 *          thread calls leave() after the messages reached the socket. A producer may write a
 *          message into the socket itself only while the gate is clear, otherwise the new
 *          message would overtake an older one.
 *
 * \note    The gate does not replace the writer lock of the socket, see areg::SocketWriter. The
 *          gate says that nothing older is waiting, the writer lock says that nobody else is
 *          writing at this moment. A producer needs both.
 **/
class SendQueueGate
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    constexpr SendQueueGate() noexcept;
    ~SendQueueGate() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Announces one message that is about to be put into the send queue. Call it
     *          before the message is queued, never after.
     **/
    inline void enter() noexcept;

    /**
     * \brief   Announces that the given number of messages left the send queue and reached the
     *          socket. Call it after the write, never before.
     *
     * \param   count   The number of messages taken out of the queue and dealt with.
     **/
    inline void leave(uint32_t count) noexcept;

    /**
     * \brief   Returns true when the send queue owes nothing to the socket, so a producer may
     *          write its message itself. Returns false when at least one message is still on its
     *          way and the producer must queue its message.
     **/
    [[nodiscard]]
    inline bool is_clear() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< The number of messages announced but not yet written.
    std::atomic<uint32_t>   mPending;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SendQueueGate );
};

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// areg::SendQueueGate class inline implementation
//////////////////////////////////////////////////////////////////////////

constexpr areg::SendQueueGate::SendQueueGate() noexcept
    : mPending  ( 0u )
{
}

inline void areg::SendQueueGate::enter() noexcept
{
    mPending.fetch_add(1u, std::memory_order_seq_cst);
}

inline void areg::SendQueueGate::leave(uint32_t count) noexcept
{
    if ( count != 0u )
    {
        mPending.fetch_sub(count, std::memory_order_seq_cst);
    }
}

inline bool areg::SendQueueGate::is_clear() const noexcept
{
    return (mPending.load(std::memory_order_seq_cst) == 0u);
}

#endif  // AREG_IPC_PRIVATE_CONNECTIONDEFS_HPP
