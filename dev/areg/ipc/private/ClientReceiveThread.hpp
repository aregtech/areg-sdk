#ifndef AREG_IPC_PRIVATE_CLIENTRECEIVETHREAD_HPP
#define AREG_IPC_PRIVATE_CLIENTRECEIVETHREAD_HPP

/************************************************************************
 * \file        areg/ipc/private/ClientReceiveThread.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Receive Message Thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/private/RemoteServiceEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IERemoteServiceHandler;
class ClientConnection;

//////////////////////////////////////////////////////////////////////////
// ClientConnection class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The message receiver thread from remote routing service.
 *          All received messages are passed to receiver thread for further dispatching
 *          and distribution between components and services.
 **/
class AREG_API ClientReceiveThread    : public    DispatcherThread
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes Service handler and client connection objects.
     * \param   remoteService   The instance of remote service to process messages.
     * \param   connection      The instance of client connection object to read messages.
     **/
    ClientReceiveThread( IERemoteServiceHandler & remoteService, ClientConnection & connection );

    /**
     * \brief   Destructor.
     **/
    virtual ~ClientReceiveThread( void );

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
    virtual bool runDispatcher( void );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote service handler to dispatch messages.
     **/
    IERemoteServiceHandler &    mRemoteService;
    /**
     * \brief   The instance of connection to receive messages from remote routing service.
     **/
    ClientConnection &          mConnection;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientReceiveThread( void );
    ClientReceiveThread( const ClientReceiveThread & /*src*/ );
    const ClientReceiveThread & operator = ( const ClientReceiveThread & /*src*/ );
};

#endif  // AREG_IPC_PRIVATE_CLIENTRECEIVETHREAD_HPP
