/************************************************************************
 * \file        areg/component/private/EventConsumerMap.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Consumer Map class implementation
 *
 ************************************************************************/
#include "areg/component/private/EventConsumerMap.hpp"

#include "areg/component/IEEventConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
EventConsumerList::EventConsumerList( void )
    : TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList> ( )
{
    ; // do nothing
}

EventConsumerList::EventConsumerList( const EventConsumerList &src )
    : TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList> ( static_cast<const TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList> &>(src) )
{
    ; // do nothing
}

EventConsumerList::~EventConsumerList( void )
{
    removeAllConsumers();
}

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class, methods
//////////////////////////////////////////////////////////////////////////
bool EventConsumerList::addConsumer( IEEventConsumer& whichConsumer )
{
    bool result = false;
    if (TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>::pushLast(&whichConsumer) != NULL)
    {
        result = true;
        whichConsumer.consumerRegistered(true);
    }
    
    return result;
}

bool EventConsumerList::removeConsumer( IEEventConsumer& whichConsumer )
{
    bool result = false;
    if ( TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>::removeEntry(&whichConsumer, NULL) )
    {
        result = true;
        whichConsumer.consumerRegistered(false);
    }

    return result;
}

void EventConsumerList::removeAllConsumers( void )
{
    while (TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>::isEmpty() == false)
    {
        IEEventConsumer *consumer = TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>::removeFirst();
        ASSERT(consumer != NULL);
        consumer->consumerRegistered(false);
    }
    
    TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>::removeAll();
}

//////////////////////////////////////////////////////////////////////////
// EventConsumerMapImpl class implementation
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// EventConsumerMapImpl class, methods
//////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) && defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_DEBUG)
void EventConsumerMapImpl::implCleanResource( RuntimeClassID & Key, EventConsumerList * Resource )
{
    OUTPUT_DBG("Resource [ %s ]: Removing all consumers and deleting resource at address [ %p ]", Key.getName(), Resource);
    ASSERT(Resource != NULL);
    Resource->removeAllConsumers();
    delete Resource;
}

#else
void EventConsumerMapImpl::implCleanResource( RuntimeClassID & /*Key*/, EventConsumerList * Resource )
{
    Resource->removeAllConsumers();
    delete Resource;
}
#endif

//////////////////////////////////////////////////////////////////////////
// EventConsumerMap class implementation
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// EventConsumerMap class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
EventConsumerMap::EventConsumerMap( void )
    : TELockRuntimeResourceMap<EventConsumerList, EventConsumerMapImpl> ( )
{
    ; // do nothing
}

EventConsumerMap::~EventConsumerMap( void )
{
    ; // do nothing
}
