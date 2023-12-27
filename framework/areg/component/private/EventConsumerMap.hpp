#ifndef AREG_COMPONENT_PRIVATE_EVENTCONSUMERMAP_HPP
#define AREG_COMPONENT_PRIVATE_EVENTCONSUMERMAP_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventConsumerMap.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Event Consumer Resources Object declaration.
 *              This Resource Map object contains information of Event Consumers
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/TERuntimeResourceMap.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/TEResourceMap.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
class EventConsumerList;

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
using EventConsumerListBase	= TELinkedList<IEEventConsumer *>;

/**
 * \brief   Event Consumer List is a helper class containing 
 *          Event Consumer objects. It is used in Dispatcher, when 
 *          collecting list of Consumers, which are registered 
 *          to dispatch certain Event Object.
 *          For use, see implementation of EventDispatcherBase class
 **/
class EventConsumerList   : public EventConsumerListBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default Constructor. Initializes empty list
     **/
    EventConsumerList( void ) = default;
    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    EventConsumerList(const EventConsumerList & src) = default;
    /**
     * \brief   Move constructor.
     * \param   src     The source of data to move.
     **/
    EventConsumerList( EventConsumerList && src ) noexcept = default;
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
    inline EventConsumerList & operator = (const EventConsumerList & src ) = default;

    /**
     * \brief   Assigns entries from given source.
     * \param   src     THe source of event consumer list.
     */
    inline EventConsumerList & operator = ( EventConsumerList && src ) noexcept = default;

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

class ImplEventConsumerMap	: public TEResourceMapImpl<RuntimeClassID, EventConsumerList *>
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
using EventConsumerMap  = TELockRuntimeResourceMap<EventConsumerList *, ImplEventConsumerMap>;

//////////////////////////////////////////////////////////////////////////
// Inline functions implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool EventConsumerList::existConsumer( IEEventConsumer & whichConsumer ) const
{
    return EventConsumerListBase::contains( &whichConsumer);
}

#endif  // AREG_COMPONENT_PRIVATE_EVENTCONSUMERMAP_HPP
