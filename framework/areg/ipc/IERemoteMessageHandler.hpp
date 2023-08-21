#ifndef AREG_IPC_IEREMOTEMESSAGEHANDLER_HPP
#define AREG_IPC_IEREMOTEMESSAGEHANDLER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/IERemoteMessageHandler.hpp
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
class RemoteMessage;
class Socket;

//////////////////////////////////////////////////////////////////////////
// IERemoteMessageHandler interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The interface of object to handler remote servicing.
 *          It contains callbacks, which are triggered either then succeeded to send data
 *          or an error occurred when running.
 **/
class AREG_API IERemoteMessageHandler
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    IERemoteMessageHandler( void ) = default;
    /**
     * \brief   Destructor
     **/
    virtual ~IERemoteMessageHandler( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IERemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     * \param   msgFailed   The message, which failed to send.
     * \param   whichTarget The target socket to send message.
     **/
    virtual void failedSendMessage( const RemoteMessage & msgFailed, Socket & whichTarget ) = 0;

    /**
     * \brief   Triggered, when failed to receive message.
     * \param   whichSource Indicates the failed source socket to receive message.
     **/
    virtual void failedReceiveMessage( Socket & whichSource ) = 0;

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    virtual void failedProcessMessage( const RemoteMessage & msgUnprocessed ) = 0;

    /**
     * \brief   Triggered, when need to process received message.
     * \param   msgReceived Received message to process.
     * \param   whichSource The source socket, which received message.
     **/
    virtual void processReceivedMessage( const RemoteMessage & msgReceived, Socket & whichSource ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IERemoteMessageHandler );
};

#endif  // AREG_IPC_IEREMOTEMESSAGEHANDLER_HPP
