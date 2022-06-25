/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ProxyBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Proxy Base class implementation.
 *
 ************************************************************************/
#include "areg/component/ProxyBase.hpp"

#include "areg/base/Version.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/NEUtilities.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/ServiceRequestEvent.hpp"
#include "areg/component/NotificationEvent.hpp"
#include "areg/component/IEProxyListener.hpp"

#include "areg/component/private/ProxyConnectEvent.hpp"
#include "areg/component/private/ComponentInfo.hpp"
#include "areg/component/private/ServiceManager.hpp"

#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// Predefined constants / statics
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(areg_component_ProxyBase_findOrCreateProxy);
DEF_TRACE_SCOPE(areg_component_ProxyBase_createRequestFailureEvent);
DEF_TRACE_SCOPE(areg_component_ProxyBase_serviceConnectionUpdated);
DEF_TRACE_SCOPE(areg_component_ProxyBase_unregisterListener);
DEF_TRACE_SCOPE(areg_component_ProxyBase_prepareListeners);
DEF_TRACE_SCOPE(areg_component_ProxyBase_stopProxy);

//////////////////////////////////////////////////////////////////////////
// ProxyBase class statics
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Resource of registered Proxies in the system.
 **/
ProxyBase::MapProxyResource     ProxyBase::_mapRegisteredProxies;

/**
 * \brief   List of created proxies per thread.
 **/
ProxyBase::MapThreadProxyList   ProxyBase::_mapThreadProxies;

//////////////////////////////////////////////////////////////////////////
// ProxyBase::Listener class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyBase::Listener class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ProxyBase::Listener::Listener( void )
    : mMessageId    (static_cast<unsigned int>(NEService::eFuncIdRange::EmptyFunctionId))
    , mSequenceNr   (NEService::SEQUENCE_NUMBER_NOTIFY)
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

ProxyBase::Listener::Listener( unsigned int msgId )
    : mMessageId    (msgId)
    , mSequenceNr   (NEService::SEQUENCE_NUMBER_NOTIFY)
    , mListener     (nullptr)
{
}

ProxyBase::Listener::Listener( unsigned int msgId, unsigned int seqNr )
    : mMessageId    (msgId)
    , mSequenceNr   (seqNr)
    , mListener     (nullptr)
{
}

