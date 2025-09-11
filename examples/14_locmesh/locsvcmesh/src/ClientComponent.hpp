#pragma once

/************************************************************************
 * \file        locsvcmesh/src/ClientComponent.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              The component that contains service clients.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/Component.hpp"
#include "locsvcmesh/src/ServiceClient.hpp"

//////////////////////////////////////////////////////////////////////////
// ClientComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The main component that controls application shutdown.
 **/
class ClientComponent : public Component
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ClientComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

private:
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////

    inline ClientComponent & self( void )
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
    ServiceClient   mClientMain;        //!< Service client of main component
    ServiceClient   mClientSecondary;   //!< Service client of secondary component.
    ServiceClient   mClientThird;       //!< Third service

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ClientComponent );
};
