#ifndef AREG_PERSIST_PERSISTENCEDEFS_HPP
#define AREG_PERSIST_PERSISTENCEDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/persist/PersistenceDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       File data persist syntax namespace
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/CommonDefs.hpp"
#include <string_view>

//////////////////////////////////////////////////////////////////////////
// namespace with persistence operations declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Constants used when parsing persistence file
 **/
namespace areg {
//////////////////////////////////////////////////////////////////////////
// Syntax to parse configuration file
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   The default version of the configuration file.
     **/
    constexpr std::string_view  CONFIG_VERSION              { "2.0.0" };

    /**
     * \brief   White space delimiter
     **/
    constexpr char              SYNTAX_WHITESPACE_DELIMITER { ' ' };

    /**
     * \brief   List delimiter in the value.
     **/
    constexpr char              SYNTAX_VALUE_LIST_DELIMITER { '|' };

    /**
     * \brief   Tab delimiter
     **/
    constexpr char              SYNTAX_TAB_DELIMITER        { '\t' };

    /**
     * \brief   End of command delimiter, optional. Might be skipped
     **/
    constexpr char              SYNTAX_END_COMMAND          { ';' };

    /**
     * \brief   Logical OR, used only when setting values as identifiers.
     *          Do not use in all other cases, since it will have no effect.
     **/
    constexpr char              SYNTAX_LOGICAL_OR           { '|' };

    /**
     * \brief   Syntax, which indicates any value.
     **/
    constexpr char              SYNTAX_ANY_VALUE            { '*' };

    constexpr char              SYNTAX_LOG_GROUP{ '*' };

    /**
     * \brief   The scope name path separator between tree nodes.
     **/
    constexpr char              SYNTAX_SCOPE_SEPARATOR{ '_' };

    /**
     * \brief   The separator between the scope path (node chain) and the method (leaf) name.
     *          Introduced to support snake_case method names unambiguously: a dot in the scope
     *          string signals that everything after it is the leaf name verbatim — the leaf name
     *          may itself contain underscores without being split into further nodes.
     *
     *          Example: "module_ClassName.some_method" -> path nodes "module", "ClassName";
     *          leaf name "some_method".
     **/
    constexpr char              SYNTAX_SCOPE_LEAF_SEPARATOR{ '.' };

    /**
     * \brief   Symbol of object separator in configuration file
     **/
    constexpr std::string_view  SYNTAX_OBJECT_SEPARATOR     { "::" };


    /**
     * \brief   End of line symbol in configuration file
     **/
    constexpr std::string_view  SYNTAX_LINEEND              { "\n" };

    /**
     * \brief   Symbol of equal operation in configuration file
     **/
    constexpr std::string_view  SYNTAX_EQUAL                { "=" };

    /**
     * \brief   Symbol of comment in configuration file
     **/
    constexpr std::string_view  SYNTAX_COMMENT              { "# " };

    /**
     * \brief   Symbol of an empty line, no comment follows the symbol.
     **/
    constexpr std::string_view  SYNTAX_EMPTY_LINE           { "#" };

    /**
     * \brief   List of valid delimiters
     **/
    constexpr std::string_view  SYNTAX_DELIMITERS           { " \t" };

    /**
     * \brief   Symbol, indicating property of all modules.
     **/
    constexpr std::string_view  SYNTAX_ALL_MODULES          { "*" };

    /**
     * \brief   Symbol, indicating group of properties.
     **/
    constexpr std::string_view  SYNTAX_GROUP                { "*" };

    /**
     * \brief   Boolean syntax 'true'
     **/
    constexpr std::string_view  SYNTAX_TRUE                 { areg::BOOLEAN_TRUE };

    /**
     * \brief   Boolean syntax 'false'
     **/
    constexpr std::string_view  SYNTAX_FALSE                { areg::BOOLEAN_FALSE };

    /**
     * \brief   The structure to define a property key of the configuration.
     **/
    struct ConfigKey
    {
        const std::string_view  section;    //!< The section of the key in the configuration.
        const std::string_view  module;     //!< The module of the key in the configuration.
        const std::string_view  property;   //!< The property of the key in the configuration.
        const std::string_view  position;   //!< The position of the key in the configuration.
    };

    /**
     * \brief   The structure identifying the property of the configuration.
     **/
    struct ConfigProperty
    {
        const ConfigKey         configKey;      //!< The configuration key.
        const std::string_view  configValue;    //!< The configuration value.
    };

    /**
     * \brief   Types of the configuration keys.
     **/
    enum class ConfigEntry  : int32_t
    {
          Invalid              = -1    //!< Invalid key of the configuration
        , ConfigVersion        = 0     //!< The configuration version.

