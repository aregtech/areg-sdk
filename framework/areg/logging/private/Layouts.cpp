/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \file        areg/logging/private/Layouts.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The collection of layouts
 ************************************************************************/
#include "areg/logging/private/Layouts.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/UtilityDefs.hpp"

#include "areg/logging/private/LogManager.hpp"

#include <utility>

#if AREG_LOGS

namespace areg {

//////////////////////////////////////////////////////////////////////////
// LogLayout interface implementation
//////////////////////////////////////////////////////////////////////////

LogLayout::LogLayout(const areg::LayoutToken layout)
    : mLayout   ( layout )
{
}

//////////////////////////////////////////////////////////////////////////
// TickCountLayout class implementation
//////////////////////////////////////////////////////////////////////////

TickCountLayout::TickCountLayout()
    : LogLayout     ( areg::LayoutToken::TickCount )
{
}

TickCountLayout::TickCountLayout( const TickCountLayout & /*src*/ )
    : LogLayout     ( areg::LayoutToken::TickCount )
{
}

TickCountLayout::TickCountLayout( TickCountLayout && /*src*/ ) noexcept
    : LogLayout     ( areg::LayoutToken::TickCount )
{
}

void TickCountLayout::log_message( const areg::LogEntry & /*msgLog*/, OutStream & stream ) const
{
#ifdef _BIT64
    constexpr char fmt[]{ "%llu" };
#else   // _BIT32
    constexpr char fmt[]{ "%u" };
#endif  // _BIT64

    char buffer[128];
    uint32_t len = static_cast<uint32_t>(String::format_string(buffer, 128, fmt, static_cast<id_type>( DateTime::process_tick_count() )));
    stream.write(reinterpret_cast<const uint8_t *>(buffer), len);
}

//////////////////////////////////////////////////////////////////////////
// DayTimeLayout class declaration
//////////////////////////////////////////////////////////////////////////


DayTimeLayout::DayTimeLayout()
    : LogLayout ( areg::LayoutToken::DayTime )
{
}

DayTimeLayout::DayTimeLayout( const DayTimeLayout & /*src*/ )
    : LogLayout ( areg::LayoutToken::DayTime )
{
}

DayTimeLayout::DayTimeLayout( DayTimeLayout && /*src*/ ) noexcept
    : LogLayout ( areg::LayoutToken::DayTime )
{
}

void DayTimeLayout::log_message( const areg::LogEntry & msgLog, OutStream & stream ) const
{
    if ( msgLog.logTimestamp != 0 )
    {
        String timestamp;
        DateTime::format_time(DateTime(msgLog.logTimestamp), timestamp, areg::TIME_FORMAT_ISO8601_OUTPUT);
        stream.write( reinterpret_cast<const uint8_t *>(timestamp.as_string()), static_cast<uint32_t>(timestamp.length()));
    }
}

//////////////////////////////////////////////////////////////////////////
// ModuleIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ModuleIdLayout::ModuleIdLayout()
    : LogLayout      ( areg::LayoutToken::ExecutableId )
{
}

ModuleIdLayout::ModuleIdLayout(const ModuleIdLayout & /*src*/)
    : LogLayout ( areg::LayoutToken::ExecutableId )
{
}

ModuleIdLayout::ModuleIdLayout( ModuleIdLayout && /*src*/ ) noexcept
    : LogLayout ( areg::LayoutToken::ExecutableId )
{
}

void ModuleIdLayout::log_message( const areg::LogEntry & msgLog, OutStream & stream ) const
{
    static const ITEM_ID _moduleId{ Process::instance().id() };
#ifdef _BIT64
    static const String  _moduleName{ String::make_string(static_cast<uint64_t>(_moduleId), areg::Radix::Hexadecimal) };
#else   // _BIT32
    static const String  _moduleName{ String::make_string(static_cast<uint32_t>(_moduleId), areg::Radix::Hexadecimal) };
#endif  // _BIT64

    if (msgLog.logModuleId != 0)
    {
        if (msgLog.logModuleId == _moduleId)
        {
            stream.write(reinterpret_cast<const uint8_t*>(_moduleName.buffer()), static_cast<uint32_t>(_moduleName.length()));
        }
        else
        {
#ifdef _BIT64
            constexpr char fmt[]{ "0x%llX" };
#else   // _BIT32
            constexpr char fmt[]{ "0x%X" };
#endif  // _BIT64
            char buffer[128];
            uint32_t len = static_cast<uint32_t>(String::format_string(buffer, 128, fmt, static_cast<id_type>(msgLog.logModuleId)));
            stream.write(reinterpret_cast<const uint8_t*>(buffer), len);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// MessageLayout class declaration
//////////////////////////////////////////////////////////////////////////

MessageLayout::MessageLayout()
    : LogLayout ( areg::LayoutToken::Message )
{
}

MessageLayout::MessageLayout(const MessageLayout & /*src*/)
    : LogLayout ( areg::LayoutToken::Message )
{
}

MessageLayout::MessageLayout( MessageLayout && /*src*/ ) noexcept
    : LogLayout ( areg::LayoutToken::Message )
{
}

void MessageLayout::log_message( const areg::LogEntry & msgLog, OutStream & stream ) const
{
    uint32_t count{ static_cast<uint32_t>(string_length<char>(msgLog.logMessage)) };
    stream.write(reinterpret_cast<const uint8_t *>(msgLog.logMessage), count);
}

//////////////////////////////////////////////////////////////////////////
// EndOfLineLayout class declaration
//////////////////////////////////////////////////////////////////////////

EndOfLineLayout::EndOfLineLayout()
    : LogLayout ( areg::LayoutToken::EndOfLine )
{
}

EndOfLineLayout::EndOfLineLayout(const EndOfLineLayout & /*src*/)
    : LogLayout ( areg::LayoutToken::EndOfLine )
{
}

EndOfLineLayout::EndOfLineLayout( EndOfLineLayout && /*src*/ ) noexcept
    : LogLayout ( areg::LayoutToken::EndOfLine )
{
}

void EndOfLineLayout::log_message( const areg::LogEntry & /*msgLog*/, OutStream & stream ) const
{
    stream.write(reinterpret_cast<const uint8_t*>(&areg::EndOfLine), 1);
}

//////////////////////////////////////////////////////////////////////////
// PriorityLayout class declaration
//////////////////////////////////////////////////////////////////////////

PriorityLayout::PriorityLayout()
    : LogLayout ( areg::LayoutToken::Priority )
{
}

PriorityLayout::PriorityLayout(const PriorityLayout & /*src*/)
    : LogLayout ( areg::LayoutToken::Priority )
{
}

PriorityLayout::PriorityLayout( PriorityLayout && /*src*/ ) noexcept
    : LogLayout ( areg::LayoutToken::Priority )
{
}

void PriorityLayout::log_message( const areg::LogEntry & msgLog, OutStream & stream ) const
{
    const String& prio{ areg::priority_to_string(msgLog.logMessagePrio) };
    stream.write(reinterpret_cast<const uint8_t *>(prio.as_string()), static_cast<uint32_t>(prio.length()));
}

//////////////////////////////////////////////////////////////////////////
// ScopeIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ScopeIdLayout::ScopeIdLayout()
    : LogLayout ( areg::LayoutToken::ScopeId )
{
}

ScopeIdLayout::ScopeIdLayout(const ScopeIdLayout & /*src*/)
    : LogLayout ( areg::LayoutToken::ScopeId )
{
}

ScopeIdLayout::ScopeIdLayout( ScopeIdLayout && /*src*/ ) noexcept
    : LogLayout ( areg::LayoutToken::ScopeId )
{
}

void ScopeIdLayout::log_message( const areg::LogEntry & msgLog, OutStream & stream ) const
{
    if ( msgLog.logScopeId != 0 )
    {
        char buffer[128];
        uint32_t len = static_cast<uint32_t>(String::format_string(buffer, 128, "%u", msgLog.logScopeId));
        stream.write( reinterpret_cast<const uint8_t *>(buffer), len );
    }
}

//////////////////////////////////////////////////////////////////////////
// ThreadIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ThreadIdLayout::ThreadIdLayout()
    : LogLayout ( areg::LayoutToken::ThreadId )
{
}

ThreadIdLayout::ThreadIdLayout(const ThreadIdLayout & /*src*/)
    : LogLayout ( areg::LayoutToken::ThreadId )
{
}

ThreadIdLayout::ThreadIdLayout( ThreadIdLayout && /*src*/ ) noexcept
    : LogLayout ( areg::LayoutToken::ThreadId )
{
}

void ThreadIdLayout::log_message( const areg::LogEntry & msgLog, OutStream & stream ) const
{
    if ( msgLog.logThreadId != 0 )
    {
#ifdef _BIT64
        constexpr char fmt[]{ "%06llu" };
#else   // _BIT32
        constexpr char fmt[]{ "%06u" };
#endif  // _BIT64

        char buffer[128];
        uint32_t len = static_cast<uint32_t>(String::format_string(buffer, 128, fmt, static_cast<id_type>(msgLog.logThreadId)));
        stream.write( reinterpret_cast<const uint8_t *>(buffer), len );
    }
}

//////////////////////////////////////////////////////////////////////////
// ModuleNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ModuleNameLayout::ModuleNameLayout()
    : LogLayout ( areg::LayoutToken::ExecutableName )
{
}

ModuleNameLayout::ModuleNameLayout(const ModuleNameLayout & /*src*/)
    : LogLayout ( areg::LayoutToken::ExecutableName )
{
}

ModuleNameLayout::ModuleNameLayout( ModuleNameLayout && /*src*/ ) noexcept
    : LogLayout ( areg::LayoutToken::ExecutableName )
{
}

void ModuleNameLayout::log_message( const areg::LogEntry & msgLog, OutStream & stream ) const
{
    if (msgLog.logDataType == areg::LogDataType::Local)
    {
        static const String& _module{ Process::instance().app_name() };
        stream.write(reinterpret_cast<const uint8_t*>(_module.as_string()), static_cast<uint32_t>(_module.length()));
    }
    else
    {
        if (msgLog.logCookie == areg::COOKIE_LOCAL)
        {
            static const String& _module{ Process::instance().app_name() };
            stream.write(reinterpret_cast<const uint8_t*>(_module.as_string()), static_cast<uint32_t>(_module.length()));
        }
        else if ((msgLog.logCookie != areg::COOKIE_UNKNOWN) && (msgLog.logModuleLen != 0))
        {
            stream.write(reinterpret_cast<const uint8_t*>(msgLog.logModule), msgLog.logModuleLen);
        }
        else
        {
            static constexpr std::string_view _unknown{ "Unknown_Module" };
            stream.write(reinterpret_cast<const uint8_t*>(_unknown.data()), static_cast<uint32_t>(_unknown.length()));
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// ThreadNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ThreadNameLayout::ThreadNameLayout()
    : LogLayout ( areg::LayoutToken::ThreadName )
{
}

ThreadNameLayout::ThreadNameLayout(const ThreadNameLayout & /*src*/)
    : LogLayout ( areg::LayoutToken::ThreadName )
{
}

ThreadNameLayout::ThreadNameLayout( ThreadNameLayout && /*src*/ ) noexcept
    : LogLayout ( areg::LayoutToken::ThreadName )
{
}

void ThreadNameLayout::log_message( const areg::LogEntry & msgLog, OutStream & stream ) const
{
    const char* name{ nullptr };
    uint32_t len{ 0 };

    if (msgLog.logDataType == areg::LogDataType::Local)
    {
        const String& thread{ Thread::thread_name(static_cast<id_type>(msgLog.logThreadId)) };
        name = thread.as_string();
        len  = static_cast<uint32_t>(thread.length());
    }
    else
    {
        name = msgLog.logThread;
        len = msgLog.logThreadLen;
    }

    if (len != 0)
    {
        stream.write(reinterpret_cast<const uint8_t*>(name), len);
    }
    else
    {
        static constexpr std::string_view _unknownThread{ "Unknown_Thread" };
        stream.write(reinterpret_cast<const uint8_t*>(_unknownThread.data()), static_cast<uint32_t>(_unknownThread.length()));
    }
}

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ScopeNameLayout::ScopeNameLayout()
    : LogLayout ( areg::LayoutToken::ScopeName )
{
}

ScopeNameLayout::ScopeNameLayout(const ScopeNameLayout & /*src*/)
    : LogLayout ( areg::LayoutToken::ScopeName )
{
}

ScopeNameLayout::ScopeNameLayout( ScopeNameLayout && /*src*/ ) noexcept
    : LogLayout ( areg::LayoutToken::ScopeName )
{
}

void ScopeNameLayout::log_message( const areg::LogEntry & msgLog, OutStream & stream ) const
{
    stream.write(reinterpret_cast<const uint8_t *>(msgLog.logMessage), msgLog.logMessageLen);
}

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

AnyTextLayout::AnyTextLayout()
    : LogLayout      ( areg::LayoutToken::AnyText )
    , mTextMessage  ( )
{
}

AnyTextLayout::AnyTextLayout(const AnyTextLayout & src)
    : LogLayout      ( areg::LayoutToken::AnyText )
    , mTextMessage  ( src.mTextMessage )
{
}

AnyTextLayout::AnyTextLayout( AnyTextLayout && src ) noexcept
    : LogLayout      ( areg::LayoutToken::AnyText )
    , mTextMessage  ( std::move(src.mTextMessage) )
{
}

AnyTextLayout::AnyTextLayout(const String & anyMessage)
    : LogLayout      ( areg::LayoutToken::AnyText )
    , mTextMessage  ( anyMessage )
{
}

AnyTextLayout::AnyTextLayout(const char * anyMessage)
    : LogLayout      ( areg::LayoutToken::AnyText )
    , mTextMessage  ( anyMessage != nullptr ? anyMessage : areg::EmptyStringA )
{
}

void AnyTextLayout::log_message( const areg::LogEntry & /*msgLog*/, OutStream & stream ) const
{
    stream.write( reinterpret_cast<const uint8_t *>(mTextMessage.as_string()), static_cast<uint32_t>(mTextMessage.length()) );
}

//////////////////////////////////////////////////////////////////////////
// CookieIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

CookieIdLayout::CookieIdLayout()
    : LogLayout(areg::LayoutToken::CookieId)
{
}

CookieIdLayout::CookieIdLayout(const CookieIdLayout& /* src */)
    : LogLayout(areg::LayoutToken::CookieId)
{
}

CookieIdLayout::CookieIdLayout(CookieIdLayout&& /* src */) noexcept
    : LogLayout(areg::LayoutToken::CookieId)
{
}

void CookieIdLayout::log_message(const areg::LogEntry& msgLog, OutStream& stream) const
{
#ifdef _BIT64
    constexpr char fmt[]{ "%03llu" };
#else   // _BIT32
    constexpr char fmt[]{ "%03u" };
#endif  // _BIT64

    char buffer[128];
    uint32_t len = static_cast<uint32_t>(String::format_string(buffer, 128, fmt, static_cast<id_type>(msgLog.logCookie)));
    stream.write(reinterpret_cast<const uint8_t*>(buffer), len);
}

} // namespace areg

#endif  // AREG_LOGS
