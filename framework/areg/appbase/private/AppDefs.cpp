/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/appbase/AppDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Application global namespace and configurations
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/appbase/AppDefs.hpp"

//! Logging type identifiers
AREG_API_IMPL const std::vector<Identifier>     NEApplication::LogTypeIdentifiers =
{
      {static_cast<uint32_t>(NELogging::LogTarget::Undefined)   , NEApplication::DefaultLogTypes[0].ltIdName        }
    , {static_cast<uint32_t>(NELogging::LogTarget::Remote)      , NEApplication::DefaultLogTypes[1].ltIdName        }
    , {static_cast<uint32_t>(NELogging::LogTarget::File)        , NEApplication::DefaultLogTypes[2].ltIdName        }
    , {static_cast<uint32_t>(NELogging::LogTarget::Debug)       , NEApplication::DefaultLogTypes[3].ltIdName        }
    , {static_cast<uint32_t>(NELogging::LogTarget::Database)    , NEApplication::DefaultLogTypes[4].ltIdName        }
};

//! Connection type identifiers
AREG_API_IMPL const std::vector<Identifier>   NEApplication::ConnectionIdentifiers
{
      { static_cast<uint32_t>(NERemoteService::ConnectionType::Undefined)  , NEApplication::DefaultConnections[0].ltIdName     }
    , { static_cast<uint32_t>(NERemoteService::ConnectionType::Tcpip)      , NEApplication::DefaultConnections[1].ltIdName     }
    , { static_cast<uint32_t>(NERemoteService::ConnectionType::Udp)        , NEApplication::DefaultConnections[2].ltIdName     }
    , { static_cast<uint32_t>(NERemoteService::ConnectionType::Web)        , NEApplication::DefaultConnections[3].ltIdName     }
    , { static_cast<uint32_t>(NERemoteService::ConnectionType::SharedMemory),NEApplication::DefaultConnections[4].ltIdName     }
};

//! Remote service identifiers
AREG_API_IMPL const std::vector<Identifier>   NEApplication::RemoteServiceIdentifiers
{
      { static_cast<uint32_t>(NERemoteService::RemoteServiceKind::Unknown)  , NEApplication::DefaultRemotetServices[0].ltIdName }
    , { static_cast<uint32_t>(NERemoteService::RemoteServiceKind::Router)   , NEApplication::DefaultRemotetServices[1].ltIdName }
    , { static_cast<uint32_t>(NERemoteService::RemoteServiceKind::Logger)   , NEApplication::DefaultRemotetServices[2].ltIdName }
};

//! Log priority identifiers
AREG_API_IMPL const std::vector<Identifier>   NEApplication::LogScopePriorityIndentifiers
{
      { static_cast<uint32_t>(NELogging::LogPriority::PrioInvalid)  , NELogging::PRIO_NO_PRIO                             }
    , { static_cast<uint32_t>(NELogging::LogPriority::PrioNotset)   , NELogging::PRIO_NOTSET_STR                          }
    , { static_cast<uint32_t>(NELogging::LogPriority::PrioScope)    , NELogging::PRIO_SCOPE_STR                           }
    , { static_cast<uint32_t>(NELogging::LogPriority::PrioFatal)    , NELogging::PRIO_FATAL_STR                           }
    , { static_cast<uint32_t>(NELogging::LogPriority::PrioError)    , NELogging::PRIO_ERROR_STR                           }
    , { static_cast<uint32_t>(NELogging::LogPriority::PrioWarning)  , NELogging::PRIO_WARNING_STR                         }
    , { static_cast<uint32_t>(NELogging::LogPriority::PrioInfo)     , NELogging::PRIO_INFO_STR                            }
    , { static_cast<uint32_t>(NELogging::LogPriority::PrioDebug)    , NELogging::PRIO_DEBUG_STR                           }
};

 //! Areg TCP/IP Multitarget Router Service name
AREG_API_IMPL char NEApplication::ROUTER_SERVICE_NAME_ASCII[]           { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! Areg TCP/IP Multitarget Router Service name
AREG_API_IMPL wchar_t NEApplication::ROUTER_SERVICE_NAME_WIDE[]         { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! Router Service executable
AREG_API_IMPL char NEApplication::ROUTER_SERVICE_EXECUTABLE_ASCII[]     { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

//! Router Service executable
AREG_API_IMPL wchar_t NEApplication::ROUTER_SERVICE_EXECUTABLE_WIDE[]   { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

//! Areg TCP/IP Log Collector Service name
AREG_API_IMPL char NEApplication::LOGGER_SERVICE_NAME_ASCII[]           { 'l', 'o', 'g', 'g', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! Areg TCP/IP Log Collector Service name
AREG_API_IMPL wchar_t NEApplication::LOGGER_SERVICE_NAME_WIDE[]         { 'l', 'o', 'g', 'g', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! Log Collector Service executable
AREG_API_IMPL char NEApplication::LOGGER_SERVICE_EXECUTABLE_ASCII[]     { 'l', 'o', 'g', 'g', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

//! Log Collector Service executable
AREG_API_IMPL wchar_t NEApplication::LOGGER_SERVICE_EXECUTABLE_WIDE[]   { 'l', 'o', 'g', 'g', 'e', 'r', '.', 'e', 'x', 'e', '\0' };
