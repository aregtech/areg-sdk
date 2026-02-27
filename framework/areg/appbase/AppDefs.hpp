#ifndef AREG_APPBASE_APPDEFS_HPP
#define AREG_APPBASE_APPDEFS_HPP
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
#include "areg/base/GEGlobal.h"

#include "areg/base/Identifier.hpp"
#include "areg/persist/PersistenceDefs.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <string_view>
#include <vector>

/**
 * \brief   In this namespace contains application default configuration values.
 **/
namespace areg
{
    /**
     * \brief   areg::DEFAULT_CONFIG_FILE
     *          The default path of tracing configuration file.
     *          The path is relative to the executable folder.
     **/
    constexpr std::string_view  DEFAULT_CONFIG_FILE             { "./config/areg.init" };

    /**
     * \brief   areg::ROUTER_SERVICE_NAME_ASCII
     *          ASCII version of router service name.
     **/
    extern AREG_API char        ROUTER_SERVICE_NAME_ASCII[];

    /**
     * \brief   areg::ROUTER_SERVICE_NAME_WIDE
     *          Unicode version of router service name.
     **/
    extern AREG_API wchar_t     ROUTER_SERVICE_NAME_WIDE[];

    /**
     * \brief   areg::ROUTER_SERVICE_EXECUTABLE_ASCII
     *          ASCII version of router service executable.
     **/
    extern AREG_API char        ROUTER_SERVICE_EXECUTABLE_ASCII[];

    /**
     * \brief   areg::ROUTER_SERVICE_EXECUTABLE_WIDE
     *          Unicode version of router service executable.
     **/
    extern AREG_API wchar_t     ROUTER_SERVICE_EXECUTABLE_WIDE[];

    /**
     * \brief   areg::LOGGER_SERVICE_NAME_ASCII
     *          ASCII version of log collector service name.
     **/
    extern AREG_API char        LOGGER_SERVICE_NAME_ASCII[];

    /**
     * \brief   areg::LOGGER_SERVICE_NAME_WIDE
     *          Unicode version of log collector service name.
     **/
    extern AREG_API wchar_t     LOGGER_SERVICE_NAME_WIDE[];

    /**
     * \brief   areg::LOGGER_SERVICE_EXECUTABLE_ASCII
     *          ASCII version of log collector service executable.
     **/
    extern AREG_API char        LOGGER_SERVICE_EXECUTABLE_ASCII[];

    /**
     * \brief   areg::LOGGER_SERVICE_EXECUTABLE_WIDE
     *          Unicode version of log collector service executable.
     **/
    extern AREG_API wchar_t     LOGGER_SERVICE_EXECUTABLE_WIDE[];

    /**
     * \brief   areg::DEFAULT_ROUTER_SERVICE_NAME
     *          The default name of Multitarget Router.
     **/
    constexpr std::string_view  DEFAULT_ROUTER_SERVICE_NAME { "mtrouter" };

    /**
     * \brief   areg::DEFAULT_ROUTER_HOST
     *          Default IP-Address of routing service.
     **/
    constexpr std::string_view  DEFAULT_ROUTER_HOST         { LocalAddress };

    /**
     * \brief   areg::DEFAULT_ROUTER_PORT
     *          Default connection port number of routing service.
     **/
    constexpr uint16_t    DEFAULT_ROUTER_PORT         { 8181 };

    /**
     * \brief   areg::DEFAULT_LOGGER_SERVICE_NAME
     *          The default name of Log Collector.
     **/
    constexpr std::string_view  DEFAULT_LOGGER_SERVICE_NAME { "logcollector" };

    /**
     * \brief   areg::DEFAULT_LOGGER_HOST
     *          Default IP-Address of routing service.
     **/
    constexpr std::string_view  DEFAULT_LOGGER_HOST         { LocalAddress };

    /**
     * \brief   areg::DEFAULT_LOGGER_PORT
     *          Default connection port number of routing service.
     **/
    constexpr uint16_t    DEFAULT_LOGGER_PORT         { 8282 };

    /**
     * \brief   areg::DEFAULT_SERVICE_HOST
     *          Default IP-Address of any remote service.
     **/
    constexpr std::string_view  DEFAULT_SERVICE_HOST        { LocalAddress };

    /**
     * \brief   areg::DEFAULT_SERVICE_ENABLED
     *          Message router enable / disable default flag. If true, by default it is enabled.
     *          The default values are used if failed to read and parse router configuration file.
     **/
    constexpr  bool              DEFAULT_SERVICE_ENABLED    { true };

    /**
     * \brief   areg::DEFAULT_LOG_ENABLED
     *          Default flag to indicate logging enable / disable status.
     **/
    constexpr bool              DEFAULT_LOG_ENABLED         { false };

     /**
      * \brief  areg::LOG_SCOPES_SELF
      *         Scope name of Areg framework internal logs.
      **/
    constexpr std::string_view  LOG_SCOPES_SELF             { "areg_*" };

