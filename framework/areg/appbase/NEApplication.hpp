#ifndef AREG_APPBASE_NEAPPLICATION_HPP
#define AREG_APPBASE_NEAPPLICATION_HPP
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
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Application global namespace and configurations
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/Identifier.hpp"
#include "areg/persist/NEPersistence.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/trace/NETrace.hpp"

#include <string_view>
#include <vector>

/**
 * \brief   In this namespace contains application default configuration values.
 **/
namespace NEApplication
{
    /**
     * \brief   NEApplication::DEFAULT_CONFIG_FILE
     *          The default path of tracing configuration file.
     *          The path is relative to the executable folder.
     **/
    constexpr std::string_view  DEFAULT_CONFIG_FILE             { "./config/areg.init" };

    /**
     * \brief   NEApplication::ROUTER_SERVICE_NAME_ASCII
     *          ASCII version of service interface router service name:
     *          "AREG TCP/IP connection Message Multicast Router Service"
     **/
    extern AREG_API char        ROUTER_SERVICE_NAME_ASCII[];

    /**
     * \brief   NEApplication::ROUTER_SERVICE_NAME_WIDE
     *          Unicode version of service interface router service name:
     *          L"AREG TCP/IP connection Message Multicast Router Service"
     **/
    extern AREG_API wchar_t     ROUTER_SERVICE_NAME_WIDE[];

    /**
     * \brief   NEApplication::LOGGER_SERVICE_NAME_ASCII
     *          ASCII version of service interface logger service name:
     *          "AREG TCP/IP Logs collection Service"
     **/
    extern AREG_API char        LOGGER_SERVICE_NAME_ASCII[];

    /**
     * \brief   NEApplication::LOGGER_SERVICE_NAME_WIDE
     *          Unicode version of service interface logger service name:
     *          L"AREG TCP/IP Log collection Service"
     **/
    extern AREG_API wchar_t     LOGGER_SERVICE_NAME_WIDE[];

    /**
     * \brief   NEApplication::DEFAULT_ROUTER_SERVICE_NAME
     *          The default name of Multicast Router.
     **/
    constexpr std::string_view  DEFAULT_ROUTER_SERVICE_NAME { "mcrouter" };

    /**
     * \brief   NEApplication::DEFAULT_ROUTER_HOST
     *          Default IP-Address of routing service.
     **/
    constexpr std::string_view  DEFAULT_ROUTER_HOST         { NESocket::LocalAddress };

    /**
     * \brief   NEApplication::DEFAULT_ROUTER_PORT
     *          Default connection port number of routing service.
     **/
    constexpr unsigned short    DEFAULT_ROUTER_PORT         { 8181 };

    /**
     * \brief   NEApplication::DEFAULT_LOGGER_SERVICE_NAME
     *          The default name of Log Collector.
     **/
    constexpr std::string_view  DEFAULT_LOGGER_SERVICE_NAME { "logger" };

    /**
     * \brief   NEApplication::DEFAULT_LOGGER_HOST
     *          Default IP-Address of routing service.
     **/
    constexpr std::string_view  DEFAULT_LOGGER_HOST         { NESocket::LocalAddress };

    /**
     * \brief   NEApplication::DEFAULT_LOGGER_PORT
     *          Default connection port number of routing service.
     **/
    constexpr unsigned short    DEFAULT_LOGGER_PORT         { 8282 };

    /**
     * \brief   NEApplication::DEFAULT_SERVICE_HOST
     *          Default IP-Address of any remote service.
     **/
    constexpr std::string_view  DEFAULT_SERVICE_HOST        { NESocket::LocalAddress };

    /**
     * \brief   NEApplication::DEFAULT_SERVICE_ENABLED
     *          Message router enable / disable default flag. If true, by default it is enabled.
     *          The default values are used if failed to read and parse router configuration file.
     **/
    constexpr  bool              DEFAULT_SERVICE_ENABLED    { true };

    /**
     * \brief   NEApplication::DEFAULT_LOG_ENABLED
     *          Default flag to indicate logging enable / disable status.
     **/
    constexpr bool              DEFAULT_LOG_ENABLED         { false };

