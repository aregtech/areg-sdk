#pragma once

/************************************************************************
 * \file        locsvcmesh/src/ServicingComponent.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/component/Component.hpp"
#include "locsvcmesh/src/ServiceHelloWorld.hpp"
#include "locsvcmesh/src/ServiceClient.hpp"

//////////////////////////////////////////////////////////////////////////
// ControllerComponent class declaration
//////////////////////////////////////////////////////////////////////////
//! \brief   The servicing component, which controls the application shutdown.
class ControllerComponent   : public    Component
{
public:
    ControllerComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline ControllerComponent & self( void )
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
    ServiceHelloWorld   mService;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ControllerComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ControllerComponent );
};

//////////////////////////////////////////////////////////////////////////
// SecondaryComponent class declaration
//////////////////////////////////////////////////////////////////////////
class SecondaryComponent    : public    Component
{
public:
    SecondaryComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

private:
//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////

    inline SecondaryComponent & self( void )
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
    ServiceHelloWorld   mService;           //!< The service included in the component
    ServiceClient       mClientMain;        //!< Service client of main component
    ServiceClient       mClientSecondary;   //!< Service client of secondary component.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SecondaryComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SecondaryComponent );
};
