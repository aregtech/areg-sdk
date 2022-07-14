#ifndef AREG_MCROUTER_TCP_PRIVATE_SERVERRECEIVETHREAD_HPP
#define AREG_MCROUTER_TCP_PRIVATE_SERVERRECEIVETHREAD_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/tcp/private/ServerReceiveThread.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Receive Message Thread of Server socket.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEServerConnectionHandler;
class IERemoteServiceHandler;
class ServerConnection;

//////////////////////////////////////////////////////////////////////////
// ServerConnection class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The IPC message receiving thread of server socket.
 **/
class ServerReceiveThread    : public    DispatcherThread
{
    static constexpr uint32_t RETRY_COUNT{ 5 };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes connection, connection servicing and connection handling objects
     * \param   connectHandler  The instance of server socket connect / disconnect handling interface
     * \param   remoteService   The instance of remote servicing handler
     * \param   connection      The instance of server connection object.
     **/
    ServerReceiveThread( IEServerConnectionHandler & connectHandler, IERemoteServiceHandler & remoteService, ServerConnection & connection );
    /**
     * \brief   Destructor
     **/
    virtual ~ServerReceiveThread( void ) = default;

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool runDispatcher( void ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote servicing interface object
     **/
    IERemoteServiceHandler &    mRemoteService;
    /**
     * \brief   The instance of connection handler interface object
     **/
    IEServerConnectionHandler & mConnectHandler;
    /**
     * \brief   The instance of server connection object
     **/
    ServerConnection &          mConnection;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServerReceiveThread( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServerReceiveThread );
};

#endif  // AREG_MCROUTER_TCP_PRIVATE_SERVERRECEIVETHREAD_HPP
