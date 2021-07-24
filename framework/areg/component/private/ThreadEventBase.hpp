#ifndef AREG_COMPONENT_PRIVATE_THREADEVENTBASE_HPP
#define AREG_COMPONENT_PRIVATE_THREADEVENTBASE_HPP
/************************************************************************
 * \file        areg/component/private/ThreadEventBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Multi-Threading communication Event.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/Event.hpp"
#include "areg/component/IEEventConsumer.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
class ThreadEventBase;
class ThreadEventConsumerBase;

/************************************************************************
 * \brief   In this file, following classes are declared:
 *              1. ThreadEventBase
 *              2. ThreadEventConsumerBase
 *          These classes are base classes for multi-threading communication.
 *          All Events and Consumers, which are supporting multi-threading
 *          communication, are derived from these classes.
 *          The objects are not possible to initialize directly. Instead,
 *          use one of derived classes, or derive class to instantiate manually.
 *          For more details see descriptions bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// ThreadEventBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for multi-threading communication.
 *          Declared to identify multi-threading events.
 *          It is hidden. Do not use directly.
 *          Use declared custom TEEvent events instead.
 *          
 **/
class AREG_API ThreadEventBase     : public Event
{
//////////////////////////////////////////////////////////////////////////
// Runtime declaration
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Declare Runtime relevant functions.
     *          We need to identify multi-threading events
     **/
    DECLARE_RUNTIME_EVENT(ThreadEventBase)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor. Protected.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialization constructor.
     *          Creates Thread Event and sets event type.
     **/
    ThreadEventBase( Event::eEventType eventType );

    /**
     * \brief   Destructor
     **/
    virtual ~ThreadEventBase( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ThreadEventBase( void );
    ThreadEventBase( const ThreadEventBase & /*src*/ );
    const ThreadEventBase & operator = ( const ThreadEventBase & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ThreadEventConsumerBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base event consumer class used for multi-threading communication.
 *          Declared to identify multi-threading event consumer.
 *          Do not use directly. 
 *          Use declared custom TEEventConsumer event consumer instead.
 *          
 **/
class AREG_API ThreadEventConsumerBase : public IEEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor. Protected.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    ThreadEventConsumerBase( void );

    /**
     * \brief   Destructor.
     **/
    virtual ~ThreadEventConsumerBase( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    ThreadEventConsumerBase( const ThreadEventConsumerBase & /*src*/ );
    const ThreadEventConsumerBase & operator = ( const ThreadEventConsumerBase & /*src*/ );
};

#endif  // AREG_COMPONENT_PRIVATE_THREADEVENTBASE_HPP
