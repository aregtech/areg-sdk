#pragma once
/************************************************************************
 * \file            chatter/services/ConnectionService.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"

class ConnectionService    : public  areg::Component
{
public:
    ConnectionService(const areg::ComponentEntry& entry, areg::ComponentThread& ownerThread);

protected:

/************************************************************************/
// Component overrides
/************************************************************************/
    /**
     * \brief	This function is triggered by component thread when it 
     *          requires component to start up. Set listeners and make
     *          initialization in this function call.
     * \param	comThread	The component thread, which triggered startup command
     **/
    void startupComponent( areg::ComponentThread & comThread ) override;

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    void shutdownComponent( areg::ComponentThread & comThread ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ConnectionService & self();

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ConnectionService();
    ConnectionService( const ConnectionService & /*src*/ );
    const ConnectionService & operator = ( const ConnectionService & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ConnectionService inline methods
//////////////////////////////////////////////////////////////////////////

inline ConnectionService & ConnectionService::self()
{   return (*this);         }
