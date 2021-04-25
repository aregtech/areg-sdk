/************************************************************************
 * \file        areg/src/component/private/CEStubBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Stub Base implementation.
 *
 ************************************************************************/
#include "areg/src/component/CEStubBase.hpp"

#include "areg/src/component/CEServiceResponseEvent.hpp"
#include "areg/src/component/CEResponseEvents.hpp"
#include "areg/src/component/CEEventDataStream.hpp"
#include "areg/src/component/CEComponentThread.hpp"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/component/private/CEStubConnectEvent.hpp"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

const SessionID             CEStubBase::INVALID_SESSION_ID          = static_cast<SessionID>(-1);       /*0xFFFFFFFF*/
CEStubBase::MapStubResource CEStubBase::_mapRegisteredStubs;

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
const CEStubBase::CEListener& CEStubBase::CEListener::operator = ( const CEStubBase::CEListener& src )
{
    if (static_cast<const CEStubBase::CEListener *>(this) != &src)
    {
        mMessageId  = src.mMessageId;
        mSequenceNr = src.mSequenceNr;
        mProxy      = src.mProxy;
    }
    return (*this);
}

bool CEStubBase::CEListener::operator == ( const CEStubBase::CEListener & other ) const
{
//     return  (   (mMessageId == other.mMessageId && NEService::SEQUENCE_NUMBER_ANY == other.mSequenceNr)    ||
//                 (mMessageId == other.mMessageId && mSequenceNr == other.mSequenceNr && mProxy == other.mProxy)
//             );
    bool result = this == &other ? true : false;
    if (result == false)
    {
        if (mMessageId == other.mMessageId)
        {
            if (NEService::SEQUENCE_NUMBER_ANY == other.mSequenceNr)
                result = true;
            else if (mSequenceNr == other.mSequenceNr || mSequenceNr == static_cast<unsigned int>( -1 * static_cast<int>(mSequenceNr == other.mSequenceNr)))
                result = true;
        }
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

CEStubBase::CEStubListenerList::CEStubListenerList( void )
    : TELinkedList<CEStubBase::CEListener, const CEStubBase::CEListener &> ( )
{
    ; // do nothing
}

CEStubBase::CEStubListenerList::~CEStubListenerList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

CEStubBase::CEStubBase( CEComponent & masterComp, const NEService::SInterfaceData & siData )
    : IEStubEventConsumer  ( mAddress )

    , mComponent            (masterComp)
    , mAddress              (siData, masterComp.GetComponentAddress().GetComponentName(), masterComp.GetComponentAddress().GetThreadAddress().GetThreadName())
    , mConnectionStatus     ( NEService::ServiceDisconnected )
    , mListListener         ( )
    , mCurrListener         (NULL)
    , mSessionId            (0)
    , mMapSessions          ( )
{
    _mapRegisteredStubs.RegisterResourceObject(mAddress, this);
    masterComp.RegisterServerItem(self());
}

CEStubBase::~CEStubBase( void )
{
    _mapRegisteredStubs.UnregisterResourceObject(mAddress);
}

const CEStubAddress & CEStubBase::GetAddress( void ) const
{
    return mAddress;
}

bool CEStubBase::IsBusy( unsigned int requestId ) const
{
    bool result = false;
    LISTPOS pos = mListListener.Find(CEStubBase::CEListener(requestId, NEService::SEQUENCE_NUMBER_ANY), NULL);
    for ( ; result == false && pos != NULL; pos = mListListener.GetNextPosition(pos))
        result = mListListener[pos].mSequenceNr != 0;
    return result;
}

SessionID CEStubBase::UnblockCurrentRequest( void )
{
    SessionID result = CEStubBase::INVALID_SESSION_ID;
    CEStubBase::CEListener listener;
    if ( (mCurrListener != NULL) && mListListener.RemoveAt(mCurrListener, listener) )
    {
        result = ++ mSessionId;
        mMapSessions.SetKey(result, listener);
        mCurrListener   = NULL;
    }
    return result;
}

void CEStubBase::PrepareResponse( SessionID sessionId )
{
    CEStubBase::CEListener listener;
    if ( mMapSessions.RemoveKey(sessionId, listener) )
        mListListener.AddHead(listener);
}

void CEStubBase::PrepareRequest( CEListener & listener, unsigned int seqNr, unsigned int responseId )
{
    listener.mMessageId = responseId;
    listener.mSequenceNr= mListListener.Find(listener, NULL) == NULL ? seqNr : static_cast<unsigned int>(-1 * static_cast<int>(seqNr));
    mCurrListener = mListListener.AddHead(listener);
}

int CEStubBase::FindListeners( unsigned int requestId, CEStubListenerList & out_listners ) const
{
    CEStubBase::CEListener listener(requestId, NEService::SEQUENCE_NUMBER_ANY);
    LISTPOS pos = mListListener.Find(listener, NULL);
    while (pos != NULL)
    {
        out_listners.AddTail(mListListener[pos]);
        pos = mListListener.Find(listener, pos);
    }
    return out_listners.GetSize();
}

void CEStubBase::ClearAllListeners( const CEProxyAddress & whichProxy, CEIntegerArray & removedIDs )
{
    LISTPOS pos = mListListener.GetHeadPosition();
    while ( pos != NULL )
    {
        if (mListListener[pos].mProxy == whichProxy)
        {
            LISTPOS temp = pos;
            pos = mListListener.GetNextPosition(pos);
            removedIDs.Add(mListListener.RemoveAt(temp).mMessageId);
        }
        else
        {
            pos = mListListener.GetNextPosition(pos);
        }
    }
}

void CEStubBase::ClearAllListeners( const CEProxyAddress & whichProxy )
{
    LISTPOS pos = mListListener.GetHeadPosition();
    while ( pos != NULL )
    {
        if (mListListener[pos].mProxy == whichProxy)
        {
            LISTPOS temp = pos;
            pos = mListListener.GetNextPosition(pos);
            mListListener.RemoveAt(temp);
        }
        else
        {
            pos = mListListener.GetNextPosition(pos);
        }
    }
}

void CEStubBase::SendResponseNotification( const CEStubListenerList & whichListeners, const CEServiceResponseEvent& masterEvent )
{
    for( LISTPOS pos = whichListeners.GetHeadPosition(); pos != NULL; pos = whichListeners.GetNextPosition(pos) )
    {
        const CEStubBase::CEListener& listener = whichListeners[pos];
        CEServiceResponseEvent* eventResp = masterEvent.CloneEventForTarget(listener.mProxy);
        if (eventResp != NULL)
        {
            if (static_cast<int>(listener.mSequenceNr) >= 0)
            {
                eventResp->SetSequenceNumber(listener.mSequenceNr);
                if (listener.mSequenceNr != 0)
                    mListListener.RemoveElem(listener);
            }
            else
            {
                eventResp->SetSequenceNumber(static_cast<unsigned int>(-1 * static_cast<int>(listener.mSequenceNr)));
                CEStubBase::CEListener removed(masterEvent.GetResponseId(), 0, listener.mProxy);
                mListListener.RemoveElem(removed, NULL);
            }
            SendServiceResponse(*eventResp);
        }
    }
}

void CEStubBase::SendErrorNotification( const CEStubListenerList & whichListeners, const CEServiceResponseEvent & masterEvent )
{
    for( LISTPOS pos = whichListeners.GetHeadPosition(); pos != NULL; pos = whichListeners.GetNextPosition(pos))
    {
        const CEStubBase::CEListener& listener = whichListeners[pos];
        CEServiceResponseEvent* eventError = masterEvent.CloneEventForTarget(listener.mProxy);
        if (eventError != NULL)
        {
            if (static_cast<int>(listener.mSequenceNr) >= 0)
            {
                eventError->SetSequenceNumber(listener.mSequenceNr);
                if (listener.mSequenceNr != 0)
                    mListListener.RemoveElem(listener);
            }
            else
            {
                eventError->SetSequenceNumber(static_cast<unsigned int>(-1 * static_cast<int>(listener.mSequenceNr)));
            }
            SendServiceResponse(*eventError);
        }
    }
}

void CEStubBase::SendUpdateNotification( const CEStubListenerList & whichListeners, const CEServiceResponseEvent & masterEvent ) const
{
    for (LISTPOS pos = whichListeners.GetHeadPosition(); pos != NULL; pos = whichListeners.GetNextPosition(pos))
    {
        const CEStubBase::CEListener& listener = whichListeners[pos];
        CEServiceResponseEvent* eventResp = masterEvent.CloneEventForTarget(listener.mProxy);
        if (eventResp != NULL)
            SendServiceResponse(*eventResp);
    }
}

void CEStubBase::SendServiceResponse( CEServiceResponseEvent & eventElem ) const
{
    eventElem.GetTargetProxy().DeliverServiceEvent(eventElem);
}

void CEStubBase::CancelCurrentRequest( void )
{
    mCurrListener   = NULL;
}

CEComponentThread & CEStubBase::GetMasterThread( void ) const
{
    return mComponent.GetMasterThread();
}

CEStubBase* CEStubBase::FindStubByAddress( const CEStubAddress& address )
{
    return _mapRegisteredStubs.FindResourceObject(address);
}

void CEStubBase::StartupServiceInterface( CEComponent&  holder )
{
    CEStubConnectEvent::AddListener( static_cast<IEStubEventConsumer &>(self()), holder.GetMasterThread() );
}

void CEStubBase::ShutdownServiceIntrface( CEComponent & holder )
{
    CEStubConnectEvent::RemoveListener( static_cast<IEStubEventConsumer &>(self()), holder.GetMasterThread() );
}

void CEStubBase::ErrorAllRequests( void )
{
    unsigned int i;

    const unsigned int numOfAttr= GetNumberOfAttributes();
    const unsigned int numOfResp= GetNumberOfResponses();
    const unsigned int numOfReqs= GetNumberOfRequests();
    const unsigned int* attrIds = GetAttributeIds();
    const unsigned int* respIds = GetResponseIds();
    const unsigned int* reqIds  = GetRequestIds();

    ASSERT(attrIds != NULL || numOfAttr == 0);
    ASSERT(respIds != NULL || numOfResp == 0);
    ASSERT(reqIds  != NULL || numOfReqs == 0);

    for ( i = 0; i < numOfAttr; ++ i )
        ErrorRequest(attrIds[i], false);
    for ( i = 0; i < numOfResp; ++ i )
        ErrorRequest(respIds[i], false);
    for ( i = 0; i < numOfReqs; ++ i )
        ErrorRequest( reqIds[i], false);
}

void CEStubBase::CancelAllRequests( void )
{
    const unsigned int numOfReqs= GetNumberOfRequests();
    const unsigned int* reqIds  = GetRequestIds();
    for ( unsigned int i = 0; i < numOfReqs; ++ i )
        ErrorRequest( reqIds[i], true);
}

void CEStubBase::InvalidateAttribute( unsigned int attrId )
{
    if ( NEService::IsAttributeId(attrId) )
        ErrorRequest(attrId, false);
}

void CEStubBase::SendUpdateEvent( unsigned int msgId, const CEEventDataStream & data, NEService::eResultType result ) const
{
    CEStubBase::CEStubListenerList listeners;
    if (FindListeners(msgId, listeners) > 0)
    {
        CEResponseEvent* eventElem = CreateResponseEvent(listeners.GetHead().mProxy, msgId, result, data);
        if (eventElem != NULL)
        {
            SendUpdateNotification(listeners, *eventElem);
            eventElem->Destroy();
        }
    }
}

void CEStubBase::SendResponseEvent( unsigned int respId, const CEEventDataStream & data )
{
    CEStubBase::CEStubListenerList listeners;
    if (FindListeners(respId, listeners) > 0)
    {
        CEResponseEvent* eventElem = CreateResponseEvent(listeners.GetHead().mProxy, respId, NEService::RESULT_OK, data);
        if (eventElem != NULL)
        {
            SendResponseNotification(listeners, *eventElem);
            eventElem->Destroy();
        }
    }
}

void CEStubBase::SendBusyRespone( const CEListener & whichListener )
{
    CEResponseEvent* eventElem = CreateResponseEvent(whichListener.mProxy, whichListener.mMessageId, NEService::RESULT_REQUEST_BUSY, CEEventDataStream::EmptyData);
    if (eventElem != NULL)
    {
        eventElem->SetSequenceNumber(whichListener.mSequenceNr);
        SendServiceResponse(*eventElem);
    }
}

bool CEStubBase::CanExecuteRequest( CEListener & whichListener, unsigned int whichResponse, unsigned int seqNr )
{
    bool result = false;
    if (IsBusy(whichResponse))
    {
        whichListener.mSequenceNr   = seqNr;
        SendBusyRespone(whichListener);
    }
    else
    {
        PrepareRequest(whichListener, seqNr, whichResponse);
        result = true;
    }

    return result;
}

bool CEStubBase::NotificationListenerExist( unsigned int msgId, const CEProxyAddress & notifySource ) const
{
    bool result = false;
    if ( notifySource.IsValid() )
    {
        LISTPOS pos = mListListener.GetHeadPosition();
        while ( pos != NULL && result == false )
        {
            const CEStubBase::CEListener & listener = mListListener.GetNext(pos);
            result = NEService::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr && msgId == listener.mMessageId && notifySource == listener.mProxy;
        }
    }
    return result;
}

bool CEStubBase::AddNotificationListener(unsigned int msgId, const CEProxyAddress & notifySource)
{
    ASSERT(notifySource.IsValid());

    bool hasEntry = false;
    for ( LISTPOS pos = mListListener.GetHeadPosition(); hasEntry && (pos != NULL); pos = mListListener.GetNextPosition(pos) )
    {
        const CEStubBase::CEListener & listener = mListListener.GetAt(pos);
        hasEntry = NEService::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr && msgId == listener.mMessageId && notifySource == listener.mProxy;
    }

    return ( hasEntry == false && notifySource.IsValid() ? mListListener.AddTail( CEStubBase::CEListener(msgId, NEService::SEQUENCE_NUMBER_NOTIFY, notifySource) ) != NULL : false );
}

void CEStubBase::RemoveNotificationListener( unsigned int msgId, const CEProxyAddress & notifySource )
{
    for ( LISTPOS pos = mListListener.GetHeadPosition(); pos != NULL; pos = mListListener.GetNextPosition(pos) )
    {
        const CEStubBase::CEListener & listener = mListListener.GetAt(pos);
        if ( NEService::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr && msgId == listener.mMessageId && notifySource == listener.mProxy )
        {
            mListListener.RemoveAt(pos);
            break;
        }
    }
}

void CEStubBase::ClientConnected(const CEProxyAddress & client, bool isConnected)
{
    if ( isConnected == false )
        ClearAllListeners(client);
}

void CEStubBase::ProcessClientConnectEvent( const CEProxyAddress & proxyAddress, NEService::eServiceConnection connectionStatus )
{
    switch ( connectionStatus )
    {
    case NEService::ServiceConnected:
        ClientConnected( proxyAddress, true );
        break;

    case NEService::ServicePending:
        break;  // do nothing, the client connection is pending

    case NEService::ServiceDisconnected:
    default:
        ClientConnected( proxyAddress, false );
        break;
    }
}

void CEStubBase::ProcessStubRegisteredEvent(const CEStubAddress & stubTarget, NEService::eServiceConnection connectionStatus)
{
    if ( connectionStatus == NEService::ServiceConnected )
    {
        ASSERT( stubTarget.IsValid() );
        _mapRegisteredStubs.ResourceLock();
        _mapRegisteredStubs.UnregisterResourceObject(mAddress);
        mAddress = stubTarget;
        _mapRegisteredStubs.RegisterResourceObject(mAddress, this);
        _mapRegisteredStubs.ResourceUnlock();
    }
    mConnectionStatus = connectionStatus;
}
