#ifndef AREG_COMPONENT_PRIVATE_EXITEVENT_HPP
#define AREG_COMPONENT_PRIVATE_EXITEVENT_HPP
/************************************************************************
 * \file        areg/component/private/ExitEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Exit Event declaration.
 *              All dispatchers receiving this event should be stopped
 *              and start preparing exit procedure
 *
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/Event.hpp"

/**
 * \brief   Special exit event used to indicate completion of a job.
 *          The exit event is a singleton object, which is shared by more than thread.
 *          Normally, used to exit thread.
 **/
class ExitEvent : public Event
{
//////////////////////////////////////////////////////////////////////////
// Declare Event runtime information.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(ExitEvent)

//////////////////////////////////////////////////////////////////////////
// Public methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the instance of special exit event object
     **/
    static ExitEvent & getExitEvent( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor. Hidden. Instantiated only in static method
     **/
    ExitEvent( void );

    /**
     * \brief   Destructor. Hidden. Cannot be deleted outside of class.
     **/
    virtual ~ExitEvent( void );

private:
/************************************************************************/
// Event class overrides. Hidden. Cannot be directly called.
/************************************************************************/

    /**
     * \brief   Call to destroy Event object.
     *          Overwrite if there is any special action should be performed
     *          before destroying event object.
     **/
    virtual void destroy( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ExitEvent( const ExitEvent & );
    const ExitEvent & operator = ( const ExitEvent & );
};

#endif  // AREG_COMPONENT_PRIVATE_EXITEVENT_HPP
