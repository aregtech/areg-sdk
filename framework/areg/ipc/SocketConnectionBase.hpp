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
    class MessageEnvelope;
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
     * \brief   Sends multiple messages to the same socket handle in a single syscall.
     *
     * \param   ioBuffer    Array of pointers to the buffers to send.
     * \param   count       Number of entries in the array.
     * \param   socket      Target socket; must be valid.
     * \param   totalSize   The total size of data in the `ioBuffer` to send. If 0, it is calculated.
     * \return  Total bytes sent on success; negative if the syscall fails.
     **/
    inline int32_t send_messages_batch(const areg::IoBuffer* const ioBuffer, uint32_t count, const Socket& socket, uint32_t totalSize = 0) const;

    /**
     * \brief   Sends multiple messages to the same socket handle in a single syscall.
     *          Semantically equivalent to send_messages_batch(ioBuffer, count, Socket(hSocket), totalSize).
     *
     * \param   ioBuffer    Array of pointers to the buffers to send.
     * \param   count       Number of entries in the array.
     * \param   hSocket     Raw OS socket handle; must be valid.
     * \param   totalSize   The total size of data in the `ioBuffer` to send. If 0, it is calculated.
     * \return  Total bytes sent on success; negative if the syscall fails.
     **/
    inline int32_t send_messages_batch(const areg::IoBuffer* const ioBuffer, uint32_t count, SOCKETHANDLE hSocket, uint32_t totalSize = 0) const;

    /**
     * \brief   Receives a message by reading EventHeader first, then payload.
     *
     * \param[out]  message     MessageEnvelope to populate; checksum validated after receiving.
     * \param       socket      A socket for communication (client or server-side accepted socket). Must be valid.
     * \return  Returns bytes received on success; zero if checksum invalid or buffer empty; negative on socket error.
     **/
    int32_t receive_message( MessageEnvelope & message, const Socket & socket ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SocketConnectionBase );
};

inline int32_t SocketConnectionBase::send_messages_batch(const areg::IoBuffer* const ioBuffer, uint32_t count, SOCKETHANDLE hSocket, uint32_t totalSize) const
{
    return areg::send_data_v(hSocket, ioBuffer, count, totalSize);
}

inline int32_t SocketConnectionBase::send_messages_batch(const areg::IoBuffer* const ioBuffer, uint32_t count, const Socket& socket, uint32_t totalSize /*= 0*/) const
{
    return areg::send_data_v(socket.handle(), ioBuffer, count, totalSize);
}

} // namespace areg
#endif  // AREG_IPC_PRIVATE_SOCKETCONNECTIONBASEE_HPP