    /**
     * \brief   NEApplication::LOG_SCOPES_GRPOUP
     *          Symbol, indicating scope group or all scopes.
     *          For example, the scope "scope.*" means all scope.
     *          And the scope "scope.areg_*" means all scopes of that start with "areg_"
     **/
    constexpr std::string_view  LOG_SCOPES_GRPOUP           { "*" };

     /**
      * \brief  NEApplication::LOG_SCOPES_SELF
      *         Scope name of AREG framework internal logs.
      **/
    constexpr std::string_view  LOG_SCOPES_SELF             { "areg_*" };

     /**
      * \brief  NEApplication::DEFAULT_LOG_FILE
      *         The default file name to log.
      **/
    constexpr std::string_view  DEFAULT_LOG_FILE            { "./logs/trace_%time%.log" };

    /**
     * \brief  NEApplication::DEFAULT_LOG_QUEUE_SIZE
     *         The default size to queue logs in the stack, used for remote logging.
     **/
    constexpr uint32_t          DEFAULT_LOG_QUEUE_SIZE      { 100 };

     /**
      * \brief  NEApplication::DEFAULT_LOG_FILE
      *         The default layout to display enter scope on console in the plain text file
      **/
    constexpr std::string_view  DEFAULT_LAYOUT_SCOPE_ENTER  { "%d: [ %t %x.%z: Enter --> ] %n" };

    /**
     * \brief  NEApplication::DEFAULT_LOG_FILE
     *         The default layout to display exit scope on console in the plain text file
     **/
    constexpr std::string_view  DEFAULT_LAYOUT_SCOPE_EXIT   { "%d: [ %t %x.%z: Exit  <-- ] %n" };

     /**
      * \brief  NEApplication::DEFAULT_LOG_FILE
      *         The default layout to display the log message on console in the plain text file
      **/
    constexpr std::string_view  DEFAULT_LAYOUT_LOG_MESSAGE  { "%d: [ %t %p >>> ] %m %n" };

    /**
     * \brief   The structure to keep service enable or disable information.
     **/
    struct sEntryTypesEnabling
    {
        unsigned int        ltId;
        std::string_view    ltIdName;
        bool                ltEnabled;
    };

    /**
     * \brief   NEApplication::DefaultLogTypes
     *          The list of logging types with the enabled / disabled flags.
     *          The disabled logging modules are not implemented yet.
     **/
    constexpr sEntryTypesEnabling DefaultLogTypes []
        {
              { static_cast<unsigned int>(NETrace::eLogingTypes::LogTypeUndefined)  , {"unknown"}, false }
            , { static_cast<unsigned int>(NETrace::eLogingTypes::LogTypeRemote)     , {"remote" }, false }
            , { static_cast<unsigned int>(NETrace::eLogingTypes::LogTypeFile)       , {"file"   }, true  }
            , { static_cast<unsigned int>(NETrace::eLogingTypes::LogTypeDebug)      , {"debug"  }, false }
            , { static_cast<unsigned int>(NETrace::eLogingTypes::LogTypeDatabase)   , {"db"     }, false}
        };

