#ifndef AREG_TRACE_NETRACE_HPP
#define AREG_TRACE_NETRACE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/NETrace.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, NETrace namespace contains constants, structures and types.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/NECommon.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/TEHashMap.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class TraceScope;
class FileBase;

//////////////////////////////////////////////////////////////////////////
// NETrace namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This namespace is a collection of helper objects like
 *          constants, structures, methods and types used
 *          to provide logging functionality.
 **/
namespace NETrace
{
    constexpr std::string_view LOG_VERSION  { "1.1.0" };

    /**
     * \brief   NETrace::eLogPriority
     *          Log priority definition set when logging message
     **/
    enum eLogPriority : unsigned int
    {
          PrioInvalid       = 0x0000  //!< Invalid priority,          bit set:  0000 0000 0000
        , PrioNotset        = 0x0001  //!< No priority is set,        bit set:  0000 0000 0001
        , PrioScope         = 0x0010  //!< Output scopes priority,    bit set:  0000 0001 0000
        , PrioFatal         = 0x0020  //!< Fatal error log priority,  bit set:  0000 0010 0000
        , PrioError         = 0x0040  //!< Error log priority,        bit set:  0000 0100 0000
        , PrioWarning       = 0x0080  //!< Warning log priority,      bit set:  0000 1000 1000
        , PrioInfo          = 0x0100  //!< Information log priority,  bit set:  0001 0000 0000
        , PrioDebug         = 0x0200  //!< Debug log priority,        bit set:  0010 0000 0000
        , PrioLogs          = 0x03E0  //!< Log is enabled priority,   bit set:  0011 1110 0000
        , PrioValidLogs     = 0x03F1  //!< The valid log priority,    bit set:  0011 1111 0001
        , PrioIgnore        = 0x0400  //!< Ignore logging priority,   bit set:  0100 0000 0000
        , PrioIgnoreLayout  = 0x0C00  //!< Ignore layout priority,    bit set:  1100 0000 0000
        , PrioAny           = 0x0FF0  //!< Log without priority,      bit set:  1111 1111 0000
        , PrioValid         = 0x0FF1  //!< Valid logging priority     bit set:  1111 1111 0001
    };

    /**
     * \brief   NETrace::getString
     *          Converts NETrace::eLogPriority values to readable string values
     **/
    inline const char * getString( NETrace::eLogPriority prio );

