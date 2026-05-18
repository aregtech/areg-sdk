#ifndef AREG_LOGGING_PRIVATE_LAYOUTS_HPP
#define AREG_LOGGING_PRIVATE_LAYOUTS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/Layouts.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The collection of layouts
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"
#include "areg/logging/private/LogOptions.hpp"
#if AREG_LOGGING

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class OutStream;
} // namespace areg

/************************************************************************
 * \brief   This file contains declaration of layout objects used
 *          to format output logging message. The layouts are created
 *          during initialization of logging when read configuration file.
 *          The output message formats are specified in configuration file
 *          and the syntax indicates which layout object to create to
 *          output logging message.
 ************************************************************************/
/************************************************************************
 * Declarations and hierarchies.
 ************************************************************************/
namespace areg {
    class LogLayout;
    class TickCountLayout;
    class DayTimeLayout;
    class ModuleIdLayout;
    class MessageLayout;
    class EndOfLineLayout;
    class PriorityLayout;
    class ScopeIdLayout;
    class ThreadIdLayout;
    class ModuleNameLayout;
    class ThreadNameLayout;
    class ScopeNameLayout;
    class AnyTextLayout;
    class CookieIdLayout;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// LogLayout interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The layout base interface to extend by all layout objects.
 **/
class LogLayout
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:

    LogLayout( areg::LayoutToken layout ) noexcept;
    
public:
    virtual ~LogLayout() noexcept = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout-specific formatted text output of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     * \note    Every layout object should override this message to make layout specific outputs.
     **/
    virtual void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the layout type.
     **/
    [[nodiscard]]
    inline constexpr areg::LayoutToken layout_type() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Layout type. Cannot be modified.
     **/
    const areg::LayoutToken  mLayout;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Layout type. Cannot be modified.
     **/
    LogLayout() = delete;
    AREG_NOCOPY_NOMOVE(LogLayout);
};

//////////////////////////////////////////////////////////////////////////
// TickCountLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the tick-count value in milliseconds since process has been started.
 **/
class TickCountLayout final  : public    LogLayout
{
public:
    TickCountLayout() noexcept;

    TickCountLayout( const TickCountLayout & src ) noexcept;

    TickCountLayout( TickCountLayout && src ) noexcept;

    ~TickCountLayout() noexcept override = default;

public:
/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Outputs formatted text of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     **/
    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;
};

//////////////////////////////////////////////////////////////////////////
// DayTimeLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout formats and outputs day-time data in streaming object.
 **/
class DayTimeLayout final : public LogLayout
{
public:
    DayTimeLayout() noexcept;

    DayTimeLayout( const DayTimeLayout & src ) noexcept;

    DayTimeLayout( DayTimeLayout && src ) noexcept;

    ~DayTimeLayout() noexcept override = default;

public:

/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout-specific formatted text output of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     **/
    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;
};

//////////////////////////////////////////////////////////////////////////
// ModuleIdLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs formatted information of running module data.
 **/
class ModuleIdLayout final : public LogLayout
{
public:
    ModuleIdLayout() noexcept;

    ModuleIdLayout( const ModuleIdLayout & src ) noexcept;

    ModuleIdLayout( ModuleIdLayout && src ) noexcept;

    ~ModuleIdLayout() noexcept override = default;

public:

/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;
};

//////////////////////////////////////////////////////////////////////////
// MessageLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs log message to the stream.
 **/
class MessageLayout final : public LogLayout
{
public:
    MessageLayout() noexcept;

    MessageLayout( const MessageLayout & src ) noexcept;

    MessageLayout( MessageLayout && src ) noexcept;

    ~MessageLayout() noexcept override = default;

public:

/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout-specific formatted text output of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     **/
    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;
};

//////////////////////////////////////////////////////////////////////////
// EndOfLineLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout prints end-of-line data at current position of cursor.
 **/
class EndOfLineLayout final : public LogLayout
{
public:
    EndOfLineLayout() noexcept;

    EndOfLineLayout( const EndOfLineLayout & src ) noexcept;

    EndOfLineLayout( EndOfLineLayout && src ) noexcept;

    ~EndOfLineLayout() noexcept override = default;

public:

/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout-specific formatted text output of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     **/
    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;
};

//////////////////////////////////////////////////////////////////////////
// PriorityLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout prints message priority information in the stream.
 **/
class PriorityLayout final : public LogLayout
{
public:
    PriorityLayout() noexcept;

    PriorityLayout( const PriorityLayout & src ) noexcept;

