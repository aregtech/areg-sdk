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
AREG_NAMESPACE_BEGIN

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
    constexpr std::string_view  SYNTAX_TRUE                 { areg::BOOLEAN_TRUE };

    /**
     * \brief   Boolean syntax 'false'
     **/
    constexpr std::string_view  SYNTAX_FALSE                { areg::BOOLEAN_FALSE };

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
    enum eConfigKeys    : int
    {
          EntryInvalid              = -1    //!< Invalid key of the configuration
        , EntryConfigVersion        = 0     //!< The configuration version.

        , EntryLogVersion           = 1     //!< The logging version.
        , EntryLogTarget            = 2     //!< The logging types (targets).
        , EntryLogStatus            = 3     //!< The logging enabled / disabled status.
        , EntryLogEnable            = 4     //!< The logging type (in target) enabled / disabled flag.
        , EntryLogFileLocation      = 5     //!< The log file location and file name mask.
        , EntryLogFileAppend        = 6     //!< The flag to append logs into the file.
        , EntryLogRemoteQueueSize   = 7     //!< The queue size of remote logging.
        , EntryLogRemoteService     = 8     //!< The service name of the remote logging.

        , EntryLogDatabaseEngine    = 9     //!< The log database name.
        , EntryLogDatabaseName      = 10    //!< The log database name.
        , EntryLogDatabaseLocation  = 11    //!< The log database location.
        , EntryLogDatabaseDriver    = 12    //!< The log database driver.
        , EntryLogDatabaseAddress   = 13    //!< The log database address.
        , EntryLogDatabasePort      = 14    //!< The log database port.
        , EntryLogDatabaseUsername  = 15    //!< The log database user-name.
        , EntryLogDatabasePassword  = 16    //!< The log database user-password.

        , EntryLogLayoutEnter       = 17    //!< The layout of enter scope message.
        , EntryLogLayoutMessage     = 18    //!< The layout of log message.
        , EntryLogLayoutExit        = 19    //!< The layout of exit scope message.
        , EntryLogScope             = 20    //!< The log scope enable / disable flag.

        , EntryServiceList          = 21    //!< The list of supported remote services.

        , EntryServiceName          = 22    //!< The process name of the remote service.
        , EntryServiceConnection    = 23    //!< The list of connection type of the remote service.
        , EntryServiceEnable        = 24    //!< The connection enable / disable flag of the remote service.
        , EntryServiceAddress       = 25    //!< The connection address of the remote service.
        , EntryServicePort          = 26    //!< The connection port number of the remote service.

        , EntryDefaultBufferBlock   = 27    //!< The size in bytes to align when allocate a block in the bugger. The default `0` means allocated `sizeof(areg::uAlignt)`
        , EntryDefaultMessageQueue  = 28    //!< The default size of message queue in the dispatcher thread. The default `0` means to ignore the limitation, increase by need.

        , EntryAnyKey               = 29    //!< Indicates any key type.
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
    inline const areg::sPropertyKey& getConfigVersion();

    /**
     * \brief   Returns the logging version property structure.
     **/
    inline const areg::sPropertyKey& getLogVersion();

    /**
     * \brief   Returns the logging types (targets) property structure.
     **/
    inline const areg::sPropertyKey& getLogTarget();

    /**
     * \brief   Returns the logging enabled / disabled status property structure.
     **/
    inline const areg::sPropertyKey& getLogStatus();

    /**
     * \brief   Returns the logging type (in target) enabled / disabled flag property structure.
     **/
    inline const areg::sPropertyKey& getLogEnable();

    /**
     * \brief   Returns the log file location and file name mask property structure.
     **/
    inline const areg::sPropertyKey& getLogFileLocation();

    /**
     * \brief   Returns the flag to append logs into the file property structure.
     **/
    inline const areg::sPropertyKey& getLogFileAppend();

    /**
     * \brief   Returns the queue size of remote logging property structure.
     **/
    inline const areg::sPropertyKey& getLogRemoteQueueSize();

    /**
     * \brief   Returns the layout of enter scope message property structure.
     **/
    inline const areg::sPropertyKey& getLogLayoutEnter();

    /**
     * \brief   Returns the layout of log message property structure.
     **/
    inline const areg::sPropertyKey& getLogLayoutMessage();

    /**
     * \brief   Returns the layout of exit scope message property structure.
     **/
    inline const areg::sPropertyKey& getLogLayoutExit();

    /**
     * \brief   Returns the log scope enable / disable flag property structure.
     **/
    inline const areg::sPropertyKey& getLogScope();

    /**
     * \brief   Returns the list of supported remote services property structure.
     **/
    inline const areg::sPropertyKey& getServiceList();

    /**
     * \brief   Returns the process name of the remote service property structure.
     **/
    inline const areg::sPropertyKey& getServiceName();

    /**
     * \brief   Returns the list of connection type of the remote service property structure.
     **/
    inline const areg::sPropertyKey& getServiceConnection();

    /**
     * \brief   Returns the connection enable / disable flag of the remote service property structure.
     **/
    inline const areg::sPropertyKey& getServiceEnable();

    /**
     * \brief   Returns the connection address of the remote service property structure.
     **/
    inline const areg::sPropertyKey& getServiceAddress();

    /**
     * \brief   Returns the connection port number of the remote service property structure.
     **/
    inline const areg::sPropertyKey& getServicePort();

    /**
     * \brief   Returns the name of log database engine.
     **/
    inline const areg::sPropertyKey& getLogDatabaseEngine();

    /**
     * \brief   Returns the log database name.
     **/
    inline const areg::sPropertyKey& getLogDatabaseName();

    /**
     * \brief   Returns the location of the log database. This can be either
     *          a file path or URI.
     **/
    inline const areg::sPropertyKey& getLogDatabaseLocation();

    /**
     * \brief   Returns the driver of the log database.
     **/
    inline const areg::sPropertyKey& getLogDatabaseDriver();

    /**
     * \brief   Returns the IP-address of the log database engine to connect.
     **/
    inline const areg::sPropertyKey& getLogDatabaseAddress();

    /**
     * \brief   Returns the port number of the log database engine to connect.
     **/
    inline const areg::sPropertyKey& getLogDatabasePort();

    /**
     * \brief   The user name to use when connect to the database engine.
     **/
    inline const areg::sPropertyKey& getLogDatabaseUser();

    /**
     * \brief   The password to use when connect to the database engine.
     **/
    inline const areg::sPropertyKey& getLogDatabasePassword();

    /**
     * \brief   The default block size in bytes to allocate in shared buffer to minimize de-fragmentation.
     **/
    inline const areg::sPropertyKey& getDefaultBufferBlockSize();

    /**
     * \brief   The default message queue size in the dispatcher thread.
     **/
    inline const areg::sPropertyKey& getDefaultMessageQueueSize();
AREG_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////
// NEPersistence namespace inline methods
//////////////////////////////////////////////////////////////////////////

inline const areg::sPropertyKey& areg::getConfigVersion()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryConfigVersion)];
}

