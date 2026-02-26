#pragma once

/************************************************************************
 * \file        locsvcmesh/src/ServicingComponent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
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
class ControllerComponent   : public    areg::Component
{
public:
    ControllerComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline ControllerComponent & self()
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
    ControllerComponent() = delete;
    AREG_NOCOPY_NOMOVE( ControllerComponent );
};

//////////////////////////////////////////////////////////////////////////
// SecondaryComponent class declaration
//////////////////////////////////////////////////////////////////////////
class SecondaryComponent    : public    areg::Component
{
public:
    SecondaryComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner );

private:
//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////

    inline SecondaryComponent & self()
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
    SecondaryComponent() = delete;
    AREG_NOCOPY_NOMOVE( SecondaryComponent );
};
