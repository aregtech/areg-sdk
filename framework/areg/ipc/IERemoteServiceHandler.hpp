#ifndef AREG_IPC_IEREMOTESERVICEHANDLER_HPP
#define AREG_IPC_IEREMOTESERVICEHANDLER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/ipc/IERemoteServiceHandler.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of remote Service Handler
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace NESocket{
    class SocketAddress;
}
class RemoteMessage;

//////////////////////////////////////////////////////////////////////////
// IERemoteServiceHandler interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The interface of object to handler remote servicing.
 *          It contains callbacks, which are triggered either then succeeded to send data
 *          or an error occurred when running.
 **/
class AREG_API IERemoteServiceHandler
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    IERemoteServiceHandler( void ) = default;
    /**
     * \brief   Destructor
     **/
    virtual ~IERemoteServiceHandler( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IERemoteServiceHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     * \param   msgFailed   The message, which failed to send.
     **/
    virtual void failedSendMessage( const RemoteMessage & msgFailed ) = 0;

    /**
     * \brief   Triggered, when failed to receive message.
     * \param   whichSource Indicated the source, which failed.
     **/
    virtual void failedReceiveMessage( SOCKETHANDLE whichSource ) = 0;

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    virtual void failedProcessMessage( const RemoteMessage & msgUnprocessed ) = 0;

    /**
     * \brief   Triggered, when need to process received message.
     * \param   msgReceived Received message to process.
     * \param   addrHost    The address of remote host, which sent message.
     * \param   whichSource The socket handle, which received message.
     **/
    virtual void processReceivedMessage( const RemoteMessage & msgReceived, const NESocket::SocketAddress & addrHost, SOCKETHANDLE whichSource ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IERemoteServiceHandler );
};

#endif  // AREG_IPC_IEREMOTESERVICEHANDLER_HPP
