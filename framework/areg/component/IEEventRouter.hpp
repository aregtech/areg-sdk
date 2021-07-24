#ifndef AREG_COMPONENT_IEEVENTROUTER_HPP
#define AREG_COMPONENT_IEEVENTROUTER_HPP
/************************************************************************
 * \file        areg/component/IEEventRouter.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Router interface
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Event;

//////////////////////////////////////////////////////////////////////////
// IEEventRouter class declarations
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Event Route interface. Defines class, which is delivering
 *              Event to its target thread.
 * 
 * \details     Dispatcher classes are instances of IEEventRouter and
 *              have implemented post method to find target thread and
 *              delivery event for further processing.
 *
 **/
class AREG_API IEEventRouter
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    IEEventRouter( void );

    /**
     * \brief   Destructor
     **/
    virtual ~IEEventRouter( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Posts event and delivers to its target thread / process.
     * \param	eventElem	Event object to post.
     * \return	Returns true if target was found and the event
     *          delivered with success. Otherwise it returns false.
     **/
    virtual bool postEvent( Event & eventElem ) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IEEventRouter(const IEEventRouter & /*src*/ );
    const IEEventRouter& operator = (const IEEventRouter & /*src*/ );
};

#endif  // AREG_COMPONENT_IEEVENTROUTER_HPP
