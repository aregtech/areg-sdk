#ifndef AREG_TRACE_GETRACE_H
#define AREG_TRACE_GETRACE_H
/************************************************************************
 * \file        areg/trace/GETrace.h
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       NETrace namespace contains , structures and types.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/NETrace.hpp"
#include "areg/trace/CETraceScope.hpp"
#include "areg/trace/CETraceMessage.hpp"

/**
 * \brief   Use these MACRO instead of direct declaration of scopes
 *          and messaging. The MACRO gives possibility to compile application(s)
 *          either with logging, or only with output on Debug output window,
 *          or ignore any logging activity.
 *
 *          ENABLED_TRACES is a global preprocessor definition
 *          indicating whether the applications should be compiled with
 *          logging or not. If this is not defined or zero,
 *          logging functionalities will be ignored, unless developer 
 *          is not setting scopes and activating tracing manually.
 *
 *          TRACE_DEBUG_OUTPUT is a global preprocessor definition
 *          indicating that the message outputs should be done on Debug
 *          output window.
 *          The TRACE_DEBUG_OUTPUT can be set only for Debug version.
 *          In release version this is reset.
 *
 *          If both preprocessor definitions are not defined or are zero,
 *          the application(s) will be compiled with no logging.
 *          To compile application(s) without logging, it is enough to
 *          set ENABLED_TRACES zero and not define TRACE_DEBUG_OUTPUT
 * 
 * \example     Start Logging:
 *              In this example, the application configures logging from default configuration file
 *              and start logging, to that the messages can be logged on target.
 *                  ...
 *              TRACER_START_LOGGING(NULL);
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

#if defined(ENABLED_TRACES) || defined(_ENABLED_TRACES)

//////////////////////////////////////////////////////////////////////////
// if ENABLED_TRACES is defined and not zero
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if logging is already configured and started
     **/
    #define IS_TRACE_STARTED()                          NETrace::IsStarted()

    /**
     * \brief   Use this macro to load configuration file and start tracer.
     *          If config file name is NULL, it will load from default folder "./config/log.init"
     **/
    #define TRACER_START_LOGGING(configFile)            NETrace::StartLogging((configFile))
    /**
     * \brief   Use this macro to stop logging
     **/
    #define TRACER_STOP_LOGGING()                       NETrace::StopLogging( )

    /**
     * \brief   Use this macro to define scope in source code. This will create scope variable and set name
     **/
    #define DEF_TRACE_SCOPE(scope)                      static CETraceScope _##scope( #scope )

    /**
     * \brief   Use this macro to define message object by passing scope in constructor.
     *          This should be used in the same source file where scope was defined.
     *          The scope object should be defined before it is used.
     **/
    #define TRACE_SCOPE(scope)                          CETraceMessage      _messager( _##scope )

    /**
     * \brief   Use this macro to log Debug priority messages in logging target (file or remote host)
     **/
    #define TRACE_DBG(...)                              _messager.DebugLog( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Information priority messages in logging target (file or remote host)
     **/
    #define TRACE_INFO(...)                             _messager.InfoLog( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Warning priority messages in logging target (file or remote host)
     **/
    #define TRACE_WARN(...)                             _messager.WarningLog( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Error priority messages in logging target (file or remote host)
     **/
    #define TRACE_ERR(...)                              _messager.ErrorLog( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Fatal Error priority messages in logging target (file or remote host)
     **/
    #define TRACE_FATAL(...)                            _messager.FatalLog( __VA_ARGS__ )

    /**
     * \brief   Use this macro to define global scope and global message object.
     *          There can be only one global scope defined in the every single source file.
     *          Call global tracing to use global scope. The global scope is used to make
     *          output generic messages withing single source.
     **/
    #define GLOBAL_TRACE_SCOPE(scope)                   static CETraceMessage & _getGlobalScope( void ) \
                                                        {                                               \
                                                            static CETraceScope     _##scope(#scope);   \
                                                            static CETraceMessage  _messager(_##scope); \
                                                            NETrace::ActivateScope( _##scope );         \
                                                            return _messager;                           \
                                                        }

    /**
     * \brief   Use this macro to log Debug priority messages in logging target (file or remote host).
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_DBG(...)                             _getGlobalScope().DebugLog( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Information priority messages in logging target (file or remote host)
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_INFO(...)                            _getGlobalScope().InfoLog( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Warning priority messages in logging target (file or remote host)
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_WARN(...)                            _getGlobalScope().WarningLog( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Error priority messages in logging target (file or remote host)
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_ERR(...)                             _getGlobalScope().ErrorLog( __VA_ARGS__ )
    /**
     * \brief   Use this macro to log Fatal Error priority messages in logging target (file or remote host)
     *          This macro will use global scope for logging. There can be only one global scope
     *          per source file defined.
     **/
    #define GLOBAL_FATAL(...)                           _getGlobalScope().FatalLog( __VA_ARGS__ )
    /**
     * \brief   Declare variable to use in the trace
     **/
    #define DECLARE_TRACE_VARIABLE(VarType, VarName, VarInit)   VarType VarName   = VarInit

#else   // defined(ENABLED_TRACES) || defined(_ENABLED_TRACES)

//////////////////////////////////////////////////////////////////////////
// if TRACE_DEBUG_OUTPUT and TRACE_DEBUG_OUTPUT are not defined or both are zero
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   If ENABLED_TRACES, returns true, makes no effect
     **/
    #define IS_TRACE_STARTED()                          (true)

    /**
     * \brief   If ENABLED_TRACES, returns true, makes no effect
     **/
    #define TRACER_START_LOGGING(configFile)            (true)
    /**
     * \brief   If ENABLED_TRACES is zero, does nothing
     **/
    #define TRACER_STOP_LOGGING()                       {}

    /**
     * \brief   If ENABLED_TRACES is zero, does nothing, no trace scope is declared.
     **/
    #define DEF_TRACE_SCOPE(scope)

    /**
     * \brief   If ENABLED_TRACES is zero, does nothing, no logging message is created.
     **/
    #define TRACE_SCOPE(scope)

    /**
     * \brief   If ENABLED_TRACES, does nothing, all parameters are ignored.
     **/
    #define TRACE_DBG(...)
    /**
     * \brief   If ENABLED_TRACES, does nothing, all parameters are ignored.
     **/
    #define TRACE_INFO(...)
    /**
     * \brief   If ENABLED_TRACES, does nothing, all parameters are ignored.
     **/
    #define TRACE_WARN(...)
    /**
     * \brief   If ENABLED_TRACES, does nothing, all parameters are ignored.
     **/
    #define TRACE_ERR(...)
    /**
     * \brief   If ENABLED_TRACES, does nothing, all parameters are ignored.
     **/
    #define TRACE_FATAL(...)

    /**
     * \brief   If ENABLED_TRACES, does nothing, no global scope is initialized.
     **/
    #define GLOBAL_TRACE_SCOPE(scope)

    /**
     * \brief   If ENABLED_TRACES, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_DBG(...)
    /**
     * \brief   If ENABLED_TRACES, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_INFO(...)
    /**
     * \brief   If ENABLED_TRACES, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_WARN(...)
    /**
     * \brief   If ENABLED_TRACES, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_ERR(...)
    /**
     * \brief   If ENABLED_TRACES, does nothing, all parameters are ignored.
     **/
    #define GLOBAL_FATAL(...)
    /**
     * \brief   Declare variable to use in the trace
     **/
    #define DECLARE_TRACE_VARIABLE(VarType, VarName, VarInit)

#endif  // (defined(ENABLED_TRACES) && defined(_ENABLED_TRACES))

#endif  // AREG_TRACE_GETRACE_H