     /**
      * \brief  areg::DEFAULT_LOG_FILE
      *         The default file name to log.
      **/
    constexpr std::string_view  DEFAULT_LOG_FILE            { "./logs/logs_%time%.log" };

    /**
     * \brief  areg::DEFAULT_LOG_QUEUE_SIZE
     *         The default size to queue logs in the stack, used for remote logging.
     **/
    constexpr uint32_t          DEFAULT_LOG_QUEUE_SIZE      { 100 };

     /**
      * \brief  areg::DEFAULT_LOG_FILE
      *         The default layout to display enter scope on console in the plain text file
      **/
    constexpr std::string_view  DEFAULT_LAYOUT_SCOPE_ENTER  { "%d: [ %t %x.%z: Enter --> ] %n" };

    /**
     * \brief  areg::DEFAULT_LOG_FILE
     *         The default layout to display exit scope on console in the plain text file
     **/
    constexpr std::string_view  DEFAULT_LAYOUT_SCOPE_EXIT   { "%d: [ %t %x.%z: Exit  <-- ] %n" };

     /**
      * \brief  areg::DEFAULT_LOG_FILE
      *         The default layout to display the log message on console in the plain text file
      **/
    constexpr std::string_view  DEFAULT_LAYOUT_LOG_MESSAGE  { "%d: [ %t %p >>> ] %m %n" };

    /**
     * \brief   The structure to keep service enable or disable information.
     **/
    struct LogTypeEntry
    {
        uint32_t            ltId;
        std::string_view    ltIdName;
        bool                ltEnabled;
    };

    /**
     * \brief   areg::DefaultLogTypes
     *          The list of logging types with the enabled / disabled flags.
     *          The disabled logging modules are not implemented yet.
     **/
    constexpr LogTypeEntry DefaultLogTypes []
        {
              { static_cast<uint32_t>(LogTarget::Undefined)  , {"unknown"}, false }
            , { static_cast<uint32_t>(LogTarget::Remote)     , {"remote" }, false }
            , { static_cast<uint32_t>(LogTarget::File)       , {"file"   }, true  }
            , { static_cast<uint32_t>(LogTarget::Debug)      , {"debug"  }, false }
            , { static_cast<uint32_t>(LogTarget::Database)   , {"db"     }, false}
        };

    /**
     * \brief   areg::DefaultConnections
     *          The list of connection types with the enabled / disabled flags.
     *          The disabled connection modules are not implemented yet.
     **/
    constexpr LogTypeEntry DefaultConnections[]
        {
              { static_cast<uint32_t>(ConnectionType::Undefined)   , {"unknown"}, false }
            , { static_cast<uint32_t>(ConnectionType::Tcpip)       , {"tcpip"  }, true  }
            , { static_cast<uint32_t>(ConnectionType::Udp)         , {"udp"    }, false }
            , { static_cast<uint32_t>(ConnectionType::Web)         , {"web"    }, false }
            , { static_cast<uint32_t>(ConnectionType::SharedMemory), {"sm"     }, false }
        };

    /**
     * \brief   areg::DefaultRemotetServices
     *          The list of connection types with the enabled / disabled flags.
     **/
    constexpr LogTypeEntry DefaultRemotetServices[]
        {
              { static_cast<uint32_t>(RemoteServiceKind::Unknown) , {"unknown"}, false }
            , { static_cast<uint32_t>(RemoteServiceKind::Router)  , {"router" }, true  }
#ifdef DEBUG
            , { static_cast<uint32_t>(RemoteServiceKind::Logger)  , {"logger" }, true  }
#else   // DEBUG
            , { static_cast<uint32_t>(RemoteServiceKind::Logger)  , {"logger" }, false }
#endif  // DEBUG
        };

