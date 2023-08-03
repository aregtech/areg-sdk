/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/NELogConfig.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       The syntax of trace log configuration and outputs
 ************************************************************************/
#include "areg/trace/private/NELogConfig.hpp"

#include "areg/base/File.hpp"
#include "areg/base/NEString.hpp"

NELogConfig::eLogConfig NELogConfig::convFromString( const char * cmdSyntax )
{
    NELogConfig::eLogConfig result = NELogConfig::eLogConfig::ConfigUnknown;

    if ( NEString::isEmpty<char>(cmdSyntax) == false )
    {
        if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_VERSION.data(), static_cast<int>(SYNTAX_CMD_LOG_VERSION.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogVersion;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_FILE.data(), static_cast<int>(SYNTAX_CMD_LOG_FILE.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogFile;
        }
        else if ( NEString::compareStrings<char, char>( cmdSyntax, SYNTAX_CMD_LOG_REMOTE_TCP_ENABLE.data( ), static_cast<int>(SYNTAX_CMD_LOG_REMOTE_TCP_ENABLE.length( )), false ) == NEMath::eCompare::Equal )
        {
            result = NELogConfig::eLogConfig::ConfigLogRemoteTcpEnable;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_REMOTE_TCP_HOST.data(), static_cast<int>(SYNTAX_CMD_LOG_REMOTE_TCP_HOST.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogRemoteTcpHost;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_REMOTE_TCP_PORT.data(), static_cast<int>(SYNTAX_CMD_LOG_REMOTE_TCP_PORT.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogRemoteTcpPort;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_DB_DRIVER.data(), static_cast<int>(SYNTAX_CMD_LOG_DB_DRIVER.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogDatabaseDriver;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_DB_HOST.data(), static_cast<int>(SYNTAX_CMD_LOG_DB_HOST.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogDatabaseHost;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_DB_USER.data(), static_cast<int>(SYNTAX_CMD_LOG_DB_USER.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogDatabaseUser;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_DB_PASSWORD.data(), static_cast<int>(SYNTAX_CMD_LOG_DB_PASSWORD.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogDatabasePwd;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_DB_NAME.data(), static_cast<int>(SYNTAX_CMD_LOG_DB_NAME.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogDatabaseName;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_DEBUG.data(), static_cast<int>(SYNTAX_CMD_LOG_DEBUG.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogDebug;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_APPEND.data( ), static_cast<int>(SYNTAX_CMD_LOG_APPEND.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogAppend;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_STACK.data( ), static_cast<int>(SYNTAX_CMD_LOG_STACK.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogStack;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_ENABLE.data( ), static_cast<int>(SYNTAX_CMD_LOG_ENABLE.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogEnable;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_LAYOUT_ENTER.data( ), static_cast<int>(SYNTAX_CMD_LOG_LAYOUT_ENTER.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogLayoutEnter;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_LAYOUT_MESSAGE.data( ), static_cast<int>(SYNTAX_CMD_LOG_LAYOUT_MESSAGE.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogLayoutMessage;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_LAYOUT_EXIT.data( ), static_cast<int>(SYNTAX_CMD_LOG_LAYOUT_EXIT.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigLogLayoutExit;
        }
        else if ( NEString::compareStrings<char, char>(cmdSyntax, SYNTAX_CMD_LOG_SCOPE.data( ), static_cast<int>(SYNTAX_CMD_LOG_SCOPE.length()), false) == NEMath::eCompare::Equal)
        {
            result = NELogConfig::eLogConfig::ConfigScope;
        }
    }

    return result;
}

const std::string_view & NELogConfig::convToString( NELogConfig::eLogConfig logConfig )
{
    switch ( logConfig )
    {
    case NELogConfig::eLogConfig::ConfigLogVersion:
        return NELogConfig::SYNTAX_CMD_LOG_VERSION;
    case NELogConfig::eLogConfig::ConfigLogFile:
        return NELogConfig::SYNTAX_CMD_LOG_FILE;
    case NELogConfig::eLogConfig::ConfigLogRemoteTcpHost:
        return NELogConfig::SYNTAX_CMD_LOG_REMOTE_TCP_HOST;
    case NELogConfig::eLogConfig::ConfigLogRemoteTcpPort:
        return NELogConfig::SYNTAX_CMD_LOG_REMOTE_TCP_PORT;
    case NELogConfig::eLogConfig::ConfigLogDatabaseDriver:
        return NELogConfig::SYNTAX_CMD_LOG_DB_DRIVER;
    case NELogConfig::eLogConfig::ConfigLogDatabaseHost:
        return NELogConfig::SYNTAX_CMD_LOG_DB_HOST;
    case NELogConfig::eLogConfig::ConfigLogDatabaseUser:
        return NELogConfig::SYNTAX_CMD_LOG_DB_USER;
    case NELogConfig::eLogConfig::ConfigLogDatabasePwd:
        return NELogConfig::SYNTAX_CMD_LOG_DB_PASSWORD;
    case NELogConfig::eLogConfig::ConfigLogDatabaseName:
        return NELogConfig::SYNTAX_CMD_LOG_DB_NAME;
    case NELogConfig::eLogConfig::ConfigLogDebug:
        return NELogConfig::SYNTAX_CMD_LOG_DEBUG;
    case NELogConfig::eLogConfig::ConfigLogAppend:
        return NELogConfig::SYNTAX_CMD_LOG_APPEND;
    case NELogConfig::eLogConfig::ConfigLogStack:
        return NELogConfig::SYNTAX_CMD_LOG_STACK;
    case NELogConfig::eLogConfig::ConfigLogEnable:
        return NELogConfig::SYNTAX_CMD_LOG_ENABLE;
    case NELogConfig::eLogConfig::ConfigLogLayoutEnter:
        return NELogConfig::SYNTAX_CMD_LOG_LAYOUT_ENTER;
    case NELogConfig::eLogConfig::ConfigLogLayoutMessage:
        return NELogConfig::SYNTAX_CMD_LOG_LAYOUT_MESSAGE;
    case NELogConfig::eLogConfig::ConfigLogLayoutExit:
        return NELogConfig::SYNTAX_CMD_LOG_LAYOUT_EXIT;
    case NELogConfig::eLogConfig::ConfigScope:
        return NELogConfig::SYNTAX_CMD_LOG_SCOPE;
    case NELogConfig::eLogConfig::ConfigUnknown:// fall through
    default:
        return NELogConfig::DEFAULT_STRING_VALUE;
    }
}
