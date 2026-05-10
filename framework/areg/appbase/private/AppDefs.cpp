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
#include "areg/persist/PersistenceDefs.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"

namespace
{
//!< The structure to keep service enable or disable information.
struct LogTypeEntry
{
    uint32_t            ltId;
    std::string_view    ltIdName;
    bool                ltEnabled;
};


/**
 * \brief   The list of logging types with the enabled / disabled flags.
 *          The disabled logging modules are not implemented yet.
 **/
constexpr LogTypeEntry _defaultLogTypes[]
{
      { static_cast<uint32_t>(areg::LogTarget::Undefined)  , {"unknown"}, false }
    , { static_cast<uint32_t>(areg::LogTarget::Remote)     , {"remote" }, false }
    , { static_cast<uint32_t>(areg::LogTarget::File)       , {"file"   }, true  }
    , { static_cast<uint32_t>(areg::LogTarget::Debug)      , {"debug"  }, false }
    , { static_cast<uint32_t>(areg::LogTarget::Database)   , {"db"     }, false}
};

/**
 * \brief   The list of connection types with the enabled / disabled flags.
 *          The disabled connection modules are not implemented yet.
 **/
constexpr LogTypeEntry _defaultConnections[]
{
      { static_cast<uint32_t>(areg::ConnectionType::Undefined)   , {"unknown"}, false }
    , { static_cast<uint32_t>(areg::ConnectionType::Tcpip)       , {"tcpip"  }, true  }
    , { static_cast<uint32_t>(areg::ConnectionType::Udp)         , {"udp"    }, false }
    , { static_cast<uint32_t>(areg::ConnectionType::Web)         , {"web"    }, false }
    , { static_cast<uint32_t>(areg::ConnectionType::SharedMemory), {"sm"     }, false }
};

/**
 * \brief   _defaultRemotetServices
 *          The list of connection types with the enabled / disabled flags.
 **/
constexpr LogTypeEntry _defaultRemotetServices[]
{
      { static_cast<uint32_t>(areg::RemoteServiceKind::Unknown) , {"unknown"}, false }
    , { static_cast<uint32_t>(areg::RemoteServiceKind::Router)  , {"router" }, true  }
#ifdef DEBUG
    , { static_cast<uint32_t>(areg::RemoteServiceKind::Logger)  , {"logger" }, true  }
#else   // DEBUG
    , { static_cast<uint32_t>(areg::RemoteServiceKind::Logger)  , {"logger" }, false }
#endif  // DEBUG
};

}

//! Logging type identifiers
AREG_API_IMPL const std::vector<areg::Identifier>     areg::LogTypeIdentifiers =
{
      {static_cast<uint32_t>(areg::LogTarget::Undefined)   , _defaultLogTypes[0].ltIdName              }
    , {static_cast<uint32_t>(areg::LogTarget::Remote)      , _defaultLogTypes[1].ltIdName              }
    , {static_cast<uint32_t>(areg::LogTarget::File)        , _defaultLogTypes[2].ltIdName              }
    , {static_cast<uint32_t>(areg::LogTarget::Debug)       , _defaultLogTypes[3].ltIdName              }
    , {static_cast<uint32_t>(areg::LogTarget::Database)    , _defaultLogTypes[4].ltIdName              }
};

//! Connection type identifiers
AREG_API_IMPL const std::vector<areg::Identifier>   areg::ConnectionIdentifiers
{
      { static_cast<uint32_t>(areg::ConnectionType::Undefined)  , _defaultConnections[0].ltIdName      }
    , { static_cast<uint32_t>(areg::ConnectionType::Tcpip)      , _defaultConnections[1].ltIdName      }
    , { static_cast<uint32_t>(areg::ConnectionType::Udp)        , _defaultConnections[2].ltIdName      }
    , { static_cast<uint32_t>(areg::ConnectionType::Web)        , _defaultConnections[3].ltIdName      }
    , { static_cast<uint32_t>(areg::ConnectionType::SharedMemory),_defaultConnections[4].ltIdName      }
};

//! Remote service identifiers
AREG_API_IMPL const std::vector<areg::Identifier>   areg::RemoteServiceIdentifiers
{
      { static_cast<uint32_t>(areg::RemoteServiceKind::Unknown)  , _defaultRemotetServices[0].ltIdName }
    , { static_cast<uint32_t>(areg::RemoteServiceKind::Router)   , _defaultRemotetServices[1].ltIdName }
    , { static_cast<uint32_t>(areg::RemoteServiceKind::Logger)   , _defaultRemotetServices[2].ltIdName }
};

//! Log priority identifiers
AREG_API_IMPL const std::vector<areg::Identifier>   areg::LogScopePriorityIndentifiers
{
      { static_cast<uint32_t>(areg::LogPriority::PrioInvalid)  , areg::PRIO_NO_PRIO                     }
    , { static_cast<uint32_t>(areg::LogPriority::PrioNotset)   , areg::PRIO_NOTSET_STR                  }
    , { static_cast<uint32_t>(areg::LogPriority::PrioScope)    , areg::PRIO_SCOPE_STR                   }
    , { static_cast<uint32_t>(areg::LogPriority::PrioFatal)    , areg::PRIO_FATAL_STR                   }
    , { static_cast<uint32_t>(areg::LogPriority::PrioError)    , areg::PRIO_ERROR_STR                   }
    , { static_cast<uint32_t>(areg::LogPriority::PrioWarning)  , areg::PRIO_WARNING_STR                 }
    , { static_cast<uint32_t>(areg::LogPriority::PrioInfo)     , areg::PRIO_INFO_STR                    }
    , { static_cast<uint32_t>(areg::LogPriority::PrioDebug)    , areg::PRIO_DEBUG_STR                   }
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
