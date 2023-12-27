#ifndef AREG_EXTEND_SERVICE_IESERVICECONNECTIONHANDLER_HPP
#define AREG_EXTEND_SERVICE_IESERVICECONNECTIONHANDLER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/service/IEServiceConnectionHandler.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of service connection handler
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class SocketAccepted;

//////////////////////////////////////////////////////////////////////////
// IEServiceConnectionHandler interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The interface of object to handler server side connection.
 *          It contains callbacks when needs to check client connection
 *          or when connection is lost.
 **/
class IEServiceConnectionHandler
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    IEServiceConnectionHandler( void ) = default;
    /**
     * \brief   Destructor
     **/
    virtual ~IEServiceConnectionHandler( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEServiceConnectionHandler interface overrides
/************************************************************************/

    /**
     * \brief   Call to check whether new client connection should be accepted
     *          or rejected. Once client is accepted, server will start to receive
     *          messages from client. Otherwise, connection with client is immediately
     *          closed and communication is stopped.
     * \param   clientSocket    Accepted client socket object to check.
     * \return  Returns true if client connection can be accepted. To reject and close
     *          connection with client, the method should return false.
     **/
    virtual bool canAcceptConnection( const SocketAccepted & clientSocket ) = 0;

    /**
     * \brief   Triggered, when lost connection with client.
     *          Passed clientSocket parameter specifies client socket, which lost connection.
     * \param   clientSocket    Client socket object, which lost connection.
     **/
    virtual void connectionLost( SocketAccepted & clientSocket ) = 0;

    /**
     * \brief   Triggered, when there is a connection failure. Normally, this should restart the connection.
     **/
    virtual void connectionFailure( void ) = 0;

    /**
     * \brief   Called when need to disconnect and unregister all service providers and service consumers.
     **/
    virtual void disconnectServices( void ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEServiceConnectionHandler );
};

#endif  // AREG_EXTEND_SERVICE_IESERVICECONNECTIONHANDLER_HPP