    /**
     * \brief   Returns true if the specified log priority value is valid.
     **/
    inline bool isValidLogPriority( NETrace::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority refers to the logging.
     **/
    inline bool isLogPriority( NETrace::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority enables logging, i.e. logs messages..
     **/
    inline bool isEnablingLog( NETrace::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority makes scope logging.
     **/
    inline bool isLogScope( NETrace::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority does not log messages.
     **/
    inline bool isDisablingLog( NETrace::eLogPriority prio );

    /**
     * \brief   NETrace::ToString
     *          Returns string value of NETrace::eLogPriority.
     *          There are following valid string priority values:
     *          NOTSET, SCOPE, FATAL, ERROR, WARNING, INFO, DEBUG.
     * \param   prio    The priority to get string value.
     * \return  Returns string priority value
     * \see     NETrace::FromString
     **/
    AREG_API const String& convToString( NETrace::eLogPriority prio );

    /**
     * \brief   NETrace::FromString
     *          From given string value returns log priority value.
     *          The string values should be followings:
     *          NOTSET, SCOPE, FATAL, ERROR, WARNING, INFO, DEBUG.
     * \param   strPrio The priority string value to convert.
     *                  The given string is not case sensitive.
     * \return  Returns appropriate logging priority value.
     **/
    AREG_API NETrace::eLogPriority convFromString( const String& strPrio );

    /**
     * \brief   Converts the bitwise set of priority into the human readable string.
     * \param   priorities      The priorities to convert. Set bitwise.
     * \return  Returns converted string that may contain logical OR ('|') if more than one priority is set.
     **/
    AREG_API String makePrioString( unsigned int priorities );

    /**
     * \brief   Returns the cookie of the logger.
     **/
    AREG_API const ITEM_ID & getCookie(void);

    /**
     * \brief   NETrace::HAS_MESSAGE_PRIORITY
     *          Flag, indicating whether there is any priority set to output message.
     *          The log has priority if one of first 5 bits are set.
     **/
    constexpr unsigned int  HAS_MESSAGE_PRIORITY    { 0x1F }; // 63, bit set 0001 1111

    /**
     * \brief   NETrace::TRACE_SCOPE_ID_NONE
     *          Constant, defines no scope ID. It is used to output message without scope
     **/
    constexpr unsigned int  TRACE_SCOPE_ID_NONE     { 0 };

    /**
     * \brief   Invalid cookie, indicating that there are no logs compiled with the project.
     **/
    constexpr ITEM_ID       INVALID_COOKIE          { 0 };

    /**
     * \brief   The cookie to indicate that messages are not for remote host.
     **/
    constexpr ITEM_ID       COOKIE_LOCAL            { static_cast<ITEM_ID>(NECommon::eCookies::CookieLocal) };

    /**
     * \brief   The cookie to indicate that messages are from logger service.
     **/
    constexpr ITEM_ID       COOKIE_SERVICE          { static_cast<ITEM_ID>(NECommon::eCookies::CookieService) };

    /**
     * \brief   The first valid cookie given by logging service.
     **/
    constexpr ITEM_ID       COOKIE_FIRST_VALID      { static_cast<ITEM_ID>(NECommon::eCookies::CookieFirstValid) };

    /**
     * \brief   The cookie to indicate that log can be either local or remote. It happens when the logger service initialization is not completed yet.
     **/
    constexpr ITEM_ID       COOKIE_ANY              { static_cast<ITEM_ID>(NECommon::eCookies::CookieAny) };

    /**
     * \brief   The string value of no priority
     **/
    const String  PRIO_NOTSET_STR     { "NOTSET" };
    /**
     * \brief   The string value of scope priority
     **/
    const String  PRIO_SCOPE_STR      { "SCOPE" };
    /**
     * \brief   The string value of fatal error priority
     **/
    const String  PRIO_FATAL_STR      { "FATAL" };
    /**
     * \brief   The string value of error priority
     **/
    const String  PRIO_ERROR_STR      { "ERROR" };
    /**
     * \brief   The string value of warning priority
     **/
    const String  PRIO_WARNING_STR    { "WARNING" };
    /**
     * \brief   The string value of information priority
     **/
    const String  PRIO_INFO_STR       { "INFO" };
    /**
     * \brief   The string value of debug priority
     **/
    const String  PRIO_DEBUG_STR      { "DEBUG" };
    /**
     * \brief   No priority string
     **/
    const String  PRIO_NO_PRIO        { "" };

    /**
     * \brief   NETrace::LOG_MESSAGE_BUFFER_SIZE
     *          The maximum size of text in log message
     **/
    constexpr uint32_t  LOG_MESSAGE_BUFFER_SIZE { 512 };
    /**
     * \brief   NETrace::LOG_NAMES_SIZE
     *          The maximum length of the names in logging objects
     **/
    constexpr uint32_t   LOG_NAMES_SIZE         { LOG_MESSAGE_BUFFER_SIZE / 2 };
    /**
     * \brief   NETrace::IP_ADDRESS_SIZE
     *          The size of buffer to reserve for IP address, like "255.255.255.255"
     **/
    constexpr uint32_t  IP_ADDRESS_SIZE         { 16 };

    /**
     * \brief   NETrace::eLogType
     *          The logging type IDs. Should be set when sending log message
     **/
    enum eLogType : unsigned int
    {
          LogUndefined  = 0 //!< Undefined log message
        , LogMessage        //!< The message type is a logging message.
        , LogRequest        //!< Any generic request sent from the host application to target logger service.
        , LogResponse       //!< Any generic response sent from target logger service to the host application.
        , LogNotify         //!< Any generic notification sent from target logger service to the host application.
    };

    /**
     * \brief   NETrace::eMessageType
     *          The logging message type.
     **/
    enum eMessageType : unsigned int
    {
          MsgUndefined  = 0 //!< Undefined message type
        , MsgScopeEnter     //!< Message entered scope
        , MsgScopeExit      //!< Message exit scope
        , MsgText           //!< Message text
    };

    /**
     * \brief   NETrace::eLogCommands
     *          The logging command type.
     **/
    enum eLogCommands : unsigned int
    {
          LogCommandUndefined           = 0x00000000    //!< The log request is undefined and invalid
        , LogRequestFirst               = 0x00000001    //!< The first ID of the request sent by host to the remote logger service.
        , LogRequestConnect                             //!< The log request to connect to the remote logger service.
        , LogRequestDisconnect                          //!< The log request to disconnection from the remote logger service
        , LogRequestRegisterScopeList                   //!< The log request to register list of scopes
        , LogResponseFirst              = 0x00001000    //!< The first ID of the response sent by remote logger service to the host
        , LogResponseConnect                            //!< The log response to connect to remote logger service
        , LogResponseRegisterScopes
        , LogNotificationFirst          = 0x00002000    //!< The first ID of the log notification
        , LogNotifyScopePriority                        //!< The notification to set the scope priority
    };

    /**
     * \brief   NETrace::eLogModuleType
     *          The type of logging module.
     **/
    enum eLogModuleType : unsigned int
    {
          LogModuleUndefien     = 0 //!< Undefined log module.
        , LogModuleGenerator        //!< The module generates logging data.
        , LogModuleService          //!< The module is logging service.
        , LogModuleCollector        //!< The module collects logs. For example, is a log viewer.
    };

    /**
     * \brief   NETrace::eScopeAction
     *          The actions take on scope list.
     **/
    enum eScopeAction   : unsigned int
    {
          ScopesNoAction
        , ScopesSet
        , ScopesAppend
        , ScopesUpdate
    };

    /**
     * \brief   NETrace::sLogHeader
     *          Structure of logging message header.
     * 
     **/
    struct sLogHeader
    {
        /**
         * \brief   The length in bytes of complete log object.
         **/
        unsigned int    hdrDataLen;
        /**
         * \brief   The type of log message.
         **/
        eLogType        hdrLogType;
        /**
         * \brief   The cookie of the message host.
         *          If the message sent to remote logging service, this is the cookie of log host, set by remote logging service.
         *          If the message sent to the log host (i.e. sent by service), this is the NETrace::COOKIE_SERVICE.
         **/
        ITEM_ID         hdrCookieHost;
        /**
         * \brief   The cookie of the message target.
         *          If the message sent to remote logging service, this is the NETrace::COOKIE_SERVICE.
         *          If the message sent to the log host (i.e. sent by service), this is the cookie set by logging service.
         **/
        ITEM_ID         hdrCookieTarget;
    };

    /**
     *\brief    NETrace::sLogCommandData
     *          The generic message structure to send communication message data between host and logging service.
     **/
    struct sLogCommandData
    {
        //!< The ID of action to perform
        eLogCommands    dataCommand;
        //!< The data buffer related with action.
        unsigned char   dataBuffer[4];
    };

    /**
     *\brief    NETrace::sLogCommand
     *          The generic message structure to send communication message between host and logging service.
     **/
    struct sLogCommand
    {
        //!< The logging message header.
        NETrace::sLogHeader         cmdHeader   { sizeof(NETrace::sLogCommandData), NETrace::eLogType::LogUndefined, NETrace::COOKIE_LOCAL, NETrace::COOKIE_LOCAL };
        //!< The data buffer of the command.
        NETrace::sLogCommandData    cmdData     { NETrace::eLogCommands::LogCommandUndefined };
    };

    /**
     * \brief   NETrace::sLogMessageData
     *          The structure of output logging message.
     **/
    struct AREG_API sLogMessageData
    {
        /**
         * \brief   Initializes empty log tracing object, set message type.
         * \param   msgType The type of message. It can be scope enter, scope exit or text.
         **/
        sLogMessageData(NETrace::eMessageType msgType = NETrace::eMessageType::MsgUndefined);

        /**
         * \brief   Initializes log tracing object, sets scope ID, priority and optional message.
         * \param   msgType     The logging message type.
         * \param   scopeId     The ID scope, which is calculated from name.
         * \param   msgPrio     The priority of output message.
         * \param   message     The message to output. Can be nullptr or empty.
         **/
        sLogMessageData(NETrace::eMessageType msgType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen );

        /**
         * \brief   Copies log tracing data from given source.
         * \param   src     The source of data to copy.
         **/
        sLogMessageData( const sLogMessageData & src );

        /**
         * \brief   Copies data from given source.
         * \param   src     The source of data to copy.
         **/
        sLogMessageData & operator = ( const sLogMessageData & src );

        NETrace::eMessageType   dataMsgType;       //!< The type of the logging message.
        ITEM_ID                 dataHostId;        //!< The ID of the host, same as 'hdrCookieTarget' in the header.
        ITEM_ID                 dataModuleId;      //!< The ID of the application module that generated log message
        ITEM_ID                 dataThreadId;      //!< The ID of thread that generated log message
        TIME64                  dataTimestamp;     //!< The time-stamp of log message
        unsigned int            dataScopeId;       //!< The ID of trace scope that generated log message
        NETrace::eLogPriority   dataMessagePrio;   //!< The log message priority
        unsigned int            dataMessageLen;    //!< The actual length of the log message
        char                    dataMessage[LOG_MESSAGE_BUFFER_SIZE];  //!< The message text to output, with maximum NETrace::LOG_MESSAGE_BUFFER_SIZE characters
    };

    /**
     * \brief   NETrace::sLogMessage
     *          The structure of logging message object to trace in output target.
     **/
    struct sLogMessage
    {
        /**
         * \brief   Initializes logging message of specified type.
         * \param   msgType     The logging message type.
         **/
        inline sLogMessage( NETrace::eMessageType msgType = NETrace::eMessageType::MsgUndefined );
        /**
         * \brief   Initializes logging message and sets specified data.
         * \param   msgType     The logging message type.
         * \param   scopeId     The ID of message scope.
         * \param   mstPrio     The priority of logging message.
         * \param   message     The message text to output on target. Can be empty.
         **/
        inline sLogMessage(NETrace::eMessageType msgType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen);
        /**
         * \brief   Copies data from given source.
         * \param   src     The source to copy data.
         **/
        inline sLogMessage( const sLogMessage & src );

        //!< The logging message header.
        NETrace::sLogHeader         lmHeader;
        //!< The logging message data with text to output on target.
        NETrace::sLogMessageData    lmTrace;
    };

    /**
     * \brief   NETrace::sLogRequestConnectData
     *          The structure of request to connect to remote logging service.
     **/
    struct AREG_API sLogRequestConnectData
    {
        DECLARE_NOMOVE(sLogRequestConnectData);

        /**
         * \brief   Set default values:
         *              - The command is the request to connect;
         *              - The type module is the log generator;
         *              - The name of the host machine;
         *              - The name of executable application.
         **/
        sLogRequestConnectData(void);

        /**
         * \brief   Copy data from given source.
         **/
        sLogRequestConnectData(const sLogRequestConnectData & src);

        /**
         * \brief   Assign values from given source.
         **/
        sLogRequestConnectData & operator = (const sLogRequestConnectData & src);

        //!< The command to execute.
        NETrace::eLogCommands   dataCommand;
        //!< The type of module that requests to connect.
        NETrace::eLogModuleType dataModuleType;
        //!< The name of the host that requested to connect.
        char                    dataHost[NETrace::LOG_NAMES_SIZE];
        //!< The name of the application that requested connect.
        char                    dataModule[NETrace::LOG_NAMES_SIZE];
    };

    /**
     *\brief    NETrace::sLogRequestConnect
     *          The message structure to send a connection request to the logger service.
     **/
    struct sLogRequestConnect
    {
        //!< The logging message header.
        NETrace::sLogHeader             reqConHeader { sizeof(NETrace::sLogRequestConnectData), NETrace::eLogType::LogRequest, NETrace::getCookie(), NETrace::COOKIE_SERVICE };
        //!< The list of names.
        NETrace::sLogRequestConnectData reqConData;
    };

    /**
     *\brief    NETrace::sLogResponseConnectData
     *          The structure of response message on the request to connect to the log service.
     **/
    struct sLogResponseConnectData
    {
        DECLARE_NOMOVE(sLogResponseConnectData);

        //!< The command for the host
        NETrace::eLogCommands   dataCommand { NETrace::eLogCommands::LogResponseConnect };
        //!< The cookie of a new connected target.
        ITEM_ID                 dataCookie  { NETrace::COOKIE_ANY };
        //!< The recognized IP-address of the host.
        char                    dataIpAddress[NETrace::IP_ADDRESS_SIZE]{ '0', '0', '0', '.', '0', '0', '0', '.', '0','0','0', '.', '0', '0', '0', '\0' };
    };

    /**
     *\brief    NETrace::sLogResponseConnect
     *          The message structure of the response to the request to connect to the logger service.
     **/
    struct sLogResponseConnect
    {
        //!< The logging message header.
        NETrace::sLogHeader                 respConHeader { sizeof(NETrace::sLogResponseConnectData), NETrace::eLogType::LogResponse, NETrace::COOKIE_SERVICE, NETrace::COOKIE_ANY };
        //!< The response data on connection request.
        NETrace::sLogResponseConnectData    respConHost;
    };

    /**
     *\brief    NETrace::sLogRequestDisconnectData
     *          The message structure to request to disconnect from the logger service.
     **/
    struct sLogRequestDisconnectData
    {
        DECLARE_NOMOVE(sLogRequestDisconnectData);

        //!< The command data
        NETrace::sLogCommandData  dataData{ NETrace::eLogCommands::LogRequestDisconnect, { '\0', '\0', '\0', '\0' } };
    };

    /**
     *\brief    NETrace::sLogRequestDisconnect
     *          The message structure to send disconnect request to the logger service.
     **/
    struct sLogRequestDisconnect
    {
        //!< The logging message header.
        NETrace::sLogHeader                 reqDisconHeader { sizeof(NETrace::sLogRequestDisconnectData), NETrace::eLogType::LogRequest, NETrace::getCookie(), NETrace::COOKIE_ANY };
        //!< The command data
        NETrace::sLogRequestDisconnectData  reqDisconData   { NETrace::eLogCommands::LogRequestDisconnect, { '\0', '\0', '\0', '\0' } };
    };

    /**
     *\brief    NETrace::sLogScopeInfo
     *          The structure of the scope information.
     **/
    struct sLogScopeInfo
    {
        DECLARE_NOMOVE(sLogScopeInfo);

        //!< The type of the command.
        NETrace::eLogCommands   dataCommand     { NETrace::eLogCommands::LogCommandUndefined };
        //!< The action take on scope list.
        NETrace::eScopeAction   dataScopeAction { NETrace::eScopeAction::ScopesNoAction };
        //!< The length of allocated buffer size in bytes to write data.
        uint32_t                dataBufferLen   { 0 };
        //!< The number of entries in the buffer.
        uint32_t                dataScopeCount  { 0 };
    };

    /**
     *\brief    NETrace::sLogRequestRegisterScopes
     *          The structure of the message to request register scopes.
     *          The buffer of 'reqScopeList' should be allocated enough to write data in a row.
     *          The 'reqScopeList' buffer structure must be a pair {"scope name", "scope priority"}
     *              - Each scope name should end with null-terminated character '\0'.
     *              - Each scope name follows the logging priority value.
     *              - The number of scope pairs entries must be specified in the 'reqScopeInfo.dataScopeCount.
     *              - The total length of 'reqScopeList' buffer must be equal to 'reqScopeInfo.dataBufferLen'.
     **/
    struct sLogRequestRegisterScopes
    {
        //!< The logging message header
        NETrace::sLogHeader     reqScopeHeader  { sizeof(NETrace::sLogScopeInfo), NETrace::eLogType::LogRequest, NETrace::getCookie(), NETrace::COOKIE_ANY };
        //!< The log scope information. Also contains the number of entries and length of 'reqScopeList' buffer.
        NETrace::sLogScopeInfo  reqScopeInfo    { NETrace::eLogCommands::LogRequestRegisterScopeList, NETrace::eScopeAction::ScopesNoAction, 0, 0 };
        //!< The buffer to save {"scope name", "scope priority"} pair to register.
        unsigned char           reqScopeList[1] { };
    };

    /**
     *\brief    NETrace::sLogResponseRegisterScopes
     *          The message structure of the response to the request to register scopes.
     **/
    struct sLogResponseRegisterScopes
    {
        //!< The logging message header.
        NETrace::sLogHeader     resConHeader     { sizeof(NETrace::sLogScopeInfo), NETrace::eLogType::LogResponse, NETrace::COOKIE_SERVICE, NETrace::COOKIE_ANY };
        //!< The log scope information. Also contains the number of entries and length of 'resScopeList' buffer. There can be separate scope of group of scopes.
        NETrace::sLogScopeInfo  resScopeInfo     { NETrace::eLogCommands::LogRequestRegisterScopeList, NETrace::eScopeAction::ScopesNoAction, 0, 0 };
        //!< The buffer to save {"scope name", "scope priority"} pair to register.
        unsigned char           resScopeList[ 1 ]{ };
    };

    /**
     *\brief    NETrace::sLogNotifyLogPriority
     *          The structure of message sent by log service to change the log priority.
     *          The buffer 'notifyScopeList' should be long enough to save all entries
     *          of {"scope name", "scope priority"} pairs.
     *              - Each entry is either a single scope or scope group ending with '*'.
     *              - Each name should end with null-terminated character '\0'.
     *              - Each scope name (or scope group) follows the scope priority.
     *              - The number of scope pairs entries must be specified in the 'reqScopeInfo.dataScopeCount.
     *              - The total length of 'reqScopeList' buffer must be equal to 'reqScopeInfo.dataBufferLen'.
     **/
    struct sLogNotifyLogPriority
    {
        //!< The logging message header
        NETrace::sLogHeader     notifyScopeHeader   { sizeof(NETrace::sLogScopeInfo), NETrace::eLogType::LogNotify, NETrace::COOKIE_SERVICE, NETrace::COOKIE_ANY };
        //!< The log scope information. Also contains the number of entries and length of 'notifyScopeList' buffer.
        NETrace::sLogScopeInfo  notifyScopeInfo     { NETrace::eLogCommands::LogNotifyScopePriority, NETrace::eScopeAction::ScopesNoAction, 0, 0 };
        //!< The buffer to save {"scope name", "scope priority"} pair to register.
        unsigned char           notifyScopeList[1]  { };
    };

    /**
     * \brief   Returns true if the log header object indicates on log message.
     * \param   logHeader   The log header object, which has indication of message type.
     **/
    inline bool isLogMessage(const NETrace::sLogHeader& logHeader);

    /**
     * \brief   Returns true if the log header object indicates on request message.
     * \param   logHeader   The log header object, which has indication of message type.
     **/
    inline bool isLogRequest(const NETrace::sLogHeader & logHeader);

    /**
     * \brief   Returns true if the log header object indicates on response message.
     * \param   logHeader   The log header object, which has indication of message type.
     **/
    inline bool isLogResponse(const NETrace::sLogHeader& logHeader);

    /**
     * \brief   Returns true if the log header object indicates on notification message.
     * \param   logHeader   The log header object, which has indication of message type.
     **/
    inline bool isLogNotify(const NETrace::sLogHeader& logHeader);

    /**
     * \brief   Start logging. If specified file is not nullptr, it configures logging first, then starts logging.
     * \param   fileConfig  The relative or absolute path to logging configuration file.
     *                      If not nullptr, the system configures logging then starts logging.
     *                      If nullptr and logging was not configured, the system configures logging using default configuration path.
     *                      If nullptr and logging was configured, the system immediately starts logging.
     * \return  Returns true if succeeded to configure and start logging.
     **/
    AREG_API bool startLogging( const char * fileConfig = nullptr );

    /**
     * \brief   Sets default configuration values and forces to start logging.
     * \return  Returns true if succeeded to start logging.
     **/
    AREG_API bool forceStartLogging( void );

    /**
     * \brief   Loads configuration values from specified configuration file and force to start logging, i.e. enables logging.
     *          If configuration file does not exit, it sets default configuration values for logging.
     *          If configuration file is nullptr or empty, it tries to load configuration values from default configuration file.
     * \param   fileConfig  If not nullptr and file exist, loads configuration data from file, enables and starts logging.
     *                      If nullptr or empty, and default configuration file exist, loads configuration data from default file,
     *                      enables and starts logging.
     *                      If neither specified, nor default configuration file exist, it sets logging default values and start logging.
     * \param   configFile
     * \return  Returns true if succeeded to start logging.
     **/
    AREG_API bool initAndStartLogging( const char * fileConfig = nullptr );

    /**
     * \brief   Stops logging. No message will be logged anymore
     *          until it is not started again.
     **/
    AREG_API void stopLogging( void );

    /**
     * \brief   Marks specified scope as active, so that message related with scope can be logged.
     *          By default, the activated scope logging priority is NETrace::PrioNotset and
     *          the messages cannot be logged yet. If the configuration contains logging
     *          information of specified log and the information permits logging, 
     *          i.e. the logging priority is not equal to NETrace::PrioNotset,
     *          the message texts can be logged in output target.
     * \param   tarceScope  The instance of scope to activate to set appropriate logging priority.
     *                      If no priority information found, the messages of scope cannot be logged yet.
     * \note    It is not enough to activate scope to output message on the target.
     *          If configuration file does not have priority information, messages are not logged.
     *          If configuration file contains information of scope and the priority is not NETrace::PrioNotset,
     *          the system can log messages of appropriate scope.
     **/
    AREG_API void activateScope( TraceScope & traceScope );

    /**
     * \brief   Returns true if logging has been started.
     **/
    AREG_API bool isStarted( void );

    /**
     * \brief   Returns true if logging has been configured.
     **/
    AREG_API bool isConfigured( void );

    /**
     * \brief   Returns true if logging is enabled.
     *          If logging is enabled in configuration file, 
     *          the system ignores logging.
     **/
    AREG_API bool isEnabled( void );

    /**
     * \brief   Returns the logging config file name.
     **/
    AREG_API const String& getConfigFile( void );

    /**
     * \brief   Initializes the logging by reading and configuration instructions
     *          from given logging file.
     * \param   fileConfig  The relative or absolute path of logging file.
     *                      If empty or nullptr, the system default path will be taken.
     * \return  Returns true if succeeded to open file and configure logging.
     **/
    AREG_API bool initializeLogging( const char * fileConfig );

    /**
     * \brief   Saves the current configuration of the logging state in the file.
     * \param   configFile  The pointer to the configuration file to save.
     * \return  Returns true if succeeded to save the current state of the logging.
     **/
    AREG_API bool saveLogging( const char * configFile = nullptr );

    /**
     * \brief   Reads the log configuration and initializes logging from the specified file.
     *          The file should be opened for reading.
     **/
    AREG_API bool readConfiguration( const FileBase & file );

    /**
     * \brief   Saves the current state of the logging in the configuration file.
     *          It keeps the existing configurations and modifies only
     *          part relevant to current module. In all other cases, nothing is changes.
     * \param   file    The file to save log configuration.
     *                  The file should be opened for writing.
     * \return  Returns true if succeeded to save current state of logging.
     **/
    AREG_API bool saveConfiguration( FileBase & file );

    /**
     * \brief   Returns the ID of given scope name.
     *          If scope name is nullptr or empty, it returns zero.
     * \param   scopeName   The name of scope. If nullptr or empty,
     *                      the return value is zero.
     * \return  Returns the ID of given scope name.
     **/
    AREG_API unsigned int makeScopeId( const char * scopeName );

    /**
     * \brief   Call to change the scope log priority.
     * \param   scopeName   The name of the existing scope. Ignored if scope does not exit.
     * \param   newPrio     The new priority to set. Can be bitwise combination with scopes.
     * \return  Returns true if scope found and priority changed.
     **/
    AREG_API unsigned int setScopePriority( const char * scopeName, unsigned int newPrio );

    /**
     * \brief   Returns the scope priority if found. Otherwise, returns invalid priority.
     * \param   scopeName   The name of the existing scope.
     * \return  Is found the scope, returns the actual priority of the scope.
     *          Otherwise, returns invalid priority (NETrace::eLogPriority::PrioInvalid).
     **/
    AREG_API unsigned int getScopePriority( const char * scopeName );

    /**
     * \brief   Creates a message for logging service to get connection registration.
     * \return  Returns generated message.
     **/
    AREG_API SharedBuffer messageConnectLogService( void );

    /**
     * \brief   Creates a message for logging service to get connection unregistration.
     * \return  Returns generated message.
     **/
    AREG_API SharedBuffer messageDisconnectLogService( void );

    /**
     * \brief   Creates a message for logging service to start registering application logging scopes.
     * \return  Returns generated message.
     **/
    AREG_API SharedBuffer messageRegisterScopesStart( void );

    /**
     * \brief   Creates a message for logging service to end registering application logging scopes.
     * \return  Returns generated message.
     **/
    AREG_API SharedBuffer messageRegisterScopesEnd( void );

    /**
     * \brief   Creates a message for logging service to register scopes with message priority.
     * \param   scopeList   The list of scopes to register.
     * \param   startAt     The position in the list to extract and register scopes.
     *                          - If in input the value is invalid, it starts to register from beginning.
     *                          - If on output the value is valid, there are still unregistered scopes 
     *                            in the list and the value indicates the next valid scope.
     *                          - In on output the value is invalid, there are no more unregistered scopes
     *                            in the list.
     * \param   maxEntries  The maximum entries to push to scope registering message.
     *                      If the value is 0xFFFFFFFF, it registers all entries.
     * \return  Returns generated message.
     **/
    AREG_API SharedBuffer messageRegisterScopes( const TEHashMap<unsigned int, TraceScope *> & scopeList
                                               , TEHashMap<unsigned int, TraceScope *>::MAPPOS & startAt
                                               , unsigned int maxEntries );
}

//////////////////////////////////////////////////////////////////////////////
// NETrace namespace streamable types
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_STREAMABLE(NETrace::eLogPriority)
IMPLEMENT_STREAMABLE(NETrace::eLogType)
IMPLEMENT_STREAMABLE(NETrace::eMessageType)
IMPLEMENT_STREAMABLE(NETrace::eLogCommands)
IMPLEMENT_STREAMABLE(NETrace::sLogHeader)
IMPLEMENT_STREAMABLE(NETrace::sLogMessageData)
IMPLEMENT_STREAMABLE(NETrace::sLogRequestConnectData)
IMPLEMENT_STREAMABLE(NETrace::sLogResponseConnectData)
IMPLEMENT_STREAMABLE(NETrace::sLogRequestDisconnectData)
IMPLEMENT_STREAMABLE(NETrace::sLogScopeInfo)

//////////////////////////////////////////////////////////////////////////////
// NETrace namespace objects
//////////////////////////////////////////////////////////////////////////////

inline NETrace::sLogMessage::sLogMessage( NETrace::eMessageType msgType /* = NETrace::eMessageType::MsgUndefined */ )
    : lmHeader  { sizeof(NETrace::sLogMessageData), NETrace::eLogType::LogMessage, NETrace::getCookie(), NETrace::COOKIE_SERVICE }
    , lmTrace   (msgType)
{
}

inline NETrace::sLogMessage::sLogMessage(const sLogMessage & src)
    : lmHeader  { src.lmHeader.hdrDataLen, src.lmHeader.hdrLogType, src.lmHeader.hdrCookieHost, src.lmHeader.hdrCookieTarget }
    , lmTrace   ( src.lmTrace )
{
}

inline NETrace::sLogMessage::sLogMessage(NETrace::eMessageType msgType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen )
    : lmHeader  { sizeof(NETrace::sLogMessageData), NETrace::eLogType::LogMessage, NETrace::getCookie(), NETrace::COOKIE_SERVICE }
    , lmTrace   ( msgType, scopeId, msgPrio, message, msgLen )
{
}

//////////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief   Deserializes log message from the stream.
 * \param   stream  The source of log message data.
 * \param   input   On output this contains structured logging message.
 **/
inline const IEInStream& operator >> (const IEInStream& stream, NETrace::sLogMessage& input)
{
    stream >> input.lmHeader;
    stream >> input.lmTrace;
    return stream;
}

/**
 * \brief   Serializes log message to the stream.
 * \param   stream  The streaming object to save log message.
 * \param   output  The source of log message to serialize message.
 **/
inline IEOutStream& operator << (IEOutStream& stream, const NETrace::sLogMessage& output)
{
    stream << output.lmHeader;
    stream << output.lmTrace;
    return stream;
}

/**
 * \brief   Deserializes log message from the stream.
 * \param   stream  The source of log message data.
 * \param   input   On output this contains structured logging message.
 **/
inline const IEInStream& operator >> (const IEInStream& stream, NETrace::sLogRequestConnect& input)
{
    stream >> input.reqConHeader;
    stream >> input.reqConData;
    return stream;
}

/**
 * \brief   Serializes log message to the stream.
 * \param   stream  The streaming object to save log message.
 * \param   output  The source of log message to serialize message.
 **/
inline IEOutStream& operator << (IEOutStream& stream, const NETrace::sLogRequestConnect& output)
{
    stream << output.reqConHeader;
    stream << output.reqConData;
    return stream;
}

/**
 * \brief   Deserializes log message from the stream.
 * \param   stream  The source of log message data.
 * \param   input   On output this contains structured logging message.
 **/
inline const IEInStream& operator >> (const IEInStream& stream, NETrace::sLogRequestDisconnect & input)
{
    stream >> input.reqDisconHeader;
    stream >> input.reqDisconData;
    return stream;
}

/**
 * \brief   Serializes log message to the stream.
 * \param   stream  The streaming object to save log message.
 * \param   output  The source of log message to serialize message.
 **/
inline IEOutStream& operator << (IEOutStream& stream, const NETrace::sLogRequestDisconnect & output)
{
    stream << output.reqDisconHeader;
    stream << output.reqDisconData;
    return stream;
}

/**
 * \brief   Deserializes structure to register logging scopes to the stream.
 * \param   stream  The source of {"scope name", "scope priority"} pairs.
 * \param   input   On output this contains structured data.
 *                  The size of the allocated buffer should be enough to save
 *                  header, scope information and the list of scopes.
 **/
inline const IEInStream& operator >> (const IEInStream& stream, NETrace::sLogRequestRegisterScopes& input)
{
    stream >> input.reqScopeHeader;
    stream >> input.reqScopeInfo;
    stream.read(input.reqScopeList, input.reqScopeInfo.dataBufferLen);

    return stream;
}

/**
 * \brief   Serializes message to register scopes in the stream.
 * \param   stream  The streaming object to save header, scope information and
 *                  the {"scope name", "scope priority"} pairs.
 * \param   output  The source of log scopes.
 **/
inline IEOutStream& operator << (IEOutStream& stream, const NETrace::sLogRequestRegisterScopes& output)
{
    stream << output.reqScopeHeader;
    stream << output.reqScopeInfo;
    stream.write(output.reqScopeList, output.reqScopeInfo.dataBufferLen);

    return stream;
}

/**
 * \brief   Deserializes structure to change logging scope priorities to send log messages.
 * \param   stream  The source of {"scope name", "scope priority"} pairs.
 * \param   input   On output this contains structured data.
 *                  The size of the allocated buffer should be enough to save
 *                  header, scope information and the list of scopes.
 **/
inline const IEInStream& operator >> (const IEInStream& stream, NETrace::sLogNotifyLogPriority& input)
{
    stream >> input.notifyScopeHeader;
    stream >> input.notifyScopeInfo;
    stream.read(input.notifyScopeList, input.notifyScopeInfo.dataBufferLen);

    return stream;
}

/**
 * \brief   Serializes message to register scopes in the stream.
 * \param   stream  The streaming object to save header, scope information and
 *                  the {"scope name", "scope priority"} pairs.
 * \param   output  The source of log scopes.
 **/
inline IEOutStream& operator << (IEOutStream& stream, const NETrace::sLogNotifyLogPriority& output)
{
    stream << output.notifyScopeHeader;
    stream << output.notifyScopeInfo;
    stream.write(output.notifyScopeList, output.notifyScopeInfo.dataBufferLen);

    return stream;
}

//////////////////////////////////////////////////////////////////////////////
// NETrace namespace inline methods
//////////////////////////////////////////////////////////////////////////////

inline bool NETrace::isLogMessage(const NETrace::sLogHeader& logHeader)
{
    return (logHeader.hdrLogType == NETrace::eLogType::LogMessage);
}

inline bool NETrace::isLogRequest(const NETrace::sLogHeader& logHeader)
{
    return (logHeader.hdrLogType == NETrace::eLogType::LogRequest);
}

inline bool NETrace::isLogResponse(const NETrace::sLogHeader& logHeader)
{
    return (logHeader.hdrLogType == NETrace::eLogType::LogResponse);
}

inline bool NETrace::isLogNotify(const NETrace::sLogHeader& logHeader)
{
    return (logHeader.hdrLogType == NETrace::eLogType::LogNotify);
}

inline const char* NETrace::getString(NETrace::eLogPriority prio)
{
    switch ( prio )
    {
    case NETrace::eLogPriority::PrioInvalid:
        return "NETrace::PrioInvalid";
    case NETrace::eLogPriority::PrioNotset:
        return "NETrace::PrioNotset";
    case NETrace::eLogPriority::PrioFatal:
        return "NETrace::PrioFatal";
    case NETrace::eLogPriority::PrioScope:
        return "NETrace::PrioScope";
    case NETrace::eLogPriority::PrioError:
        return "NETrace::PrioError";
    case NETrace::eLogPriority::PrioWarning:
        return "NETrace::PrioWarning";
    case NETrace::eLogPriority::PrioInfo:
        return "NETrace::PrioInfo";
    case NETrace::eLogPriority::PrioDebug:
        return "NETrace::PrioDebug";
    case NETrace::eLogPriority::PrioLogs:
        return "NETrace::PrioLogs";
    case NETrace::eLogPriority::PrioValidLogs:
        return "NETrace::PrioValidLogs";
    case NETrace::eLogPriority::PrioIgnore:
        return "NETrace::PrioIgnore";
    case NETrace::eLogPriority::PrioIgnoreLayout:
        return "NETrace::PrioIgnoreLayout";
    case NETrace::eLogPriority::PrioAny:
        return "NETrace::PrioAny";
    case NETrace::eLogPriority::PrioValid:
        return "NETrace::PrioValid";
    default:
        ASSERT(false);
        return "ERR: Unexpected NETrace::eLogPrior value";
    }
}

inline bool NETrace::isValidLogPriority( NETrace::eLogPriority prio )
{
    return (static_cast<unsigned int>(prio) & static_cast<unsigned int>(NETrace::eLogPriority::PrioValid)) != 0;
}

inline bool NETrace::isLogPriority( NETrace::eLogPriority prio )
{
    return (static_cast<unsigned int>(prio) & static_cast<unsigned int>(NETrace::eLogPriority::PrioValidLogs)) != 0;
}

inline bool NETrace::isEnablingLog( NETrace::eLogPriority prio )
{
    return (static_cast<unsigned int>(prio) & static_cast<unsigned int>(NETrace::eLogPriority::PrioLogs)) != 0;
}

inline bool NETrace::isLogScope( NETrace::eLogPriority prio )
{
    return (prio == NETrace::eLogPriority::PrioScope);
}

inline bool NETrace::isDisablingLog( NETrace::eLogPriority prio )
{
    return (prio == NETrace::eLogPriority::PrioNotset) || (prio == NETrace::eLogPriority::PrioInvalid);
}

#endif  // AREG_TRACE_NETRACE_HPP
