/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StubBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Stub Base implementation.
 *
 ************************************************************************/
#include "areg/component/StubBase.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/EventDataStream.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/private/StubConnectEvent.hpp"

#include "areg/logging/GELog.h"

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // StubBase class implementation
    //////////////////////////////////////////////////////////////////////////

    DEF_LOG_SCOPE( areg_component_StubBase_startupServiceInterface );
    DEF_LOG_SCOPE( areg_component_StubBase_shutdownServiceIntrface );
    DEF_LOG_SCOPE( areg_component_StubBase_errorAllRequests );
    DEF_LOG_SCOPE( areg_component_StubBase_sendUpdateEvent);
    DEF_LOG_SCOPE( areg_component_StubBase_sendBusyRespone );
    DEF_LOG_SCOPE( areg_component_StubBase_clientConnected );
    DEF_LOG_SCOPE( areg_component_StubBase_addNotificationListener );

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
                if (areg::SEQUENCE_NUMBER_ANY == other.mSequenceNr)
                {
                    result = true;
                }
                else if (mSequenceNr == other.mSequenceNr || mSequenceNr == static_cast<uint32_t>(-1 * static_cast<int32_t>(mSequenceNr == other.mSequenceNr)))
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

    inline StubBase::MapStubResource& StubBase::map_providers()
    {
        static StubBase::MapStubResource   _mapProviders;
        return _mapProviders;
    }



    StubBase::StubBase( areg::Component & masterComp, const areg::InterfaceData & siData )
        : areg::StubEventConsumer   ( mAddress )

        , mComponent            (masterComp)
        , mInterface            (siData)
        , mAddress              (siData, masterComp.getAddress().getRoleName(), masterComp.getAddress().getThreadAddress().getThreadName())
        , mConnectionStatus     ( areg::ServiceConnectionState::Disconnected )
        , mListListener         ( )
        , mCurrListener         (mListListener.invalidPosition())
        , mSessionId            (0)
        , mMapSessions          ( )
    {
        map_providers().registerResourceObject(mAddress, this);
        masterComp.registerServerItem(self());
    }

    StubBase::~StubBase()
    {
        map_providers().unregisterResourceObject(mAddress);
    }

    bool StubBase::isBusy( uint32_t requestId ) const
    {
        bool result = false;
        StubBase::StubListenerList::LISTPOS pos = mListListener.find(StubBase::Listener(requestId, areg::SEQUENCE_NUMBER_ANY));
        for ( ; (result == false) && mListListener.isValidPosition(pos); pos = mListListener.nextPosition(pos))
        {
            result = mListListener.valueAtPosition(pos).mSequenceNr != 0;
        }

        return result;
    }

    SessionID StubBase::unblockCurrentRequest()
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

    void StubBase::prepareRequest( Listener & listener, const SequenceNumber & seqNr, uint32_t responseId )
    {
        listener.mMessageId = responseId;
        listener.mSequenceNr= mListListener.isInvalidPosition(mListListener.find(listener)) ? seqNr : static_cast<SequenceNumber>(-1 * static_cast<SignedSequence>(seqNr));
        mListListener.pushFirst(listener);
        mCurrListener = mListListener.firstPosition();
    }

    uint32_t StubBase::findListeners( uint32_t requestId, StubListenerList & out_listners ) const
    {
        StubBase::Listener listener(requestId, areg::SEQUENCE_NUMBER_ANY);
        StubListenerList::LISTPOS pos = mListListener.find(listener);
        while (mListListener.isValidPosition(pos))
        {
            out_listners.pushLast(mListListener[pos]);
            pos = mListListener.find(listener, pos);
        }

        return out_listners.getSize();
    }

    void StubBase::clearAllListeners( const ProxyAddress & whichProxy, areg::IntegerArray & removedIDs )
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

    void StubBase::sendResponseNotification( const StubListenerList & whichListeners, const areg::ServiceResponseEvent& masterEvent )
    {
        for(StubListenerList::LISTPOS pos = whichListeners.firstPosition(); whichListeners.isValidPosition(pos); pos = whichListeners.nextPosition(pos) )
        {
            const StubBase::Listener& listener = whichListeners[pos];
            areg::ServiceResponseEvent* eventResp = masterEvent.cloneForTarget(listener.mProxy);
            if (eventResp != nullptr)
            {
                if (static_cast<int32_t>(listener.mSequenceNr) >= 0)
                {
                    eventResp->setSequenceNumber(listener.mSequenceNr);
                    if (listener.mSequenceNr != 0)
                        mListListener.removeEntry(listener);
                }
                else
                {
                    eventResp->setSequenceNumber(static_cast<SequenceNumber>(-1 * static_cast<SignedSequence>(listener.mSequenceNr)));
                    StubBase::Listener removed(masterEvent.getResponseId(), 0, listener.mProxy);
                    mListListener.removeEntry(removed);
                }

                sendServiceResponse(*eventResp);
            }
        }
    }

    void StubBase::sendErrorNotification( const StubListenerList & whichListeners, const areg::ServiceResponseEvent & masterEvent )
    {
        for(StubListenerList::LISTPOS pos = whichListeners.firstPosition(); whichListeners.isValidPosition(pos); pos = whichListeners.nextPosition(pos))
        {
            const StubBase::Listener& listener = whichListeners[pos];
            areg::ServiceResponseEvent* eventError = masterEvent.cloneForTarget(listener.mProxy);
            if (eventError != nullptr)
            {
                if (static_cast<int32_t>(listener.mSequenceNr) >= 0)
                {
                    eventError->setSequenceNumber(listener.mSequenceNr);
                    if (listener.mSequenceNr != 0)
                        mListListener.removeEntry(listener);
                }
                else
                {
                    eventError->setSequenceNumber(static_cast<SequenceNumber>(-1 * static_cast<SignedSequence>(listener.mSequenceNr)));
                }

                sendServiceResponse(*eventError);
            }
        }
    }

    void StubBase::sendUpdateNotification( const StubListenerList & whichListeners, const areg::ServiceResponseEvent & masterEvent ) const
    {
        for (StubListenerList::LISTPOS pos = whichListeners.firstPosition(); whichListeners.isValidPosition(pos); pos = whichListeners.nextPosition(pos))
        {
            const StubBase::Listener& listener = whichListeners[pos];
            areg::ServiceResponseEvent* eventResp = masterEvent.cloneForTarget(listener.mProxy);
            if ( eventResp != nullptr )
            {
                sendServiceResponse( *eventResp );
            }
        }
    }

    void StubBase::sendServiceResponse( areg::ServiceResponseEvent & eventElem ) const
    {
        eventElem.getTargetProxy().deliverServiceEvent(eventElem);
    }

    void StubBase::cancelCurrentRequest()
    {
        mCurrListener   = mListListener.invalidPosition();
    }

    areg::ComponentThread & StubBase::getComponentThread() const
    {
        return mComponent.getMasterThread();
    }

    StubBase* StubBase::findStubByAddress( const areg::StubAddress& address )
    {
        return map_providers().findResourceObject(address);
    }

    void StubBase::startupServiceInterface( areg::Component&  holder )
    {
        LOG_SCOPE( areg_component_StubBase_startupServiceInterface );
        LOG_DBG( "Service with role [ %s ] and interface [ %s ] is started", getServiceRole( ).getString( ), getServiceName( ).getString( ) );

        StubConnectEvent::addListener( static_cast<areg::StubEventConsumer &>(self()), holder.getMasterThread() );
    }

    void StubBase::shutdownServiceInterface( areg::Component & holder )
    {
        LOG_SCOPE( areg_component_StubBase_shutdownServiceIntrface );
        LOG_INFO( "Service with role [ %s ] and interface [ %s ] is stopped", getServiceRole().getString(), getServiceName().getString() );
        StubConnectEvent::removeListener( static_cast<areg::StubEventConsumer &>(self()), holder.getMasterThread() );
    }

    void StubBase::errorAllRequests()
    {
        LOG_SCOPE( areg_component_StubBase_errorAllRequests );
        LOG_INFO( "Service [ %s ] with interface [ %s ] send errors to all consumer.", getServiceRole().getString(), getServiceName().getString() );

        uint32_t i;

        const uint32_t numOfAttr= getNumberOfAttributes();
        const uint32_t numOfResp= getNumberOfResponses();
        const uint32_t numOfReqs= getNumberOfRequests();
        const uint32_t* attrIds = getAttributeIds();
        const uint32_t* respIds = getResponseIds();
        const uint32_t* reqIds  = getRequestIds();

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

    void StubBase::cancelAllRequests()
    {
        const uint32_t numOfReqs= getNumberOfRequests();
        const uint32_t* reqIds  = getRequestIds();
        for ( uint32_t i = 0; i < numOfReqs; ++ i )
        {
            errorRequest( reqIds[i], true );
        }
    }

    void StubBase::invalidateAttribute( uint32_t attrId )
    {
        if ( areg::isAttributeId(attrId) )
            errorRequest(attrId, false);
    }

    void StubBase::sendUpdateEvent( uint32_t msgId, const EventDataStream & data, areg::ResultType result ) const
    {
        LOG_SCOPE( areg_component_StubBase_sendUpdateEvent);
        StubBase::StubListenerList listeners;
        if (findListeners(msgId, listeners) > 0)
        {
            const ProxyAddress & proxy = listeners.firstEntry( ).mProxy;
            LOG_WARN( "Sends busy message to proxy [ %s ] for the request [ %u ]", ProxyAddress::convAddressToPath( proxy).getString(), msgId);

            areg::ResponseEvent* eventElem = createResponseEvent(proxy, msgId, result, data);
            if (eventElem != nullptr)
            {
                sendUpdateNotification(listeners, *eventElem);
                eventElem->destroy();
            }
        }
    }

    void StubBase::sendUpdateNotificationOnce( const ProxyAddress & target, uint32_t msgId, const EventDataStream & data, areg::ResultType result ) const
    {
        areg::ResponseEvent * eventElem = createResponseEvent( target, msgId, result, data );
        if ( eventElem != nullptr )
        {
            sendServiceResponse( *eventElem );
        }
    }

    void StubBase::sendResponseEvent( uint32_t respId, const EventDataStream & data )
    {
        StubBase::StubListenerList listeners;
        if (findListeners(respId, listeners) > 0)
        {
            areg::ResponseEvent* eventElem = createResponseEvent(listeners.firstEntry().mProxy, respId, areg::ResultType::RequestOK, data);
            if (eventElem != nullptr)
            {
                sendResponseNotification(listeners, *eventElem);
                eventElem->destroy();
            }
        }
    }

    void StubBase::sendBusyRespone( const Listener & whichListener )
    {
        LOG_SCOPE(areg_component_StubBase_sendBusyRespone);
        areg::ResponseEvent* eventElem = createResponseEvent(whichListener.mProxy, whichListener.mMessageId, areg::ResultType::RequestBusy, EventDataStream::empty_data());
        if (eventElem != nullptr)
        {
            LOG_WARN("Sending busy response for request message [ %p ] from source [ %p ] to target [ %p ], sequence [ %llu ]"
                        , whichListener.mMessageId
                        , whichListener.mProxy.getTarget()
                        , whichListener.mProxy.getSource()
                        , whichListener.mSequenceNr);

            eventElem->setSequenceNumber(whichListener.mSequenceNr);
            sendServiceResponse(*eventElem);
        }
    }

    bool StubBase::canExecuteRequest( Listener & whichListener, uint32_t whichResponse, const SequenceNumber & seqNr )
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

    bool StubBase::existNotificationListener( uint32_t msgId, const ProxyAddress & notifySource ) const
    {
        bool result = false;
        if ( notifySource.isValid() )
        {
            StubListenerList::LISTPOS pos = mListListener.firstPosition();
            for ( ; (result == false) && mListListener.isValidPosition(pos); pos = mListListener.nextPosition(pos) )
            {
                const StubBase::Listener & listener = mListListener.valueAtPosition(pos);
                result = (areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr) &&
                        (msgId == listener.mMessageId) &&
                        (notifySource == listener.mProxy);
            }
        }

        return result;
    }

    bool StubBase::addNotificationListener(uint32_t msgId, const ProxyAddress & notifySource)
    {
        LOG_SCOPE(areg_component_StubBase_addNotificationListener);

        bool result { false };
        if (notifySource.isValid())
        {
            bool hasEntry{ false };
            auto pos = mListListener.firstPosition();
            for ( ; (hasEntry == false) && mListListener.isValidPosition(pos); pos = mListListener.nextPosition(pos) )
            {
                const StubBase::Listener & listener = mListListener.valueAtPosition(pos);
                hasEntry = (areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr) &&
                        (msgId == listener.mMessageId) &&
                        (notifySource == listener.mProxy);
            }

            if ( hasEntry == false)
            {
                LOG_DBG("For the message [ %u ] new listener [ %s ] is added"
                            , msgId
                            , ProxyAddress::convAddressToPath(notifySource).getString());

                mListListener.pushLast(StubBase::Listener(msgId, areg::SEQUENCE_NUMBER_NOTIFY, notifySource));
                result = true;
            }
    #if AREG_LOGS
            else
            {
                LOG_WARN("For the message [ %u ] there is already registered client [ %s ]"
                        , msgId
                        , ProxyAddress::convAddressToPath(notifySource).getString());
            }
    #endif  // AREG_LOGS
        }

        return result;
    }

    void StubBase::removeNotificationListener( uint32_t msgId, const ProxyAddress & notifySource )
    {
        for (StubListenerList::LISTPOS pos = mListListener.firstPosition(); mListListener.isValidPosition(pos); pos = mListListener.nextPosition(pos) )
        {
            const StubBase::Listener & listener = mListListener.valueAtPosition(pos);
            if ( areg::SEQUENCE_NUMBER_NOTIFY == listener.mSequenceNr && msgId == listener.mMessageId && notifySource == listener.mProxy )
            {
                mListListener.removeAt(pos);
                break;
            }
        }
    }

    bool StubBase::clientConnected(const ProxyAddress & client, areg::ServiceConnectionState status )
    {
        bool result{ false };
        if (mAddress == client)
        {
            LOG_SCOPE(areg_component_StubBase_clientConnected);
            LOG_DBG("Service consumer [ %s ] connection event with status [ %s ]"
                    , ProxyAddress::convAddressToPath(client).getString()
                    , areg::getString(status));

            result = true;
            if (areg::isServiceDisconnected(status))
            {
                clearAllListeners(client);
            }
        }

        return result;
    }

    void StubBase::processClientConnectEvent( const ProxyAddress & proxyAddress, areg::ServiceConnectionState status )
    {
        clientConnected( proxyAddress, status );
    }

    void StubBase::processStubRegisteredEvent(const areg::StubAddress & stubTarget, areg::ServiceConnectionState status )
    {
        if ( areg::isServiceConnected( status) )
        {
            ASSERT( stubTarget.isValid() );
            map_providers().lock();
            map_providers().unregisterResourceObject(mAddress);

            mAddress = stubTarget;
            
            map_providers().registerResourceObject(mAddress, this);
            map_providers().unlock();
        }

        mConnectionStatus = status;
    }

    const areg::Version & StubBase::getImplVersion() const
    {
        return mInterface.idVersion;
    }

    uint32_t StubBase::getNumberOfRequests() const
    {
        return mInterface.idRequestCount;
    }

    uint32_t StubBase::getNumberOfResponses() const
    {
        return mInterface.idResponseCount;
    }

    uint32_t StubBase::getNumberOfAttributes() const
    {
        return mInterface.idAttributeCount;
    }

    const uint32_t * StubBase::getRequestIds() const
    {
        return mInterface.idRequestList;
    }

    const uint32_t * StubBase::getResponseIds() const
    {
        return mInterface.idResponseList;
    }

    const uint32_t * StubBase::getAttributeIds() const
    {
        return mInterface.idAttributeList;
    }

    areg::ResponseEvent * StubBase::createResponseEvent( const ProxyAddress &     /* proxy */
                                                , uint32_t             /* msgId */
                                                , areg::ResultType   /* result */
                                                , const EventDataStream &  /* data */ ) const
    {
        return nullptr;
    }

    RemoteRequestEvent * StubBase::createRemoteRequestEvent( const areg::InStream & /* stream */ ) const
    {
        return nullptr;
    }

    RemoteNotifyRequestEvent * StubBase::createRemoteNotifyRequestEvent( const areg::InStream & /* stream */ ) const
    {
        return nullptr;
    }

    void StubBase::processStubEvent( areg::StubEvent & /* eventElem */ )
    {
    }

    void StubBase::processGenericEvent(Event & /* eventElem */)
    {
    }

} // namespace areg