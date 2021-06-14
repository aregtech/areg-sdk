/************************************************************************
 * \file        areg/ipc/private/ClientSendThread.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Send Message Thread
 ************************************************************************/
#include "areg/ipc/private/ClientSendThread.hpp"

#include "areg/ipc/private/NEConnection.hpp"
#include "areg/ipc/private/ClientConnection.hpp"
#include "areg/ipc/private/IERemoteServiceHandler.hpp"

#include "areg/trace/GETrace.h"
DEF_TRACE_SCOPE(areg_ipc_private_ClientSendThread_runDispatcher);

ClientSendThread::ClientSendThread( IERemoteServiceHandler & remoteService, ClientConnection & connection )
    : DispatcherThread  ( NEConnection::CLIENT_SEND_MESSAGE_THREAD )
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
{
    ; // do nothing
}

ClientSendThread::~ClientSendThread(void)
{
    ; // do nothing
}

bool ClientSendThread::runDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_ClientSendThread_runDispatcher);
    TRACE_DBG("Starting client service dispatcher thread [ %s ]", getName().getString());
    SendMessageEvent::addListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this));

    bool result = DispatcherThread::runDispatcher();

    SendMessageEvent::removeListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this));
    TRACE_DBG("Exiting client service dispatcher thread [ %s ] with result [ %s ]", getName().getString(), result ? "SUCCESS" : "FAILURE");
    return result;
}

void ClientSendThread::processEvent( const SendMessageEventData & data )
{
    const RemoteMessage & msg = data.getRemoteMessage();
    if ( msg.isValid())
    {
        if ( mConnection.sendMessage( msg ) <= 0 )
        {
            mRemoteService.failedSendMessage( msg );
        }
    }
}

bool ClientSendThread::postEvent(Event & eventElem)
{
    return ( RUNTIME_CAST(&eventElem, SendMessageEvent) != NULL ? EventDispatcher::postEvent(eventElem) : false );
}
