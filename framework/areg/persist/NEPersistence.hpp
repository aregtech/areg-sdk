#ifndef AREG_PERSIST_NEPERSISTEN_HPP
#define AREG_PERSIST_NEPERSISTEN_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/persist/NEPersistence.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       File data persist syntax namespace
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/NECommon.hpp"
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
        , EntryLogLayoutEnter       = 9     //!< The layout of enter scope message.
        , EntryLogLayoutMessage     = 10    //!< The layout of log message.
        , EntryLogLayoutExit        = 11    //!< The layout of exit scope message.
        , EntryLogScope             = 12    //!< The log scope enable / disable flag.

        , EntryServiceList          = 13    //!< The list of supported remote services.

        , EntryServiceName          = 14    //!< The process name of the remote service.
        , EntryServiceConnection    = 15    //!< The list of connection type of the remote service.
        , EntryServiceEnable        = 16    //!< The connection enable / disable flag of the remote service.
        , EntryServiceAddress       = 17    //!< The connection address of the remote service.
        , EntryServicePort          = 18    //!< The connection port number of the remote service.

        , EntryAnyKey               = 19    //!< Indicates any key type.
    };

    /**
     * \brief   The list of property key structures supported by AREG
     **/
    constexpr sPropertyKey   DefaultPropertyKeys[]
        {
              {"config" , "*"   , "version" , ""        }   //! 0   , The configuration version property structure.

            , {"log"    , "*"   , "version" , ""        }   //! 1   , The logging version property structure.
            , {"log"    , "*"   , "target"  , ""        }   //! 2   , The logging types (targets) property structure.
            , {"log"    , "*"   , "enable"  , ""        }   //! 3   , The logging enabled / disabled status property structure.
            , {"log"    , "*"   , "enable"  , "*"       }   //! 4   , The logging type (in target) enabled / disabled flag property structure.
            , {"log"    , "*"   , "file"    , "location"}   //! 5   , The log file location and file name mask property structure.
            , {"log"    , "*"   , "file"    , "append"  }   //! 6   , The flag to append logs into the file property structure.
            , {"log"    , "*"   , "remote"  , "queue"   }   //! 7   , The queue size of remote logging property structure.
            , {"log"    , "*"   , "remote"  , "service" }   //! 8   , The service name of the remote logging.
            , {"log"    , "*"   , "layout"  , "enter"   }   //! 9   , The layout of enter scope message property structure.
            , {"log"    , "*"   , "layout"  , "message" }   //! 10  , The layout of log message property structure.
            , {"log"    , "*"   , "layout"  , "exit"    }   //! 11  , The layout of exit scope message property structure.
            , {"log"    , "*"   , "scope"   , "*"       }   //! 12  , The log scope enable / disable flag property structure.

            , {"service", "*"   , "list"    , ""        }   //! 13  , The list of supported remote services property structure.

            , {"*"      , "*"   , "service" , ""        }   //! 14  , The process name of the remote service property structure.
            , {"*"      , "*"   , "connect" , ""        }   //! 15  , The list of connection type of the remote service property structure.
            , {"*"      , "*"   , "enable"  , "*"       }   //! 16  , The connection enable / disable flag of the remote service property structure.
            , {"*"      , "*"   , "address" , "*"       }   //! 17  , The connection address of the remote service property structure.
            , {"*"      , "*"   , "port"    , "*"       }   //! 18  , The connection port number of the remote service property structure.
            , {"*"      , "*"   , "*"       , "*"       }   //! 19  , Indicates any key type.
    };

    /**
     * \brief   Returns the configuration version property structure.
     **/
    inline const NEPersistence::sPropertyKey& getConfigVersion(void);

    /**
     * \brief   Returns the logging version property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogVersion(void);

    /**
     * \brief   Returns the logging types (targets) property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogTarget(void);

    /**
     * \brief   Returns the logging enabled / disabled status property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogStatus(void);

    /**
     * \brief   Returns the logging type (in target) enabled / disabled flag property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogEnable(void);

    /**
     * \brief   Returns the log file location and file name mask property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogFileLocation(void);

    /**
     * \brief   Returns the flag to append logs into the file property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogFileAppend(void);

    /**
     * \brief   Returns the queue size of remote logging property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogRemoteQueueSize(void);

    /**
     * \brief   Returns the layout of enter scope message property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogLayoutEnter(void);

    /**
     * \brief   Returns the layout of log message property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogLayoutMessage(void);

    /**
     * \brief   Returns the layout of exit scope message property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogLayoutExit(void);

    /**
     * \brief   Returns the log scope enable / disable flag property structure.
     **/
    inline const NEPersistence::sPropertyKey& getLogScope(void);

    /**
     * \brief   Returns the list of supported remote services property structure.
     **/
    inline const NEPersistence::sPropertyKey& getServiceList(void);

    /**
     * \brief   Returns the process name of the remote service property structure.
     **/
    inline const NEPersistence::sPropertyKey& getServiceName(void);

    /**
     * \brief   Returns the list of connection type of the remote service property structure.
     **/
    inline const NEPersistence::sPropertyKey& getServiceConnection(void);

    /**
     * \brief   Returns the connection enable / disable flag of the remote service property structure.
     **/
    inline const NEPersistence::sPropertyKey& getServiceEnable(void);

    /**
     * \brief   Returns the connection address of the remote service property structure.
     **/
    inline const NEPersistence::sPropertyKey& getServiceAddress(void);

    /**
     * \brief   Returns the connection port number of the remote service property structure.
     **/
    inline const NEPersistence::sPropertyKey& getServicePort(void);
}

//////////////////////////////////////////////////////////////////////////
// NEPersistence namespace inline methods
//////////////////////////////////////////////////////////////////////////

inline const NEPersistence::sPropertyKey& NEPersistence::getConfigVersion(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryConfigVersion)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getServiceList(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryServiceList)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogVersion(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryLogVersion)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogTarget(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryLogTarget)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogStatus(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryLogStatus)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogEnable(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryLogEnable)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogFileLocation(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryLogFileLocation)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogFileAppend(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryLogFileAppend)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogRemoteQueueSize(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryLogRemoteQueueSize)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogLayoutEnter(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryLogLayoutEnter)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogLayoutMessage(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryLogLayoutMessage)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogLayoutExit(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryLogLayoutExit)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getLogScope(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryLogScope)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getServiceConnection(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryServiceConnection)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getServiceEnable(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryServiceEnable)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getServiceName(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryServiceName)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getServiceAddress(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryServiceAddress)];
}

inline const NEPersistence::sPropertyKey& NEPersistence::getServicePort(void)
{
    return NEPersistence::DefaultPropertyKeys[static_cast<int>(NEPersistence::eConfigKeys::EntryServicePort)];
}

#endif  // AREG_PERSIST_NEPERSISTEN_HPP
