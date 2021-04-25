/************************************************************************
 * \file        areg/src/component/private/CEStubEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component Thread.
 *
 ************************************************************************/
#include "areg/src/component/CEStubEvent.hpp"

#include "areg/src/component/CEStubBase.hpp"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/component/CEComponentThread.hpp"
#include "areg/src/component/CERequestEvents.hpp"
#include "areg/src/component/private/CEStubConnectEvent.hpp"


//////////////////////////////////////////////////////////////////////////
// CEStubEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEStubEvent class, implement runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CEStubEvent, CEStreamableEvent)

//////////////////////////////////////////////////////////////////////////
// CEStubEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEStubEvent::CEStubEvent( const CEStubAddress& toTarget, CEEvent::eEventType eventType )
    : CEStreamableEvent (eventType)
    , mTargetStubAddress(toTarget)
{
    ; // do nothing
}

CEStubEvent::CEStubEvent( const IEInStream & stream  )
    : CEStreamableEvent (stream)
    , mTargetStubAddress(stream)
{
    ; // do nothing
}

CEStubEvent::~CEStubEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEStubEvent class, methods
//////////////////////////////////////////////////////////////////////////
const IEInStream & CEStubEvent::ReadFromStream( const IEInStream & stream )
{
    CEStreamableEvent::ReadFromStream(stream);
    stream >> mTargetStubAddress;
    return stream;
}

IEOutStream & CEStubEvent::WriteToStream( IEOutStream & stream ) const
{
    CEStreamableEvent::WriteToStream(stream);
    stream << mTargetStubAddress;
    return stream;
}

void CEStubEvent::SendEvent( void )
{
    if ( mTargetThread == NULL )
    {
        CEThread * thread = CEThread::FindThreadByName( mTargetStubAddress.GetThread() );
        RegisterForThread( thread != NULL ? RUNTIME_CAST(thread, CEDispatcherThread) : NULL );
    }

    if ( mTargetThread != NULL )
        CEStreamableEvent::SendEvent();
    else
        this->Destroy();
}

//////////////////////////////////////////////////////////////////////////
// IEStubEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IEStubEventConsumer class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
IEStubEventConsumer::IEStubEventConsumer( const CEStubAddress & stubAddress )
    : CEThreadEventConsumerBase ( )
    , mStubAddress              ( stubAddress )
{
    ; // do nothing
}

IEStubEventConsumer::~IEStubEventConsumer( void )
{
    ; // do nothing
}

inline void IEStubEventConsumer::processRequestEvent( CERequestEvent & requestEvent )
{
    CEComponent *curComponent   = CEComponent::FindComponentByName(requestEvent.GetTargetStub().GetRoleName());
    CEComponentThread::SetCurrentComponent(curComponent);

    if (NEService::IsRequestId(requestEvent.GetRequestId()))
        ProcessRequestEvent(requestEvent);
    else
        ProcessStubEvent( static_cast<CEStubEvent &>(requestEvent));

    CEComponentThread::SetCurrentComponent(NULL);
}

inline void IEStubEventConsumer::processNotifyRequestEvent( CENotifyRequestEvent & notifyRequest )
{
    CEComponent *curComponent   = CEComponent::FindComponentByName(notifyRequest.GetTargetStub().GetRoleName());
    CEComponentThread::SetCurrentComponent(curComponent);

    unsigned int reqId = notifyRequest.GetRequestId();
    if (NEService::IsAttributeId(reqId) || NEService::IsResponseId(reqId))
    {
        ProcessAttributeEvent(notifyRequest);
    }
    else
    {
        ProcessStubEvent( static_cast<CEStubEvent &>(notifyRequest) );
    }

    CEComponentThread::SetCurrentComponent(NULL);
}

inline void IEStubEventConsumer::processConnectEvent( CEStubConnectEvent & notifyConnect )
{
    if ( notifyConnect.GetRequestId() == static_cast<unsigned int>(NEService::SI_SERVICE_CONNECTION_NOTIFY) )
    {
        if ( notifyConnect.GetRequestType() == NEService::REQUEST_CONNECTION)
            ProcessStubRegisteredEvent( notifyConnect.GetTargetStub(), notifyConnect.GetConnectionStatus() );
        else
            ProcessClientConnectEvent( notifyConnect.GetEventSource(), notifyConnect.GetConnectionStatus() );
    }
    else
        ProcessStubEvent( static_cast<CEStubEvent &>(notifyConnect));
}

//////////////////////////////////////////////////////////////////////////
// IEStubEventConsumer class, methods
//////////////////////////////////////////////////////////////////////////
void IEStubEventConsumer::StartEventProcessing( CEEvent & eventElem )
{
    CEStubEvent* stubEvent = RUNTIME_CAST(&eventElem, CEStubEvent);
    if ( stubEvent != NULL )
    {
        if ( stubEvent->GetTargetStub() == mStubAddress )
        {
            CERequestEvent* reqEvent = RUNTIME_CAST(stubEvent, CERequestEvent);
            if (reqEvent != NULL)
            {
                processRequestEvent(*reqEvent);
            }
            else
            {
                CENotifyRequestEvent * notifyRequest = RUNTIME_CAST(stubEvent, CENotifyRequestEvent);
                if ( notifyRequest != NULL )
                {
                    processNotifyRequestEvent(*notifyRequest);
                }
                else
                {
                    CEStubConnectEvent * stubConnectEvent = RUNTIME_CAST(stubEvent, CEStubConnectEvent);
                    if ( stubConnectEvent != NULL )
                        processConnectEvent(*stubConnectEvent);
                    else
                        ProcessStubEvent(*stubEvent);
                }
            }
        }
        else
        {
            ; // ignore, wrong stub address
        }
    }
    else
    {
        ProcessGenericEvent(eventElem);
    }
}

void IEStubEventConsumer::ConsumerRegistered( bool isRegistered )
{
    CEThreadEventConsumerBase::ConsumerRegistered(isRegistered);
}