    /**
     * \brief   NEApplication::DefaultConnections
     *          The list of connection types with the enabled / disabled flags.
     *          The disabled connection modules are not implemented yet.
     **/
    constexpr sEntryTypesEnabling DefaultConnections[]
        {
              { static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectUndefined)  , {"unknown"}, false }
            , { static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectTcpip)      , {"tcpip"  }, true  }
            , { static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectUdp)        , {"udp"    }, false }
            , { static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectWeb)        , {"web"    }, false }
            , { static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectSM)         , {"sm"     }, false }
        };

    /**
     * \brief   NEApplication::DefaultRemotetServices
     *          The list of connection types with the enabled / disabled flags.
     **/
    constexpr sEntryTypesEnabling DefaultRemotetServices[]
        {
              { static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceUnknown) , {"unknown"}, false }
            , { static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceRouter)  , {"router" }, true  }
            , { static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceLogger)  , {"logger" }, false }
        };

    /**
     * \brief   NEApplication::DefaultReadonlyProperties
     *          The list of read-only global properties that cannot be changed,
     *          but can be changed for certain modules and saved in writable configuration.
     **/
    constexpr NEPersistence::sProperty  DefaultReadonlyProperties[]
        {
              { {"config"   , "*"   , "version" , ""        }, NEPersistence::CONFIG_VERSION    }   //!< The configuration version.

            , { {"log"      , "*"   , "version" , ""        }, NETrace::LOG_VERSION             }   //!< The logging version.
            , { {"log"      , "*"   , "target"  , ""        }, "remote | file | debug | db"     }   //!< The logging types.
            , { {"log"      , "*"   , "enable"  , ""        }, "true"                           }   //!< The logging enabled / disabled status.
            , { {"log"      , "*"   , "enable"  , "remote"  }, "true"                          }   //!< The logging in remote log collector enabled / disabled flag.
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

            , { {"router"   , "*"   , "service" , ""        }, "mcrouter"                       }   //!< The process name of the 'router' service.
            , { {"router"   , "*"   , "connect" , ""        }, "tcpip"                          }   //!< The list of connection type of the 'router' service.
            , { {"router"   , "*"   , "enable"  , "tcpip"   }, "true"                           }   //!< The TCP/IP connection enable / disable flag of the 'router' service.
            , { {"router"   , "*"   , "address" , "tcpip"   }, DEFAULT_ROUTER_HOST              }   //!< The TCP/IP connection address of the 'router' service.
            , { {"router"   , "*"   , "port"    , "tcpip"   }, "8181"                           }   //!< The TCP/IP connection port number of the 'router' service.

            , { {"logger"   , "*"   , "service" , ""        }, "logger"                         }   //!< The process name of the 'logger' service.
            , { {"logger"   , "*"   , "connect" , ""        }, "tcpip"                          }   //!< The list of connection type of the 'logger' service.
            , { {"logger"   , "*"   , "enable"  , "tcpip"   }, "true"                           }   //!< The TCP/IP connection enable / disable flag of the 'logger' service
            , { {"logger"   , "*"   , "address" , "tcpip"   }, DEFAULT_ROUTER_HOST              }   //!< The TCP/IP connection address of the 'logger' service.
            , { {"logger"   , "*"   , "port"    , "tcpip"   }, "8282"                           }   //!< The TCP/IP connection port number of the 'logger' service.

            , { {"log"      , "*"   , "scope"   , "*"       }, "NOTSET"                         }   //!< The default log scopes to enable / disable.
        };


    /**
     * \brief   NEApplication::DefaultLogScopesConfig
     *          The list of default scopes and priorities set in writable properties
     *          in case if configuration file cannot be loaded.
     **/
    constexpr NEPersistence::sProperty DefaultLogScopesConfig[]
        {
              { {"log", "mcrouter"  , "scope"   , "*"       }, "NOTSET"                         }   //!< The 'mcrouter' service scopes to enable / disable.
            , { {"log", "logger"    , "scope"   , "*"       }, "NOTSET"                         }   //!< The 'logger' service scopes to enable / disable.
        };

    /**
     * \brief   NEApplication::LogTypeIdentifiers
     *          The list of logging type identifiers to convert to string or NETrace::eLogingTypes types
     **/
    extern AREG_API const std::vector<Identifier> LogTypeIdentifiers;

    /**
     * \brief   NEApplication::ConnectionIdentifiers
     *          The list of connection type identifiers to convert to string or NERemoteService::eConnectionTypes types
     **/
    extern AREG_API const std::vector<Identifier> ConnectionIdentifiers;

    /**
     * \brief   NEApplication::RemoteServiceIdentifiers
     *          The list of remote servicing type identifiers to convert to string or NERemoteService::eRemoteServices types
     **/
    extern AREG_API const std::vector<Identifier> RemoteServiceIdentifiers;

    /**
     * \brief   NEApplication::LogScopePriorityIndentifiers
     *          The list of logging priority type identifiers to convert to string or NETrace::eLogPriority types
     **/
    extern AREG_API const std::vector<Identifier> LogScopePriorityIndentifiers;
}

#endif // AREG_APPBASE_NEAPPLICATION_HPP
