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
#include "areg/base/GEGlobal.h"

#include "areg/base/CommonDefs.hpp"
#include <string_view>

//////////////////////////////////////////////////////////////////////////
// NEPersistence namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Constants used when parsing persistence file
 **/
namespace NEPersistence
{
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
    constexpr std::string_view  SYNTAX_TRUE                 { NECommon::BOOLEAN_TRUE };

    /**
     * \brief   Boolean syntax 'false'
     **/
    constexpr std::string_view  SYNTAX_FALSE                { NECommon::BOOLEAN_FALSE };

    /**
     * \brief   The structure to define a property key of the configuration.
     **/
    struct sPropertyKey
    {
        const std::string_view  section;    //!< The section of the key in the configuration.
        const std::string_view  module;     //!< The module of the key in the configuration.
        const std::string_view  property;   //!< The property of the key in the configuration.
        const std::string_view  position;   //!< The position of the key in the configuration.
    };

    /**
     * \brief   The structure identifying the property of the configuration.
     **/
    struct sProperty
    {
        const sPropertyKey      configKey;      //!< The configuration key.
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

        , DefaultBufferBlock   = 27    //!< The size in bytes to align when allocate a block in the bugger. The default `0` means allocated `sizeof(NEMemory::uAlignt)`
        , DefaultMessageQueue  = 28    //!< The default size of message queue in the dispatcher thread. The default `0` means to ignore the limitation, increase by need.

        , AnyKey               = 29    //!< Indicates any key type.
    };

    /**
     * \brief   The list of property key structures supported by Areg
     **/
    constexpr sPropertyKey   DefaultPropertyKeys[]
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

            , {"*"      , "*"   , "*"       , "*"               }   //! 29  , Indicates any key type.
        };

    /**
     * \brief   Returns the configuration version property structure.
     **/
    inline const NEPersistence::sPropertyKey& getConfigVersion();

    /**
     * \brief   Returns the logging version property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogVersion();

    /**
     * \brief   Returns the logging types (targets) property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogTarget();

    /**
     * \brief   Returns the logging enabled / disabled status property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogStatus();

    /**
     * \brief   Returns the logging type (in target) enabled / disabled flag property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogEnable();

    /**
     * \brief   Returns the log file location and file name mask property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogFileLocation();

    /**
     * \brief   Returns the flag to append logs into the file property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogFileAppend();

    /**
     * \brief   Returns the queue size of remote logging property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogRemoteQueueSize();

    /**
     * \brief   Returns the layout of enter scope message property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogLayoutEnter();

    /**
     * \brief   Returns the layout of log message property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogLayoutMessage();

    /**
     * \brief   Returns the layout of exit scope message property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogLayoutExit();

    /**
     * \brief   Returns the log scope enable / disable flag property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogScope();

    /**
     * \brief   Returns the list of supported remote services property structure.
     **/
    inline const NEPersistence::sPropertyKey& getServiceList();

    /**
     * \brief   Returns the process name of the remote service property structure.
     **/
    inline const NEPersistence::sPropertyKey& getServiceName();

    /**
     * \brief   Returns the list of connection type of the remote service property structure.
     **/
    inline const NEPersistence::sPropertyKey& getServiceConnection();

    /**
     * \brief   Returns the connection enable / disable flag of the remote service property structure.
     **/
    inline const NEPersistence::sPropertyKey& getServiceEnable();

    /**
     * \brief   Returns the connection address of the remote service property structure.
     **/
    inline const NEPersistence::sPropertyKey& getServiceAddress();

    /**
     * \brief   Returns the connection port number of the remote service property structure.
     **/
    inline const NEPersistence::sPropertyKey& getServicePort();

    /**
     * \brief   Returns the name of log database engine.
     **/
    inline const NEPersistence::sPropertyKey& getLogDatabaseEngine();

    /**
     * \brief   Returns the log database name.
     **/
    inline const NEPersistence::sPropertyKey& getLogDatabaseName();

    /**
     * \brief   Returns the location of the log database. This can be either
     *          a file path or URI.
     **/
    inline const NEPersistence::sPropertyKey& getLogDatabaseLocation();

    /**
     * \brief   Returns the driver of the log database.
     **/
    inline const NEPersistence::sPropertyKey& getLogDatabaseDriver();

    /**
     * \brief   Returns the IP-address of the log database engine to connect.
     **/
    inline const NEPersistence::sPropertyKey& getLogDatabaseAddress();

    /**
     * \brief   Returns the port number of the log database engine to connect.
     **/
    inline const NEPersistence::sPropertyKey& getLogDatabasePort();

    /**
     * \brief   The user name to use when connect to the database engine.
     **/
    inline const NEPersistence::sPropertyKey& getLogDatabaseUser();

    /**
     * \brief   The password to use when connect to the database engine.
     **/
    inline const NEPersistence::sPropertyKey& getLogDatabasePassword();

    /**
     * \brief   The default block size in bytes to allocate in shared buffer to minimize de-fragmentation.
     **/
    inline const NEPersistence::sPropertyKey& getDefaultBufferBlockSize();

    /**
     * \brief   The default message queue size in the dispatcher thread.
     **/
    inline const NEPersistence::sPropertyKey& getDefaultMessageQueueSize();
}

//////////////////////////////////////////////////////////////////////////
// NEPersistence namespace inline methods
//////////////////////////////////////////////////////////////////////////

inline const NEPersistence::sPropertyKey& NEPersistence::getConfigVersion()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::ConfigVersion)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getServiceList()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::ServiceList)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogVersion()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogVersion)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogTarget()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogTarget)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogStatus()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogStatus)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogEnable()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogEnable)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogFileLocation()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogFileLocation)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogFileAppend()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogFileAppend)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogRemoteQueueSize()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogRemoteQueueSize)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogLayoutEnter()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogLayoutEnter)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogLayoutMessage()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogLayoutMessage)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogLayoutExit()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogLayoutExit)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogScope()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogScope)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getServiceConnection()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::ServiceConnection)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getServiceEnable()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::ServiceEnable)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getServiceName()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::ServiceName)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getServiceAddress()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::ServiceAddress)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getServicePort()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::ServicePort)];
}

const NEPersistence::sPropertyKey& NEPersistence::getLogDatabaseEngine()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogDatabaseEngine)];
}

const NEPersistence::sPropertyKey& NEPersistence::getLogDatabaseName()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogDatabaseName)];
}

const NEPersistence::sPropertyKey& NEPersistence::getLogDatabaseLocation()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogDatabaseLocation)];
}

const NEPersistence::sPropertyKey& NEPersistence::getLogDatabaseDriver()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogDatabaseDriver)];
}

const NEPersistence::sPropertyKey& NEPersistence::getLogDatabaseAddress()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogDatabaseAddress)];
}

const NEPersistence::sPropertyKey& NEPersistence::getLogDatabasePort()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogDatabasePort)];
}

const NEPersistence::sPropertyKey& NEPersistence::getLogDatabaseUser()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogDatabaseUsername)];
}

const NEPersistence::sPropertyKey& NEPersistence::getLogDatabasePassword()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::LogDatabasePassword)];
}

const NEPersistence::sPropertyKey& NEPersistence::getDefaultBufferBlockSize()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::DefaultBufferBlock)];
}

const NEPersistence::sPropertyKey& NEPersistence::getDefaultMessageQueueSize()
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int32_t>(NEPersistence::ConfigEntry::DefaultMessageQueue)];
}

#endif  // AREG_PERSIST_PERSISTENCEDEFS_HPP
