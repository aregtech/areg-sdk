/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ProxyBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Proxy Base class implementation.
 *
 ************************************************************************/
#include "areg/component/ProxyBase.hpp"

#include "areg/base/Version.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/UtilityDefs.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/ServiceRequestEvent.hpp"
#include "areg/component/NotificationEvent.hpp"
#include "areg/component/ProxyListener.hpp"

#include "areg/component/private/ProxyConnectEvent.hpp"
#include "areg/component/private/ComponentInfo.hpp"
#include "areg/component/private/ServiceManager.hpp"

#include "areg/logging/GELog.h"

//////////////////////////////////////////////////////////////////////////
// Predefined constants / statics
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(areg_component_ProxyBase_findOrCreateProxy);
DEF_LOG_SCOPE(areg_component_ProxyBase_createRequestFailureEvent);
DEF_LOG_SCOPE(areg_component_ProxyBase_serviceConnectionUpdated);
DEF_LOG_SCOPE(areg_component_ProxyBase_unregisterListener);
DEF_LOG_SCOPE(areg_component_ProxyBase_prepareListeners);
DEF_LOG_SCOPE(areg_component_ProxyBase_stopProxy);

//////////////////////////////////////////////////////////////////////////
// ProxyBase::Listener class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyBase::Listener class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ProxyBase::Listener::Listener()
    : mMessageId    (static_cast<uint32_t>(areg::FuncIdRange::EmptyFunctionId))
    , mSequenceNr   (areg::SEQUENCE_NUMBER_NOTIFY)
    , mListener     (nullptr)
{
}

ProxyBase::Listener::Listener( const Listener & src )
    : mMessageId    (src.mMessageId)
    , mSequenceNr   (src.mSequenceNr)
    , mListener     (src.mListener)
{
}

ProxyBase::Listener::Listener( Listener && src ) noexcept
    : mMessageId    ( src.mMessageId )
    , mSequenceNr   ( src.mSequenceNr )
    , mListener     ( src.mListener )
{
}

ProxyBase::Listener::Listener( uint32_t msgId )
    : mMessageId    (msgId)
    , mSequenceNr   (areg::SEQUENCE_NUMBER_NOTIFY)
    , mListener     (nullptr)
{
}

ProxyBase::Listener::Listener( uint32_t msgId, const SequenceNumber & seqNr )
    : mMessageId    (msgId)
    , mSequenceNr   (seqNr)
    , mListener     (nullptr)
{
}

