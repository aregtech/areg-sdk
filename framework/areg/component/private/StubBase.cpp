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

#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// StubBase class implementation
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE( areg_component_StubBase_startupServiceInterface );
DEF_TRACE_SCOPE( areg_component_StubBase_shutdownServiceIntrface );
DEF_TRACE_SCOPE( areg_component_StubBase_errorAllRequests );
DEF_TRACE_SCOPE( areg_component_StubBase_sendUpdateEvent);
DEF_TRACE_SCOPE( areg_component_StubBase_sendBusyRespone );
DEF_TRACE_SCOPE( areg_component_StubBase_clientConnected );
DEF_TRACE_SCOPE( areg_component_StubBase_addNotificationListener );

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
    TRACE_SCOPE( areg_component_StubBase_startupServiceInterface );
    TRACE_DBG( "Service with role [ %s ] and interface [ %s ] is started", getServiceRole( ).getString( ), getServiceName( ).getString( ) );

    StubConnectEvent::addListener( static_cast<IEStubEventConsumer &>(self()), holder.getMasterThread() );
}

void StubBase::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE( areg_component_StubBase_shutdownServiceIntrface );
    TRACE_INFO( "Service with role [ %s ] and interface [ %s ] is stopped", getServiceRole().getString(), getServiceName().getString() );
    StubConnectEvent::removeListener( static_cast<IEStubEventConsumer &>(self()), holder.getMasterThread() );
}

void StubBase::errorAllRequests( void )
{
    TRACE_SCOPE( areg_component_StubBase_errorAllRequests );
    TRACE_INFO( "Service [ %s ] with interface [ %s ] send errors to all consumer.", getServiceRole().getString(), getServiceName().getString() );

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
    TRACE_SCOPE( areg_component_StubBase_sendUpdateEvent);
    StubBase::StubListenerList listeners;
    if (findListeners(msgId, listeners) > 0)
    {
        const ProxyAddress & proxy = listeners.firstEntry( ).mProxy;
        TRACE_WARN( "Sends busy message to proxy [ %s ] for the request [ %u ]", ProxyAddress::convAddressToPath( proxy).getString(), msgId);

        ResponseEvent* eventElem = createResponseEvent(proxy, msgId, result, data);
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
        ResponseEvent* eventElem = createResponseEvent(listeners.firstEntry().mProxy, respId, NEService::eResultType::RequestOK, data);
        if (eventElem != nullptr)
        {
            sendResponseNotification(listeners, *eventElem);
            eventElem->destroy();
        }
    }
}

void StubBase::sendBusyRespone( const Listener & whichListener )
{
    TRACE_SCOPE(areg_component_StubBase_sendBusyRespone);
    ResponseEvent* eventElem = createResponseEvent(whichListener.mProxy, whichListener.mMessageId, NEService::eResultType::RequestBusy, EventDataStream::EmptyData);
    if (eventElem != nullptr)
    {
        TRACE_WARN("Sending busy response for request message [ %p ] from source [ %p ] to target [ %p ], sequence [ %u ]"
                    , whichListener.mMessageId
                    , whichListener.mProxy.getTarget()
                    , whichListener.mProxy.getSource()
                    , whichListener.mSequenceNr);

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
    TRACE_SCOPE(areg_component_StubBase_addNotificationListener);

    bool result { false };
    if (notifySource.isValid())
    {
        bool hasEntry{ false };
        auto pos = mListListener.firstPosition();
        for ( ; (hasEntry == false) && mListListener.isValidPosition(pos); pos = mListListener.nextPosition(pos) )
        {
            const StubBase::Listener & listener = mListListener.valueAtPosition(pos);
            hasEntry = (NEService::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr) &&
                       (msgId == listener.mMessageId) &&
                       (notifySource == listener.mProxy);
        }

        if ( hasEntry == false)
        {
            TRACE_DBG("For the message [ %u ] new listener [ %s ] is added"
                        , msgId
                        , ProxyAddress::convAddressToPath(notifySource).getString());

            mListListener.pushLast(StubBase::Listener(msgId, NEService::SEQUENCE_NUMBER_NOTIFY, notifySource));
            result = true;
        }
#if AREG_LOGS
        else
        {
            TRACE_WARN("For the message [ %u ] there is already registered client [ %s ]"
                       , msgId
                       , ProxyAddress::convAddressToPath(notifySource).getString());
        }
#endif  // AREG_LOGS
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

bool StubBase::clientConnected(const ProxyAddress & client, NEService::eServiceConnection status )
{
    bool result{ false };
    if (mAddress == client)
    {
        TRACE_SCOPE(areg_component_StubBase_clientConnected);
        TRACE_DBG("Service consumer [ %s ] connection event with status [ %s ]"
                  , ProxyAddress::convAddressToPath(client).getString()
                  , NEService::getString(status));

        result = true;
        if (NEService::isServiceDisconnected(status))
        {
            clearAllListeners(client);
        }
    }

    return result;
}

void StubBase::processClientConnectEvent( const ProxyAddress & proxyAddress, NEService::eServiceConnection status )
{
    clientConnected( proxyAddress, status );
}

void StubBase::processStubRegisteredEvent(const StubAddress & stubTarget, NEService::eServiceConnection status )
{
    if ( NEService::isServiceConnected( status) )
    {
        ASSERT( stubTarget.isValid() );
        _mapRegisteredStubs.lock();
        _mapRegisteredStubs.unregisterResourceObject(mAddress);

        mAddress = stubTarget;
        
        _mapRegisteredStubs.registerResourceObject(mAddress, this);
        _mapRegisteredStubs.unlock();
    }

    mConnectionStatus = status;
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
