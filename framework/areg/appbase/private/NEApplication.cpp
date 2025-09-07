/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/appbase/NEApplication.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Application global namespace and configurations
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/appbase/NEApplication.hpp"

//! Logging type identifiers
AREG_API_IMPL const std::vector<Identifier>     NEApplication::LogTypeIdentifiers =
{
      {static_cast<unsigned int>(NELogging::eLogingTypes::LogTypeUndefined)               , NEApplication::DefaultLogTypes[0].ltIdName        }
    , {static_cast<unsigned int>(NELogging::eLogingTypes::LogTypeRemote)                  , NEApplication::DefaultLogTypes[1].ltIdName        }
    , {static_cast<unsigned int>(NELogging::eLogingTypes::LogTypeFile)                    , NEApplication::DefaultLogTypes[2].ltIdName        }
    , {static_cast<unsigned int>(NELogging::eLogingTypes::LogTypeDebug)                   , NEApplication::DefaultLogTypes[3].ltIdName        }
    , {static_cast<unsigned int>(NELogging::eLogingTypes::LogTypeDatabase)                , NEApplication::DefaultLogTypes[4].ltIdName        }
};

//! Connection type identifiers
AREG_API_IMPL const std::vector<Identifier>   NEApplication::ConnectionIdentifiers
{
      { static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectUndefined)  , NEApplication::DefaultConnections[0].ltIdName     }
    , { static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectTcpip)      , NEApplication::DefaultConnections[1].ltIdName     }
    , { static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectUdp)        , NEApplication::DefaultConnections[2].ltIdName     }
    , { static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectWeb)        , NEApplication::DefaultConnections[3].ltIdName     }
    , { static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectSM)         , NEApplication::DefaultConnections[4].ltIdName     }
};

//! Remote service identifiers
AREG_API_IMPL const std::vector<Identifier>   NEApplication::RemoteServiceIdentifiers
{
      { static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceUnknown)     , NEApplication::DefaultRemotetServices[0].ltIdName }
    , { static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceRouter)      , NEApplication::DefaultRemotetServices[1].ltIdName }
    , { static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceLogger)      , NEApplication::DefaultRemotetServices[2].ltIdName }
};

//! Log priority identifiers
AREG_API_IMPL const std::vector<Identifier>   NEApplication::LogScopePriorityIndentifiers
{
      { static_cast<unsigned int>(NELogging::eLogPriority::PrioInvalid)                   , NELogging::PRIO_NO_PRIO                             }
    , { static_cast<unsigned int>(NELogging::eLogPriority::PrioNotset)                    , NELogging::PRIO_NOTSET_STR                          }
    , { static_cast<unsigned int>(NELogging::eLogPriority::PrioScope)                     , NELogging::PRIO_SCOPE_STR                           }
    , { static_cast<unsigned int>(NELogging::eLogPriority::PrioFatal)                     , NELogging::PRIO_FATAL_STR                           }
    , { static_cast<unsigned int>(NELogging::eLogPriority::PrioError)                     , NELogging::PRIO_ERROR_STR                           }
    , { static_cast<unsigned int>(NELogging::eLogPriority::PrioWarning)                   , NELogging::PRIO_WARNING_STR                         }
    , { static_cast<unsigned int>(NELogging::eLogPriority::PrioInfo)                      , NELogging::PRIO_INFO_STR                            }
    , { static_cast<unsigned int>(NELogging::eLogPriority::PrioDebug)                     , NELogging::PRIO_DEBUG_STR                           }
};

 //! AREG TCP/IP Multitarget Router Service name
AREG_API_IMPL char NEApplication::ROUTER_SERVICE_NAME_ASCII[]           { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! AREG TCP/IP Multitarget Router Service name
AREG_API_IMPL wchar_t NEApplication::ROUTER_SERVICE_NAME_WIDE[]         { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! Router Service executable
AREG_API_IMPL char NEApplication::ROUTER_SERVICE_EXECUTABLE_ASCII[]     { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

//! Router Service executable
AREG_API_IMPL wchar_t NEApplication::ROUTER_SERVICE_EXECUTABLE_WIDE[]   { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

//! AREG TCP/IP Log Collector Service name
AREG_API_IMPL char NEApplication::LOGGER_SERVICE_NAME_ASCII[]           { 'l', 'o', 'g', 'g', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! AREG TCP/IP Log Collector Service name
AREG_API_IMPL wchar_t NEApplication::LOGGER_SERVICE_NAME_WIDE[]         { 'l', 'o', 'g', 'g', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! Log Collector Service executable
AREG_API_IMPL char NEApplication::LOGGER_SERVICE_EXECUTABLE_ASCII[]     { 'l', 'o', 'g', 'g', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

//! Log Collector Service executable
AREG_API_IMPL wchar_t NEApplication::LOGGER_SERVICE_EXECUTABLE_WIDE[]   { 'l', 'o', 'g', 'g', 'e', 'r', '.', 'e', 'x', 'e', '\0' };
