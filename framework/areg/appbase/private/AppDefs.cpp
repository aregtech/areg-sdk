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
      {static_cast<unsigned int>(areg::LoggingType::LogTypeUndefined)               , areg::DefaultLogTypes[0].ltIdName        }
    , {static_cast<unsigned int>(areg::LoggingType::LogTypeRemote)                  , areg::DefaultLogTypes[1].ltIdName        }
    , {static_cast<unsigned int>(areg::LoggingType::LogTypeFile)                    , areg::DefaultLogTypes[2].ltIdName        }
    , {static_cast<unsigned int>(areg::LoggingType::LogTypeDebug)                   , areg::DefaultLogTypes[3].ltIdName        }
    , {static_cast<unsigned int>(areg::LoggingType::LogTypeDatabase)                , areg::DefaultLogTypes[4].ltIdName        }
};

//! Connection type identifiers
AREG_API_IMPL const std::vector<Identifier>   areg::ConnectionIdentifiers
{
      { static_cast<unsigned int>(areg::eConnectionTypes::ConnectUndefined)  , areg::DefaultConnections[0].ltIdName     }
    , { static_cast<unsigned int>(areg::eConnectionTypes::ConnectTcpip)      , areg::DefaultConnections[1].ltIdName     }
    , { static_cast<unsigned int>(areg::eConnectionTypes::ConnectUdp)        , areg::DefaultConnections[2].ltIdName     }
    , { static_cast<unsigned int>(areg::eConnectionTypes::ConnectWeb)        , areg::DefaultConnections[3].ltIdName     }
    , { static_cast<unsigned int>(areg::eConnectionTypes::ConnectSM)         , areg::DefaultConnections[4].ltIdName     }
};

//! Remote service identifiers
AREG_API_IMPL const std::vector<Identifier>   areg::RemoteServiceIdentifiers
{
      { static_cast<unsigned int>(areg::eRemoteServices::ServiceUnknown)     , areg::DefaultRemotetServices[0].ltIdName }
    , { static_cast<unsigned int>(areg::eRemoteServices::ServiceRouter)      , areg::DefaultRemotetServices[1].ltIdName }
    , { static_cast<unsigned int>(areg::eRemoteServices::ServiceLogger)      , areg::DefaultRemotetServices[2].ltIdName }
};

//! Log priority identifiers
AREG_API_IMPL const std::vector<Identifier>   areg::LogScopePriorityIndentifiers
{
      { static_cast<unsigned int>(areg::eLogPriority::PrioInvalid)                   , areg::PRIO_NO_PRIO                             }
    , { static_cast<unsigned int>(areg::eLogPriority::PrioNotset)                    , areg::PRIO_NOTSET_STR                          }
    , { static_cast<unsigned int>(areg::eLogPriority::PrioScope)                     , areg::PRIO_SCOPE_STR                           }
    , { static_cast<unsigned int>(areg::eLogPriority::PrioFatal)                     , areg::PRIO_FATAL_STR                           }
    , { static_cast<unsigned int>(areg::eLogPriority::PrioError)                     , areg::PRIO_ERROR_STR                           }
    , { static_cast<unsigned int>(areg::eLogPriority::PrioWarning)                   , areg::PRIO_WARNING_STR                         }
    , { static_cast<unsigned int>(areg::eLogPriority::PrioInfo)                      , areg::PRIO_INFO_STR                            }
    , { static_cast<unsigned int>(areg::eLogPriority::PrioDebug)                     , areg::PRIO_DEBUG_STR                           }
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