    PriorityLayout( PriorityLayout && src ) noexcept;

    ~PriorityLayout() noexcept override = default;

public:

/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout-specific formatted text output of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     **/
    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;
};

//////////////////////////////////////////////////////////////////////////
// ScopeIdLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the information of scope ID in the streaming object.
 **/
class ScopeIdLayout final : public LogLayout
{
public:
    ScopeIdLayout() noexcept;

    ScopeIdLayout( const ScopeIdLayout & src ) noexcept;

    ScopeIdLayout( ScopeIdLayout && src ) noexcept;

    ~ScopeIdLayout() noexcept override = default;

public:

/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout-specific formatted text output of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     **/
    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;
};

//////////////////////////////////////////////////////////////////////////
// ThreadIdLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs thread ID in the streaming object.
 **/
class ThreadIdLayout final : public LogLayout
{
public:
    ThreadIdLayout() noexcept;

    ThreadIdLayout( const ThreadIdLayout & src ) noexcept;

    ThreadIdLayout( ThreadIdLayout && src ) noexcept;

    ~ThreadIdLayout() noexcept override = default;

public:

/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout-specific formatted text output of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     **/
    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;
};

//////////////////////////////////////////////////////////////////////////
// ModuleNameLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the name of the thread, which logs message.
 **/
class ModuleNameLayout final : public    LogLayout
{
public:
    ModuleNameLayout() noexcept;

    ModuleNameLayout( const ModuleNameLayout & src ) noexcept;

    ModuleNameLayout( ModuleNameLayout && src ) noexcept;

    ~ModuleNameLayout() noexcept override = default;

public:

/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Outputs formatted text of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     **/
    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;
};

//////////////////////////////////////////////////////////////////////////
// ThreadNameLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the name of the thread in output logging message.
 **/
class ThreadNameLayout final : public LogLayout
{
public:
    ThreadNameLayout() noexcept;

    ThreadNameLayout( const ThreadNameLayout & src ) noexcept;

    ThreadNameLayout( ThreadNameLayout && src ) noexcept;

    ~ThreadNameLayout() noexcept override = default;

public:

/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Outputs formatted text of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     **/
    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;
};

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs scope name in the streaming object.
 **/
class ScopeNameLayout final : public LogLayout
{
public:
    ScopeNameLayout() noexcept;

    ScopeNameLayout( const ScopeNameLayout & src ) noexcept;

    ScopeNameLayout( ScopeNameLayout && src ) noexcept;

    ~ScopeNameLayout() noexcept override = default;

public:

/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout-specific formatted text output of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     **/
    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;
};

//////////////////////////////////////////////////////////////////////////
// AnyTextLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs any text message as it is without formatting in the streaming object.
 **/
class AnyTextLayout final : public LogLayout
{
public:
    AnyTextLayout();

    explicit AnyTextLayout( const String & anyMessage );

    explicit AnyTextLayout( const char * anyMessage );

    AnyTextLayout( const AnyTextLayout & src );

    AnyTextLayout( AnyTextLayout && src ) noexcept;

    ~AnyTextLayout() override = default;

public:

/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Outputs formatted text of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     **/
    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;

//////////////////////////////////////////////////////////////////////////
// Member variable
//////////////////////////////////////////////////////////////////////////
private:
    String    mTextMessage; //!< The text message to log
};

//////////////////////////////////////////////////////////////////////////
// CookieIdLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the cookie ID of the log message module.
 **/
class CookieIdLayout final : public LogLayout
{
public:
    CookieIdLayout() noexcept;

    CookieIdLayout( const CookieIdLayout& src ) noexcept;

    CookieIdLayout(CookieIdLayout&& src ) noexcept;

    ~CookieIdLayout() noexcept override = default;

public:

/************************************************************************/
// LogLayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout-specific formatted text output of given message to the streaming object.
     *
     * \param   msgLog      The log message data structure that contains information to output message.
     * \param   stream      The streaming object, where the text message should be written.
     **/
    void log_message( const areg::LogEntry & msgLog, OutStream & stream ) const final;
};

//////////////////////////////////////////////////////////////////////////
// Inline methods
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LogLayout interface inline methods
//////////////////////////////////////////////////////////////////////////

inline constexpr areg::LayoutToken LogLayout::layout_type() const noexcept
{
    return mLayout;
}

} // namespace areg

#endif  // AREG_LOGGING
#endif  // AREG_LOGGING_PRIVATE_LAYOUTS_HPP
