/************************************************************************
 * \file        areg/src/component/private/CEProxyBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Base class implementation.
 *
 ************************************************************************/
#include "areg/src/component/CEProxyBase.hpp"

#include "areg/src/base/CEVersion.hpp"
#include "areg/src/base/NEMemory.hpp"
#include "areg/src/base/NEUtilities.hpp"

#include "areg/src/component/CEServiceResponseEvent.hpp"
#include "areg/src/component/CEServiceRequestEvent.hpp"
#include "areg/src/component/CENotificationEvent.hpp"
#include "areg/src/component/IEProxyListener.hpp"

#include "areg/src/component/private/CEProxyConnectEvent.hpp"
#include "areg/src/component/private/CEComponentInfo.hpp"
#include "areg/src/component/private/CEServiceManager.hpp"

//////////////////////////////////////////////////////////////////////////
// Predefined constants / statics
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEProxyBase class statics
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Resource of registered Proxies in the system.
 **/
CEProxyBase::MapProxyResource CEProxyBase::_mapRegisteredProxies;

//////////////////////////////////////////////////////////////////////////
// CEProxyBase::CEListener class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEProxyBase::CEListener class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEProxyBase::CEListener::CEListener( void )
    : mMessageId    (NEService::EMPTY_FUNCTION_ID)
    , mSequenceNr   (NEService::SEQUENCE_NUMBER_NOTIFY)
    , mListener     (NULL)
{
    ; // do nothing
}

CEProxyBase::CEListener::CEListener( const CEListener & src )
    : mMessageId    (src.mMessageId)
    , mSequenceNr   (src.mSequenceNr)
    , mListener     (src.mListener)
{
    ; // do nothing
}

CEProxyBase::CEListener::CEListener( unsigned int msgId )
    : mMessageId    (msgId)
    , mSequenceNr   (NEService::SEQUENCE_NUMBER_NOTIFY)
    , mListener     (NULL)
{
    ; // do nothing
}

CEProxyBase::CEListener::CEListener( unsigned int msgId, unsigned int seqNr )
    : mMessageId    (msgId)
    , mSequenceNr   (seqNr)
    , mListener     (NULL)
{
    ; // do nothing
}

CEProxyBase::CEListener::CEListener( unsigned int msgId, unsigned int seqNr, IENotificationEventConsumer* caller )
    : mMessageId    (msgId)
    , mSequenceNr   (seqNr)
    , mListener     (caller)
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEProxyBase::CEListener class, operators
//////////////////////////////////////////////////////////////////////////
const CEProxyBase::CEListener& CEProxyBase::CEListener::operator = ( const CEProxyBase::CEListener& src )
{
    if (static_cast<const CEProxyBase::CEListener *>(this) != &src)
    {
        mMessageId  = src.mMessageId;
        mSequenceNr = src.mSequenceNr;
        mListener   = src.mListener;
    }
    return (*this);
}

bool CEProxyBase::CEListener::operator == ( const CEProxyBase::CEListener& other ) const
{
    bool result = this == &other ? true : false;
    if (result == false && other.mMessageId == mMessageId)
    {
        if (other.mSequenceNr == NEService::SEQUENCE_NUMBER_ANY)
            result = true;
        else if (other.mSequenceNr == mSequenceNr && other.mListener == mListener)
            result = true;
        else if (other.mSequenceNr == NEService::SEQUENCE_NUMBER_NOTIFY && other.mListener == NULL)
            result = true;
    }
    return result;
}


//////////////////////////////////////////////////////////////////////////
// CEProxyBase::CEProxyListenerList class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEProxyBase::CEProxyListenerList class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEProxyBase::CEProxyListenerList::CEProxyListenerList( int capacity /*= 0*/ )
    : TEArrayList<CEProxyBase::CEListener, const CEProxyBase::CEListener &>(capacity)
{
    ; // do nothing
}

