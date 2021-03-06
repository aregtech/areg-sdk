/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StubBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Stub Base implementation.
 *
 ************************************************************************/
#include "areg/component/StubBase.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/EventDataStream.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/private/StubConnectEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// StubBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// StubBase class statics
//////////////////////////////////////////////////////////////////////////

StubBase::MapStubResource   StubBase::_mapRegisteredStubs;

//////////////////////////////////////////////////////////////////////////
// StubBase::Listener implementation
//////////////////////////////////////////////////////////////////////////

bool StubBase::Listener::operator == ( const StubBase::Listener & other ) const
{
    bool result = this == &other;
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


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

StubBase::StubBase( Component & masterComp, const NEService::SInterfaceData & siData )
    : IEStubEventConsumer  ( mAddress )

    , mComponent            (masterComp)
    , mInterface            (siData)
    , mAddress              (siData, masterComp.getAddress().getRoleName(), masterComp.getAddress().getThreadAddress().getThreadName())
    , mConnectionStatus     ( NEService::eServiceConnection::ServiceDisconnected )
    , mListListener         ( )
    , mCurrListener         (nullptr)
    , mSessionId            (0)
    , mMapSessions          ( )
{
    _mapRegisteredStubs.registerResourceObject(mAddress, this);
    masterComp.registerServerItem(self());
}

StubBase::~StubBase( void )
{
    _mapRegisteredStubs.unregisterResourceObject(mAddress);
}

const StubAddress & StubBase::getAddress( void ) const
{
    return mAddress;
}

bool StubBase::isBusy( unsigned int requestId ) const
{
    bool result = false;
    LISTPOS pos = mListListener.find(StubBase::Listener(requestId, NEService::SEQUENCE_NUMBER_ANY), nullptr);
    for ( ; result == false && pos != nullptr; pos = mListListener.nextPosition(pos))
    {
        result = mListListener[pos].mSequenceNr != 0;
    }

    return result;
}

SessionID StubBase::unblockCurrentRequest( void )
{
    SessionID result = StubBase::INVALID_SESSION_ID;
    StubBase::Listener listener;
    if ( mCurrListener != nullptr )
    {
        mListListener.removeAt(mCurrListener, listener);
        result = ++ mSessionId;
        mMapSessions.setAt(result, listener);
        mCurrListener   = nullptr;
    }
    return result;
}

void StubBase::prepareResponse( SessionID sessionId )
{
    StubBase::Listener listener;
    if ( mMapSessions.removeAt(sessionId, listener) )
        mListListener.pushFirst(listener);
}

void StubBase::prepareRequest( Listener & listener, unsigned int seqNr, unsigned int responseId )
{
    listener.mMessageId = responseId;
    listener.mSequenceNr= mListListener.find(listener, nullptr) == nullptr ? seqNr : static_cast<unsigned int>(-1 * static_cast<int>(seqNr));
    mCurrListener = mListListener.pushFirst(listener);
}

int StubBase::findListeners( unsigned int requestId, StubListenerList & out_listners ) const
{
    StubBase::Listener listener(requestId, NEService::SEQUENCE_NUMBER_ANY);
    LISTPOS pos = mListListener.find(listener, nullptr);
    while (pos != nullptr)
    {
        out_listners.pushLast(mListListener[pos]);
        pos = mListListener.find(listener, pos);
    }
    return out_listners.getSize();
}

void StubBase::clearAllListeners( const ProxyAddress & whichProxy, IntegerArray & removedIDs )
{
    LISTPOS pos = mListListener.firstPosition();
    while ( pos != nullptr )
    {
        if (mListListener[pos].mProxy == whichProxy)
        {
            LISTPOS temp = pos;
            pos = mListListener.nextPosition(pos);
            removedIDs.add(mListListener.removeAt(temp).mMessageId);
        }
        else
        {
            pos = mListListener.nextPosition(pos);
        }
    }
}

void StubBase::clearAllListeners( const ProxyAddress & whichProxy )
{
    LISTPOS pos = mListListener.firstPosition();
    while ( pos != nullptr )
    {
        if (mListListener[pos].mProxy == whichProxy)
        {
            LISTPOS temp = pos;
            pos = mListListener.nextPosition(pos);
            mListListener.removeAt(temp);
        }
        else
        {
            pos = mListListener.nextPosition(pos);
        }
    }
}

void StubBase::sendResponseNotification( const StubListenerList & whichListeners, const ServiceResponseEvent& masterEvent )
{
    for( LISTPOS pos = whichListeners.firstPosition(); pos != nullptr; pos = whichListeners.nextPosition(pos) )
    {
        const StubBase::Listener& listener = whichListeners[pos];
        ServiceResponseEvent* eventResp = masterEvent.cloneForTarget(listener.mProxy);
        if (eventResp != nullptr)
        {
            if (static_cast<int>(listener.mSequenceNr) >= 0)
            {
                eventResp->setSequenceNumber(listener.mSequenceNr);
                if (listener.mSequenceNr != 0)
                    mListListener.removeEntry(listener);
            }
            else
            {
                eventResp->setSequenceNumber(static_cast<unsigned int>(-1 * static_cast<int>(listener.mSequenceNr)));
                StubBase::Listener removed(masterEvent.getResponseId(), 0, listener.mProxy);
                mListListener.removeEntry(removed, nullptr);
            }

            sendServiceResponse(*eventResp);
        }
    }
}

void StubBase::sendErrorNotification( const StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent )
{
    for( LISTPOS pos = whichListeners.firstPosition(); pos != nullptr; pos = whichListeners.nextPosition(pos))
    {
        const StubBase::Listener& listener = whichListeners[pos];
        ServiceResponseEvent* eventError = masterEvent.cloneForTarget(listener.mProxy);
        if (eventError != nullptr)
        {
            if (static_cast<int>(listener.mSequenceNr) >= 0)
            {
                eventError->setSequenceNumber(listener.mSequenceNr);
                if (listener.mSequenceNr != 0)
                    mListListener.removeEntry(listener);
            }
            else
            {
                eventError->setSequenceNumber(static_cast<unsigned int>(-1 * static_cast<int>(listener.mSequenceNr)));
            }

            sendServiceResponse(*eventError);
        }
    }
}

void StubBase::sendUpdateNotification( const StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent ) const
{
    for (LISTPOS pos = whichListeners.firstPosition(); pos != nullptr; pos = whichListeners.nextPosition(pos))
    {
        const StubBase::Listener& listener = whichListeners[pos];
        ServiceResponseEvent* eventResp = masterEvent.cloneForTarget(listener.mProxy);
        if (eventResp != nullptr)
            sendServiceResponse(*eventResp);
    }
}

void StubBase::sendServiceResponse( ServiceResponseEvent & eventElem ) const
{
    eventElem.getTargetProxy().deliverServiceEvent(eventElem);
}

void StubBase::cancelCurrentRequest( void )
{
    mCurrListener   = nullptr;
}

ComponentThread & StubBase::getComponentThread( void ) const
{
    return mComponent.getMasterThread();
}

StubBase* StubBase::findStubByAddress( const StubAddress& address )
{
    return _mapRegisteredStubs.findResourceObject(address);
}

void StubBase::startupServiceInterface( Component&  holder )
{
    StubConnectEvent::addListener( static_cast<IEStubEventConsumer &>(self()), holder.getMasterThread() );
}

void StubBase::shutdownServiceIntrface( Component & holder )
{
    StubConnectEvent::removeListener( static_cast<IEStubEventConsumer &>(self()), holder.getMasterThread() );
}

void StubBase::errorAllRequests( void )
{
    unsigned int i;

    const unsigned int numOfAttr= getNumberOfAttributes();
    const unsigned int numOfResp= getNumberOfResponses();
    const unsigned int numOfReqs= getNumberOfRequests();
    const unsigned int* attrIds = getAttributeIds();
    const unsigned int* respIds = getResponseIds();
    const unsigned int* reqIds  = getRequestIds();

    ASSERT(attrIds != nullptr || numOfAttr == 0);
    ASSERT(respIds != nullptr || numOfResp == 0);
    ASSERT(reqIds  != nullptr || numOfReqs == 0);

    for ( i = 0; i < numOfAttr; ++ i )
        errorRequest(attrIds[i], false);

    for ( i = 0; i < numOfResp; ++ i )
        errorRequest(respIds[i], false);

    for ( i = 0; i < numOfReqs; ++ i )
        errorRequest( reqIds[i], false);
}

void StubBase::cancelAllRequests( void )
{
    const unsigned int numOfReqs= getNumberOfRequests();
    const unsigned int* reqIds  = getRequestIds();
    for ( unsigned int i = 0; i < numOfReqs; ++ i )
        errorRequest( reqIds[i], true);
}

void StubBase::invalidateAttribute( unsigned int attrId )
{
    if ( NEService::isAttributeId(attrId) )
        errorRequest(attrId, false);
}

void StubBase::sendUpdateEvent( unsigned int msgId, const EventDataStream & data, NEService::eResultType result ) const
{
    StubBase::StubListenerList listeners;
    if (findListeners(msgId, listeners) > 0)
    {
        ResponseEvent* eventElem = createResponseEvent(listeners.getFirstEntry().mProxy, msgId, result, data);
        if (eventElem != nullptr)
        {
            sendUpdateNotification(listeners, *eventElem);
            eventElem->destroy();
        }
    }
}

void StubBase::sendResponseEvent( unsigned int respId, const EventDataStream & data )
{
    StubBase::StubListenerList listeners;
    if (findListeners(respId, listeners) > 0)
    {
        ResponseEvent* eventElem = createResponseEvent(listeners.getFirstEntry().mProxy, respId, NEService::eResultType::RequestOK, data);
        if (eventElem != nullptr)
        {
            sendResponseNotification(listeners, *eventElem);
            eventElem->destroy();
        }
    }
}

void StubBase::sendBusyRespone( const Listener & whichListener )
{
    ResponseEvent* eventElem = createResponseEvent(whichListener.mProxy, whichListener.mMessageId, NEService::eResultType::RequestBusy, EventDataStream::EmptyData);
    if (eventElem != nullptr)
    {
        eventElem->setSequenceNumber(whichListener.mSequenceNr);
        sendServiceResponse(*eventElem);
    }
}

bool StubBase::canExecuteRequest( Listener & whichListener, unsigned int whichResponse, unsigned int seqNr )
{
    bool result = false;
    if (isBusy(whichResponse))
    {
        whichListener.mSequenceNr   = seqNr;
        sendBusyRespone(whichListener);
    }
    else
    {
        prepareRequest(whichListener, seqNr, whichResponse);
        result = true;
    }

    return result;
}

bool StubBase::existNotificationListener( unsigned int msgId, const ProxyAddress & notifySource ) const
{
    bool result = false;
    if ( notifySource.isValid() )
    {
        LISTPOS pos = mListListener.firstPosition();
        while ( (pos != nullptr) && (result == false) )
        {
            const StubBase::Listener & listener = mListListener.getNext(pos);
            result = (NEService::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr) &&
                     (msgId == listener.mMessageId) &&
                     (notifySource == listener.mProxy);
        }
    }

    return result;
}

bool StubBase::addNotificationListener(unsigned int msgId, const ProxyAddress & notifySource)
{
    bool result = false;

    if (notifySource.isValid())
    {
        bool hasEntry   = false;
        LISTPOS pos     = mListListener.firstPosition();
        while ( (hasEntry == false) && (pos != nullptr) )
        {
            const StubBase::Listener & listener = mListListener.getNext(pos);
            hasEntry = (NEService::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr) &&
                       (msgId == listener.mMessageId) &&
                       (notifySource == listener.mProxy);
        }

        result = ( (hasEntry == false) && (mListListener.pushLast( StubBase::Listener(msgId, NEService::SEQUENCE_NUMBER_NOTIFY, notifySource) ) != nullptr));
    }

    return result;
}

void StubBase::removeNotificationListener( unsigned int msgId, const ProxyAddress & notifySource )
{
    for ( LISTPOS pos = mListListener.firstPosition(); pos != nullptr; pos = mListListener.nextPosition(pos) )
    {
        const StubBase::Listener & listener = mListListener.getAt(pos);
        if ( NEService::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr && msgId == listener.mMessageId && notifySource == listener.mProxy )
        {
            mListListener.removeAt(pos);
            break;
        }
    }
}

void StubBase::clientConnected(const ProxyAddress & client, bool isConnected)
{
    if ( isConnected == false )
        clearAllListeners(client);
}

void StubBase::processClientConnectEvent( const ProxyAddress & proxyAddress, NEService::eServiceConnection connectionStatus )
{
    switch ( connectionStatus )
    {
    case NEService::eServiceConnection::ServiceConnected:
        clientConnected( proxyAddress, true );
        break;

    case NEService::eServiceConnection::ServicePending:
        break;  // do nothing, the client connection is pending

    case NEService::eServiceConnection::ServiceFailed:              // fall through
    case NEService::eServiceConnection::ServiceRejected:            // fall through
    case NEService::eServiceConnection::ServiceConnectionUnknown:
        ASSERT(false);  // unexpected here
        break;

    case NEService::eServiceConnection::ServiceDisconnected:        // fall through
    case NEService::eServiceConnection::ServiceShutdown:            // fall through
    default:
        clientConnected( proxyAddress, false );
        break;
    }
}

void StubBase::processStubRegisteredEvent(const StubAddress & stubTarget, NEService::eServiceConnection connectionStatus)
{
    if ( connectionStatus == NEService::eServiceConnection::ServiceConnected )
    {
        ASSERT( stubTarget.isValid() );
        _mapRegisteredStubs.lock();
        _mapRegisteredStubs.unregisterResourceObject(mAddress);

        mAddress = stubTarget;
        
        _mapRegisteredStubs.registerResourceObject(mAddress, this);
        _mapRegisteredStubs.unlock();
    }

    mConnectionStatus = connectionStatus;
}

const Version & StubBase::getImplVersion( void ) const
{
    return mInterface.idVersion;
}

unsigned int StubBase::getNumberOfRequests( void ) const
{
    return mInterface.idRequestCount;
}

unsigned int StubBase::getNumberOfResponses( void ) const
{
    return mInterface.idResponseCount;
}

unsigned int StubBase::getNumberOfAttributes( void ) const
{
    return mInterface.idAttributeCount;
}

const unsigned int * StubBase::getRequestIds( void ) const
{
    return mInterface.idRequestList;
}

const unsigned int * StubBase::getResponseIds( void ) const
{
    return mInterface.idResponseList;
}

const unsigned int * StubBase::getAttributeIds( void ) const
{
    return mInterface.idAttributeList;
}

ResponseEvent * StubBase::createResponseEvent( const ProxyAddress &     /* proxy */
                                             , unsigned int             /* msgId */
                                             , NEService::eResultType   /* result */
                                             , const EventDataStream &  /* data */ ) const
{
    return nullptr;
}

RemoteRequestEvent * StubBase::createRemoteRequestEvent( const IEInStream & /* stream */ ) const
{
    return nullptr;
}

RemoteNotifyRequestEvent * StubBase::createRemoteNotifyRequestEvent( const IEInStream & /* stream */ ) const
{
    return nullptr;
}

void StubBase::processStubEvent( StubEvent & /* eventElem */ )
{
}

void StubBase::processGenericEvent(Event & /* eventElem */)
{
}
