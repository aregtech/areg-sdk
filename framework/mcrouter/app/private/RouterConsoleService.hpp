#ifndef AREG_MCROUTER_APP_ROUTERCONSOLESERVICE_HPP
#define AREG_MCROUTER_APP_ROUTERCONSOLESERVICE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/RouterConsoleService.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing, service that outputs statistics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "aregextend/console/SystemServiceConsole.hpp"

//////////////////////////////////////////////////////////////////////////
// RouterConsoleService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A service to output statistics..
 **/
class RouterConsoleService  : private    SystemServiceConsole
{
public:
    //!< The console service role name
    static constexpr std::string_view   SERVICE_NAME    { "RouterConcoleService" };

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The instance of the component entry that contains the component information.
     * \param   owner   The instance of component owner thread.
     * \param   data    The optional component data set in system. Can be empty / no data.
     **/
    RouterConsoleService( const NERegistry::ComponentEntry & entry, ComponentThread & owner, NEMemory::uAlign OPT data );

    /**
     * \brief   Destructor.
     **/
    virtual ~RouterConsoleService( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RouterConsoleService( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( RouterConsoleService );
};

#endif  // AREG_MCROUTER_APP_ROUTERCONSOLESERVICE_HPP
