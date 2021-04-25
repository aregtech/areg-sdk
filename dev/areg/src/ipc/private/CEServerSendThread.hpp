#ifndef AREG_IPC_PRIVATE_CESERVERSENDTHREAD_HPP
#define AREG_IPC_PRIVATE_CESERVERSENDTHREAD_HPP

/************************************************************************
 * \file        areg/src/ipc/private/CEServerSendThread.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Send Message Thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEDispatcherThread.hpp"
#include "areg/src/ipc/private/CERemoteServiceEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IERemoteServiceHandler;
class CEServerConnection;

//////////////////////////////////////////////////////////////////////////
// CEServerSendThread class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The IPC message sender thread
 **/
class AREG_API CEServerSendThread   : public    CEDispatcherThread
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
    CEServerSendThread( IERemoteServiceHandler & remoteService, CEServerConnection & connection );

    /**
     * \brief   Destructor
     **/
    virtual ~CEServerSendThread( void );

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
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    IERemoteServiceHandler &    mRemoteService;
    CEServerConnection &        mConnection;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEServerSendThread( void );
    CEServerSendThread( const CEServerSendThread & /*src*/ );
    const CEServerSendThread & operator = ( const CEServerSendThread & /*src*/ );
};

#endif  // AREG_IPC_PRIVATE_CESERVERSENDTHREAD_HPP