CEProxyBase::CEProxyListenerList::~CEProxyListenerList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEProxyBase::CEProxyMapImpl class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEProxyBase::CEProxyMapImpl class, constructor / destructor
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEProxyBase::CEProxyMapImpl class, Methods
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEProxyBase::CEServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(CEProxyBase::CEServiceAvailableEvent, CEThreadEventBase)

CEProxyBase::CEServiceAvailableEvent::CEServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : CEThreadEventBase ( CEEvent::EventExternal )
    , mConsumer         ( consumer )
{
    ; // do nothing
}

CEProxyBase::CEServiceAvailableEvent::~CEServiceAvailableEvent( void )
{
    ; // do nothing
}

IENotificationEventConsumer & CEProxyBase::CEServiceAvailableEvent::GetConsumer( void ) const
{
    return mConsumer;
}

//////////////////////////////////////////////////////////////////////////
// CEProxyBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEProxyBase class, static methods
//////////////////////////////////////////////////////////////////////////
CEProxyBase * CEProxyBase::FindOrCreateProxy(  const char * roleName
                                             , const NEService::SInterfaceData & serviceIfData
                                             , IEProxyListener & connect
                                             , FuncCreateProxy funcCreate
                                             , const char * ownerThread /*= static_cast<const char *>(NULL)*/)
{
    return CEProxyBase::FindOrCreateProxy(roleName, serviceIfData, connect, funcCreate, CEDispatcherThread::GetDispatcherThread(ownerThread) );
}

CEProxyBase * CEProxyBase::FindOrCreateProxy(  const char * roleName
                                             , const NEService::SInterfaceData & serviceIfData
                                             , IEProxyListener & connect
                                             , FuncCreateProxy funcCreate
                                             , CEDispatcherThread & ownerThread )
{
    CEProxyBase*   proxy = NULL;
    CEProxyAddress Key(serviceIfData, roleName, ownerThread.IsValid() ? ownerThread.GetThreadName().GetBuffer() : static_cast<const char *>(NULL) );
    proxy = _mapRegisteredProxies.FindResourceObject(Key);
    if (proxy == NULL)
    {
        proxy = funcCreate(roleName, &ownerThread);
    }

    if (proxy != NULL)
    {
        static_cast<void>(proxy->AddListener( static_cast<unsigned int>(NEService::SI_SERVICE_CONNECTION_NOTIFY), NEService::SEQUENCE_NUMBER_NOTIFY, static_cast<IENotificationEventConsumer *>(&connect) ));
        ++ proxy->mProxyInstCount;
        if ( proxy->mProxyInstCount == 1 )
        {
            proxy->RegisterServiceListeners( );
            CEServiceManager::RequestRegisterClient( proxy->GetProxyAddress( ) );
        }
        else if ( proxy->mIsConnected )
        {
            proxy->SendServiceAvailableEvent( proxy->CreateServiceAvailableEvent(connect) );
        }
    }
    return proxy;
}

//////////////////////////////////////////////////////////////////////////
// CEProxyBase class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEProxyBase::CEProxyBase(const char* roleName, const NEService::SInterfaceData & serviceIfData, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )

    : IEProxyEventConsumer  ( mProxyAddress )
    
    , mProxyAddress     ( serviceIfData, roleName, (ownerThread != static_cast<CEDispatcherThread *>(NULL)) && (ownerThread->IsValid()) ? ownerThread->GetThreadName().GetBuffer() : static_cast<const char *>(NULL) )
    , mImplStubAddress  ( CEStubAddress::INVALID_STUB_ADDRESS )
    , mSequenceCount    ( 0 )
    , mListenerList     ( serviceIfData.idAttributeCount + serviceIfData.idResponseCount )
    , mIsConnected      ( false )

    , mProxyData        ( serviceIfData )

    , mDispatcherThread ( (ownerThread != static_cast<CEDispatcherThread *>(NULL)) && (ownerThread->IsValid()) ? *ownerThread : CEDispatcherThread::GetDispatcherThread( static_cast<const char *>(mProxyAddress.GetThread())) )

    , mProxyInstCount   ( 0 )
{
    ASSERT(mDispatcherThread.IsValid());
    _mapRegisteredProxies.RegisterResourceObject(mProxyAddress, this);
}

