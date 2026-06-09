#ifndef AREG_IPC_REMOTEMESSAGEHANDLER_HPP
#define AREG_IPC_REMOTEMESSAGEHANDLER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/RemoteMessageHandler.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Interface of remote Service Handler
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class MessageEnvelope;
    class Socket;
}

namespace areg {

//////////////////////////////////////////////////////////////////////////
// RemoteMessageHandler interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface for handling remote service operations with callbacks for send/receive success and failure.
 **/
class AREG_API RemoteMessageHandler
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    RemoteMessageHandler() = default;

    virtual ~RemoteMessageHandler() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// RemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when message send fails.
     *
     * \param   msgFailed       The envelope that failed to send.
     * \param   whichTarget     The target socket.
     **/
    virtual void failed_send_message( const MessageEnvelope & msgFailed, Socket & whichTarget ) = 0;

    /**
     * \brief   Triggered when message receive fails.
     *
     * \param   whichSource     The source socket from which receive failed.
     **/
    virtual void failed_receive_message( Socket & whichSource ) = 0;

    /**
     * \brief   Triggered when message processing fails because target is not found. For request
     *          messages, source receives error notification.
     *
     * \param   msgUnprocessed      The unprocessed envelope.
     **/
    virtual void failed_process_message( const MessageEnvelope & msgUnprocessed ) = 0;

    /**
     * \brief   Triggered to process a received message.
     *          The callee may move from \a msgReceived when forwarding the
     *          payload to another thread (e.g. the send thread), avoiding an
     *          unnecessary shared-buffer reference-count round-trip.
     *
     * \param   msgReceived     The received envelope to process.
     * \param   whichSource     The source socket from which message was received.
     **/
    virtual void process_received_message( MessageEnvelope & msgReceived, Socket & whichSource ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( RemoteMessageHandler );
};

} // namespace areg
#endif  // AREG_IPC_REMOTEMESSAGEHANDLER_HPP
