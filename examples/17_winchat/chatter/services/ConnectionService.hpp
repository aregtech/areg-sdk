#pragma once
/************************************************************************
 * \file            chatter/services/ConnectionService.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"

class ConnectionService    : public  Component
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
    ConnectionService( const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread );
    virtual ~ConnectionService( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ConnectionService & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ConnectionService( void );
    ConnectionService( const ConnectionService & /*src*/ );
    const ConnectionService & operator = ( const ConnectionService & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ConnectionService inline methods
//////////////////////////////////////////////////////////////////////////

inline ConnectionService & ConnectionService::self( void )
{   return (*this);         }
