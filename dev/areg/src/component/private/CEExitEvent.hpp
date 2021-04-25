#ifndef AREG_COMPONENT_PRIVATE_CEEXITEVENT_HPP
#define AREG_COMPONENT_PRIVATE_CEEXITEVENT_HPP
/************************************************************************
 * \file        areg/src/component/private/CEExitEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Exit Event declaration.
 *              All dispatchers receiving this event should be stopped
 *              and start preparing exit procedure
 *
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEEvent.hpp"

/**
 * \brief   Special exit event used to indicate completion of a job.
 *          The exit event is a singleton object, which is shared by more than thread.
 *          Normally, used to exit thread.
 **/
class CEExitEvent   : public CEEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare Event runtime information.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CEExitEvent)

//////////////////////////////////////////////////////////////////////////
// Public methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the instance of special exit event object
     **/
    static CEExitEvent & GetExitEvent( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor. Hidden. Instantiated only in static method
     **/
    CEExitEvent( void );

    /**
     * \brief   Destructor. Hidden. Cannot be deleted outside of class.
     **/
    virtual ~CEExitEvent( void );

private:
/************************************************************************/
// CEEvent class overrides. Hidden. Cannot be directly called.
/************************************************************************/

    /**
     * \brief   Call to destroy Event object.
     *          Overwrite if there is any special action should be performed
     *          before destroying event object.
     **/
    virtual void Destroy( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEExitEvent( const CEExitEvent & );
    const CEExitEvent & operator = ( const CEExitEvent & );
};

#endif  // AREG_COMPONENT_PRIVATE_CEEXITEVENT_HPP