ProxyBase::Listener::Listener( unsigned int msgId, unsigned int seqNr, IENotificationEventConsumer* caller )
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
    if (result == false && other.mMessageId == mMessageId)
    {
        if (other.mSequenceNr == NEService::SEQUENCE_NUMBER_ANY)
            result = true;
        else if (other.mSequenceNr == mSequenceNr && other.mListener == mListener)
            result = true;
        else if (other.mSequenceNr == NEService::SEQUENCE_NUMBER_NOTIFY && other.mListener == nullptr)
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

IMPLEMENT_RUNTIME_EVENT(ProxyBase::ServiceAvailableEvent, ThreadEventBase)

ProxyBase::ServiceAvailableEvent::ServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ThreadEventBase   ( Event::eEventType::EventExternal )
    , mConsumer         ( consumer )
{
}


//////////////////////////////////////////////////////////////////////////
// ProxyBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, static methods
//////////////////////////////////////////////////////////////////////////
ProxyBase * ProxyBase::findOrCreateProxy( const char * roleName
                                        , const NEService::SInterfaceData & serviceIfData
                                        , IEProxyListener & connect
                                        , FuncCreateProxy funcCreate
                                        , const char * ownerThread /*= nullptr*/)
{
    return ProxyBase::findOrCreateProxy(roleName, serviceIfData, connect, funcCreate, DispatcherThread::getDispatcherThread(ownerThread) );
}

ProxyBase * ProxyBase::findOrCreateProxy( const char * roleName
                                        , const NEService::SInterfaceData & serviceIfData
                                        , IEProxyListener & connect
                                        , FuncCreateProxy funcCreate
                                        , DispatcherThread & ownerThread )
{
    TRACE_SCOPE(areg_component_ProxyBase_findOrCreateProxy);

    ProxyBase*   proxy = nullptr;
    if (ownerThread.isValid())
    {
        ProxyAddress Key(serviceIfData, roleName, ownerThread.getName().getString() );
        proxy = _mapRegisteredProxies.findResourceObject(Key);
        if (proxy == nullptr)
        {
            TRACE_DBG("No proxy [ %s ] found, creating one in thread [ %u ]", ProxyAddress::convAddressToPath(Key).getString(), ownerThread.getId());
            proxy = funcCreate(roleName, &ownerThread);
        }

        if (proxy != nullptr)
        {
            if (proxy->mListConnect.addUnique(&connect))
            {
                TRACE_DBG("Add Service Connect notification for client [ %p ]", &connect);

                static_cast<void>(proxy->addListener( static_cast<unsigned int>(NEService::eFuncIdRange::ServiceNotifyConnection)
                                                    , NEService::SEQUENCE_NUMBER_NOTIFY
                                                    , static_cast<IENotificationEventConsumer *>(&connect) ));
                ++ proxy->mProxyInstCount;
                proxy->mIsStopped = false;

                if ( proxy->mProxyInstCount == 1 )
                {
                    proxy->registerServiceListeners( );
                    ServiceManager::requestRegisterClient( proxy->getProxyAddress( ) );
                }
                else if ( proxy->mIsConnected )
                {
                    proxy->sendServiceAvailableEvent( proxy->createServiceAvailableEvent(connect) );
                }
            }
            else
            {
                TRACE_WARN("The client [ %p ] is already registeref foro service connection notification", &connect);
            }
        }
    }

    return proxy;
}


int ProxyBase::findThreadProxies(DispatcherThread & ownerThread, TEArrayList<ProxyBase *> & OUT threadProxyList )
{
    ThreadProxyList * proxyList = ProxyBase::_mapThreadProxies.findResource(ownerThread.getName());
    int result = proxyList != nullptr ? proxyList->getSize() : 0;
    if ( result > 0 )
    {
        threadProxyList = static_cast<const TEArrayList<ProxyBase *> &>(*proxyList);
    }

    return result;
}

RemoteResponseEvent * ProxyBase::createRequestFailureEvent(const ProxyAddress & target, unsigned int msgId, NEService::eResultType errCode, unsigned int seqNr)
{
    TRACE_SCOPE(areg_component_ProxyBase_createRequestFailureEvent);

    RemoteResponseEvent * result = nullptr;

    ProxyBase::_mapRegisteredProxies.lock();
    
    ProxyBase * proxy = ProxyBase::findProxyByAddress(target);
    if (proxy != nullptr)
    {
        result = proxy->createRemoteRequestFailedEvent(target, msgId, errCode, seqNr);
    }

    ProxyBase::_mapRegisteredProxies.unlock();

    return result;
}


//////////////////////////////////////////////////////////////////////////
// ProxyBase class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ProxyBase::ProxyBase(const char* roleName, const NEService::SInterfaceData & serviceIfData, DispatcherThread * ownerThread /*= nullptr*/ )

    : IEProxyEventConsumer  ( mProxyAddress )
    
    , mProxyAddress     ( serviceIfData, roleName, (ownerThread != nullptr) && (ownerThread->isValid()) ? ownerThread->getName().getString() : nullptr )
    , mStubAddress      ( StubAddress::INVALID_STUB_ADDRESS )
    , mSequenceCount    ( 0 )
    , mListenerList     ( static_cast<int>(serviceIfData.idAttributeCount + serviceIfData.idResponseCount) )
    , mListConnect      (   )
    , mIsConnected      ( false )
    , mIsStopped        ( false )

    , mProxyData        ( serviceIfData )

    , mDispatcherThread ( (ownerThread != nullptr) && (ownerThread->isValid()) ? *ownerThread : DispatcherThread::getDispatcherThread( static_cast<const char *>(mProxyAddress.getThread())) )

    , mProxyInstCount   ( 0 )
{
    ASSERT(mDispatcherThread.isValid());
    _mapRegisteredProxies.registerResourceObject(mProxyAddress, this);
    _mapThreadProxies.registerResourceObject(mDispatcherThread.getName(), this);
}

ProxyBase::~ProxyBase( void )
{
    _mapRegisteredProxies.unregisterResourceObject(mProxyAddress);
    _mapThreadProxies.unregisterResourceObject(mDispatcherThread.getName(), this, true);
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, methods
//////////////////////////////////////////////////////////////////////////

void ProxyBase::registerServiceListeners( void )
{
    ProxyConnectEvent::addListener( static_cast<IEEventConsumer &>(self( )), mDispatcherThread );
}

void ProxyBase::unregisterServiceListeners( void )
{
    ProxyConnectEvent::removeListener( static_cast<IEEventConsumer &>(self( )), mDispatcherThread );
    ProxyBase::ServiceAvailableEvent::removeListener( static_cast<IEEventConsumer &>(self( )), mDispatcherThread );
}

void ProxyBase::freeProxy( IEProxyListener & connect )
{
    int exists = mListConnect.find(&connect, 0);
    if ( exists >= 0 )
    {
        mListConnect.removeAt(exists);
        connect.serviceConnected(false, self());
    }
    
    removeListener( static_cast<unsigned int>(NEService::eFuncIdRange::ServiceNotifyConnection)
                  , NEService::SEQUENCE_NUMBER_NOTIFY
                  , static_cast<IENotificationEventConsumer *>(&connect));

    if (mProxyInstCount == 1)
    {
        OUTPUT_WARN("The proxy [ %s ] instance count is zero, going to delete object at address [ %p]"
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString()
                        , this);

        if ( false == mIsStopped )
        {
            stopAllServiceNotifications( );
            unregisterServiceListeners( );
            mListenerList.clear();

            ServiceManager::requestUnregisterClient( getProxyAddress( ) );
            mDispatcherThread.removeConsumer( *this );
        }

        mIsConnected = false;
        mIsStopped   = true;

        -- mProxyInstCount;
        delete this;
    }
    else if ( mProxyInstCount > 0 )
    {
        -- mProxyInstCount;
    }
}

void ProxyBase::serviceConnectionUpdated( const StubAddress & server, const Channel & channel, NEService::eServiceConnection status )
{
    TRACE_SCOPE(areg_component_ProxyBase_serviceConnectionUpdated);

    ASSERT(server.isValid());
    if ( server.isProxyCompatible( getProxyAddress() ) )
    {
        TRACE_DBG("The proxy [ %s ] have got [ %s ] status update notification event from stub [ %s ]"
                    , ProxyAddress::convAddressToPath(getProxyAddress()).getString()
                    , NEService::getString(status)
                    , StubAddress::convAddressToPath(server).getString());

        ASSERT(channel.getTarget() == server.getSource() || status != NEService::eServiceConnection::ServiceConnected);
        mProxyAddress.setChannel(channel);
        if ( status == NEService::eServiceConnection::ServiceConnected )
        {
            mIsConnected = true;
            mStubAddress = server;
        }
        else
        {
            mIsConnected = false;
            mStubAddress = StubAddress::INVALID_STUB_ADDRESS;
            mProxyData.resetStates();
        }

        // first collect listeners, because on connect / disconnect
        // the listener list might be updated!
        TEArrayList<ProxyBase::Listener> conListeners;
        uint32_t index = 0;
        for (index = 0 ; index < mListenerList.getSize(); ++ index)
        {
            ProxyBase::Listener& listener = mListenerList[index];
            if ( NEService::isConnectNotifyId(listener.mMessageId) )
            {
                ASSERT(listener.mListener != nullptr);
                conListeners.add(listener);
            }
        }

        TRACE_DBG("Notifying [ %d ] clients the service connection", conListeners.getSize());

        for (index = 0 ; index < conListeners.getSize(); ++ index)
        {
            IEProxyListener * listener = static_cast<IEProxyListener *>(conListeners[index].mListener);
            if (mIsConnected)
            {
                mListConnect.addUnique(listener);
            }
            else
            {
                mListConnect.removeElem(listener, 0);
            }

            listener->serviceConnected(mIsConnected, *this);
        }
    }
}

void ProxyBase::setNotification( unsigned int msgId, IENotificationEventConsumer* caller, bool alwaysNotify /*= false*/ )
{
    if (isConnected())
    {
        if ( addListener(msgId, NEService::SEQUENCE_NUMBER_NOTIFY, caller) )
        {
            startNotification(msgId);
        }
        else if ( alwaysNotify )
        {
            // the listener already exists
            if ( NEService::isAttributeId(msgId) )
            {
                sendNotificationEvent( msgId
                                     , mProxyData.getAttributeState(msgId) == NEService::eDataStateType::DataIsOK ? NEService::eResultType::DataOK : NEService::eResultType::DataInvalid
                                     , NEService::SEQUENCE_NUMBER_NOTIFY, caller);
            }
            else if ( NEService::isResponseId(msgId) && (mProxyData.getParamState(msgId) == NEService::eDataStateType::DataIsOK) )
            {
                sendNotificationEvent(  msgId, NEService::eResultType::RequestOK, NEService::SEQUENCE_NUMBER_NOTIFY, caller);
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

void ProxyBase::clearNotification( unsigned int msgId, IENotificationEventConsumer* caller )
{
    removeListener(msgId, NEService::SEQUENCE_NUMBER_NOTIFY, caller);
    if (hasNotificationListener(msgId) == false)
    {
        stopNotification(msgId);
        mProxyData.setDataState(msgId, NEService::eDataStateType::DataIsUnavailable);
    }
}

void ProxyBase::unregisterListener( IENotificationEventConsumer *consumer )
{
    TRACE_SCOPE(areg_component_ProxyBase_unregisterListener);
    TRACE_DBG("Unregisters proxy client [ %p ]", consumer);

    uint32_t index = 0;
    while (index < mListenerList.getSize())
    {
        const ProxyBase::Listener& elem = mListenerList[index];
        if (elem.mListener == consumer)
        {
            unsigned int msgId = elem.mMessageId;
            mListenerList.removeAt(index);
            TRACE_DBG("Removes proxy client listener of message [ %u ] at index [ %d ]", msgId, index);

            if (hasNotificationListener(msgId) == false)
            {
                stopNotification(msgId);
                mProxyData.setDataState(msgId, NEService::eDataStateType::DataIsUnavailable);
            }
        }
        else
        {
            index ++;
        }
    }
}

int ProxyBase::prepareListeners( ProxyBase::ProxyListenerList& out_listenerList, unsigned int msgId, unsigned int seqNrToSearch )
{
    TRACE_SCOPE(areg_component_ProxyBase_prepareListeners);
    ProxyBase::Listener searchListener(msgId, NEService::SEQUENCE_NUMBER_ANY);
    for (uint32_t i = 0; i < mListenerList.getSize(); ++ i )
    {
        const ProxyBase::Listener & elem = mListenerList.getAt(i);
        if ( elem == searchListener )
        {
            if ( elem.mSequenceNr == NEService::SEQUENCE_NUMBER_NOTIFY )
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

    TRACE_DBG("Prepared [ %d ] proxy clients for message [ %u ] and sequence numbe [ %u ]",out_listenerList.getSize(), msgId, seqNrToSearch);

    return out_listenerList.getSize();
}

void ProxyBase::notifyListeners( unsigned int respId, NEService::eResultType result, unsigned int seqNrToSearch )
{
    ProxyBase::ProxyListenerList listenerList;
    prepareListeners(listenerList, respId, seqNrToSearch);
    for (uint32_t i = 0; i < listenerList.getSize(); ++ i)
    {
        const ProxyBase::Listener & elem = listenerList.getAt(i);
        sendNotificationEvent(respId, result, seqNrToSearch, elem.mListener);
    }
}

void ProxyBase::sendNotificationEvent( unsigned int msgId, NEService::eResultType resType, unsigned int seqNr, IENotificationEventConsumer* caller )
{
    NotificationEventData data(self(), resType, msgId, seqNr);
    NotificationEvent* eventElem = createNotificationEvent(data);
    if (eventElem != nullptr)
    {
        if (caller != nullptr)
        {
            eventElem->setEventConsumer(static_cast<IEEventConsumer *>(caller));
        }
        static_cast<Event *>(eventElem)->deliverEvent();
    }
}

#ifdef  DEBUG
void ProxyBase::processProxyEvent( ProxyEvent& eventElem )
{
    ASSERT(eventElem.getTargetProxy() == getProxyAddress());
}
#else
void ProxyBase::processProxyEvent( ProxyEvent& /*eventElem*/ )
{
}
#endif

void ProxyBase::processGenericEvent( Event& eventElem )
{
    ProxyBase::ServiceAvailableEvent * serviceEvent = RUNTIME_CAST( &eventElem, ProxyBase::ServiceAvailableEvent );
    if ( serviceEvent != nullptr )
    {
        processServiceAvailableEvent( serviceEvent->getConsumer() );
    }
}

ProxyBase* ProxyBase::findProxyByAddress( const ProxyAddress& proxyAddress )
{
    return _mapRegisteredProxies.findResourceObject(proxyAddress);
}

void ProxyBase::sendRequestEvent( unsigned int reqId, const EventDataStream& args, IENotificationEventConsumer *caller )
{
    ServiceRequestEvent* evenElem = createRequestEvent(args, reqId);
    if ( evenElem != nullptr )
    {
        unsigned int respId = getProxyData().getResponseId(static_cast<unsigned int>(reqId));
        ASSERT(NEService::isResponseId(respId) || (respId == NEService::RESPONSE_ID_NONE));

        if (respId != NEService::RESPONSE_ID_NONE)
        {
            static_cast<void>( addListener(respId, ++ mSequenceCount, caller) );
            evenElem->setSequenceNumber(mSequenceCount);
        }

        mProxyAddress.deliverServiceEvent(*evenElem);
    }
}

void ProxyBase::sendNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
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

bool ProxyBase::isServiceListenerRegistered( IENotificationEventConsumer & caller ) const
{
    bool result = false;
    for (uint32_t i = 0; (result == false) && (i < mListenerList.getSize()); ++ i )
    {
        const ProxyBase::Listener & listener = mListenerList[i];
        result = NEService::isConnectNotifyId( listener.mMessageId ) && (&caller == listener.mListener);
    }
    return result;
}

void ProxyBase::processServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    if ( mIsConnected && isServiceListenerRegistered(consumer) )
        static_cast<IEProxyListener &>(consumer).serviceConnected(true, self());
}

RemoteResponseEvent * ProxyBase::createRemoteResponseEvent(const IEInStream & /* stream */) const
{
    return nullptr;
}

RemoteResponseEvent * ProxyBase::createRemoteRequestFailedEvent(  const ProxyAddress &  /* addrProxy */
                                                                , unsigned int          /* msgId */
                                                                , NEService::eResultType/* reason */
                                                                , unsigned int          /* seqNr */ ) const
{
    return nullptr;
}

void ProxyBase::stopProxy(void)
{
    TRACE_SCOPE(areg_component_ProxyBase_stopProxy);

    if ( false == mIsStopped )
    {
        TRACE_WARN("Going to stop proxy [ %s ]", ProxyAddress::convAddressToPath(mProxyAddress).getString());

        for (uint32_t i = 0 ; i < mListConnect.getSize(); ++ i)
        {
            IEProxyListener * listener = mListConnect.getAt(i);
            ASSERT(listener != nullptr);
            listener->serviceConnected(false, *this);
        }

        mListConnect.clear();

        mIsConnected = false;
        mIsStopped   = true;

        stopAllServiceNotifications( );
        unregisterServiceListeners( );
        mListenerList.clear();
        ServiceManager::requestUnregisterClient( getProxyAddress( ) );
        mDispatcherThread.removeConsumer( *this );

        mStubAddress = StubAddress::INVALID_STUB_ADDRESS;
        mProxyData.resetStates();
        mProxyAddress.setChannel(Channel::INVALID_CHANNEL);
    }
}
