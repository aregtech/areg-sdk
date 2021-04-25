#pragma once
/************************************************************************
 * \file            edge/services/CEConnectionServicing.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/CEComponent.hpp"

class CEConnectionServicing    : public  CEComponent
{
//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    static CEComponent * CreateComponent( const NERegistry::CEComponentEntry & entry, CEComponentThread & owner );

    static void DeleteComponent( CEComponent & compObject, const NERegistry::CEComponentEntry & entry );

public:

/************************************************************************/
// CEComponent overrides
/************************************************************************/
    /**
     * \brief	This function is triggered by component thread when it 
     *          requires component to start up. Set listeners and make
     *          initialization in this function call.
     * \param	comThread	The component thread, which triggered startup command
     **/
    virtual void StartupComponent( CEComponentThread & comThread );

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    virtual void ShutdownComponent( CEComponentThread & comThread );

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    CEConnectionServicing( CEComponentThread & masterThread, const char * const roleName );
    virtual ~CEConnectionServicing( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline CEConnectionServicing & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    CEConnectionServicing( void );
    CEConnectionServicing( const CEConnectionServicing & /*src*/ );
    const CEConnectionServicing & operator = ( const CEConnectionServicing & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEConnectionServicing inline methods
//////////////////////////////////////////////////////////////////////////

inline CEConnectionServicing & CEConnectionServicing::self( void )
{   return (*this);         }