CEProxyBase::~CEProxyBase( void )
{
    _mapRegisteredProxies.UnregisterResourceObject(mProxyAddress);
}

//////////////////////////////////////////////////////////////////////////
// CEProxyBase class, methods
//////////////////////////////////////////////////////////////////////////

void CEProxyBase::RegisterServiceListeners( void )
{
    CEProxyConnectEvent::AddListener( static_cast<IEEventConsumer &>(self( )), mDispatcherThread );
}

void CEProxyBase::UnregisterServiceListeners( void )
{
    CEProxyConnectEvent::RemoveListener( static_cast<IEEventConsumer &>(self( )), mDispatcherThread );
    CEProxyBase::CEServiceAvailableEvent::RemoveListener( static_cast<IEEventConsumer &>(self( )), mDispatcherThread );
}

void CEProxyBase::FreeProxy( IEProxyListener & connect )
{
    connect.ServiceConnected(false, self());

    RemoveListener(NEService::SI_SERVICE_CONNECTION_NOTIFY, NEService::SEQUENCE_NUMBER_NOTIFY, static_cast<IENotificationEventConsumer *>(&connect));
    if (mProxyInstCount == 1)
    {
        -- mProxyInstCount;
        OUTPUT_WARN("The proxy [ %s ] instance count is zero, going to delete object at address [ %p]", CEProxyAddress::ConvertAddressToPath(GetProxyAddress()).GetBuffer(), (void *)this);
        StopAllServiceNotifications( );
        UnregisterServiceListeners( );
        mListenerList.RemoveAll();

        CEServiceManager::RequestUnregisterClient( GetProxyAddress( ) );
        mDispatcherThread.RemoveConsumer( *this );

        delete this;
    }
    else if ( mProxyInstCount != 0 )
    {
        -- mProxyInstCount;
    }
}

void CEProxyBase::ServiceConnectionUpdated( const CEStubAddress & Server, const CEChannel & Channel, NEService::eServiceConnection Status )
{
    ASSERT(Server.IsValid());
    if ( Server.IsProxyCompatible( GetProxyAddress() ) )
    {
        OUTPUT_DBG("The proxy [ %s ] have got [ %s ] status update notification event from stub [ %s ]"
                    , CEProxyAddress::ConvertAddressToPath(GetProxyAddress()).GetBuffer()
                    , NEService::GetString(Status)
                    , CEStubAddress::ConvertAddressToPath(Server).GetBuffer());

        ASSERT(Channel.GetTarget() == Server.GetSource() || Status != NEService::ServiceConnected);
        mProxyAddress.SetChannel(Channel);
        if ( Status == NEService::ServiceConnected )
        {
            mIsConnected        = true;
            mImplStubAddress    = Server;
        }
        else
        {
            mIsConnected        = false;
            mImplStubAddress    = CEStubAddress::INVALID_STUB_ADDRESS;
            mProxyData.ResetStates();
        }

        // first collect listeners, because on connect / disconnect
        // the listener list might be updated!
        TEArrayList<CEProxyBase::CEListener> conListeners;
        int index = 0;
        for (index = 0 ; index < mListenerList.GetSize(); ++ index)
        {
            CEProxyBase::CEListener& listener = mListenerList[index];
            if ( NEService::IsConnectNotifyId(listener.mMessageId) )
            {
                ASSERT(listener.mListener != NULL);
                conListeners.Add(listener);
                // mListenerList.RemoveAt(index);
            }
        }

        for (index = 0 ; index < conListeners.GetSize(); ++ index)
            static_cast<IEProxyListener *>(conListeners[index].mListener)->ServiceConnected(mIsConnected, *this);
    }
}

