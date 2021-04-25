/************************************************************************
 * \file        areg/src/ipc/private/CEServerSendThread.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Send Message Thread
 ************************************************************************/
#include "areg/src/ipc/private/CEServerSendThread.hpp"

#include "areg/src/ipc/private/NEConnection.hpp"
#include "areg/src/ipc/private/CEServerConnection.hpp"
#include "areg/src/ipc/private/IERemoteServiceHandler.hpp"
#include "areg/src/component/NEService.hpp"

#include "areg/src/trace/GETrace.h"
DEF_TRACE_SCOPE(areg_ipc_private_CEServerSendThread_ProcessEvent);

CEServerSendThread::CEServerSendThread(IERemoteServiceHandler & remoteService, CEServerConnection & connection)
    : CEDispatcherThread            ( NEConnection::SERVER_SEND_MESSAGE_THREAD )
    , IESendMessageEventConsumer   ( )
    , mRemoteService                ( remoteService )
    , mConnection                   ( connection )
{
    ; // do nothing
}

CEServerSendThread::~CEServerSendThread( void )
{
    ; // do nothing
}

bool CEServerSendThread::RunDispatcher( void )
{
    CESendMessageEvent::AddListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<CEDispatcherThread &>(*this));

    bool result = CEDispatcherThread::RunDispatcher();

    CESendMessageEvent::RemoveListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<CEDispatcherThread &>(*this));
    return result;
}

void CEServerSendThread::ProcessEvent( const CESendMessageEventData & data )
{
    TRACE_SCOPE(areg_ipc_private_CEServerSendThread_ProcessEvent);
    const CERemoteMessage & msgSend = data.GetRemoteMessage();
    if ( msgSend.IsValid() )
    {
        ITEM_ID target = msgSend.GetTarget();
        CESocketAccepted client = mConnection.GetClientByCookie(target);
        
        TRACE_DBG("Processing to send message [ %s ] ( ID = %p ) to client [ %s : %d ] of socket [ %u ]. The message sent from source [ %p ] to target [ %p ]"
                    , NEService::GetString( static_cast<NEService::eFuncIdRange>(msgSend.GetMessageId()) )
                    , msgSend.GetMessageId()
                    , client.GetAddress().GetHostAddress().GetBuffer()
                    , client.GetAddress().GetHostPort()
                    , client.GetHandle()
                    , msgSend.GetSource()
                    , msgSend.GetTarget());

        if ( mConnection.SendMessage( msgSend, client ) < 0 )
        {
            TRACE_WARN("Failed to send message [ %u ] to target [ %p ]", msgSend.GetMessageId(), msgSend.GetTarget());
            mRemoteService.FailedSendMessage( msgSend );
        }
        else
        {
            TRACE_DBG("Succeeded to send message [ %u ] to target [ %p ]", msgSend.GetMessageId(), msgSend.GetTarget());
        }
    }
    else
    {
        TRACE_DBG("Message [ %s ] ( ID = %p ) from source [ %p ] to target [ %p ], ignoring to send message"
                        , NEService::GetString( static_cast<NEService::eFuncIdRange>(msgSend.GetMessageId()) )
                        , msgSend.GetMessageId()
                        , msgSend.GetSource()
                        , msgSend.GetTarget());
    }
}

bool CEServerSendThread::PostEvent(CEEvent & eventElem)
{
    return ( RUNTIME_CAST(&eventElem, CESendMessageEvent) != NULL ? CEEventDispatcher::PostEvent(eventElem) : false );
}
