#ifndef AREG_mtrouter_APP_ROUTERCONSOLESERVICE_HPP
#define AREG_mtrouter_APP_ROUTERCONSOLESERVICE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/app/private/RouterConsoleService.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Multi-cast routing, service that outputs statistics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "aregextend/console/SystemServiceConsole.hpp"

//////////////////////////////////////////////////////////////////////////
// RouterConsoleService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Console service that outputs router statistics.
 **/
class RouterConsoleService  : public    areg::ext::SystemServiceConsole
{
public:
    //!< The console service role name
    static constexpr std::string_view   SERVICE_NAME    { "RouterConcoleService" };

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes the component with entry information and owner thread.
     *
     * \param   entry       The component entry containing component information.
     * \param   owner       The component owner thread.
     **/
    RouterConsoleService( const areg::ComponentEntry & entry, areg::ComponentThread & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~RouterConsoleService() = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RouterConsoleService() = delete;
    AREG_NOCOPY_NOMOVE( RouterConsoleService );
};

#endif  // AREG_mtrouter_APP_ROUTERCONSOLESERVICE_HPP