bool CEProxyBase::AddListener(unsigned int msgId, unsigned int seqNr, IENotificationEventConsumer* caller)
{
    CEProxyBase::CEListener listener(msgId, seqNr, caller);
    bool result = false;
    if (mListenerList.ElementExist(listener, 0) == false)
    {
        static_cast<void>(mListenerList.Add(listener));
        result = true;
    }
    return result;
}

void CEProxyBase::SetNotification( unsigned int msgId, IENotificationEventConsumer* caller, bool alwaysNotify /*= false*/ )
{
    if (IsConnected())
    {
        if ( AddListener(msgId, NEService::SEQUENCE_NUMBER_NOTIFY, caller) )
        {
            StartNotification(msgId);
        }
        else if ( alwaysNotify )
        {
            // the listener already exists
            if ( NEService::IsAttributeId(msgId) )
            {
                SendNotificationEvent(  msgId, mProxyData.GetAttributeState(msgId) == NEService::DATA_OK ? NEService::RESULT_DATA_OK : NEService::RESULT_DATA_INVALID, NEService::SEQUENCE_NUMBER_NOTIFY, caller);
            }
            else if ( NEService::IsResponseId(msgId) && (mProxyData.GetParamState(msgId) == NEService::DATA_OK) )
            {
                SendNotificationEvent(  msgId, NEService::RESULT_OK, NEService::SEQUENCE_NUMBER_NOTIFY, caller);
            }
            else
            {
                ; // ignore
            }
        }
    }
    else
    {
        ; // ignore
    }
}

void CEProxyBase::ClearNotification( unsigned int msgId, IENotificationEventConsumer* caller )
{
    RemoveListener(msgId, NEService::SEQUENCE_NUMBER_NOTIFY, caller);
    if (HasNotificationListener(msgId) == false)
    {
        StopNotification(msgId);
        mProxyData.SetDataState(msgId, NEService::DATA_UNAVAILABLE);
    }
}

void CEProxyBase::SetState( unsigned int msgId, NEService::eDataStateType newState )
{
    mProxyData.SetDataState(msgId, newState);
}

void CEProxyBase::UnregisterListener( IENotificationEventConsumer *consumer )
{
    int index = 0;
    while (index < mListenerList.GetSize())
    {
        const CEProxyBase::CEListener& elem = mListenerList[index];
        if (elem.mListener == consumer)
        {
            unsigned int msgId = elem.mMessageId;
            mListenerList.RemoveAt(index);
            if (HasNotificationListener(msgId) == false)
            {
                StopNotification(msgId);
                mProxyData.SetDataState(msgId, NEService::DATA_UNAVAILABLE);
            }
        }
        else
        {
            index ++;
        }
    }
}

int CEProxyBase::PrepareListeners( CEProxyBase::CEProxyListenerList& out_listenerList, unsigned int msgId, unsigned int seqNrToSearch )
{
    CEProxyBase::CEListener searchListener(msgId, NEService::SEQUENCE_NUMBER_ANY);
    for ( int i = 0; i < mListenerList.GetSize(); ++ i )
    {
        const CEProxyBase::CEListener & elem = mListenerList.GetAt(i);
        if ( elem == searchListener )
        {
            if ( elem.mSequenceNr == NEService::SEQUENCE_NUMBER_NOTIFY )
            {
                out_listenerList.Add( elem );
            }
            else if ( elem.mSequenceNr == seqNrToSearch )
            {
                out_listenerList.Add( elem );
                mListenerList.RemoveAt(i --);   // <== go one index back, because remove one element
            }
        }
    }
    return out_listenerList.GetSize();
}

void CEProxyBase::NotifyListeners( int respId, NEService::eResultType result, unsigned int seqNrToSearch )
{
    CEProxyBase::CEProxyListenerList listenerList;
    PrepareListeners(listenerList, respId, seqNrToSearch);
    for (int i = 0; i < listenerList.GetSize(); ++ i)
    {
        const CEProxyBase::CEListener & elem = listenerList.GetAt(i);
        SendNotificationEvent(respId, result, seqNrToSearch, elem.mListener);
    }
}

