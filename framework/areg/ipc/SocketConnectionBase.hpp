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
#include "areg/base/Socket.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class RemoteMessage;
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
    inline int32_t send_message( const RemoteMessage & message, const Socket & socket ) const;

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
    inline int32_t send_messages_batch( const RemoteMessage* const* messages, uint32_t count, const Socket & socket ) const;

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

    inline int32_t send_messages_batch(const areg::IoBuffer* const ioBuffer, uint32_t count, const Socket& socket, uint32_t totalSize = 0) const;

    inline int32_t send_messages_batch(const areg::IoBuffer* const ioBuffer, uint32_t count, SOCKETHANDLE hSocket, uint32_t totalSize = 0) const;

    /**
     * \brief   Receives message data via socket and validates checksum. Blocking operation.
     *
     * \param[out]  message     Buffer to receive data; checksum validated after receiving.
     * \param       socket      A socket for communication (client or server-side accepted socket).
     * \return  Returns bytes received on success; zero if checksum invalid or buffer empty; negative on socket error.
     **/
    int32_t receive_message( RemoteMessage & message, const Socket & socket ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SocketConnectionBase );
};

inline int32_t SocketConnectionBase::send_message(const RemoteMessage& message, const Socket& socket) const
{
    return send_message(message, socket.handle());
}

inline int32_t SocketConnectionBase::send_messages_batch(const areg::IoBuffer* const ioBuffer, uint32_t count, SOCKETHANDLE hSocket, uint32_t totalSize) const
{
    if ((count == 0u) || (ioBuffer == nullptr) || !areg::is_valid_socket(hSocket))
        return 0;
    else if (count == 1)
        return areg::send_data(hSocket, ioBuffer[0].data, static_cast<uint32_t>(ioBuffer[0].size));
    else
        return areg::send_data_v(hSocket, ioBuffer, count, totalSize);
}

inline int32_t SocketConnectionBase::send_messages_batch(const areg::IoBuffer* const ioBuffer, uint32_t count, const Socket& socket, uint32_t totalSize /*= 0*/) const
{
    return send_messages_batch(ioBuffer, count, socket.handle(), totalSize);
}

inline int32_t SocketConnectionBase::send_messages_batch(const RemoteMessage* const* messages, uint32_t count, const Socket& socket) const
{
    return send_messages_batch(messages, count, socket.handle());
}

} // namespace areg
#endif  // AREG_IPC_PRIVATE_SOCKETCONNECTIONBASEE_HPP
