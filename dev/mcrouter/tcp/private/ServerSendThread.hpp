#ifndef MCROUTER_TCP_PRIVATE_SERVERSENDTHREAD_HPP
#define MCROUTER_TCP_PRIVATE_SERVERSENDTHREAD_HPP

/************************************************************************
 * \file        mcrouter/tcp/private/ServerSendThread.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Send Message Thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/RemoteServiceEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IERemoteServiceHandler;
class ServerConnection;

//////////////////////////////////////////////////////////////////////////
// ServerSendThread class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The IPC message sender thread
 **/
class ServerSendThread  : public    DispatcherThread
                        , public    IESendMessageEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes connection servicing handler and server connection objects.
     * \param   remoteService   The instance of remote servicing handle to set.
     * \param   connection      The instance of server socket connection object.
     **/
    ServerSendThread( IERemoteServiceHandler & remoteService, ServerConnection & connection );

    /**
     * \brief   Destructor
     **/
    virtual ~ServerSendThread( void );

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

/************************************************************************/
// IEEventRouter interface overrides
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target.
     *          Since the Dispatcher Thread is a Base object for
     *          Worker and Component threads, it does nothing
     *          and only destroys event object without processing.
     *          Override this method or use Worker / Component thread.
     * \param	eventElem	Event object to post
     * \return	In this class it always returns true.
     **/
    virtual bool postEvent( Event & eventElem );

private:
/************************************************************************/
// IESendMessageEventConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Automatically triggered when event is dispatched by registered
     *          worker / component thread.
     * \param   data    The data object passed in event. It should have at least
     *                  default constructor and assigning operator.
     *                  This object is not used for IPC.
     **/
    virtual void processEvent( const SendMessageEventData & data );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    IERemoteServiceHandler &    mRemoteService;
    ServerConnection &        mConnection;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServerSendThread( void );
    ServerSendThread( const ServerSendThread & /*src*/ );
    const ServerSendThread & operator = ( const ServerSendThread & /*src*/ );
};

#endif  // MCROUTER_TCP_PRIVATE_SERVERSENDTHREAD_HPP
