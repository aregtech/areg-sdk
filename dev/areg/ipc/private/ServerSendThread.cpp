/************************************************************************
 * \file        areg/ipc/private/ServerSendThread.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Send Message Thread
 ************************************************************************/
#include "areg/ipc/private/ServerSendThread.hpp"

#include "areg/ipc/private/NEConnection.hpp"
#include "areg/ipc/private/ServerConnection.hpp"
#include "areg/ipc/private/IERemoteServiceHandler.hpp"
#include "areg/component/NEService.hpp"

#include "areg/trace/GETrace.h"
DEF_TRACE_SCOPE(areg_ipc_private_ServerSendThread_processEvent);

ServerSendThread::ServerSendThread(IERemoteServiceHandler & remoteService, ServerConnection & connection)
    : DispatcherThread            ( NEConnection::SERVER_SEND_MESSAGE_THREAD )
    , IESendMessageEventConsumer   ( )
    , mRemoteService                ( remoteService )
    , mConnection                   ( connection )
{
    ; // do nothing
}

ServerSendThread::~ServerSendThread( void )
{
    ; // do nothing
}

bool ServerSendThread::runDispatcher( void )
{
    SendMessageEvent::addListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this));

    bool result = DispatcherThread::runDispatcher();

    SendMessageEvent::removeListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this));
    return result;
}

void ServerSendThread::processEvent( const SendMessageEventData & data )
{
    TRACE_SCOPE(areg_ipc_private_ServerSendThread_processEvent);
    const RemoteMessage & msgSend = data.getRemoteMessage();
    if ( msgSend.isValid() )
    {
        ITEM_ID target = static_cast<ITEM_ID>(msgSend.getTarget());
        SocketAccepted client = mConnection.getClientByCookie(target);
        
        TRACE_DBG("Processing to send message [ %s ] ( ID = %p ) to client [ %s : %d ] of socket [ %u ]. The message sent from source [ %p ] to target [ %p ]"
                    , NEService::getString( static_cast<NEService::eFuncIdRange>(msgSend.getMessageId()) )
                    , static_cast<id_type>(msgSend.getMessageId())
                    , client.getAddress().getHostAddress().getString()
                    , client.getAddress().getHostPort()
                    , ((unsigned int)(client.getHandle()))
                    , static_cast<id_type>(msgSend.getSource())
                    , static_cast<id_type>(msgSend.getTarget()));

        if ( mConnection.sendMessage( msgSend, client ) <= 0 )
        {
            TRACE_WARN("Failed to send message [ %u ] to target [ %p ]", msgSend.getMessageId(), static_cast<id_type>(msgSend.getTarget()));
            mRemoteService.failedSendMessage( msgSend );
        }
        else
        {
            TRACE_DBG("Succeeded to send message [ %u ] to target [ %p ]", msgSend.getMessageId(), static_cast<id_type>(msgSend.getTarget()));
        }
    }
    else
    {
        TRACE_DBG("Message [ %s ] ( ID = %p ) from source [ %p ] to target [ %p ], ignoring to send message"
                        , NEService::getString( static_cast<NEService::eFuncIdRange>(msgSend.getMessageId()) )
                        , static_cast<id_type>(msgSend.getMessageId())
                        , static_cast<id_type>(msgSend.getSource())
                        , static_cast<id_type>(msgSend.getTarget()));
    }
}

bool ServerSendThread::postEvent(Event & eventElem)
{
    return ( RUNTIME_CAST(&eventElem, SendMessageEvent) != NULL ? EventDispatcher::postEvent(eventElem) : false );
}
