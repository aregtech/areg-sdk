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

//////////////////////////////////////////////////////////////////////////
// LogLayout interface implementation
//////////////////////////////////////////////////////////////////////////

LogLayout::LogLayout(const areg::eLayouts layout)
    : mLayout   ( layout )
{
}

//////////////////////////////////////////////////////////////////////////
// TickCountLayout class implementation
//////////////////////////////////////////////////////////////////////////

TickCountLayout::TickCountLayout()
    : LogLayout     ( areg::eLayouts::LayoutTickCount )
{
}

TickCountLayout::TickCountLayout( const TickCountLayout & /*src*/ )
    : LogLayout     ( areg::eLayouts::LayoutTickCount )
{
}

TickCountLayout::TickCountLayout( TickCountLayout && /*src*/ ) noexcept
    : LogLayout     ( areg::eLayouts::LayoutTickCount )
{
}

void TickCountLayout::logMessage( const areg::sLogMessage & /*msgLog*/, OutStream & stream ) const
{
#ifdef _BIT64
    constexpr char fmt[]{ "%llu" };
#else   // _BIT32
    constexpr char fmt[]{ "%u" };
#endif  // _BIT64

    char buffer[128];
    uint32_t len = static_cast<uint32_t>(String::formatString(buffer, 128, fmt, static_cast<id_type>( DateTime::getProcessTickCount() )));
    stream.write(reinterpret_cast<const unsigned char *>(buffer), len);
}

//////////////////////////////////////////////////////////////////////////
// DayTimeLayout class declaration
//////////////////////////////////////////////////////////////////////////


DayTimeLayout::DayTimeLayout()
    : LogLayout ( areg::eLayouts::LayoutDayTime )
{
}

DayTimeLayout::DayTimeLayout( const DayTimeLayout & /*src*/ )
    : LogLayout ( areg::eLayouts::LayoutDayTime )
{
}

DayTimeLayout::DayTimeLayout( DayTimeLayout && /*src*/ ) noexcept
    : LogLayout ( areg::eLayouts::LayoutDayTime )
{
}

void DayTimeLayout::logMessage( const areg::sLogMessage & msgLog, OutStream & stream ) const
{
    if ( msgLog.logTimestamp != 0 )
    {
        String timestamp;
        DateTime::formatTime(DateTime(msgLog.logTimestamp), timestamp, areg::TIME_FORMAT_ISO8601_OUTPUT);
        stream.write( reinterpret_cast<const unsigned char *>(timestamp.getString()), static_cast<uint32_t>(timestamp.getLength()));
    }
}

//////////////////////////////////////////////////////////////////////////
// ModuleIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ModuleIdLayout::ModuleIdLayout()
    : LogLayout      ( areg::eLayouts::LayoutExecutableId )
{
}

ModuleIdLayout::ModuleIdLayout(const ModuleIdLayout & /*src*/)
    : LogLayout ( areg::eLayouts::LayoutExecutableId )
{
}

ModuleIdLayout::ModuleIdLayout( ModuleIdLayout && /*src*/ ) noexcept
    : LogLayout ( areg::eLayouts::LayoutExecutableId )
{
}

