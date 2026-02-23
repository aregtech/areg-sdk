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
AREG_API_IMPL const std::vector<Identifier>     areg::LogTypeIdentifiers =
{
      {static_cast<uint32_t>(areg::LogTarget::Undefined)   , areg::DefaultLogTypes[0].ltIdName        }
    , {static_cast<uint32_t>(areg::LogTarget::Remote)      , areg::DefaultLogTypes[1].ltIdName        }
    , {static_cast<uint32_t>(areg::LogTarget::File)        , areg::DefaultLogTypes[2].ltIdName        }
    , {static_cast<uint32_t>(areg::LogTarget::Debug)       , areg::DefaultLogTypes[3].ltIdName        }
    , {static_cast<uint32_t>(areg::LogTarget::Database)    , areg::DefaultLogTypes[4].ltIdName        }
};

//! Connection type identifiers
AREG_API_IMPL const std::vector<Identifier>   areg::ConnectionIdentifiers
{
      { static_cast<uint32_t>(areg::ConnectionType::Undefined)  , areg::DefaultConnections[0].ltIdName     }
    , { static_cast<uint32_t>(areg::ConnectionType::Tcpip)      , areg::DefaultConnections[1].ltIdName     }
    , { static_cast<uint32_t>(areg::ConnectionType::Udp)        , areg::DefaultConnections[2].ltIdName     }
    , { static_cast<uint32_t>(areg::ConnectionType::Web)        , areg::DefaultConnections[3].ltIdName     }
    , { static_cast<uint32_t>(areg::ConnectionType::SharedMemory),areg::DefaultConnections[4].ltIdName     }
};

//! Remote service identifiers
AREG_API_IMPL const std::vector<Identifier>   areg::RemoteServiceIdentifiers
{
      { static_cast<uint32_t>(areg::RemoteServiceKind::Unknown)  , areg::DefaultRemotetServices[0].ltIdName }
    , { static_cast<uint32_t>(areg::RemoteServiceKind::Router)   , areg::DefaultRemotetServices[1].ltIdName }
    , { static_cast<uint32_t>(areg::RemoteServiceKind::Logger)   , areg::DefaultRemotetServices[2].ltIdName }
};

//! Log priority identifiers
AREG_API_IMPL const std::vector<Identifier>   areg::LogScopePriorityIndentifiers
{
      { static_cast<uint32_t>(areg::LogPriority::PrioInvalid)  , areg::PRIO_NO_PRIO                             }
    , { static_cast<uint32_t>(areg::LogPriority::PrioNotset)   , areg::PRIO_NOTSET_STR                          }
    , { static_cast<uint32_t>(areg::LogPriority::PrioScope)    , areg::PRIO_SCOPE_STR                           }
    , { static_cast<uint32_t>(areg::LogPriority::PrioFatal)    , areg::PRIO_FATAL_STR                           }
    , { static_cast<uint32_t>(areg::LogPriority::PrioError)    , areg::PRIO_ERROR_STR                           }
    , { static_cast<uint32_t>(areg::LogPriority::PrioWarning)  , areg::PRIO_WARNING_STR                         }
    , { static_cast<uint32_t>(areg::LogPriority::PrioInfo)     , areg::PRIO_INFO_STR                            }
    , { static_cast<uint32_t>(areg::LogPriority::PrioDebug)    , areg::PRIO_DEBUG_STR                           }
};

 //! Areg TCP/IP Multitarget Router Service name
AREG_API_IMPL char areg::ROUTER_SERVICE_NAME_ASCII[]           { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! Areg TCP/IP Multitarget Router Service name
AREG_API_IMPL wchar_t areg::ROUTER_SERVICE_NAME_WIDE[]         { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! Router Service executable
AREG_API_IMPL char areg::ROUTER_SERVICE_EXECUTABLE_ASCII[]     { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

//! Router Service executable
AREG_API_IMPL wchar_t areg::ROUTER_SERVICE_EXECUTABLE_WIDE[]   { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

//! Areg TCP/IP Log Collector Service name
AREG_API_IMPL char areg::LOGGER_SERVICE_NAME_ASCII[]           { 'l', 'o', 'g', 'g', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! Areg TCP/IP Log Collector Service name
AREG_API_IMPL wchar_t areg::LOGGER_SERVICE_NAME_WIDE[]         { 'l', 'o', 'g', 'g', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! Log Collector Service executable
AREG_API_IMPL char areg::LOGGER_SERVICE_EXECUTABLE_ASCII[]     { 'l', 'o', 'g', 'g', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

//! Log Collector Service executable
AREG_API_IMPL wchar_t areg::LOGGER_SERVICE_EXECUTABLE_WIDE[]   { 'l', 'o', 'g', 'g', 'e', 'r', '.', 'e', 'x', 'e', '\0' };
