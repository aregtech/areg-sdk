#pragma once
/************************************************************************
 * \file            chatter/services/ConnectionServicing.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"

class ConnectionServicing    : public  Component
{
//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

public:

/************************************************************************/
// Component overrides
/************************************************************************/
    /**
     * \brief	This function is triggered by component thread when it 
     *          requires component to start up. Set listeners and make
     *          initialization in this function call.
     * \param	comThread	The component thread, which triggered startup command
     **/
    virtual void startupComponent( ComponentThread & comThread );

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    virtual void shutdownComponent( ComponentThread & comThread );

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    ConnectionServicing( ComponentThread & masterThread, const char * const roleName );
    virtual ~ConnectionServicing( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ConnectionServicing & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ConnectionServicing( void );
    ConnectionServicing( const ConnectionServicing & /*src*/ );
    const ConnectionServicing & operator = ( const ConnectionServicing & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ConnectionServicing inline methods
//////////////////////////////////////////////////////////////////////////

inline ConnectionServicing & ConnectionServicing::self( void )
{   return (*this);         }
