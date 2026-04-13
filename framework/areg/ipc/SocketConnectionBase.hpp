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
    SocketConnectionBase() = default;
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

} // namespace areg
#endif  // AREG_IPC_PRIVATE_SOCKETCONNECTIONBASEE_HPP
