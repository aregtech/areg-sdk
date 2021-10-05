#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/tcp/private/IEServerConnectionHandler.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of server connection handler
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
// IEServerConnectionHandler interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The interface of object to handler server side connection.
 *          It contains callbacks when needs to check client connection
 *          or when connection is lost.
 **/
class IEServerConnectionHandler
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    IEServerConnectionHandler( void ) = default;
    /**
     * \brief   Destructor
     **/
    virtual ~IEServerConnectionHandler( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEServerConnectionHandler interface overrides
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

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEServerConnectionHandler );
};
