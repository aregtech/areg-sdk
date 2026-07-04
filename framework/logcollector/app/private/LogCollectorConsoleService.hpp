#ifndef AREG_LOGCOLLECTOR_APP_LOGCOLLECTORCONSOLESERVICE_HPP
#define AREG_LOGCOLLECTOR_APP_LOGCOLLECTORCONSOLESERVICE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        logcollector/app/private/LogCollectorConsoleService.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Collector console.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "aregextend/console/SystemServiceConsole.hpp"

//////////////////////////////////////////////////////////////////////////
// LogCollectorConsoleService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service component that outputs statistics and provides console interaction for the
 *          LogCollector application.
 **/
class LogCollectorConsoleService  : public areg::ext::SystemServiceConsole
{
public:
    //!< The console service role name
    static constexpr std::string_view   SERVICE_NAME    { "LoggerConcoleService" };

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Instantiates the component with registry entry and owner thread.
     *
     * \param   entry       The registry entry that describes the component.
     * \param   owner       The component owner thread.
     **/
    LogCollectorConsoleService( const areg::ComponentEntry & entry, areg::ComponentThread & owner );

    virtual ~LogCollectorConsoleService() = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LogCollectorConsoleService() = delete;
    AREG_NOCOPY_NOMOVE( LogCollectorConsoleService );
};

#endif  // AREG_LOGCOLLECTOR_APP_LOGCOLLECTORCONSOLESERVICE_HPP
