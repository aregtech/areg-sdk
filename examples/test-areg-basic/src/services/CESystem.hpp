/************************************************************************
 * \file        src/services/CESystem.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, tests
 *
 ************************************************************************/
#ifndef SERVICES_CESYSTEM_HPP_
#define SERVICES_CESYSTEM_HPP_

/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/src/base/GEGlobal.h"
#include "src/gen/CESystemStub.hpp"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/component/CETimer.hpp"
#include "areg/src/component/CETimerEvent.hpp"

class CESystem  : public    CEComponent
                , protected CESystemStub
                , protected IETimerConsumer
{
//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    static CEComponent * CreateComponent( const NERegistry::CEComponentEntry & entry, CEComponentThread & owner );

    static void DeleteComponent( CEComponent & compObject, const NERegistry::CEComponentEntry & entry );

//////////////////////////////////////////////////////////////////////////
// System Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Requested to power off.
     * \note    Has no response
     **/
    virtual void RequestPowerOff( void );

    /**
     * \brief   Request call.
     *          requested to power on
     * \note    Has no response
     **/
    virtual void RequestPowerOn( void );

    /**
     * \brief   Request call.
     *          requested to shutdown
     * \note    Has no response
     **/
    virtual void RequestShutdown( void );

    /**
     * \brief   Request call.
     *          requested to start.
     * \note    Has no response
     **/
    virtual void RequestStart( void );

protected:
/************************************************************************/
// CEStubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void StartupServiceInterface( CEComponent & holder );

    /**
     * \brief   This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and
     *          make cleanups in this function call.
     * \param   comThread   The component thread, which triggered shutdown command.
     **/
    virtual void ShutdownComponent( CEComponentThread & comThread );

protected:
/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired.
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void ProcessTimer( CETimer & timer );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CESystem( CEComponentThread & masterThread, const char * const roleName, NEMemory::uAlign data );

    virtual ~CESystem( void );

    inline CESystem & self( void );


private:
    CETimer mTimer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
public:
    CESystem( void );
    CESystem( const CESystem & /*src*/);
    const CESystem & operator = ( const CESystem & /*src*/ );
};

inline CESystem & CESystem::self( void )
{   return (*this);     }

#endif /* COMP_CESYSTEM_HPP_ */