void CEProxyBase::SendNotificationEvent( unsigned int msgId, NEService::eResultType resType, unsigned int seqNr, IENotificationEventConsumer* caller )
{
    CENotificationEventData data(self(), resType, msgId, seqNr);
    CENotificationEvent* eventElem = CreateNotificationEvent(data);
    if (eventElem != NULL)
    {
        if (caller != NULL)
        {
            eventElem->SetEventConsumer(static_cast<IEEventConsumer *>(caller));
        }
        static_cast<CEEvent *>(eventElem)->SendEvent();
    }
}

#ifdef  DEBUG
void CEProxyBase::ProcessProxyEvent( CEProxyEvent& eventElem )
{
    ASSERT(eventElem.GetTargetProxy() == GetProxyAddress());
}
#else
void CEProxyBase::ProcessProxyEvent( CEProxyEvent& /*eventElem*/ )
{
}
#endif

void CEProxyBase::ProcessGenericEvent( CEEvent& eventElem )
{
    CEProxyBase::CEServiceAvailableEvent * serviceEvent = RUNTIME_CAST( &eventElem, CEProxyBase::CEServiceAvailableEvent );
    if ( serviceEvent != NULL )
    {
        ProcessServiceAvailableEvent( serviceEvent->GetConsumer() );
    }
}

CEProxyBase* CEProxyBase::FindProxyByAddress( const CEProxyAddress& proxyAddress )
{
    return _mapRegisteredProxies.FindResourceObject(proxyAddress);
}

void CEProxyBase::RegisterForEvent( const CERuntimeClassID& eventClass )
{
    CEEvent::AddListener(eventClass, static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName());
}

void CEProxyBase::UnregisterForEvent( const CERuntimeClassID& eventClass )
{
    CEEvent::RemoveListener(eventClass, static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName());
}

void CEProxyBase::SendRequestEvent( unsigned int reqId, const CEEventDataStream& args, IENotificationEventConsumer *caller )
{
    CEServiceRequestEvent* evenElem = CreateRequestEvent(args, reqId);
    if ( evenElem != NULL )
    {
        unsigned int respId = GetProxyData().GetResponseId(static_cast<unsigned int>(reqId));
        ASSERT(NEService::IsResponseId(respId) || (respId == NEService::RESPONSE_ID_NONE));

        if (respId != NEService::RESPONSE_ID_NONE)
        {
            static_cast<void>( AddListener(respId, ++ mSequenceCount, caller) );
            evenElem->SetSequenceNumber(mSequenceCount);
        }

        mProxyAddress.DeliverServiceEvent(*evenElem);
    }
}

void CEProxyBase::SendNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    CEServiceRequestEvent* notifyEvent = CreateNotificationRequestEvent(msgId, reqType);
    if (notifyEvent != NULL)
        mProxyAddress.DeliverServiceEvent(*notifyEvent);
}

void CEProxyBase::SendServiceAvailableEvent( CEProxyBase::CEServiceAvailableEvent * eventInstance )
{
    if ( eventInstance != static_cast<CEProxyBase::CEServiceAvailableEvent *>(NULL) )
    {
        eventInstance->AddListener( self(), mDispatcherThread);
        eventInstance->SetEventConsumer(this);
        eventInstance->RegisterForThread(&mDispatcherThread);
        eventInstance->SendEvent( );
    }
}

bool CEProxyBase::IsServiceListenerRegistered( IENotificationEventConsumer & caller ) const
{
    bool result = false;
    for ( int i = 0; (result == false) && (i < mListenerList.GetSize()); ++ i )
    {
        const CEProxyBase::CEListener & listener = mListenerList[i];
        result = NEService::IsConnectNotifyId( listener.mMessageId ) && (&caller == listener.mListener);
    }
    return result;
}

void CEProxyBase::ProcessServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    if ( mIsConnected && IsServiceListenerRegistered(consumer) )
        static_cast<IEProxyListener &>(consumer).ServiceConnected(true, self());
}