        , LogVersion           = 1     //!< The logging version.
        , LogTarget            = 2     //!< The logging types (targets).
        , LogStatus            = 3     //!< The logging enabled / disabled status.
        , LogEnable            = 4     //!< The logging type (in target) enabled / disabled flag.
        , LogFileLocation      = 5     //!< The log file location and file name mask.
        , LogFileAppend        = 6     //!< The flag to append logs into the file.
        , LogRemoteQueueSize   = 7     //!< The queue size of remote logging.
        , LogRemoteService     = 8     //!< The service name of the remote logging.

        , LogDatabaseEngine    = 9     //!< The log database name.
        , LogDatabaseName      = 10    //!< The log database name.
        , LogDatabaseLocation  = 11    //!< The log database location.
        , LogDatabaseDriver    = 12    //!< The log database driver.
        , LogDatabaseAddress   = 13    //!< The log database address.
        , LogDatabasePort      = 14    //!< The log database port.
        , LogDatabaseUsername  = 15    //!< The log database user-name.
        , LogDatabasePassword  = 16    //!< The log database user-password.

        , LogLayoutEnter       = 17    //!< The layout of enter scope message.
        , LogLayoutMessage     = 18    //!< The layout of log message.
        , LogLayoutExit        = 19    //!< The layout of exit scope message.
        , LogScope             = 20    //!< The log scope enable / disable flag.

        , ServiceList          = 21    //!< The list of supported remote services.

        , ServiceName          = 22    //!< The process name of the remote service.
        , ServiceConnection    = 23    //!< The list of connection type of the remote service.
        , ServiceEnable        = 24    //!< The connection enable / disable flag of the remote service.
        , ServiceAddress       = 25    //!< The connection address of the remote service.
        , ServicePort          = 26    //!< The connection port number of the remote service.

        , DefaultBufferBlock   = 27    //!< The size in bytes to align when allocate a block in the bugger. The default `0` means allocated `sizeof(areg::uAlignt)`
        , DefaultMessageQueue  = 28    //!< The default size of message queue in the dispatcher thread. The default `0` means to ignore the limitation, increase by need.

        , ServiceSocketSndbuf  = 29    //!< The SO_SNDBUF size (bytes) applied to every socket of the remote service connection.
        , ServiceSocketRcvbuf  = 30    //!< The SO_RCVBUF size (bytes) applied to every socket of the remote service connection.

        , AnyKey               = 31    //!< Indicates any key type.
    };

    /**
     * \brief   The list of property key structures supported by Areg
     **/
    constexpr ConfigKey   DefaultPropertyKeys[]
        {
              {"config" , "*"   , "version" , ""                }   //! 0   , The configuration version property structure.

            , {"log"    , "*"   , "version" , ""                }   //! 1   , The logging version property structure.
            , {"log"    , "*"   , "target"  , ""                }   //! 2   , The logging types (targets) property structure.
            , {"log"    , "*"   , "enable"  , ""                }   //! 3   , The logging enabled / disabled status property structure.
            , {"log"    , "*"   , "enable"  , "*"               }   //! 4   , The logging type (in target) enabled / disabled flag property structure.
            , {"log"    , "*"   , "file"    , "location"        }   //! 5   , The log file location and file name mask property structure.
            , {"log"    , "*"   , "file"    , "append"          }   //! 6   , The flag to append logs into the file property structure.
            , {"log"    , "*"   , "remote"  , "queue"           }   //! 7   , The queue size of remote logging property structure.
            , {"log"    , "*"   , "remote"  , "service"         }   //! 8   , The service name of the remote logging.
        
            , {"log"    , "*"   , "db"      , "engine"          }   //! 9   , The name of the log database engine.
            , {"log"    , "*"   , "db"      , "name"            }   //! 10  , The name of the log database.
            , {"log"    , "*"   , "db"      , "location"        }   //! 11  , The location of log database.
            , {"log"    , "*"   , "db"      , "driver"          }   //! 12  , The log database driver.
            , {"log"    , "*"   , "db"      , "address"         }   //! 13  , The address of the remote log database engine (DB Server).
            , {"log"    , "*"   , "db"      , "port"            }   //! 14  , The port of the remote log database engine (DB Server).
            , {"log"    , "*"   , "db"      , "username"        }   //! 15  , The user name to log-in into the log database.
            , {"log"    , "*"   , "db"      , "password"        }   //! 16  , The user password to log-in into the log database.

            , {"log"    , "*"   , "layout"  , "enter"           }   //! 17  , The layout of enter scope message property structure.
            , {"log"    , "*"   , "layout"  , "message"         }   //! 18  , The layout of log message property structure.
            , {"log"    , "*"   , "layout"  , "exit"            }   //! 19  , The layout of exit scope message property structure.
            , {"log"    , "*"   , "scope"   , "*"               }   //! 20  , The log scope enable / disable flag property structure.

            , {"service", "*"   , "list"    , ""                }   //! 21  , The list of supported remote services property structure.

            , {"*"      , "*"   , "service" , ""                }   //! 22  , The process name of the remote service property structure.
            , {"*"      , "*"   , "connect" , ""                }   //! 23  , The list of connection type of the remote service property structure.
            , {"*"      , "*"   , "enable"  , "*"               }   //! 24  , The connection enable / disable flag of the remote service property structure.
            , {"*"      , "*"   , "address" , "*"               }   //! 25  , The connection address of the remote service property structure.
            , {"*"      , "*"   , "port"    , "*"               }   //! 26  , The connection port number of the remote service property structure.

            , {"config" , "*"   , "default" , "blocksize"       }   //! 27  , The default block size in bytes to allocate in shared buffer.
            , {"config" , "*"   , "default" , "messagequeue"    }   //! 28  , The default message queue size in the dispatcher thread.

            , {"*"      , "*"   , "socket"  , "sndbuf"          }   //! 29  , The SO_SNDBUF size (bytes) applied to every socket of the service connection.
            , {"*"      , "*"   , "socket"  , "rcvbuf"          }   //! 30  , The SO_RCVBUF size (bytes) applied to every socket of the service connection.

            , {"*"      , "*"   , "*"       , "*"               }   //! 31  , Indicates any key type.
        };


//////////////////////////////////////////////////////////////////////////
// areg namespace inline methods
//////////////////////////////////////////////////////////////////////////

inline constexpr const areg::ConfigKey& config_version() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::ConfigVersion)];
}

