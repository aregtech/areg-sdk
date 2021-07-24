/************************************************************************
 * \file        areg/trace/private/NELogConfig.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The syntax of trace log configuration and outputs
 ************************************************************************/
#include "areg/trace/private/NELogConfig.hpp"

#include "areg/base/File.hpp"
#include "areg/base/NEString.hpp"

/**
 * \brief   Use timestamp in name (file name, database name)
 **/
const char * const  NELogConfig::LOG_FILENAME_MASK_TIMESTAMP            = File::FILE_MASK_TIMESTAMP;
const char * const  NELogConfig::LOG_FILE_NAME_MASK_APPDATA             = File::SPEACIAL_MASKS[File::SpecialAppData];
const char * const  NELogConfig::LOG_FORMAT_TIMESTAMP                   = File::TIMESTAMP_FORMAT;
const int           NELogConfig::SYNTAX_CMD_EMPTY_LENGTH                = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_EMPTY));
const int           NELogConfig::SYNTAX_CMD_LOG_LENGTH                  = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG));
const int           NELogConfig::SYNTAX_CMD_LOG_VERSION_LENGTH          = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_VERSION));
const int           NELogConfig::SYNTAX_CMD_LOG_FILE_LENGTH             = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_FILE));
const int           NELogConfig::SYNTAX_CMD_LOG_REMOTE_LENGTH           = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_REMOTE));
const int           NELogConfig::SYNTAX_CMD_LOG_REMOTE_HOST_LENGTH      = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_REMOTE_HOST));
const int           NELogConfig::SYNTAX_CMD_LOG_REMOTE_PORT_LENGTH      = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_REMOTE_PORT));
const int           NELogConfig::SYNTAX_CMD_LOG_DB_LENGTH               = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_DB));
const int           NELogConfig::SYNTAX_CMD_LOG_DB_DRIVER_LENGTH        = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_DB_DRIVER));
const int           NELogConfig::SYNTAX_CMD_LOG_DB_HOST_LENGTH          = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_DB_HOST));
const int           NELogConfig::SYNTAX_CMD_LOG_DB_USER_LENGTH          = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_DB_USER));
const int           NELogConfig::SYNTAX_CMD_LOG_DB_PASSWORD_LENGTH      = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_DB_PASSWORD));
const int           NELogConfig::SYNTAX_CMD_LOG_DB_NAME_LENGTH          = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_DB_NAME));
const int           NELogConfig::SYNTAX_CMD_LOG_DEBUG_LENGTH            = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_DEBUG));
const int           NELogConfig::SYNTAX_CMD_LOG_APPEND_LENGTH           = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_APPEND));
const int           NELogConfig::SYNTAX_CMD_LOG_STACK_LENGTH            = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_STACK));
const int           NELogConfig::SYNTAX_CMD_LOG_ENABLE_LENGTH           = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_ENABLE));
const int           NELogConfig::SYNTAX_CMD_LOG_LAYOUT_ENTER_LENGTH     = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_LAYOUT_ENTER));
const int           NELogConfig::SYNTAX_CMD_LOG_LAYOUT_MESSAGE_LENGTH   = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_LAYOUT_MESSAGE));
const int           NELogConfig::SYNTAX_CMD_LOG_LAYOUT_EXIT_LENGTH      = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_LAYOUT_EXIT));
const int           NELogConfig::SYNTAX_CMD_LOG_SCOPE_LENGTH            = static_cast<int>(NEString::getStringLength<char>(NELogConfig::SYNTAX_CMD_LOG_SCOPE));


