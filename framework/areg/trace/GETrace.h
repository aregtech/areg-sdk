#ifndef AREG_TRACE_GETRACE_H
#define AREG_TRACE_GETRACE_H
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/GETrace.h
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       NETrace namespace contains , structures and types.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/NETrace.hpp"
#include "areg/trace/TraceScope.hpp"
#include "areg/trace/TraceMessage.hpp"

/**
 * \brief   Use these MACRO instead of direct declaration of scopes
 *          and messaging. The MACRO gives possibility to compile application(s)
 *          either with logging, or only with output on Debug output window,
 *          or ignore any logging activity.
 *
 *          AREG_LOGS is a global preprocessor definition
 *          indicating whether the applications should be compiled with
 *          logging or not. If this is not defined or zero,
 *          logging functionalities will be ignored, unless developer 
 *          is not explicitly setting scopes and manually activating tracing.
 *
 * \example     Start Logging:
 *              In this example, the application configures logging from default configuration file
 *              and start logging, to that the messages can be logged on target.
 *                  ...
 *              TRACER_START_LOGGING(nullptr);
 * 
 * \example     Logging:
 *              In this example, the application created scope, which are used to start output messages.
 *              Depending on scope enabled priority, the appropriate message is logged.
 *              If scope priority is DEBUG, all message are logged.
 *              If scope enabled to output enter and exit, before any message is logged,
 *              the scope 'enter' message is logged on output target and when function is
 *              executed, at the end scope 'exit' message is logged.
 *              The scope should be declared before it is used. It can be used just before
 *              method where it is used. The scope should be declared only once, but can be
 *              used in more than one method.
 *                  ...
 *              DEF_TRACE_SCOPE(my_message_scope_in_MyFunction);
 *              void MyFunction( void )
 *              {
 *                  TRACE_SCOPE(my_message_scope_in_MyFunction);
 * 
 *                  TRACE_DBG("If debug priority is enabled, this message is logged");
 *                  TRACE_INFO("If logging priority is info of debug, this message is logged");
 *                  TRACE_WARN("If logging priority is warn, info or debug, this message is logged");
 *                  TRACE_ERROR("If logging priority is error, warn, info or debug, this message is logged");
 *                  TRACE_FATAL("If logging priority is set, this message is logged");
 *              }
 **/

#if AREG_LOGS

//////////////////////////////////////////////////////////////////////////
// if AREG_LOGS is defined, set logging priorities
//////////////////////////////////////////////////////////////////////////

    //!< Priority to log everything
    #define PRIO_LOG_ALL        (static_cast<unsigned int>(NETrace::eLogPriority::PrioDebug)    | static_cast<unsigned int>(NETrace::eLogPriority::PrioScope))

    //!< Priority to log info, warnings, error and fatal
    #define PRIO_INFO           (static_cast<unsigned int>(NETrace::eLogPriority::PrioInfo)     | static_cast<unsigned int>(NETrace::eLogPriority::PrioScope))

    //!< Priority to log warnings, error and fatal
    #define PRIO_WARNING        (static_cast<unsigned int>(NETrace::eLogPriority::PrioWarning)  | static_cast<unsigned int>(NETrace::eLogPriority::PrioScope))

    //!< Priority to log error and fatal
    #define PRIO_ERROR          (static_cast<unsigned int>(NETrace::eLogPriority::PrioError)    | static_cast<unsigned int>(NETrace::eLogPriority::PrioScope))

    //!< Priority to log only fatal
    #define PRIO_FATAL          (static_cast<unsigned int>(NETrace::eLogPriority::PrioFatal)    | static_cast<unsigned int>(NETrace::eLogPriority::PrioScope))

    //!< Unset logging scopes in priorities
    #define PRIO_NOSCOPES(x)    ((x) & (~static_cast<unsigned int>(NETrace::eLogPriority::PrioScope)))

    //!< No logging
    #define PRIO_NOLOGS         (static_cast<unsigned int>(NETrace::eLogPriority::PrioNotset))

    /**
     * \brief   Change scope log priority during runtime.
     *          Pass scope set in DEF_TRACE_SCOPE and priority value with OR operation specified in NETrace::eLogPriority.
     **/
    #define SCOPE_PRIORITY_CHANGE(scope, prio)          NETrace::setScopePriority(#scope, static_cast<unsigned int>(prio))

    /**
     * \brief   Get the actual priority of the scope. Pass scope set in DEF_TRACE_SCOPE.
     **/
    #define SCOPE_PRIORITY_GET(scope)                   NETrace::getScopePriority(#scope)

