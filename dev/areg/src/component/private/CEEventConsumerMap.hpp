#ifndef AREG_COMPONENT_PRIVATE_CEEVENTCONSUMERMAP_HPP
#define AREG_COMPONENT_PRIVATE_CEEVENTCONSUMERMAP_HPP
/************************************************************************
 * \file        areg/src/component/private/CEEventConsumerMap.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Consumer Resources Object declaration.
 *              This Resource Map object contains information of Event Consumers
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/src/base/TERuntimeResourceMap.hpp"
#include "areg/src/base/EContainers.hpp"
#include "areg/src/base/TEResourceMap.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
class CEEventConsumerList;
class CEEventConsumerMap;

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEEventConsumer;

/************************************************************************
 * \brief   In this file are declared Event Consumer contain classes:
 *              1. CEEventConsumerList  -- List of Event Consumers
 *              2. CEEventConsumerMap   -- Map of Event Consumer.
 *          These are helper classes used in Dispatcher object.
 *          For details, see description bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// CEEventConsumerList class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEListImpl<IEEventConsumer *>       ImplEventConsumerList;
/**
 * \brief   Event Consumer List is a helper class containing 
 *          Event Consumer objects. It is used in Dispatcher, when 
 *          collecting list of Consumers, which are registered 
 *          to dispatch certain Event Object.
 *          For use, see implementation of CEEventDispatcherBase class
 **/
class AREG_API CEEventConsumerList   : public TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default Constructor. Initializes empty list
     **/
    CEEventConsumerList( void );
    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    CEEventConsumerList(const CEEventConsumerList &src);
    /**
     * \brief   Destructor
     **/
    ~CEEventConsumerList( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigns entries from given source.
     * \param   src     THe source of event consumer list.
     */
    inline const CEEventConsumerList & operator = (const CEEventConsumerList & src );

    /**
     * \brief   Adds Event Consumer object to the list. Returns true, 
     *          if Event Consumer object is added to the list. The function does not 
     *          check whether the Event Consumer already exists in the list or not.
     *          To check whether there is already Event Consumer in the list
     *          call ConsumerExist() method.
     * \param   whichConsumer   The Event Consumer object to add to the list.
     * \return  Returns true if Event Consumer is added to the list.
     **/
    bool AddConsumer( IEEventConsumer & whichConsumer );

    /**
     * \brief   Removes Event Consumer object from the List. The function will 
     *          search for the Event Consumer object by pointer and remove 
     *          the first match. The function does not check whether there is still 
     *          same Event Consumer exist in the list or not.
     *          To check whether there is already Event Consumer in the list
     *          call ConsumerExist() method.
     * \param   whichConsumer   The Event Consumer object to remove from the list.
     * \return  Returns true if Event Consumer was removed from the list.
     **/
    bool RemoveConsumer( IEEventConsumer & whichConsumer );

    /**
     * \brief   Removes all Event Consumers from the list.
     **/
    void RemoveAllConsumers( void );

    /**
     * \brief   Returns true, if the specified Event Consumer already exists in the list.
     *          The lookup will be done by pointer address value.
     * \param   whichConsumer   The Event Consumer object to search.
     * \return  Returns true, if the specified Event Consumer already exists in the list.
     **/
    inline bool ConsumerExist( IEEventConsumer & whichConsumer ) const;

};

//////////////////////////////////////////////////////////////////////////
// CEEventConsumerMap class declaration
//////////////////////////////////////////////////////////////////////////

class AREG_API CEEventConsumerMapImpl   : public TEResourceMapImpl<CERuntimeClassID, CEEventConsumerList>
{
public:
    /**
     * \brief	Called when all resources are removed.
     * \param	Key	        The Key value of resource
     * \param	Resource	Pointer to resource object
     **/
    void ImplCleanResourceElement( CERuntimeClassID & Key, CEEventConsumerList * Resource );
};
/**
 * \brief   Event Consumer Map is a helper class containing 
 *          Event Consumer List objects registered for certain Events,
 *          which are Runtime type of objects. The Key value of the map
 *          is Runtime object and the Values are Consumer Event List objects.
 *          It is used in Dispatcher, when a Consumer is registered for Event.
 *          For use, see implementation of CEEventDispatcherBase class
 **/
class AREG_API CEEventConsumerMap    : public TELockRuntimeResourceMap<CEEventConsumerList, CEEventConsumerMapImpl>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default Constructor. Initializes empty map.
     **/
    CEEventConsumerMap( void );

    /**
     * \brief   Destructor
     **/
    virtual ~CEEventConsumerMap( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    CEEventConsumerMap(const CEEventConsumerMap &);
    const CEEventConsumerMap& operator = (const CEEventConsumerMap &);
};

//////////////////////////////////////////////////////////////////////////
// Inline functions implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEEventConsumerList class inline functions
//////////////////////////////////////////////////////////////////////////
inline const CEEventConsumerList & CEEventConsumerList::operator = (const CEEventConsumerList & src )
{
    static_cast<TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList> &>(*this) = static_cast<const TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList> &>(src);
    return (*this);
}

inline bool CEEventConsumerList::ConsumerExist( IEEventConsumer & whichConsumer ) const
{   return (TELinkedList<IEEventConsumer *, IEEventConsumer *, ImplEventConsumerList>::Find( &whichConsumer, NULL) != NULL);    }

//////////////////////////////////////////////////////////////////////////
// CEEventConsumerMap class inline functions
//////////////////////////////////////////////////////////////////////////

#endif  // AREG_COMPONENT_PRIVATE_CEEVENTCONSUMERMAP_HPP
