/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
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
            {
                result = true;
            }
            else if (mSequenceNr == other.mSequenceNr || mSequenceNr == static_cast<unsigned int>(-1 * static_cast<int>(mSequenceNr == other.mSequenceNr)))
            {
                result = true;
            }
        }
    }

    return result;
}


//////////////////////////////////////////////////////////////////////////
// StubBase implementation
//////////////////////////////////////////////////////////////////////////

StubBase::StubBase( Component & masterComp, const NEService::SInterfaceData & siData )
    : IEStubEventConsumer   ( mAddress )

    , mComponent            (masterComp)
    , mInterface            (siData)
    , mAddress              (siData, masterComp.getAddress().getRoleName(), masterComp.getAddress().getThreadAddress().getThreadName())
    , mConnectionStatus     ( NEService::eServiceConnection::ServiceDisconnected )
    , mListListener         ( )
    , mCurrListener         (mListListener.invalidPosition())
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

bool StubBase::isBusy( unsigned int requestId ) const
{
    bool result = false;
    StubBase::StubListenerList::LISTPOS pos = mListListener.find(StubBase::Listener(requestId, NEService::SEQUENCE_NUMBER_ANY));
    for ( ; (result == false) && mListListener.isValidPosition(pos); pos = mListListener.nextPosition(pos))
    {
        result = mListListener.valueAtPosition(pos).mSequenceNr != 0;
    }

    return result;
}

SessionID StubBase::unblockCurrentRequest( void )
{
    SessionID result = StubBase::INVALID_SESSION_ID;
    StubBase::Listener listener;
    if (mListListener.isValidPosition(mCurrListener) )
    {
        mListListener.removeAt(mCurrListener, listener);
        result = ++ mSessionId;
        mMapSessions.setAt(result, listener);
        mCurrListener   = mListListener.invalidPosition();
    }

    return result;
}

bool StubBase::prepareResponse( SessionID sessionId )
{
    bool result{ false };
    StubBase::Listener listener;
    if (mMapSessions.removeAt(sessionId, listener))
    {
        mListListener.pushFirst(listener);
        result = true;
    }

    return result;
}

void StubBase::prepareRequest( Listener & listener, unsigned int seqNr, unsigned int responseId )
{
    listener.mMessageId = responseId;
    listener.mSequenceNr= mListListener.isInvalidPosition(mListListener.find(listener)) ? seqNr : static_cast<unsigned int>(-1 * static_cast<int>(seqNr));
    mListListener.pushFirst(listener);
    mCurrListener = mListListener.firstPosition();
}

int StubBase::findListeners( unsigned int requestId, StubListenerList & out_listners ) const
{
    StubBase::Listener listener(requestId, NEService::SEQUENCE_NUMBER_ANY);
    StubListenerList::LISTPOS pos = mListListener.find(listener);
    while (mListListener.isValidPosition(pos))
    {
        out_listners.pushLast(mListListener[pos]);
        pos = mListListener.find(listener, pos);
    }

    return out_listners.getSize();
}

void StubBase::clearAllListeners( const ProxyAddress & whichProxy, IntegerArray & removedIDs )
{
    StubListenerList::LISTPOS pos = mListListener.firstPosition();
    while ( mListListener.isValidPosition(pos))
    {
        if (mListListener[pos].mProxy == whichProxy)
        {
            removedIDs.add(mListListener.valueAtPosition(pos).mMessageId);
            pos = mListListener.removeAt(pos);
        }
        else
        {
            pos = mListListener.nextPosition(pos);
        }
    }
}

void StubBase::clearAllListeners( const ProxyAddress & whichProxy )
{
    StubListenerList::LISTPOS pos = mListListener.firstPosition();
    while ( mListListener.isValidPosition(pos) )
    {
        if (mListListener[pos].mProxy == whichProxy)
        {
            pos = mListListener.removeAt(pos);
        }
        else
        {
            pos = mListListener.nextPosition(pos);
        }
    }
}

void StubBase::sendResponseNotification( const StubListenerList & whichListeners, const ServiceResponseEvent& masterEvent )
{
    for(StubListenerList::LISTPOS pos = whichListeners.firstPosition(); whichListeners.isValidPosition(pos); pos = whichListeners.nextPosition(pos) )
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
                mListListener.removeEntry(removed);
            }

            sendServiceResponse(*eventResp);
        }
    }
}

void StubBase::sendErrorNotification( const StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent )
{
    for(StubListenerList::LISTPOS pos = whichListeners.firstPosition(); whichListeners.isValidPosition(pos); pos = whichListeners.nextPosition(pos))
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
    for (StubListenerList::LISTPOS pos = whichListeners.firstPosition(); whichListeners.isValidPosition(pos); pos = whichListeners.nextPosition(pos))
    {
        const StubBase::Listener& listener = whichListeners[pos];
        ServiceResponseEvent* eventResp = masterEvent.cloneForTarget(listener.mProxy);
        if ( eventResp != nullptr )
        {
            sendServiceResponse( *eventResp );
        }
    }
}

void StubBase::sendServiceResponse( ServiceResponseEvent & eventElem ) const
{
    eventElem.getTargetProxy().deliverServiceEvent(eventElem);
}

void StubBase::cancelCurrentRequest( void )
{
    mCurrListener   = mListListener.invalidPosition();
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

    for (i = 0; i < numOfAttr; ++i)
    {
        errorRequest(attrIds[i], false);
    }

    for (i = 0; i < numOfResp; ++i)
    {
        errorRequest(respIds[i], false);
    }

    for (i = 0; i < numOfReqs; ++i)
    {
        errorRequest(reqIds[i], false);
    }
}

void StubBase::cancelAllRequests( void )
{
    const unsigned int numOfReqs= getNumberOfRequests();
    const unsigned int* reqIds  = getRequestIds();
    for ( unsigned int i = 0; i < numOfReqs; ++ i )
    {
        errorRequest( reqIds[i], true );
    }
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

void StubBase::sendUpdateNotificationOnce( const ProxyAddress & target, unsigned int msgId, const EventDataStream & data, NEService::eResultType result ) const
{
    ResponseEvent * eventElem = createResponseEvent( target, msgId, result, data );
    if ( eventElem != nullptr )
    {
        sendServiceResponse( *eventElem );
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
        StubListenerList::LISTPOS pos = mListListener.firstPosition();
        for ( ; (result == false) && mListListener.isValidPosition(pos); pos = mListListener.nextPosition(pos) )
        {
            const StubBase::Listener & listener = mListListener.valueAtPosition(pos);
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
        StubListenerList::LISTPOS pos     = mListListener.firstPosition();
        for ( ; (hasEntry == false) && mListListener.isValidPosition(pos); pos = mListListener.nextPosition(pos) )
        {
            const StubBase::Listener & listener = mListListener.valueAtPosition(pos);
            hasEntry = (NEService::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr) &&
                       (msgId == listener.mMessageId) &&
                       (notifySource == listener.mProxy);
        }

        mListListener.pushLast(StubBase::Listener(msgId, NEService::SEQUENCE_NUMBER_NOTIFY, notifySource));
        result = (hasEntry == false);
    }

    return result;
}

void StubBase::removeNotificationListener( unsigned int msgId, const ProxyAddress & notifySource )
{
    for (StubListenerList::LISTPOS pos = mListListener.firstPosition(); mListListener.isValidPosition(pos); pos = mListListener.nextPosition(pos) )
    {
        const StubBase::Listener & listener = mListListener.valueAtPosition(pos);
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