ProxyBase::Listener::Listener( uint32_t msgId, const SequenceNumber & seqNr, NotificationConsumer* caller )
    : mMessageId    (msgId)
    , mSequenceNr   (seqNr)
    , mListener     (caller)
{
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase::Listener class, operators
//////////////////////////////////////////////////////////////////////////

ProxyBase::Listener & ProxyBase::Listener::operator = ( const ProxyBase::Listener & src )
{
    mMessageId  = src.mMessageId;
    mSequenceNr = src.mSequenceNr;
    mListener   = src.mListener;

    return (*this);
}

ProxyBase::Listener & ProxyBase::Listener::operator = ( ProxyBase::Listener && src ) noexcept
{
    mMessageId  = src.mMessageId;
    mSequenceNr = src.mSequenceNr;
    mListener   = src.mListener;

    return (*this);
}

bool ProxyBase::Listener::operator == ( const ProxyBase::Listener& other ) const
{
    bool result = this == &other ? true : false;
    if ((result == false) && (other.mMessageId == mMessageId))
    {
        if (other.mSequenceNr == areg::SEQUENCE_NUMBER_ANY)
            result = true;
        else if (other.mSequenceNr == mSequenceNr && other.mListener == mListener)
            result = true;
        else if (other.mSequenceNr == areg::SEQUENCE_NUMBER_NOTIFY && other.mListener == nullptr)
            result = true;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase::ImplProxyMap class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyBase::ServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

AREG_IMPLEMENT_RUNTIME_EVENT(ProxyBase::ServiceAvailableEvent, Event)

ProxyBase::ServiceAvailableEvent::ServiceAvailableEvent( NotificationConsumer & consumer )
    : Event             ( Event::EventType::EventExternal )
    , mNotifyConsumer   ( consumer )
    , mDelayConnectEvent( 0 )
{
}


//////////////////////////////////////////////////////////////////////////
// ProxyBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, static methods
//////////////////////////////////////////////////////////////////////////

ProxyBase::MapProxyResource& ProxyBase::map_proxies()
{
    static MapProxyResource _mapProxies;
    return _mapProxies;
}

ProxyBase::MapThreadProxyList& ProxyBase::thread_proxies()
{
    static MapThreadProxyList   _threadProxies;
    return _threadProxies;
}


std::shared_ptr<ProxyBase> ProxyBase::findOrCreateProxy( const areg::String & roleName
                                                       , const areg::InterfaceData & serviceIfData
                                                       , ProxyListener & connect
                                                       , FuncCreateProxy funcCreate
                                                       , const areg::String & ownerThread /*= areg::String::getEmptyString()*/)
{
    return ProxyBase::findOrCreateProxy(roleName, serviceIfData, connect, funcCreate, DispatcherThread::getDispatcherThread(ownerThread) );
}

std::shared_ptr<ProxyBase> ProxyBase::findOrCreateProxy( const areg::String & roleName
                                                       , const areg::InterfaceData & serviceIfData
                                                       , ProxyListener & connect
                                                       , FuncCreateProxy funcCreate
                                                       , DispatcherThread & ownerThread )
{
    LOG_SCOPE(areg_component_ProxyBase_findOrCreateProxy);

    std::shared_ptr<ProxyBase> proxy{ nullptr };
    if (ownerThread.isValid())
    {
        ProxyAddress Key(serviceIfData, roleName, ownerThread.getName() );
        proxy = map_proxies().findResourceObject(Key);
        if (proxy.get() == nullptr )
        {
            LOG_DBG("No proxy [ %s ] found, creating one in thread [ %u ]", ProxyAddress::convAddressToPath(Key).getString(), ownerThread.getId());
            std::shared_ptr<ProxyBase> newProxy{ funcCreate( roleName, &ownerThread ) };
            if ( newProxy.get() != nullptr )
            {
                proxy.swap( newProxy );
                map_proxies().registerResourceObject( proxy->mProxyAddress, proxy );
                thread_proxies().registerResourceObject( proxy->mDispatcherThread.getName( ), proxy );
            }
        }

        if (proxy.get() != nullptr)
        {
            if (proxy->mListConnect.addIfUnique(&connect))
            {
                LOG_DBG("Add Service Connect notification for client [ %p ]", &connect);

                static_cast<void>(proxy->addListener( static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection)
                                                    , areg::SEQUENCE_NUMBER_NOTIFY
                                                    , static_cast<NotificationConsumer *>(&connect), true ));
                ++ proxy->mProxyInstCount;
                proxy->mIsStopped = false;

                if ( proxy->mProxyInstCount == 1 )
                {
                    proxy->registerServiceListeners( );
                    ServiceManager::requestRegisterClient( proxy->getProxyAddress( ) );
                }
                else if ( proxy->isConnected() )
                {
                    proxy->sendServiceAvailableEvent( proxy->createServiceAvailableEvent(connect) );
                }
            }
            else
            {
                LOG_WARN("The client [ %p ] is already registered for service connection notification", &connect);
            }
        }
    }

    return proxy;
}


int32_t ProxyBase::findThreadProxies(DispatcherThread & ownerThread, areg::ArrayList<std::shared_ptr<ProxyBase>> & threadProxyList )
{
    ThreadProxyList * proxyList = ProxyBase::thread_proxies().findResource(ownerThread.getName());
    int32_t result = proxyList != nullptr ? static_cast<int32_t>(proxyList->getSize()) : 0;
    if ( result > 0 )
    {
        threadProxyList = static_cast<const areg::ArrayList<std::shared_ptr<ProxyBase>> &>(*proxyList);
    }

    return result;
}

RemoteResponseEvent * ProxyBase::createRequestFailureEvent(const ProxyAddress & target, uint32_t msgId, areg::ResultType errCode, const SequenceNumber & seqNr)
{
    LOG_SCOPE(areg_component_ProxyBase_createRequestFailureEvent);

    RemoteResponseEvent * result = nullptr;
    std::shared_ptr<ProxyBase> proxy = ProxyBase::findProxyByAddress(target);
    if (proxy.get() != nullptr)
    {
        result = proxy->createRemoteRequestFailedEvent(target, msgId, errCode, seqNr);
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ProxyBase::ProxyBase(const areg::String & roleName, const areg::InterfaceData & serviceIfData, DispatcherThread * ownerThread /*= nullptr*/ )

    : ProxyEventConsumer  ( mProxyAddress )

    , mProxyAddress     ( serviceIfData, roleName, (ownerThread != nullptr) && (ownerThread->isValid()) ? ownerThread->getName() : areg::String::getEmptyString() )
    , mStubAddress      ( areg::StubAddress::getInvalidStubAddress() )
    , mSequenceCount    ( 0 )
    , mListenerList     ( serviceIfData.idAttributeCount + serviceIfData.idResponseCount )
    , mListConnect      (   )
    , mProxyInstCount   ( 0 )

    , mIsStopped        ( false )

    , mProxyData        ( serviceIfData )

    , mDispatcherThread ( (ownerThread != nullptr) && (ownerThread->isValid()) ? *ownerThread : DispatcherThread::getDispatcherThread( mProxyAddress.getThread()) )
    , mConnectionStatus ( areg::ServiceConnectionState::Unknown )
    , mIsConnected      ( false )
{
    ASSERT(mDispatcherThread.isValid());
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, methods
//////////////////////////////////////////////////////////////////////////

void ProxyBase::registerServiceListeners()
{
    ProxyConnectEvent::addListener( static_cast<EventConsumer &>(self( )), mDispatcherThread );
}

void ProxyBase::unregisterServiceListeners()
{
    ProxyConnectEvent::removeListener( static_cast<EventConsumer &>(self( )), mDispatcherThread );
    ProxyBase::ServiceAvailableEvent::removeListener( static_cast<EventConsumer &>(self( )), mDispatcherThread );
}

void ProxyBase::freeProxy( ProxyListener & connect )
{
    int32_t exists = mListConnect.find(&connect, 0);
    if ( exists >= 0 )
    {
        mListConnect.removeAt(static_cast<uint32_t>(exists));
        connect.serviceConnected(areg::ServiceConnectionState::Disconnected, self());
    }

    removeListener( static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection)
                  , areg::SEQUENCE_NUMBER_NOTIFY
                  , static_cast<NotificationConsumer *>(&connect));

    std::shared_ptr<ProxyBase> proxy = ProxyBase::findProxyByAddress( mProxyAddress );

    if ((proxy.use_count() == 1) || (mProxyInstCount == 1))
    {
        if ( false == mIsStopped )
        {
            stopAllServiceNotifications( );
            unregisterServiceListeners( );
            mListenerList.clear();

            ServiceManager::requestUnregisterClient( getProxyAddress( ), areg::DisconnectReason::ConsumerDisconnected );
            mDispatcherThread.removeConsumer( *this );
        }

        setConnectionStatus( areg::ServiceConnectionState::Disconnected );
        mIsStopped   = true;

        mProxyInstCount = 0;
        map_proxies().unregisterResourceObject(mProxyAddress);
        thread_proxies().unregisterResourceObject( mDispatcherThread.getName( ), proxy, true );
    }
    else if ( mProxyInstCount > 0 )
    {
        -- mProxyInstCount;
    }
}

void ProxyBase::terminateSelf()
{
    if (mProxyInstCount != 0)
    {
        mListenerList.clear();
        if (mIsStopped == false)
        {
            ServiceManager::requestUnregisterClient(getProxyAddress(), areg::DisconnectReason::ConsumerDisconnected );
        }

        setConnectionStatus( areg::ServiceConnectionState::Disconnected );
        mIsStopped      = true;
        mProxyInstCount = 0;

        delete this;
    }
}

void ProxyBase::serviceConnectionUpdated( const areg::StubAddress & server, const Channel & channel, areg::ServiceConnectionState status )
{
    LOG_SCOPE(areg_component_ProxyBase_serviceConnectionUpdated);

    ASSERT(server.isValid());
    if ( server.isProxyCompatible( getProxyAddress() ) )
    {
        LOG_DBG("The proxy [ %s ] have got [ %s ] status update notification event from stub [ %s ]"
                    , ProxyAddress::convAddressToPath(getProxyAddress()).getString()
                    , areg::getString(status)
                    , areg::StubAddress::convAddressToPath(server).getString());

        ASSERT(channel.getTarget() == server.getSource() || status != areg::ServiceConnectionState::Connected);
        mProxyAddress.setChannel(channel);
        setConnectionStatus( status );
        bool proxyConnected{ isConnected() };
        if ( proxyConnected )
        {
            mStubAddress = server;
        }
        else
        {
            mStubAddress = areg::StubAddress::getInvalidStubAddress();
            mProxyData.resetStates();
        }

        // first collect listeners, because on connect / disconnect
        // the listener list might be updated!
        areg::ArrayList<ProxyBase::Listener> conListeners;
        uint32_t index = 0;
        for (index = 0 ; index < mListenerList.getSize(); ++ index)
        {
            ProxyBase::Listener& listener = mListenerList[index];
            if ( areg::isConnectNotifyId(listener.mMessageId) )
            {
                ASSERT(listener.mListener != nullptr);
                conListeners.add(listener);
            }
        }

        LOG_DBG("Notifying [ %d ] clients the service connection", conListeners.getSize());

        for (index = 0 ; index < conListeners.getSize(); ++ index)
        {
            const ProxyBase::Listener& listener = conListeners[index];
            ProxyListener * connect = static_cast<ProxyListener *>(listener.mListener);
            if ( proxyConnected )
            {
                mListConnect.addIfUnique(connect);
                connect->serviceConnected( status, *this );
            }
            else
            {
                mListConnect.removeElem(connect, 0);
                connect->serviceConnected( status, *this );
                mListenerList.addIfUnique(listener);
            }
        }
    }
}

void ProxyBase::setNotification( uint32_t msgId, NotificationConsumer* caller, bool alwaysNotify /*= false*/ )
{
    if (isConnected())
    {
        bool hasListener{ hasNotificationListener(msgId) };
        if ( addListener(msgId, areg::SEQUENCE_NUMBER_NOTIFY, caller, hasListener) )
        {
            // new listener, if attribute, send actual data.
            if (areg::isAttributeId(msgId))
            {
                sendNotificationEvent( msgId
                                     , mProxyData.getAttributeState(msgId) == areg::DataState::DataIsOK ? areg::ResultType::DataOK : areg::ResultType::DataInvalid
                                     , areg::SEQUENCE_NUMBER_NOTIFY, caller);
            }

            // assign only if there was no listener
            if (hasListener == false)
            {
                startNotification(msgId);
            }
        }
        else if ( alwaysNotify )
        {
            // the listener already exists
            if ( areg::isAttributeId(msgId) )
            {
                sendNotificationEvent( msgId
                                     , mProxyData.getAttributeState(msgId) == areg::DataState::DataIsOK ? areg::ResultType::DataOK : areg::ResultType::DataInvalid
                                     , areg::SEQUENCE_NUMBER_NOTIFY, caller);
            }
            else if ( areg::isResponseId(msgId) && (mProxyData.getParamState(msgId) == areg::DataState::DataIsOK) )
            {
                sendNotificationEvent(  msgId, areg::ResultType::RequestOK, areg::SEQUENCE_NUMBER_NOTIFY, caller);
            }
            else
            {
                // ignore
            }
        }
    }
    else
    {
        // ignore
    }
}

void ProxyBase::clearNotification( uint32_t msgId, NotificationConsumer* caller )
{
    removeListener(msgId, areg::SEQUENCE_NUMBER_NOTIFY, caller);
    if (hasNotificationListener(msgId) == false)
    {
        stopNotification(msgId);
        mProxyData.setDataState(msgId, areg::DataState::DataIsUnavailable);
    }
}

void ProxyBase::unregisterListener( NotificationConsumer *consumer )
{
    LOG_SCOPE(areg_component_ProxyBase_unregisterListener);
    LOG_DBG("Unregisters proxy client [ %p ]", consumer);

    uint32_t index = 0;
    while (index < mListenerList.getSize())
    {
        const ProxyBase::Listener& elem = mListenerList[index];
        if (elem.mListener == consumer)
        {
            uint32_t msgId = elem.mMessageId;
            mListenerList.removeAt(index);
            LOG_DBG("Removes proxy client listener of message [ %u ] at index [ %d ]", msgId, index);

            if (hasNotificationListener(msgId) == false)
            {
                stopNotification(msgId);
                mProxyData.setDataState(msgId, areg::DataState::DataIsUnavailable);
            }
        }
        else
        {
            index ++;
        }
    }
}

uint32_t ProxyBase::prepareListeners( ProxyBase::ProxyListenerList& out_listenerList, uint32_t msgId, const SequenceNumber & seqNrToSearch )
{
    LOG_SCOPE(areg_component_ProxyBase_prepareListeners);
    ProxyBase::Listener searchListener(msgId, areg::SEQUENCE_NUMBER_ANY);
    for (uint32_t i = 0; i < mListenerList.getSize(); ++ i )
    {
        const ProxyBase::Listener & elem = mListenerList.getAt(i);
        if ( elem == searchListener )
        {
            if ( elem.mSequenceNr == areg::SEQUENCE_NUMBER_NOTIFY )
            {
                out_listenerList.add( elem );
            }
            else if ( elem.mSequenceNr == seqNrToSearch )
            {
                out_listenerList.add( elem );
                mListenerList.removeAt(i --);   // <== go one index back, because remove one element
            }
        }
    }

    LOG_DBG("Prepared [ %d ] proxy clients for message [ %u ] and sequence number [ %llu ]", out_listenerList.getSize(), msgId, seqNrToSearch);

    return out_listenerList.getSize();
}

void ProxyBase::notifyListeners( uint32_t respId, areg::ResultType result, const SequenceNumber & seqNrToSearch )
{
    ProxyBase::ProxyListenerList listenerList;
    prepareListeners(listenerList, respId, seqNrToSearch);
    for (uint32_t i = 0; i < listenerList.getSize(); ++ i)
    {
        const ProxyBase::Listener & elem = listenerList.getAt(i);
        sendNotificationEvent(respId, result, seqNrToSearch, elem.mListener);
    }
}

void ProxyBase::sendNotificationEvent( uint32_t msgId, areg::ResultType resType, const SequenceNumber & seqNr, NotificationConsumer* caller )
{
    NotificationEventData data(self(), resType, msgId, seqNr);
    NotificationEvent* eventElem = createNotificationEvent(data);
    if (eventElem != nullptr)
    {
        if (caller != nullptr)
        {
            eventElem->setEventConsumer(static_cast<EventConsumer *>(caller));
        }

        static_cast<Event *>(eventElem)->deliverEvent();
    }
}

#ifdef  DEBUG
void ProxyBase::processProxyEvent( ProxyEvent& eventElem )
{
    ASSERT(eventElem.getTargetProxy() == getProxyAddress());
}
#else   // !DEBUG
void ProxyBase::processProxyEvent( ProxyEvent& /*eventElem*/ )
{
}
#endif  // DEBUG

void ProxyBase::processGenericEvent( Event& eventElem )
{
    ProxyBase::ServiceAvailableEvent * serviceEvent = AREG_RUNTIME_CAST( &eventElem, ProxyBase::ServiceAvailableEvent );
    if ( serviceEvent != nullptr )
    {
        processServiceAvailableEvent( serviceEvent->getConsumer(), serviceEvent->getEventDalay() );
    }
}

std::shared_ptr<ProxyBase> ProxyBase::findProxyByAddress( const ProxyAddress& proxyAddress )
{
    return map_proxies().findResourceObject(proxyAddress);
}

void ProxyBase::sendRequestEvent( uint32_t reqId, const EventDataStream& args, NotificationConsumer *caller )
{
    ServiceRequestEvent* evenElem = createRequestEvent(args, reqId);
    if ( evenElem != nullptr )
    {
        uint32_t respId = getProxyData().getResponseId(static_cast<uint32_t>(reqId));
        ASSERT(areg::isResponseId(respId) || (respId == areg::RESPONSE_ID_NONE));

        if (respId != areg::RESPONSE_ID_NONE)
        {
            static_cast<void>( addListener(respId, ++ mSequenceCount, caller, true) );
            evenElem->setSequenceNumber(mSequenceCount);
        }

        mProxyAddress.deliverServiceEvent(*evenElem);
    }
}

void ProxyBase::sendNotificationRequestEvent( uint32_t msgId, areg::RequestType reqType )
{
    ServiceRequestEvent* notifyEvent = createNotificationRequestEvent(msgId, reqType);
    if (notifyEvent != nullptr)
    {
        mProxyAddress.deliverServiceEvent( *notifyEvent );
    }
}

void ProxyBase::sendServiceAvailableEvent( ProxyBase::ServiceAvailableEvent * eventInstance )
{
    if ( eventInstance != nullptr )
    {
        eventInstance->addListener( self(), mDispatcherThread);
        eventInstance->setEventConsumer(this);
        eventInstance->registerForThread(&mDispatcherThread);
        eventInstance->deliverEvent( );
    }
}

bool ProxyBase::isServiceListenerRegistered( NotificationConsumer & caller ) const
{
    bool result = false;
    for (uint32_t i = 0; i < mListenerList.getSize(); ++i)
    {
        const ProxyBase::Listener & listener = mListenerList[i];
        if (areg::isConnectNotifyId(listener.mMessageId) && (&caller == listener.mListener))
        {
            result = true;
            break;
        }
    }

    return result;
}

void ProxyBase::processServiceAvailableEvent( NotificationConsumer & consumer, uint32_t delayEvent)
{
    if (isConnected() && isServiceListenerRegistered( consumer ) )
    {
        if (delayEvent != areg::DO_NOT_WAIT)
        {
            areg::Thread::sleep(delayEvent);
        }

        static_cast<ProxyListener&>(consumer).serviceConnected(getConnectionStatus(), self());
    }
}

RemoteResponseEvent * ProxyBase::createRemoteResponseEvent(const areg::InStream & /* stream */) const
{
    return nullptr;
}

RemoteResponseEvent * ProxyBase::createRemoteRequestFailedEvent(  const ProxyAddress &  /* addrProxy */
                                                                , uint32_t          /* msgId */
                                                                , areg::ResultType/* reason */
                                                                , const SequenceNumber &/* seqNr */ ) const
{
    return nullptr;
}

void ProxyBase::stopProxy()
{
    LOG_SCOPE(areg_component_ProxyBase_stopProxy);

    if ( false == mIsStopped )
    {
        LOG_WARN("Going to stop proxy [ %s ]", ProxyAddress::convAddressToPath(mProxyAddress).getString());

        for (uint32_t i = 0 ; i < mListConnect.getSize(); ++ i)
        {
            ProxyListener * listener = mListConnect.getAt(i);
            ASSERT(listener != nullptr);
            listener->serviceConnected( areg::ServiceConnectionState::Disconnected, *this);
        }

        mListConnect.clear();

        setConnectionStatus( areg::ServiceConnectionState::Disconnected );
        mIsStopped = true;

        stopAllServiceNotifications( );
        unregisterServiceListeners( );
        mListenerList.clear();
        ServiceManager::requestUnregisterClient( getProxyAddress( ), areg::DisconnectReason::ConsumerDisconnected );
        mDispatcherThread.removeConsumer( *this );

        mStubAddress = areg::StubAddress::getInvalidStubAddress();
        mProxyData.resetStates();
        // mProxyAddress.setChannel(Channel::getInvalidChannel());
    }
}
