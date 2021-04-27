#ifndef AREG_IPC_PRIVATE_CECLIENTSENDTHREAD_HPP
#define AREG_IPC_PRIVATE_CECLIENTSENDTHREAD_HPP

/************************************************************************
 * \file        areg/ipc/private/CEClientSendThread.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Send Message Thread
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
// CEClientSendThread class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The message sender thread. All messages to be sent to remote Service Broker
 *          are queued in message sender thread. 
 **/
class AREG_API CEClientSendThread   : public    CEDispatcherThread
                                    , public    IESendMessageEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes Service handler and client connection objects.
     * \param   remoteService   The instance of remote service to process messages.
     * \param   connection      The instance of client connection object to send messages.
     **/
    CEClientSendThread( IERemoteServiceHandler & remoteService, CEClientConnection & connection );
    /**
     * \brief   Destructor
     **/
    virtual ~CEClientSendThread( void );

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
    virtual bool PostEvent( CEEvent & eventElem );

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
    virtual void ProcessEvent( const CESendMessageEventData & data );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote service handler to dispatch messages.
     **/
    IERemoteServiceHandler &    mRemoteService;
    /**
     * \brief   The instance of connection to send messages from remote Service Broker.
     **/
    CEClientConnection &        mConnection;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEClientSendThread( void );
    CEClientSendThread( const CEClientSendThread & /*src*/ );
    const CEClientSendThread & operator = ( const CEClientSendThread & /*src*/ );
};

#endif  // AREG_IPC_PRIVATE_CECLIENTSENDTHREAD_HPP