inline const areg::sPropertyKey& areg::getServiceList()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryServiceList)];
}

inline const areg::sPropertyKey& areg::getLogVersion()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogVersion)];
}

inline const areg::sPropertyKey& areg::getLogTarget()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogTarget)];
}

inline const areg::sPropertyKey& areg::getLogStatus()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogStatus)];
}

inline const areg::sPropertyKey& areg::getLogEnable()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogEnable)];
}

inline const areg::sPropertyKey& areg::getLogFileLocation()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogFileLocation)];
}

inline const areg::sPropertyKey& areg::getLogFileAppend()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogFileAppend)];
}

inline const areg::sPropertyKey& areg::getLogRemoteQueueSize()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogRemoteQueueSize)];
}

inline const areg::sPropertyKey& areg::getLogLayoutEnter()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogLayoutEnter)];
}

inline const areg::sPropertyKey& areg::getLogLayoutMessage()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogLayoutMessage)];
}

inline const areg::sPropertyKey& areg::getLogLayoutExit()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogLayoutExit)];
}

inline const areg::sPropertyKey& areg::getLogScope()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogScope)];
}

inline const areg::sPropertyKey& areg::getServiceConnection()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryServiceConnection)];
}

inline const areg::sPropertyKey& areg::getServiceEnable()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryServiceEnable)];
}

inline const areg::sPropertyKey& areg::getServiceName()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryServiceName)];
}

inline const areg::sPropertyKey& areg::getServiceAddress()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryServiceAddress)];
}

inline const areg::sPropertyKey& areg::getServicePort()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryServicePort)];
}

const areg::sPropertyKey& areg::getLogDatabaseEngine()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogDatabaseEngine)];
}

const areg::sPropertyKey& areg::getLogDatabaseName()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogDatabaseName)];
}

const areg::sPropertyKey& areg::getLogDatabaseLocation()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogDatabaseLocation)];
}

const areg::sPropertyKey& areg::getLogDatabaseDriver()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogDatabaseDriver)];
}

const areg::sPropertyKey& areg::getLogDatabaseAddress()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogDatabaseAddress)];
}

const areg::sPropertyKey& areg::getLogDatabasePort()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogDatabasePort)];
}

const areg::sPropertyKey& areg::getLogDatabaseUser()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogDatabaseUsername)];
}

const areg::sPropertyKey& areg::getLogDatabasePassword()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryLogDatabasePassword)];
}

const areg::sPropertyKey& areg::getDefaultBufferBlockSize()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryDefaultBufferBlock)];
}

const areg::sPropertyKey& areg::getDefaultMessageQueueSize()
{
    return areg::DefaultPropertyKeys[static_cast<int>(areg::eConfigKeys::EntryDefaultMessageQueue)];
}

#endif  // AREG_PERSIST_PERSISTENCEDEFS_HPP
