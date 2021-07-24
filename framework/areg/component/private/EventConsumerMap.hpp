#ifndef AREG_COMPONENT_PRIVATE_EVENTCONSUMERMAP_HPP
#define AREG_COMPONENT_PRIVATE_EVENTCONSUMERMAP_HPP
/************************************************************************
 * \file        areg/component/private/EventConsumerMap.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Consumer Resources Object declaration.
 *              This Resource Map object contains information of Event Consumers
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/TERuntimeResourceMap.hpp"
#include "areg/base/EContainers.hpp"
#include "areg/base/TEResourceMap.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
class EventConsumerList;
class EventConsumerMap;

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEEventConsumer;

/************************************************************************
 * \brief   In this file are declared Event Consumer contain classes:
 *              1. EventConsumerList  -- List of Event Consumers
 *              2. EventConsumerMap   -- Map of Event Consumer.
 *          These are helper classes used in Dispatcher object.
 *          For details, see description bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEListImpl<IEEventConsumer *>       ImplEventConsumerList;
/**
 * \brief   Event Consumer List is a helper class containing 
 *          Event Consumer objects. It is used in Dispatcher, when 
 *          collecting list of Consumers, which are registered 
 *          to dispatch certain Event Object.
 *          For use, see implementation of EventDispatcherBase class
 **/
class AREG_API EventConsumerList   : public TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default Constructor. Initializes empty list
     **/
    EventConsumerList( void );
    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    EventConsumerList(const EventConsumerList &src);
    /**
     * \brief   Destructor
     **/
    ~EventConsumerList( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigns entries from given source.
     * \param   src     THe source of event consumer list.
     */
    inline const EventConsumerList & operator = (const EventConsumerList & src );

    /**
     * \brief   Adds Event Consumer object to the list. Returns true, 
     *          if Event Consumer object is added to the list. The function does not 
     *          check whether the Event Consumer already exists in the list or not.
     *          To check whether there is already Event Consumer in the list
     *          call existConsumer() method.
     * \param   whichConsumer   The Event Consumer object to add to the list.
     * \return  Returns true if Event Consumer is added to the list.
     **/
    bool addConsumer( IEEventConsumer & whichConsumer );

    /**
     * \brief   Removes Event Consumer object from the List. The function will 
     *          search for the Event Consumer object by pointer and remove 
     *          the first match. The function does not check whether there is still 
     *          same Event Consumer exist in the list or not.
     *          To check whether there is already Event Consumer in the list
     *          call existConsumer() method.
     * \param   whichConsumer   The Event Consumer object to remove from the list.
     * \return  Returns true if Event Consumer was removed from the list.
     **/
    bool removeConsumer( IEEventConsumer & whichConsumer );

    /**
     * \brief   Removes all Event Consumers from the list.
     **/
    void removeAllConsumers( void );

    /**
     * \brief   Returns true, if the specified Event Consumer already exists in the list.
     *          The lookup will be done by pointer address value.
     * \param   whichConsumer   The Event Consumer object to search.
     * \return  Returns true, if the specified Event Consumer already exists in the list.
     **/
    inline bool existConsumer( IEEventConsumer & whichConsumer ) const;
};

//////////////////////////////////////////////////////////////////////////
// EventConsumerMap class declaration
//////////////////////////////////////////////////////////////////////////

class AREG_API EventConsumerMapImpl   : public TEResourceMapImpl<RuntimeClassID, EventConsumerList>
{
public:
    /**
     * \brief	Called when all resources are removed.
     * \param	Key	        The Key value of resource
     * \param	Resource	Pointer to resource object
     **/
    void implCleanResource( RuntimeClassID & Key, EventConsumerList * Resource );
};
/**
 * \brief   Event Consumer Map is a helper class containing 
 *          Event Consumer List objects registered for certain Events,
 *          which are Runtime type of objects. The Key value of the map
 *          is Runtime object and the Values are Consumer Event List objects.
 *          It is used in Dispatcher, when a Consumer is registered for Event.
 *          For use, see implementation of EventDispatcherBase class
 **/
class AREG_API EventConsumerMap    : public TELockRuntimeResourceMap<EventConsumerList, EventConsumerMapImpl>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default Constructor. Initializes empty map.
     **/
    EventConsumerMap( void );

    /**
     * \brief   Destructor
     **/
    virtual ~EventConsumerMap( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    EventConsumerMap(const EventConsumerMap &);
    const EventConsumerMap& operator = (const EventConsumerMap &);
};

//////////////////////////////////////////////////////////////////////////
// Inline functions implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class inline functions
//////////////////////////////////////////////////////////////////////////
inline const EventConsumerList & EventConsumerList::operator = (const EventConsumerList & src )
{
    static_cast<TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList> &>(*this) = static_cast<const TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList> &>(src);
    return (*this);
}

inline bool EventConsumerList::existConsumer( IEEventConsumer & whichConsumer ) const
{
    return (TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>::find( &whichConsumer, NULL) != NULL);
}

//////////////////////////////////////////////////////////////////////////
// EventConsumerMap class inline functions
//////////////////////////////////////////////////////////////////////////

#endif  // AREG_COMPONENT_PRIVATE_EVENTCONSUMERMAP_HPP
