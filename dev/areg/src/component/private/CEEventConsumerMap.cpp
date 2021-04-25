/************************************************************************
 * \file        areg/src/component/private/CEEventConsumerMap.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Consumer Map class implementation
 *
 ************************************************************************/
#include "areg/src/component/private/CEEventConsumerMap.hpp"

#include "areg/src/component/IEEventConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// CEEventConsumerList class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEEventConsumerList class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CEEventConsumerList::CEEventConsumerList( void )
    : TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList> ( )
{
    ; // do nothing
}

CEEventConsumerList::CEEventConsumerList( const CEEventConsumerList &src )
    : TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList> ( static_cast<const TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList> &>(src) )
{
    ; // do nothing
}

CEEventConsumerList::~CEEventConsumerList( void )
{
    RemoveAllConsumers();
}

//////////////////////////////////////////////////////////////////////////
// CEEventConsumerList class, methods
//////////////////////////////////////////////////////////////////////////
bool CEEventConsumerList::AddConsumer( IEEventConsumer& whichConsumer )
{
    bool result = false;
    if (TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>::AddTail(&whichConsumer) != NULL)
    {
        result = true;
        whichConsumer.ConsumerRegistered(true);
    }
    
    return result;
}

bool CEEventConsumerList::RemoveConsumer( IEEventConsumer& whichConsumer )
{
    bool result = false;
    if ( TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>::RemoveElem(&whichConsumer, NULL) )
    {
        result = true;
        whichConsumer.ConsumerRegistered(false);
    }

    return result;
}

void CEEventConsumerList::RemoveAllConsumers( void )
{
    while (TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>::IsEmpty() == false)
    {
        IEEventConsumer *consumer = TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>::RemoveHead();
        ASSERT(consumer != NULL);
        consumer->ConsumerRegistered(false);
    }
    
    TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>::RemoveAll();
}

//////////////////////////////////////////////////////////////////////////
// CEEventConsumerMapImpl class implementation
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CEEventConsumerMapImpl class, methods
//////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) && defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_DEBUG)
void CEEventConsumerMapImpl::ImplCleanResourceElement( CERuntimeClassID & Key, CEEventConsumerList * Resource )
{
    OUTPUT_DBG("Resource [ %s ]: Removing all consumers and deleting resource at address [ %p ]", Key.GetName(), Resource);
    ASSERT(Resource != NULL);
    Resource->RemoveAllConsumers();
    delete Resource;
}

#else
void CEEventConsumerMapImpl::ImplCleanResourceElement( CERuntimeClassID & /*Key*/, CEEventConsumerList * Resource )
{
    Resource->RemoveAllConsumers();
    delete Resource;
}
#endif

//////////////////////////////////////////////////////////////////////////
// CEEventConsumerMap class implementation
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CEEventConsumerMap class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEEventConsumerMap::CEEventConsumerMap( void )
    : TELockRuntimeResourceMap<CEEventConsumerList, CEEventConsumerMapImpl> ( )
{
    ; // do nothing
}

CEEventConsumerMap::~CEEventConsumerMap( void )
{
    ; // do nothing
}
