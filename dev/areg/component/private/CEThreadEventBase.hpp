#ifndef AREG_COMPONENT_PRIVATE_CETHREADEVENTBASE_HPP
#define AREG_COMPONENT_PRIVATE_CETHREADEVENTBASE_HPP
/************************************************************************
 * \file        areg/component/private/CEThreadEventBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Multi-Threading communication Event.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/CEEvent.hpp"
#include "areg/component/IEEventConsumer.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
class CEThreadEventBase;
class CEThreadEventConsumerBase;

/************************************************************************
 * \brief   In this file, following classes are declared:
 *              1. CEThreadEventBase
 *              2. CEThreadEventConsumerBase
 *          These classes are base classes for multi-threading communication.
 *          All Events and Consumers, which are supporting multi-threading
 *          communication, are derived from these classes.
 *          The objects are not possible to initialize directly. Instead,
 *          use one of derived classes, or derive class to instantiate manually.
 *          For more details see descriptions bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// CEThreadEventBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for multi-threading communication.
 *          Declared to identify multi-threading events.
 *          It is hidden. Do not use directly.
 *          Use declared custom TEEvent events instead.
 *          
 **/
class AREG_API CEThreadEventBase     : public CEEvent
{
//////////////////////////////////////////////////////////////////////////
// Runtime declaration
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Declare Runtime relevant functions.
     *          We need to identify multi-threading events
     **/
    DECLARE_RUNTIME_EVENT(CEThreadEventBase)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor. Protected.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialization constructor.
     *          Creates Thread Event and sets event type.
     **/
    CEThreadEventBase( CEEvent::eEventType eventType );

    /**
     * \brief   Destructor
     **/
    virtual ~CEThreadEventBase( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEThreadEventBase( void );
    CEThreadEventBase( const CEThreadEventBase & /*src*/ );
    const CEThreadEventBase & operator = ( const CEThreadEventBase & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEThreadEventConsumerBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base event consumer class used for multi-threading communication.
 *          Declared to identify multi-threading event consumer.
 *          Do not use directly. 
 *          Use declared custom TEEventConsumer event consumer instead.
 *          
 **/
class AREG_API CEThreadEventConsumerBase : public IEEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor. Protected.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    CEThreadEventConsumerBase( void );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEThreadEventConsumerBase( void );
};

#endif  // AREG_COMPONENT_PRIVATE_CETHREADEVENTBASE_HPP
