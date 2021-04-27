#ifndef AREG_IPC_PRIVATE_CECLIENTRECEIVETHREAD_HPP
#define AREG_IPC_PRIVATE_CECLIENTRECEIVETHREAD_HPP

/************************************************************************
 * \file        areg/ipc/private/CEClientReceiveThread.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Receive Message Thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/CEDispatcherThread.hpp"
#include "areg/ipc/private/CERemoteServiceEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IERemoteServiceHandler;
class CEClientConnection;

//////////////////////////////////////////////////////////////////////////
// CEClientConnection class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The message receiver thread from remote Service Broker.
 *          All received messages are passed to receiver thread for further dispatching
 *          and distribution between components and services.
 **/
class AREG_API CEClientReceiveThread    : public    CEDispatcherThread
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
    CEClientReceiveThread( IERemoteServiceHandler & remoteService, CEClientConnection & connection );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEClientReceiveThread( void );

protected:
/************************************************************************/
// CEDispatcherThread overrides
/************************************************************************/

    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool RunDispatcher( void );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote service handler to dispatch messages.
     **/
    IERemoteServiceHandler &    mRemoteService;
    /**
     * \brief   The instance of connection to receive messages from remote Service Broker.
     **/
    CEClientConnection &        mConnection;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEClientReceiveThread( void );
    CEClientReceiveThread( const CEClientReceiveThread & /*src*/ );
    const CEClientReceiveThread & operator = ( const CEClientReceiveThread & /*src*/ );
};

#endif  // AREG_IPC_PRIVATE_CECLIENTRECEIVETHREAD_HPP