const NELogConfig::sConfigSytax    NELogConfig::ValidSytaxList[NELogConfig::ConfigLast]  =
{
      {NEString::EmptyStringA                       , 0                                                 }
    , {NELogConfig::SYNTAX_CMD_LOG_VERSION          , NELogConfig::SYNTAX_CMD_LOG_VERSION_LENGTH        }
    , {NELogConfig::SYNTAX_CMD_LOG_FILE             , NELogConfig::SYNTAX_CMD_LOG_FILE_LENGTH           }
    , {NELogConfig::SYNTAX_CMD_LOG_REMOTE_HOST      , NELogConfig::SYNTAX_CMD_LOG_REMOTE_HOST_LENGTH    }
    , {NELogConfig::SYNTAX_CMD_LOG_REMOTE_PORT      , NELogConfig::SYNTAX_CMD_LOG_REMOTE_PORT_LENGTH    }
    , {NELogConfig::SYNTAX_CMD_LOG_DB_DRIVER        , NELogConfig::SYNTAX_CMD_LOG_DB_DRIVER_LENGTH      }
    , {NELogConfig::SYNTAX_CMD_LOG_DB_HOST          , NELogConfig::SYNTAX_CMD_LOG_DB_HOST_LENGTH        }
    , {NELogConfig::SYNTAX_CMD_LOG_DB_USER          , NELogConfig::SYNTAX_CMD_LOG_DB_USER_LENGTH        }
    , {NELogConfig::SYNTAX_CMD_LOG_DB_PASSWORD      , NELogConfig::SYNTAX_CMD_LOG_DB_PASSWORD_LENGTH    }
    , {NELogConfig::SYNTAX_CMD_LOG_DB_NAME          , NELogConfig::SYNTAX_CMD_LOG_DB_NAME_LENGTH        }
    , {NELogConfig::SYNTAX_CMD_LOG_DEBUG            , NELogConfig::SYNTAX_CMD_LOG_DEBUG_LENGTH          }
    , {NELogConfig::SYNTAX_CMD_LOG_APPEND           , NELogConfig::SYNTAX_CMD_LOG_APPEND_LENGTH         }
    , {NELogConfig::SYNTAX_CMD_LOG_STACK            , NELogConfig::SYNTAX_CMD_LOG_STACK_LENGTH          }
    , {NELogConfig::SYNTAX_CMD_LOG_ENABLE           , NELogConfig::SYNTAX_CMD_LOG_ENABLE_LENGTH         }
    , {NELogConfig::SYNTAX_CMD_LOG_LAYOUT_ENTER     , NELogConfig::SYNTAX_CMD_LOG_LAYOUT_ENTER_LENGTH   }
    , {NELogConfig::SYNTAX_CMD_LOG_LAYOUT_MESSAGE   , NELogConfig::SYNTAX_CMD_LOG_LAYOUT_MESSAGE_LENGTH }
    , {NELogConfig::SYNTAX_CMD_LOG_LAYOUT_EXIT      , NELogConfig::SYNTAX_CMD_LOG_LAYOUT_EXIT_LENGTH    }
    , {NELogConfig::SYNTAX_CMD_LOG_SCOPE            , NELogConfig::SYNTAX_CMD_LOG_SCOPE_LENGTH          }
};