//////////////////////////////////////////////////////////////////////////
// if AREG_LOGS is defined and not zero
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if logging is already configured and started
     **/
    #define IS_TRACE_STARTED()                          NETrace::isStarted()

    /**
     * \brief   Returns true if logging is enabled
     **/
    #define IS_LOG_ENABLED()                            NETrace::isEnabled()

    /**
     * \brief   Use this macro to load configuration file and start tracer.
     *          If config file name is nullptr, it will load from default folder "./config/areg.init"
     **/
    #define TRACER_START_LOGGING(configFile)            NETrace::startLogging((configFile))

    /**
     * \brief   Call to force to start logging.
     **/
    #define TRACER_FORCE_LOGGING()                      NETrace::forceStartLogging()

    /**
     * \brief   Either configures logging values from file or sets default values, enables and starts logging
     **/
    #define TRACER_CONFIGURE_AND_START(configFile)      NETrace::initAndStartLogging((configFile))

    /**
     * \brief   Use this macro to stop logging. This blocks the calling thread until logging thread completes the job.
     **/
    #define TRACER_STOP_LOGGING()                       NETrace::stopLogging( true )

    /**
     * \brief   Use this macro to define scope in source code. This will create scope variable and set name
     **/
    #define DEF_TRACE_SCOPE(scope)                      static TraceScope _##scope( #scope )

    /**
     * \brief   Use this macro to define message object by passing scope in constructor.
     *          This should be used in the same source file where scope was defined.
     *          The scope object should be defined before it is used.
     **/
    #define TRACE_SCOPE(scope)                          TraceMessage      _messager( _##scope )

    /**
     * \brief   Use this macro to log Debug priority messages in logging target (file or remote host)
     **/
    #define TRACE_DBG(...)                              if (_messager.isDbgEnabled())   _messager.logMessage( NETrace::PrioDebug    , __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Information priority messages in logging target (file or remote host)
     **/
    #define TRACE_INFO(...)                             if (_messager.isInfoEnabled())  _messager.logMessage( NETrace::PrioInfo     , __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Warning priority messages in logging target (file or remote host)
     **/
    #define TRACE_WARN(...)                             if (_messager.isWarnEnabled())  _messager.logMessage( NETrace::PrioWarning  , __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Error priority messages in logging target (file or remote host)
     **/
    #define TRACE_ERR(...)                              if (_messager.isErrEnabled())   _messager.logMessage( NETrace::PrioError    , __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Fatal Error priority messages in logging target (file or remote host)
     **/
    #define TRACE_FATAL(...)                            if (_messager.isFatalEnabled()) _messager.logMessage( NETrace::PrioFatal    , __VA_ARGS__ )

    /**
     * \brief   Use this macro to define global scope and global message object.
     *          There can be only one global scope defined in the every single source file.
     *          Call global tracing to use global scope. The global scope is used to make
     *          output generic messages withing single source.
     **/
    #define GLOBAL_TRACE_SCOPE(scope)                   static TraceMessage & _getGlobalScope( void )   \
                                                        {                                               \
                                                            static TraceScope     _##scope(#scope);     \
                                                            static TraceMessage  _messager(_##scope);   \
                                                            static bool isActivated { false };          \
                                                            if (isActivated == false)                   \
                                                            {                                           \
                                                                NETrace::activateScope( _##scope );     \
                                                                isActivated = true;                     \
                                                            }                                           \
                                                            return _messager;                           \
                                                        }

    /**
     * \brief   Use this macro to log Debug priority messages in logging target (file or remote host).
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_DBG(...)                             _getGlobalScope().logDebug( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Information priority messages in logging target (file or remote host)
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_INFO(...)                            _getGlobalScope().logInfo( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Warning priority messages in logging target (file or remote host)
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_WARN(...)                            _getGlobalScope().logWarning( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Error priority messages in logging target (file or remote host)
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_ERR(...)                             _getGlobalScope().logError( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Fatal Error priority messages in logging target (file or remote host)
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_FATAL(...)                           _getGlobalScope().logFatal( __VA_ARGS__ )

#else   // !AREG_LOGS

    //////////////////////////////////////////////////////////////////////////
    // if AREG_LOGS is not defined, disable priorities
    //////////////////////////////////////////////////////////////////////////

        //!< Priority to log everything
    #define PRIO_LOG_ALL        0

    //!< Priority to log info, warnings, error and fatal
    #define PRIO_INFO           0

    //!< Priority to log warnings, error and fatal
    #define PRIO_WARNING        0

    //!< Priority to log error and fatal
    #define PRIO_ERROR          0

    //!< Priority to log only fatal
    #define PRIO_FATAL          0

    //!< Unset logging scopes in priorities
    #define PRIO_NOSCOPES(x)    0

    //!< No logging
    #define PRIO_NOLOGS         0

    /**
     * \brief   Always returns true
     **/
    #define SCOPE_PRIORITY_CHANGE(scope, prio)          ((3-2) > 0)

    /**
     * \brief   Always returns 'NETrace::eLogPriority::PrioInvalid'
     **/
    #define SCOPE_PRIORITY_GET(scope)                   static_cast<unsigned int>(NETrace::eLogPriority::PrioInvalid)

//////////////////////////////////////////////////////////////////////////
// if AREG_LOG is not defined
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   If !AREG_LOGS, returns true, makes no effect
     **/
    #define IS_TRACE_STARTED()                          ((3-2) < 0)

    /**
     * \brief   Returns true if logging is enabled
     **/
    #define IS_LOG_ENABLED()                            ((3-2) < 0)

    /**
     * \brief   If !AREG_LOGS, returns true, makes no effect
     **/
    #define TRACER_START_LOGGING(configFile)            ((3-2) < 0)
    /**
     * \brief   If !AREG_LOGS, returns true, makes no effect
     **/
    #define TRACER_FORCE_LOGGING()                      ((3-2) < 0)
    /**
     * \brief   If !AREG_LOGS, returns true, makes no effect
     **/
    #define TRACER_CONFIGURE_AND_START(configFile)      ((3-2) < 0)
    /**
     * \brief   If !AREG_LOGS is zero, does nothing
     **/
    #define TRACER_STOP_LOGGING()                       {}

    /**
     * \brief   If !AREG_LOGS is zero, does nothing, no trace scope is declared.
     **/
    #define DEF_TRACE_SCOPE(scope)

    /**
     * \brief   If !AREG_LOGS is zero, does nothing, no logging message is created.
     **/
    #define TRACE_SCOPE(scope)

    /**
     * \brief   If !AREG_LOGS, does nothing, all parameters are ignored.
     **/
    #define TRACE_DBG(...)
    /**
     * \brief   If !AREG_LOGS, does nothing, all parameters are ignored.
     **/
    #define TRACE_INFO(...)
    /**
     * \brief   If !AREG_LOGS, does nothing, all parameters are ignored.
     **/
    #define TRACE_WARN(...)
    /**
     * \brief   If !AREG_LOGS, does nothing, all parameters are ignored.
     **/
    #define TRACE_ERR(...)
    /**
     * \brief   If !AREG_LOGS, does nothing, all parameters are ignored.
     **/
    #define TRACE_FATAL(...)

    /**
     * \brief   If !AREG_LOGS, does nothing, no global scope is initialized.
     **/
    #define GLOBAL_TRACE_SCOPE(scope)

    /**
     * \brief   If !AREG_LOGS, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_DBG(...)
    /**
     * \brief   If !AREG_LOGS, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_INFO(...)
    /**
     * \brief   If !AREG_LOGS, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_WARN(...)
    /**
     * \brief   If !AREG_LOGS, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_ERR(...)
    /**
     * \brief   If !AREG_LOGS, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_FATAL(...)

#endif  // AREG_LOGS

#endif  // AREG_TRACE_GETRACE_H