    /**
     * \brief   areg::DefaultReadonlyProperties
     *          The list of read-only global properties that cannot be changed,
     *          but can be changed for certain modules and saved in writable configuration.
     **/
    constexpr ConfigProperty  DefaultReadonlyProperties[]
        {
              { {"config"   , "*"   , "version" , ""        }, CONFIG_VERSION    }   //!< The configuration version.

            , { {"log"      , "*"   , "version" , ""        }, LOG_VERSION             }   //!< The logging version.
            , { {"log"      , "*"   , "target"  , ""        }, "remote | file | debug | db"     }   //!< The logging types.
            , { {"log"      , "*"   , "enable"  , ""        }, "true"                           }   //!< The logging enabled / disabled status.
            , { {"log"      , "*"   , "enable"  , "remote"  }, "true"                           }   //!< The logging in remote log collector enabled / disabled flag.
            , { {"log"      , "*"   , "enable"  , "file"    }, "true"                           }   //!< The logging in file enabled / disabled flag.
            , { {"log"      , "*"   , "enable"  , "output"  }, "false"                          }   //!< The logging in output console enabled / disabled flag.
            , { {"log"      , "*"   , "enable"  , "db"      }, "false"                          }   //!< The logging in database enabled / disabled flag.
            , { {"log"      , "*"   , "file"    , "location"}, DEFAULT_LOG_FILE                 }   //!< The log file location and file name mask.
            , { {"log"      , "*"   , "file"    , "append"  }, "false"                          }   //!< The flag to append logs into the file.
            , { {"log"      , "*"   , "remote"  , "queue"   }, "100"                            }   //!< The queue size of remote logging.
            , { {"log"      , "*"   , "remote"  , "service" }, "logger"                         }   //!< The service name of the remote logging.
            , { {"log"      , "*"   , "layout"  , "enter"   }, DEFAULT_LAYOUT_SCOPE_EXIT        }   //!< The layout of enter scope message.
            , { {"log"      , "*"   , "layout"  , "message" }, DEFAULT_LAYOUT_LOG_MESSAGE       }   //!< The layout of log message.
            , { {"log"      , "*"   , "layout"  , "exit"    }, DEFAULT_LAYOUT_SCOPE_EXIT        }   //!< The layout of exit scope message.

            , { {"service"  , "*"   , "list"    , ""        }, "router | logger"                }   //!< The list of supported remote services.

            , { {"router"   , "*"   , "service" , ""        }, "mtrouter"                       }   //!< The process name of the 'router' service.
            , { {"router"   , "*"   , "connect" , ""        }, "tcpip"                          }   //!< The list of connection type of the 'router' service.
            , { {"router"   , "*"   , "enable"  , "tcpip"   }, "true"                           }   //!< The TCP/IP connection enable / disable flag of the 'router' service.
            , { {"router"   , "*"   , "address" , "tcpip"   }, DEFAULT_ROUTER_HOST              }   //!< The TCP/IP connection address of the 'router' service.
            , { {"router"   , "*"   , "port"    , "tcpip"   }, "8181"                           }   //!< The TCP/IP connection port number of the 'router' service.

            , { {"logger"   , "*"   , "service" , ""        }, "logcollector"                   }   //!< The process name of the 'logger' service.
            , { {"logger"   , "*"   , "connect" , ""        }, "tcpip"                          }   //!< The list of connection type of the 'logger' service.
            , { {"logger"   , "*"   , "enable"  , "tcpip"   }, "true"                           }   //!< The TCP/IP connection enable / disable flag of the 'logger' service
            , { {"logger"   , "*"   , "address" , "tcpip"   }, DEFAULT_ROUTER_HOST              }   //!< The TCP/IP connection address of the 'logger' service.
            , { {"logger"   , "*"   , "port"    , "tcpip"   }, "8282"                           }   //!< The TCP/IP connection port number of the 'logger' service.

            , { {"log"      , "*"   , "scope"   , "*"       }, "NOTSET"                         }   //!< The default log scopes to enable / disable.
        };


    /**
     * \brief   areg::DefaultLogScopesConfig
     *          The list of default scopes and priorities set in writable properties
     *          in case if configuration file cannot be loaded.
     **/
    constexpr ConfigProperty DefaultLogScopesConfig[]
        {
              { {"log", "mtrouter"      , "scope"   , "*"       }, "NOTSET"                     }   //!< The 'mtrouter' service scopes to enable / disable.
            , { {"log", "logcollector"  , "scope"   , "*"       }, "NOTSET"                     }   //!< The 'logcollector' service scopes to enable / disable.
        };

    /**
     * \brief   areg::LogTypeIdentifiers
     *          The list of logging type identifiers to convert to string or areg::LogTarget types
     **/
    extern AREG_API const std::vector<Identifier> LogTypeIdentifiers;

    /**
     * \brief   areg::ConnectionIdentifiers
     *          The list of connection type identifiers to convert to string or areg::ConnectionType types
     **/
    extern AREG_API const std::vector<Identifier> ConnectionIdentifiers;

    /**
     * \brief   areg::RemoteServiceIdentifiers
     *          The list of remote servicing type identifiers to convert to string or areg::RemoteServiceKind types
     **/
    extern AREG_API const std::vector<Identifier> RemoteServiceIdentifiers;

    /**
     * \brief   areg::LogScopePriorityIndentifiers
     *          The list of logging priority type identifiers to convert to string or areg::LogPriority types
     **/
    extern AREG_API const std::vector<Identifier> LogScopePriorityIndentifiers;

    /**
     * \brief   areg::AppState
     *          Describes the application states.
     *          -   Initially, the application is in undefined state.
     *          -   The application is in initialization state, when Service Manager did not started yet and it
     *              initializes any module like tracing, timer, etc.
     *          -   Application is in ready state only when Service Manager is started.
     *          -   Application is in release state when it is going to stop Service Manager.
     **/
    enum class AppState : uint8_t
    {
          Stopped       //!< Application state is undefined
        , Initializing  //!< Application is initializing
        , Ready         //!< Application is ready. The application is ready only when Service Manager runs.
        , Releasing     //!< Application is releasing.
        , Failure       //!< Application is failure state and cannot be continued
    };

}

#endif // AREG_APPBASE_APPDEFS_HPP
