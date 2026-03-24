#ifndef AREG_LOGGING_AREG_LOG_H
#define AREG_LOGGING_AREG_LOG_H
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/areg_log.h
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       areg namespace contains , structures and types.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/ScopeMessage.hpp"

/**
 * \brief   Use these MACRO instead of direct declaration of scopes
 *          and messaging. The MACRO gives possibility to compile application(s)
 *          either with logging, or only with output on Debug output window,
 *          or ignore any logging activity.
 *
 *          AREG_LOGGING is a global preprocessor definition
 *          indicating whether the applications should be compiled with
 *          logging or not. If this is not defined or zero,
 *          logging functionalities will be ignored, unless developer 
 *          is not explicitly setting scopes and manually activating tracing.
 *
 * \example     Start Logging:
 *              In this example, the application configures logging from default configuration file
 *              and start logging, to that the messages can be logged on target.
 *                  ...
 *              LOGGING_START(nullptr);
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
 *              DEF_LOG_SCOPE(my_message_scope, MyFunction);
 *              void MyFunction()
 *              {
 *                  LOG_SCOPE(my_message_scope, MyFunction);
 * 
 *                  LOG_DBG("If debug priority is enabled, this message is logged");
 *                  LOG_INFO("If logging priority is info of debug, this message is logged");
 *                  LOG_WARN("If logging priority is warn, info or debug, this message is logged");
 *                  LOG_ERROR("If logging priority is error, warn, info or debug, this message is logged");
 *                  LOG_FATAL("If logging priority is set, this message is logged");
 *              }
 **/

#if AREG_LOGGING

//////////////////////////////////////////////////////////////////////////
// if AREG_LOGGING is defined, set logging priorities
//////////////////////////////////////////////////////////////////////////

    //!< Priority to log everything
    #define PRIO_LOG_ALL        (static_cast<unsigned int>(areg::LogPriority::PrioDebug)    | static_cast<unsigned int>(areg::LogPriority::PrioScope))

    //!< Priority to log info, warnings, error and fatal
    #define PRIO_INFO           (static_cast<unsigned int>(areg::LogPriority::PrioInfo)     | static_cast<unsigned int>(areg::LogPriority::PrioScope))

    //!< Priority to log warnings, error and fatal
    #define PRIO_WARNING        (static_cast<unsigned int>(areg::LogPriority::PrioWarning)  | static_cast<unsigned int>(areg::LogPriority::PrioScope))

    //!< Priority to log error and fatal
    #define PRIO_ERROR          (static_cast<unsigned int>(areg::LogPriority::PrioError)    | static_cast<unsigned int>(areg::LogPriority::PrioScope))

    //!< Priority to log only fatal
    #define PRIO_FATAL          (static_cast<unsigned int>(areg::LogPriority::PrioFatal)    | static_cast<unsigned int>(areg::LogPriority::PrioScope))

    //!< Unset logging scopes in priorities
    #define PRIO_NOSCOPES(x)    ((x) & (~static_cast<unsigned int>(areg::LogPriority::PrioScope)))

    //!< No logging
    #define PRIO_NOLOGS         (static_cast<unsigned int>(areg::LogPriority::PrioNotset))

