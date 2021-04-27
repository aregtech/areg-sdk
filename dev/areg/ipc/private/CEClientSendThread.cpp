/************************************************************************
 * \file        areg/ipc/private/CEClientSendThread.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Send Message Thread
 ************************************************************************/
#include "areg/ipc/private/CEClientSendThread.hpp"

#include "areg/ipc/private/NEConnection.hpp"
#include "areg/ipc/private/CEClientConnection.hpp"
#include "areg/ipc/private/IERemoteServiceHandler.hpp"

#include "areg/trace/GETrace.h"
DEF_TRACE_SCOPE(areg_ipc_private_CEClientSendThread_RunDispatcher);

CEClientSendThread::CEClientSendThread( IERemoteServiceHandler & remoteService, CEClientConnection & connection )
    : CEDispatcherThread( NEConnection::CLIENT_SEND_MESSAGE_THREAD )
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
{
    ; // do nothing
}

CEClientSendThread::~CEClientSendThread(void)
{
    ; // do nothing
}

bool CEClientSendThread::RunDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_CEClientSendThread_RunDispatcher);
    TRACE_DBG("Starting client service dispatcher thread [ %s ]", GetThreadName().GetBuffer());
    CESendMessageEvent::AddListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<CEDispatcherThread &>(*this));

    bool result = CEDispatcherThread::RunDispatcher();

    CESendMessageEvent::RemoveListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<CEDispatcherThread &>(*this));
    TRACE_DBG("Exiting client service dispatcher thread [ %s ] with result [ %s ]", GetThreadName().GetBuffer(), result ? "SUCCESS" : "FAILURE");
    return result;
}

void CEClientSendThread::ProcessEvent( const CESendMessageEventData & data )
{
    const CERemoteMessage & msg = data.GetRemoteMessage();
    if ( msg.IsValid() )
    {
        if ( mConnection.SendMessage( msg ) < 0 )
            mRemoteService.FailedSendMessage( msg );
    }
}

bool CEClientSendThread::PostEvent(CEEvent & eventElem)
{
    return ( RUNTIME_CAST(&eventElem, CESendMessageEvent) != NULL ? CEEventDispatcher::PostEvent(eventElem) : false );
}