void ModuleIdLayout::logMessage( const areg::sLogMessage & msgLog, OutStream & stream ) const
{
    static const ITEM_ID _moduleId{ Process::getInstance().getId() };
#ifdef _BIT64
    static const String  _moduleName{ String::makeString(static_cast<uint64_t>(_moduleId), areg::eRadix::RadixHexadecimal) };
#else   // _BIT32
    static const String  _moduleName{ String::makeString(static_cast<uint32_t>(_moduleId), areg::eRadix::RadixHexadecimal) };
#endif  // _BIT64

    if (msgLog.logModuleId != 0)
    {
        if (msgLog.logModuleId == _moduleId)
        {
            stream.write(reinterpret_cast<const unsigned char*>(_moduleName.getBuffer()), static_cast<uint32_t>(_moduleName.getLength()));
        }
        else
        {
#ifdef _BIT64
            constexpr char fmt[]{ "0x%llX" };
#else   // _BIT32
            constexpr char fmt[]{ "0x%X" };
#endif  // _BIT64
            char buffer[128];
            uint32_t len = static_cast<uint32_t>(String::formatString(buffer, 128, fmt, static_cast<id_type>(msgLog.logModuleId)));
            stream.write(reinterpret_cast<const unsigned char*>(buffer), len);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// MessageLayout class declaration
//////////////////////////////////////////////////////////////////////////

MessageLayout::MessageLayout()
    : LogLayout ( areg::eLayouts::LayoutMessage )
{
}

MessageLayout::MessageLayout(const MessageLayout & /*src*/)
    : LogLayout ( areg::eLayouts::LayoutMessage )
{
}

MessageLayout::MessageLayout( MessageLayout && /*src*/ ) noexcept
    : LogLayout ( areg::eLayouts::LayoutMessage )
{
}

void MessageLayout::logMessage( const areg::sLogMessage & msgLog, OutStream & stream ) const
{
    uint32_t count{ static_cast<uint32_t>(areg::getStringLength<char>(msgLog.logMessage)) };
    stream.write(reinterpret_cast<const unsigned char *>(msgLog.logMessage), count);
}

//////////////////////////////////////////////////////////////////////////
// EndOfLineLayout class declaration
//////////////////////////////////////////////////////////////////////////

EndOfLineLayout::EndOfLineLayout()
    : LogLayout ( areg::eLayouts::LayoutEndOfLine )
{
}

EndOfLineLayout::EndOfLineLayout(const EndOfLineLayout & /*src*/)
    : LogLayout ( areg::eLayouts::LayoutEndOfLine )
{
}

EndOfLineLayout::EndOfLineLayout( EndOfLineLayout && /*src*/ ) noexcept
    : LogLayout ( areg::eLayouts::LayoutEndOfLine )
{
}

void EndOfLineLayout::logMessage( const areg::sLogMessage & /*msgLog*/, OutStream & stream ) const
{
    stream.write(reinterpret_cast<const unsigned char*>(&areg::EndOfLine), 1);
}

//////////////////////////////////////////////////////////////////////////
// PriorityLayout class declaration
//////////////////////////////////////////////////////////////////////////

PriorityLayout::PriorityLayout()
    : LogLayout ( areg::eLayouts::LayoutPriority )
{
}

PriorityLayout::PriorityLayout(const PriorityLayout & /*src*/)
    : LogLayout ( areg::eLayouts::LayoutPriority )
{
}

PriorityLayout::PriorityLayout( PriorityLayout && /*src*/ ) noexcept
    : LogLayout ( areg::eLayouts::LayoutPriority )
{
}

void PriorityLayout::logMessage( const areg::sLogMessage & msgLog, OutStream & stream ) const
{
    const String& prio{ areg::logPrioToString(msgLog.logMessagePrio) };
    stream.write(reinterpret_cast<const unsigned char *>(prio.getString()), static_cast<uint32_t>(prio.getLength()));
}

//////////////////////////////////////////////////////////////////////////
// ScopeIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ScopeIdLayout::ScopeIdLayout()
    : LogLayout ( areg::eLayouts::LaytoutScopeId )
{
}

ScopeIdLayout::ScopeIdLayout(const ScopeIdLayout & /*src*/)
    : LogLayout ( areg::eLayouts::LaytoutScopeId )
{
}

ScopeIdLayout::ScopeIdLayout( ScopeIdLayout && /*src*/ ) noexcept
    : LogLayout ( areg::eLayouts::LaytoutScopeId )
{
}

void ScopeIdLayout::logMessage( const areg::sLogMessage & msgLog, OutStream & stream ) const
{
    if ( msgLog.logScopeId != 0 )
    {
        char buffer[128];
        uint32_t len = static_cast<uint32_t>(String::formatString(buffer, 128, "%u", msgLog.logScopeId));
        stream.write( reinterpret_cast<const unsigned char *>(buffer), len );
    }
}

//////////////////////////////////////////////////////////////////////////
// ThreadIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ThreadIdLayout::ThreadIdLayout()
    : LogLayout ( areg::eLayouts::LayoutThreadId )
{
}

ThreadIdLayout::ThreadIdLayout(const ThreadIdLayout & /*src*/)
    : LogLayout ( areg::eLayouts::LayoutThreadId )
{
}

ThreadIdLayout::ThreadIdLayout( ThreadIdLayout && /*src*/ ) noexcept
    : LogLayout ( areg::eLayouts::LayoutThreadId )
{
}

void ThreadIdLayout::logMessage( const areg::sLogMessage & msgLog, OutStream & stream ) const
{
    if ( msgLog.logThreadId != 0 )
    {
#ifdef _BIT64
        constexpr char fmt[]{ "%06llu" };
#else   // _BIT32
        constexpr char fmt[]{ "%06u" };
#endif  // _BIT64

        char buffer[128];
        uint32_t len = static_cast<uint32_t>(String::formatString(buffer, 128, fmt, static_cast<id_type>(msgLog.logThreadId)));
        stream.write( reinterpret_cast<const unsigned char *>(buffer), len );
    }
}

//////////////////////////////////////////////////////////////////////////
// ModuleNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ModuleNameLayout::ModuleNameLayout()
    : LogLayout ( areg::eLayouts::LayoutExecutableName )
{
}

ModuleNameLayout::ModuleNameLayout(const ModuleNameLayout & /*src*/)
    : LogLayout ( areg::eLayouts::LayoutExecutableName )
{
}

ModuleNameLayout::ModuleNameLayout( ModuleNameLayout && /*src*/ ) noexcept
    : LogLayout ( areg::eLayouts::LayoutExecutableName )
{
}

void ModuleNameLayout::logMessage( const areg::sLogMessage & msgLog, OutStream & stream ) const
{
    if (msgLog.logDataType == areg::eLogDataType::LogDataLocal)
    {
        static const String& _module{ Process::getInstance().getAppName() };
        stream.write(reinterpret_cast<const unsigned char*>(_module.getString()), static_cast<uint32_t>(_module.getLength()));
    }
    else
    {
        if (msgLog.logCookie == areg::COOKIE_LOCAL)
        {
            static const String& _module{ Process::getInstance().getAppName() };
            stream.write(reinterpret_cast<const unsigned char*>(_module.getString()), static_cast<uint32_t>(_module.getLength()));
        }
        else if ((msgLog.logCookie != areg::COOKIE_UNKNOWN) && (msgLog.logModuleLen != 0))
        {
            stream.write(reinterpret_cast<const unsigned char*>(msgLog.logModule), msgLog.logModuleLen);
        }
        else
        {
            static constexpr std::string_view _unknown{ "Unknown_Module" };
            stream.write(reinterpret_cast<const unsigned char*>(_unknown.data()), static_cast<unsigned int>(_unknown.length()));
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// ThreadNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ThreadNameLayout::ThreadNameLayout()
    : LogLayout ( areg::eLayouts::LayoutThreadName )
{
}

ThreadNameLayout::ThreadNameLayout(const ThreadNameLayout & /*src*/)
    : LogLayout ( areg::eLayouts::LayoutThreadName )
{
}

ThreadNameLayout::ThreadNameLayout( ThreadNameLayout && /*src*/ ) noexcept
    : LogLayout ( areg::eLayouts::LayoutThreadName )
{
}

void ThreadNameLayout::logMessage( const areg::sLogMessage & msgLog, OutStream & stream ) const
{
    const char* name{ nullptr };
    uint32_t len{ 0 };

    if (msgLog.logDataType == areg::eLogDataType::LogDataLocal)
    {
        const String& thread{ Thread::getThreadName(static_cast<id_type>(msgLog.logThreadId)) };
        name = thread.getString();
        len  = static_cast<uint32_t>(thread.getLength());
    }
    else
    {
        name = msgLog.logThread;
        len = msgLog.logThreadLen;
    }

    if (len != 0)
    {
        stream.write(reinterpret_cast<const unsigned char*>(name), len);
    }
    else
    {
        static constexpr std::string_view _unknownThread{ "Unknown_Thread" };
        stream.write(reinterpret_cast<const unsigned char*>(_unknownThread.data()), static_cast<uint32_t>(_unknownThread.length()));
    }
}

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ScopeNameLayout::ScopeNameLayout()
    : LogLayout ( areg::eLayouts::LaytoutScopeName )
{
}

ScopeNameLayout::ScopeNameLayout(const ScopeNameLayout & /*src*/)
    : LogLayout ( areg::eLayouts::LaytoutScopeName )
{
}

ScopeNameLayout::ScopeNameLayout( ScopeNameLayout && /*src*/ ) noexcept
    : LogLayout ( areg::eLayouts::LaytoutScopeName )
{
}

void ScopeNameLayout::logMessage( const areg::sLogMessage & msgLog, OutStream & stream ) const
{
    stream.write(reinterpret_cast<const unsigned char *>(msgLog.logMessage), msgLog.logMessageLen);
}

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

AnyTextLayout::AnyTextLayout()
    : LogLayout      ( areg::eLayouts::LayoutAnyText )
    , mTextMessage  ( )
{
}

AnyTextLayout::AnyTextLayout(const AnyTextLayout & src)
    : LogLayout      ( areg::eLayouts::LayoutAnyText )
    , mTextMessage  ( src.mTextMessage )
{
}

AnyTextLayout::AnyTextLayout( AnyTextLayout && src ) noexcept
    : LogLayout      ( areg::eLayouts::LayoutAnyText )
    , mTextMessage  ( std::move(src.mTextMessage) )
{
}

AnyTextLayout::AnyTextLayout(const String & anyMessage)
    : LogLayout      ( areg::eLayouts::LayoutAnyText )
    , mTextMessage  ( anyMessage )
{
}

AnyTextLayout::AnyTextLayout(const char * anyMessage)
    : LogLayout      ( areg::eLayouts::LayoutAnyText )
    , mTextMessage  ( anyMessage != nullptr ? anyMessage : areg::EmptyStringA )
{
}

void AnyTextLayout::logMessage( const areg::sLogMessage & /*msgLog*/, OutStream & stream ) const
{
    stream.write( reinterpret_cast<const unsigned char *>(mTextMessage.getString()), static_cast<uint32_t>(mTextMessage.getLength()) );
}

//////////////////////////////////////////////////////////////////////////
// CookieIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

CookieIdLayout::CookieIdLayout()
    : LogLayout(areg::eLayouts::LayoutCookieId)
{
}

CookieIdLayout::CookieIdLayout(const CookieIdLayout& /* src */)
    : LogLayout(areg::eLayouts::LayoutCookieId)
{
}

CookieIdLayout::CookieIdLayout(CookieIdLayout&& /* src */) noexcept
    : LogLayout(areg::eLayouts::LayoutCookieId)
{
}

void CookieIdLayout::logMessage(const areg::sLogMessage& msgLog, OutStream& stream) const
{
#ifdef _BIT64
    constexpr char fmt[]{ "%03llu" };
#else   // _BIT32
    constexpr char fmt[]{ "%03u" };
#endif  // _BIT64

    char buffer[128];
    uint32_t len = static_cast<uint32_t>(String::formatString(buffer, 128, fmt, static_cast<id_type>(msgLog.logCookie)));
    stream.write(reinterpret_cast<const unsigned char*>(buffer), len);
}

#endif  // AREG_LOGS