//////////////////////////////////////////////////////////////////////////
// if AREG_LOGGING is defined and not zero
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if logging is already configured and started
     **/
    #define IS_LOGGING_STARTED()                            areg::is_started()

    /**
     * \brief   Returns true if logging is enabled
     **/
    #define IS_LOGGING_ENABLED()                            areg::is_enabled()

    /**
     * \brief   Use this macro to load configuration file and start logging.
     *          If config file name is nullptr, it will load from default folder "./config/areg.init"
     **/
    #define LOGGING_START(configFile)                       areg::start_logging((configFile))

    /**
     * \brief   Call to force to start logging.
     **/
    #define LOGGING_FORCE_START()                           areg::force_start_logging()

    /**
     * \brief   Either configures logging values from file or sets default values, enables and starts logging
     **/
    #define LOGGING_CONFIGURE_AND_START(configFile, force)  areg::init_logging((configFile), (force))

    /**
     * \brief   Use this macro to stop logging. This blocks the calling thread until logging thread completes the job.
     **/
    #define LOGGING_STOP()                                  areg::stop_logging( true )

    /**
     * \brief   Defines a log scope variable. The scope string stored at runtime is "path.method".
     *          path uses '_' as node separator; method is a snake_case function name and may
     *          contain '_' without being split into further tree nodes.
     *
     * \param   path    Underscore-separated node path identifying the module and class,
     *                  e.g. areg_component_ServiceManager.
     * \param   method  The snake_case method name, e.g. start_service.
     **/
    #define DEF_LOG_SCOPE(path, method)                     static areg::LogScope _##path##_##method( #path "." #method )

    /**
     * \brief   Creates a ScopeMessage for the named scope. Must appear inside the same method
     *          that matches the DEF_LOG_SCOPE declaration (same path and method identifiers).
     *
     * \param   path    The same path identifier used in DEF_LOG_SCOPE.
     * \param   method  The same method identifier used in DEF_LOG_SCOPE.
     **/
    #define LOG_SCOPE(path, method)                         areg::ScopeMessage  _messager( _##path##_##method )

    /**
     * \brief   Use this macro to log Debug priority messages in logging target (file or remote host)
     **/
    #define LOG_DBG(...)                                do { if (_messager.is_dbg_enabled())   _messager.log_message( areg::LogPriority::PrioDebug    , __VA_ARGS__ ); } while(0)
    /**
     * \brief   Use this macro to log Information priority messages in logging target (file or remote host)
     **/
    #define LOG_INFO(...)                               do { if (_messager.is_info_enabled())  _messager.log_message( areg::LogPriority::PrioInfo     , __VA_ARGS__ ); } while(0)
    /**
     * \brief   Use this macro to log Warning priority messages in logging target (file or remote host)
     **/
    #define LOG_WARN(...)                               do { if (_messager.is_warn_enabled())  _messager.log_message( areg::LogPriority::PrioWarning  , __VA_ARGS__ ); } while(0)
    /**
     * \brief   Use this macro to log Error priority messages in logging target (file or remote host)
     **/
    #define LOG_ERR(...)                                do { if (_messager.is_err_enabled())   _messager.log_message( areg::LogPriority::PrioError    , __VA_ARGS__ ); } while(0)
    /**
     * \brief   Use this macro to log Fatal Error priority messages in logging target (file or remote host)
     **/
    #define LOG_FATAL(...)                              do { if (_messager.is_fatal_enabled()) _messager.log_message( areg::LogPriority::PrioFatal    , __VA_ARGS__ ); } while(0)

    /**
     * \brief   Use this macro to log Debug priority messages in logging target (file or remote host)
     **/
    #define LOG_DBG_IF(cond, ...)                       do { if (_messager.is_dbg_enabled() && (cond))   _messager.log_message( areg::LogPriority::PrioDebug    , __VA_ARGS__ ); } while(0)
    /**
     * \brief   Use this macro to log Information priority messages in logging target (file or remote host)
     **/
    #define LOG_INFO_IF(cond, ...)                      do { if (_messager.is_info_enabled() && (cond))  _messager.log_message( areg::LogPriority::PrioInfo     , __VA_ARGS__ ); } while(0)
    /**
     * \brief   Use this macro to log Warning priority messages in logging target (file or remote host)
     **/
    #define LOG_WARN_IF(cond, ...)                      do { if (_messager.is_warn_enabled() && (cond))  _messager.log_message( areg::LogPriority::PrioWarning  , __VA_ARGS__ ); } while(0)
    /**
     * \brief   Use this macro to log Error priority messages in logging target (file or remote host)
     **/
    #define LOG_ERR_IF(cond, ...)                       do { if (_messager.is_err_enabled() && (cond))   _messager.log_message( areg::LogPriority::PrioError    , __VA_ARGS__ ); } while(0)
    /**
     * \brief   Use this macro to log Fatal Error priority messages in logging target (file or remote host)
     **/
    #define LOG_FATAL_IF(cond, ...)                     do { if (_messager.is_fatal_enabled() && (cond)) _messager.log_message( areg::LogPriority::PrioFatal    , __VA_ARGS__ ); } while(0)
    /**
     * \brief   Defines the global scope and a static accessor function. There can be only one global
     *          scope per source file. The scope string stored at runtime is "path.method".
     *
     * \param   path    Underscore-separated node path identifying the module and class,
     *                  e.g. areg_component_ServiceManager.
     * \param   method  The snake_case method name, e.g. start_service.
     **/
    #define GLOBAL_LOG_SCOPE(path, method)                                                                      \
        static areg::ScopeMessage & _global_scope()                                                             \
        {                                                                                                       \
            static areg::LogScope     _##path##_##method(#path "." #method);                                    \
            static areg::ScopeMessage  _messager(_##path##_##method);                                           \
            static bool is_activated { false };                                                                 \
            if (!is_activated) { areg::activate_scope( _##path##_##method ); is_activated = true; }            \
            return _messager;                                                                                   \
        }

    /**
     * \brief   Use this macro to log Debug priority messages in logging target (file or remote host).
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_DBG(...)                             _global_scope().log_debug( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Information priority messages in logging target (file or remote host)
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_INFO(...)                            _global_scope().log_info( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Warning priority messages in logging target (file or remote host)
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_WARN(...)                            _global_scope().log_warning( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Error priority messages in logging target (file or remote host)
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_ERR(...)                             _global_scope().log_error( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Fatal Error priority messages in logging target (file or remote host)
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_FATAL(...)                           _global_scope().log_fatal( __VA_ARGS__ )

#else   // !AREG_LOGGING

    //////////////////////////////////////////////////////////////////////////
    // if AREG_LOGGING is not defined, disable priorities
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

//////////////////////////////////////////////////////////////////////////
// if AREG_LOG is not defined
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   If !AREG_LOGGING, returns true, makes no effect
     **/
    #define IS_LOGGING_STARTED()                            ((3-2) < 0)

    /**
     * \brief   Returns true if logging is enabled
     **/
    #define IS_LOGGING_ENABLED()                            ((3-2) < 0)

    /**
     * \brief   If !AREG_LOGGING, returns true, makes no effect
     **/
    #define LOGGING_START(configFile)                       ((3-2) < 0)
    /**
     * \brief   If !AREG_LOGGING, returns true, makes no effect
     **/
    #define LOGGING_FORCE_START()                           ((3-2) < 0)
    /**
     * \brief   If !AREG_LOGGING, returns true, makes no effect
     **/
    #define LOGGING_CONFIGURE_AND_START(configFile, force)  ((3-2) < 0)
    /**
     * \brief   If !AREG_LOGGING is zero, does nothing
     **/
    #define LOGGING_STOP()                       

    /**
     * \brief   If !AREG_LOGGING is zero, does nothing, no log scope is declared.
     **/
    #define DEF_LOG_SCOPE(path, method)

    /**
     * \brief   If !AREG_LOGGING is zero, does nothing, no logging message is created.
     **/
    #define LOG_SCOPE(path, method)

    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define LOG_DBG(...)
    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define LOG_INFO(...)
    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define LOG_WARN(...)
    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define LOG_ERR(...)
    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define LOG_FATAL(...)

    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define LOG_DBG_IF(cond, ...)
    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define LOG_INFO_IF(cond, ...)
    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define LOG_WARN_IF(cond, ...)
    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define LOG_ERR_IF(cond, ...)
    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define LOG_FATAL_IF(cond, ...)

    /**
     * \brief   If !AREG_LOGGING, does nothing, no global scope is initialized.
     **/
    #define GLOBAL_LOG_SCOPE(path, method)

    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_DBG(...)
    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_INFO(...)
    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_WARN(...)
    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_ERR(...)
    /**
     * \brief   If !AREG_LOGGING, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_FATAL(...)

#endif  // AREG_LOGGING

#endif  // AREG_LOGGING_AREG_LOG_H
