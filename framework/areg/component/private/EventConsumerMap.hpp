#ifndef AREG_COMPONENT_PRIVATE_EVENTCONSUMERMAP_HPP
#define AREG_COMPONENT_PRIVATE_EVENTCONSUMERMAP_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/EventConsumerMap.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Consumer Resources Object declaration.
 *              This Resource Map object contains information of Event Consumers
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/RuntimeResourceMap.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/ResourceListMap.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class EventConsumer;
} // namespace areg

namespace areg {

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
using EventConsumerListBase	= ArrayList<EventConsumer *>;

/**
 * \brief   Helper class containing Event Consumer objects. Used in Dispatcher when collecting list
 *          of Consumers registered to dispatch certain Event Object.
 **/
class EventConsumerList   : public EventConsumerListBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    EventConsumerList() = default;

    EventConsumerList(const EventConsumerList & src) = default;

    EventConsumerList( EventConsumerList && src ) noexcept = default;

    ~EventConsumerList();

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigns entries from given source.
     *
     * \param   src     The source of event consumer list.
     **/
    inline EventConsumerList & operator = (const EventConsumerList & src ) = default;

    /**
     * \brief   Assigns entries from given source.
     *
     * \param   src     The source of event consumer list.
     **/
    inline EventConsumerList & operator = ( EventConsumerList && src ) noexcept = default;

    /**
     * \brief   Adds Event Consumer object to the list. Returns true if successfully added. Does not
     *          check for duplicates; use exist() to verify.
     *
     * \param   whichConsumer       The Event Consumer object to add to the list.
     * \return  Returns true if Event Consumer is added to the list.
     **/
    bool add_consumer( EventConsumer & whichConsumer );

    /**
     * \brief   Removes Event Consumer object from the list. Searches by pointer and removes the
     *          first match. Does not check for further occurrences; use exist() to verify.
     *
     * \param   whichConsumer       The Event Consumer object to remove from the list.
     * \return  Returns true if Event Consumer was removed from the list.
     **/
    bool remove_consumer( EventConsumer & whichConsumer ) noexcept;

    /**
     * \brief   Removes all Event Consumers from the list.
     **/
    void remove_all_consumers() noexcept;

    /**
     * \brief   Returns true if the specified Event Consumer exists in the list. Lookup is done by
     *          pointer address value.
     *
     * \param   whichConsumer       The Event Consumer object to search.
     * \return  Returns true if the specified Event Consumer already exists in the list.
     **/
    [[nodiscard]]
    inline bool exist( EventConsumer & whichConsumer ) const noexcept;
};

//////////////////////////////////////////////////////////////////////////
// ImplConsumerMap internal class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Helper class for managing maps of thread-local proxy lists.
 **/
class ImplConsumerMap : public ResourceListMapImpl<uint32_t, EventConsumer*, EventConsumerList>
{
public:
    /**
     * \brief   Called when all resources are removed from the map.
     **/
    inline void impl_clean_list(const uint32_t& /* Key */, EventConsumerList& List) noexcept
    {
        List.remove_all_consumers();
    }

    /**
     * \brief   Adds a proxy object to the given list.
     *
     * \param   List        The list of proxy objects to add to.
     * \param   Resource    The proxy object to add.
     **/
    inline void impl_add_resource(EventConsumerList& List, EventConsumer* Resource)
    {
        if (Resource != nullptr)
        {
            List.add_consumer(*Resource);
        }
    }

    /**
     * \brief   Removes a proxy object from the given list.
     *
     * \param   List        The list of proxy objects to remove from.
     * \param   Resource    The proxy object to remove.
     * \return  True if the resource was found and removed; false otherwise.
     **/
    inline bool impl_remove_resource(EventConsumerList& List, EventConsumer* Resource) noexcept
    {
        return (Resource != nullptr ? List.remove_consumer(*Resource) : false);
    }
};

/**
 * \brief   The integer hash map which values are list of proxies.
 **/
using MapConsumer = IntegerHashMap<EventConsumerList>;

/**
 * \brief   The Map of the list, where the key is a magic number and values are list of proxies.
 **/
using EventConsumerMap = ConcurrentResourceListMap<uint32_t, EventConsumer*, EventConsumerList, MapConsumer, ImplConsumerMap>;

//////////////////////////////////////////////////////////////////////////
// Inline functions implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool EventConsumerList::exist( EventConsumer & whichConsumer ) const noexcept
{
    return EventConsumerListBase::contains( &whichConsumer);
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_EVENTCONSUMERMAP_HPP
