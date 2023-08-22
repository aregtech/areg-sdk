#ifndef AREG_EXTEND_SERVICE_SERVERSENDTHREAD_HPP
#define AREG_EXTEND_SERVICE_SERVERSENDTHREAD_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/service/ServerSendThread.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service connectivity server send message thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/SendMessageEvent.hpp"

#include <atomic>

/************************************************************************
 * Dependencies
 ************************************************************************/
class IERemoteMessageHandler;
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
    ServerSendThread(IERemoteMessageHandler& remoteService, ServerConnection & connection );

    /**
     * \brief   Destructor
     **/
    virtual ~ServerSendThread( void ) = default;

/************************************************************************/
// Actions and attributes.
/************************************************************************/
public:
    /**
     * \brief   Returns accumulative value of sent data size and rests the existing value to zero.
     *          The operations are atomic. The value can be used to display data rate, for example.
     **/
    inline uint32_t extractDataSend( void );

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
    virtual bool postEvent( Event & eventElem ) override;

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
    virtual void processEvent( const SendMessageEventData & data ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote servicing interface object
     **/
    IERemoteMessageHandler& mRemoteService;
    /**
     * \brief   The instance of server connection object
     **/
    ServerConnection &      mConnection;
    /**
     * \brief   Accumulative value of sent data size.
     **/
    std::atomic_uint        mBytesSend;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServerSendThread( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServerSendThread );
};

//////////////////////////////////////////////////////////////////////////
// ServerSendThread class inline methods
//////////////////////////////////////////////////////////////////////////

inline uint32_t ServerSendThread::extractDataSend(void)
{
    return static_cast<uint32_t>(mBytesSend.exchange(0));
}

#endif  // AREG_EXTEND_SERVICE_SERVERSENDTHREAD_HPP
