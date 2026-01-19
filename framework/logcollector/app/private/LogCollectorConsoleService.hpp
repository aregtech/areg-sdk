#ifndef AREG_LOGCOLLECTOR_APP_LOGCOLLECTORCONSOLESERVICE_HPP
#define AREG_LOGCOLLECTOR_APP_LOGCOLLECTORCONSOLESERVICE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        logcollector/app/private/LogCollectorConsoleService.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Collector console.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "aregextend/console/SystemServiceConsole.hpp"

//////////////////////////////////////////////////////////////////////////
// LogCollectorConsoleService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A service to output statistics..
 **/
class LogCollectorConsoleService  : public SystemServiceConsole
{
public:
    //!< The console service role name
    static constexpr std::string_view   SERVICE_NAME    { "LoggerConcoleService" };

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owner thread.
     * \param   data    The optional component data set in system. Can be empty / no data.
     **/
    LogCollectorConsoleService( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~LogCollectorConsoleService( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LogCollectorConsoleService( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LogCollectorConsoleService );
};

#endif  // AREG_LOGCOLLECTOR_APP_LOGCOLLECTORCONSOLESERVICE_HPP