NELogConfig::eLogConfig NELogConfig::convFromString( const char * cmdSyntax )
{
    NELogConfig::eLogConfig result = NELogConfig::ConfigUnknown;
    if ( NEString::isEmpty<char>(cmdSyntax) == false )
    {
        if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_VERSION, NELogConfig::SYNTAX_CMD_LOG_VERSION_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogVersion;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_FILE, NELogConfig::SYNTAX_CMD_LOG_FILE_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogFile;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_REMOTE_HOST, NELogConfig::SYNTAX_CMD_LOG_REMOTE_HOST_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogRemoteHost;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_REMOTE_PORT, NELogConfig::SYNTAX_CMD_LOG_REMOTE_PORT_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogRemotePort;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_DB_DRIVER, NELogConfig::SYNTAX_CMD_LOG_DB_DRIVER_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogDatabaseDriver;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_DB_HOST, NELogConfig::SYNTAX_CMD_LOG_DB_HOST_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogDatabaseHost;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_DB_USER, NELogConfig::SYNTAX_CMD_LOG_DB_USER_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogDatabaseUser;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_DB_PASSWORD, NELogConfig::SYNTAX_CMD_LOG_DB_PASSWORD_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogDatabasePwd;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_DB_NAME, NELogConfig::SYNTAX_CMD_LOG_DB_NAME_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogDatabaseName;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_DEBUG, NELogConfig::SYNTAX_CMD_LOG_DEBUG_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogDebug;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_APPEND, NELogConfig::SYNTAX_CMD_LOG_APPEND_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogAppend;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_STACK, NELogConfig::SYNTAX_CMD_LOG_STACK_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogStack;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_ENABLE, NELogConfig::SYNTAX_CMD_LOG_ENABLE_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogEnable;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_LAYOUT_ENTER, NELogConfig::SYNTAX_CMD_LOG_LAYOUT_ENTER_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogLayoutEnter;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_LAYOUT_MESSAGE, NELogConfig::SYNTAX_CMD_LOG_LAYOUT_MESSAGE_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogLayoutMessage;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_LAYOUT_EXIT, NELogConfig::SYNTAX_CMD_LOG_LAYOUT_EXIT_LENGTH, false) == 0 )
            result = NELogConfig::ConfigLogLayoutExit;
        else if ( NEString::compareStrings<char, char>(cmdSyntax, NELogConfig::SYNTAX_CMD_LOG_SCOPE, NELogConfig::SYNTAX_CMD_LOG_SCOPE_LENGTH, false) == 0 )
            result = NELogConfig::ConfigScope;
    }
    return result;
}

const char * NELogConfig::convToString( NELogConfig::eLogConfig logConfig )
{
    switch ( logConfig )
    {
    case NELogConfig::ConfigLogVersion:
        return NELogConfig::SYNTAX_CMD_LOG_VERSION;
    case NELogConfig::ConfigLogFile:
        return NELogConfig::SYNTAX_CMD_LOG_FILE;
    case NELogConfig::ConfigLogRemoteHost:
        return NELogConfig::SYNTAX_CMD_LOG_REMOTE_HOST;
    case NELogConfig::ConfigLogRemotePort:
        return NELogConfig::SYNTAX_CMD_LOG_REMOTE_PORT;
    case NELogConfig::ConfigLogDatabaseDriver:
        return NELogConfig::SYNTAX_CMD_LOG_DB_DRIVER;
    case NELogConfig::ConfigLogDatabaseHost:
        return NELogConfig::SYNTAX_CMD_LOG_DB_HOST;
    case NELogConfig::ConfigLogDatabaseUser:
        return NELogConfig::SYNTAX_CMD_LOG_DB_USER;
    case NELogConfig::ConfigLogDatabasePwd:
        return NELogConfig::SYNTAX_CMD_LOG_DB_PASSWORD;
    case NELogConfig::ConfigLogDatabaseName:
        return NELogConfig::SYNTAX_CMD_LOG_DB_NAME;
    case NELogConfig::ConfigLogDebug:
        return NELogConfig::SYNTAX_CMD_LOG_DEBUG;
    case NELogConfig::ConfigLogAppend:
        return NELogConfig::SYNTAX_CMD_LOG_APPEND;
    case NELogConfig::ConfigLogStack:
        return NELogConfig::SYNTAX_CMD_LOG_STACK;
    case NELogConfig::ConfigLogEnable:
        return NELogConfig::SYNTAX_CMD_LOG_ENABLE;
    case NELogConfig::ConfigLogLayoutEnter:
        return NELogConfig::SYNTAX_CMD_LOG_LAYOUT_ENTER;
    case NELogConfig::ConfigLogLayoutMessage:
        return NELogConfig::SYNTAX_CMD_LOG_LAYOUT_MESSAGE;
    case NELogConfig::ConfigLogLayoutExit:
        return NELogConfig::SYNTAX_CMD_LOG_LAYOUT_EXIT;
    case NELogConfig::ConfigScope:
        return NELogConfig::SYNTAX_CMD_LOG_SCOPE;
    case NELogConfig::ConfigUnknown:// fall through
    case NELogConfig::ConfigLast:   // fall through
    default:
        return NELogConfig::DEFAULT_STRING_VALUE;
    }
}
