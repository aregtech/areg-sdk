#ifndef AREG_mtrouter_APP_ROUTERCONSOLESERVICE_HPP
#define AREG_mtrouter_APP_ROUTERCONSOLESERVICE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/app/private/RouterConsoleService.hpp
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
class RouterConsoleService  : public    SystemServiceConsole
{
public:
    //!< The console service role name
    static constexpr std::string_view   SERVICE_NAME    { "RouterConcoleService" };

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The instance of the component entry that contains the component information.
     * \param   owner   The instance of component owner thread.
     **/
    RouterConsoleService( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

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

#endif  // AREG_mtrouter_APP_ROUTERCONSOLESERVICE_HPP
