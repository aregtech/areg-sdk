#ifndef AREG_IPC_PRIVATE_SOCKETCONNECTIONBASEE_HPP
#define AREG_IPC_PRIVATE_SOCKETCONNECTIONBASEE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/SocketConnectionBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Connection Base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SocketDefs.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class RemoteMessage;
    class Socket;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SocketConnectionBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for socket-based client and server connection send and receive operations.
 **/
class AREG_API SocketConnectionBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    SocketConnectionBase() noexcept;
    virtual ~SocketConnectionBase() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Controls whether MSG_ZEROCOPY is attempted on the next create_socket()
     *          call (Linux 4.14+ only). Defaults to false — safe on WSL2.
     *          Even when true, zerocopy is active only if the kernel accepts SO_ZEROCOPY.
     *
     * \param   enable  Pass true to attempt MSG_ZEROCOPY; false to skip.
     **/
    inline void set_zerocopy_wanted(bool enable) noexcept;

    /**
     * \brief   Returns true if MSG_ZEROCOPY was successfully enabled on the current
     *          socket (Linux 4.14+ only). Always false on other platforms and before
     *          create_socket() is called.
     **/
    [[nodiscard]]
    inline bool is_zerocopy_enabled() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Sends message data via socket after validating checksum. Blocking operation.
     *
     * \param[in]   message     Buffer to send; checksum will be validated before sending.
     * \param       socket      A socket for communication (client or server-side accepted socket).
     * \return  Returns bytes sent on success; zero if checksum invalid or buffer empty; negative on socket error.
     **/
    int32_t send_message( const RemoteMessage & message, const Socket & socket ) const;

    /**
     * \brief   Sends multiple messages to the same socket in a single syscall using
     *          scatter/gather I/O (writev on POSIX, WSASend on Windows). All messages
     *          must target the same socket.
     *
     *          buffer_completion_fix() is called on each message before sending.
     *          Messages that are not valid (is_valid() returns false) are skipped.
     *
     * \param   messages    Array of pointers to messages to send.
     * \param   count       Number of entries in the array.
     * \param   socket      Target socket; must be valid.
     * \return  Total bytes sent on success; negative if the syscall fails.
     **/
    int32_t send_messages_batch( const RemoteMessage* const* messages, uint32_t count, const Socket & socket ) const;

    /**
     * \brief   Sends message data using a raw socket handle. Avoids constructing a Socket wrapper
     *          in the hot path. Semantically equivalent to send_message(message, Socket(hSocket)).
     *
     * \param   message     Buffer to send; checksum will be validated before sending.
     * \param   hSocket     Raw OS socket handle; must be valid.
     * \return  Returns bytes sent on success; zero if checksum invalid or buffer empty; negative on error.
     **/
    int32_t send_message( const RemoteMessage & message, SOCKETHANDLE hSocket ) const;

    /**
     * \brief   Sends multiple messages to the same socket handle in a single syscall.
     *          Semantically equivalent to send_messages_batch(messages, count, Socket(hSocket)).
     *
     * \param   messages    Array of pointers to messages to send.
     * \param   count       Number of entries in the array.
     * \param   hSocket     Raw OS socket handle; must be valid.
     * \return  Total bytes sent on success; negative if the syscall fails.
     **/
    int32_t send_messages_batch( const RemoteMessage* const* messages, uint32_t count, SOCKETHANDLE hSocket ) const;

    /**
     * \brief   Receives message data via socket and validates checksum. Blocking operation.
     *
     * \param[out]  message     Buffer to receive data; checksum validated after receiving.
     * \param       socket      A socket for communication (client or server-side accepted socket).
     * \return  Returns bytes received on success; zero if checksum invalid or buffer empty; negative on socket error.
     **/
    int32_t receive_message( RemoteMessage & message, const Socket & socket ) const;

#if defined(__linux__)
    /**
     * \brief   Sends a message via MSG_ZEROCOPY on the given socket handle.
     *          The caller must keep the message buffer alive until the kernel confirms
     *          the corresponding zerocopy sequence ID via ERRQUEUE.
     *
     * \param   message     Pre-built message; must be valid.
     * \param   hSocket     Raw OS socket handle; must be valid and have SO_ZEROCOPY set.
     * \return  Total bytes sent on success; 0 if message not valid; -1 on hard error.
     **/
    int32_t send_message_zerocopy( const RemoteMessage & message, SOCKETHANDLE hSocket ) const;
#endif  // defined(__linux__)

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   True if the caller requested MSG_ZEROCOPY (set via set_zerocopy_wanted()).
     *          Even when true, zerocopy is only active if mZerocopyEnabled is also true.
     *          Defaults to false — safe on WSL2 where SO_ZEROCOPY gives no benefit.
     **/
    bool    mZerocopyWanted;

    /**
     * \brief   True when SO_ZEROCOPY was successfully set on the socket (Linux 4.14+).
     *          Set by the derived class in create_socket(); cleared in close_socket().
     *          Always false on non-Linux platforms.
     **/
    bool    mZerocopyEnabled;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SocketConnectionBase );
};

//////////////////////////////////////////////////////////////////////////
// SocketConnectionBase inline methods
//////////////////////////////////////////////////////////////////////////

inline void SocketConnectionBase::set_zerocopy_wanted(bool enable) noexcept
{
    mZerocopyWanted = enable;
}

inline bool SocketConnectionBase::is_zerocopy_enabled() const noexcept
{
    return mZerocopyEnabled;
}

} // namespace areg
#endif  // AREG_IPC_PRIVATE_SOCKETCONNECTIONBASEE_HPP