inline constexpr const areg::ConfigKey& service_list() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::ServiceList)];
}

inline constexpr const areg::ConfigKey& log_version() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogVersion)];
}

inline constexpr const areg::ConfigKey& log_target() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogTarget)];
}

inline constexpr const areg::ConfigKey& log_status() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogStatus)];
}

inline constexpr const areg::ConfigKey& log_enable() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogEnable)];
}

inline constexpr const areg::ConfigKey& log_file_location() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogFileLocation)];
}

inline constexpr const areg::ConfigKey& log_file_append() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogFileAppend)];
}

inline constexpr const areg::ConfigKey& remote_queue_size() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogRemoteQueueSize)];
}

inline constexpr const areg::ConfigKey& log_layout_enter() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogLayoutEnter)];
}

inline constexpr const areg::ConfigKey& log_layout_message() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogLayoutMessage)];
}

inline constexpr const areg::ConfigKey& log_layout_exit() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogLayoutExit)];
}

inline constexpr const areg::ConfigKey& log_scope() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogScope)];
}

inline constexpr const areg::ConfigKey& service_connection() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::ServiceConnection)];
}

inline constexpr const areg::ConfigKey& service_enable() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::ServiceEnable)];
}

inline constexpr const areg::ConfigKey& service_name() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::ServiceName)];
}

inline constexpr const areg::ConfigKey& service_address() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::ServiceAddress)];
}

inline constexpr const areg::ConfigKey& service_port() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::ServicePort)];
}

inline constexpr const areg::ConfigKey& log_database_engine() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogDatabaseEngine)];
}

inline constexpr const areg::ConfigKey& log_database_name() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogDatabaseName)];
}

inline constexpr const areg::ConfigKey& log_database_location() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogDatabaseLocation)];
}

inline constexpr const areg::ConfigKey& log_database_driver() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogDatabaseDriver)];
}

inline constexpr const areg::ConfigKey& log_database_address() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogDatabaseAddress)];
}

inline constexpr const areg::ConfigKey& log_database_port() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogDatabasePort)];
}

inline constexpr const areg::ConfigKey& log_database_user() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogDatabaseUsername)];
}

inline constexpr const areg::ConfigKey& log_database_password() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::LogDatabasePassword)];
}

inline constexpr const areg::ConfigKey& buffer_block_size() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::DefaultBufferBlock)];
}

inline constexpr const areg::ConfigKey& message_queue_size() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::DefaultMessageQueue)];
}

inline constexpr const areg::ConfigKey& service_socket_sndbuf() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::ServiceSocketSndbuf)];
}

inline constexpr const areg::ConfigKey& service_socket_rcvbuf() noexcept
{
    return areg::DefaultPropertyKeys[static_cast<int32_t>(areg::ConfigEntry::ServiceSocketRcvbuf)];
}

} // namespace areg

#endif  // AREG_PERSIST_PERSISTENCEDEFS_HPP
