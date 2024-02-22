/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \file        areg/trace/private/Layouts.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The collection of layouts
 ************************************************************************/
#include "areg/trace/private/Layouts.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/NEUtilities.hpp"

#include "areg/trace/private/TraceManager.hpp"

#include <utility>

#if AREG_LOGS

//////////////////////////////////////////////////////////////////////////
// IELayout interface implementation
//////////////////////////////////////////////////////////////////////////

IELayout::IELayout(const NELogging::eLayouts layout)
    : mLayout   ( layout )
{
}

//////////////////////////////////////////////////////////////////////////
// TickCountLayout class implementation
//////////////////////////////////////////////////////////////////////////

TickCountLayout::TickCountLayout( void )
    : IELayout     ( NELogging::eLayouts::LayoutTickCount )
{
}

TickCountLayout::TickCountLayout( const TickCountLayout & /*src*/ )
    : IELayout     ( NELogging::eLayouts::LayoutTickCount )
{
}

TickCountLayout::TickCountLayout( TickCountLayout && /*src*/ ) noexcept
    : IELayout     ( NELogging::eLayouts::LayoutTickCount )
{
}

void TickCountLayout::logMessage( const NETrace::sLogMessage & /*msgLog*/, IEOutStream & stream ) const
{
#ifdef _BIT64
    constexpr char fmt[]{ "%llu" };
#else   // _BIT32
    constexpr char fmt[]{ "%u" };
#endif  // _BIT64

    char buffer[128];
    int len = String::formatString(buffer, 128, fmt, static_cast<id_type>( DateTime::getProcessTickCount() ));
    stream.write(reinterpret_cast<const unsigned char *>(buffer), len > 0 ? len : 0);
}

//////////////////////////////////////////////////////////////////////////
// DayTimeLaytout class declaration
//////////////////////////////////////////////////////////////////////////


DayTimeLaytout::DayTimeLaytout( void )
    : IELayout ( NELogging::eLayouts::LayoutDayTime )
{
}

DayTimeLaytout::DayTimeLaytout( const DayTimeLaytout & /*src*/ )
    : IELayout ( NELogging::eLayouts::LayoutDayTime )
{
}

DayTimeLaytout::DayTimeLaytout( DayTimeLaytout && /*src*/ ) noexcept
    : IELayout ( NELogging::eLayouts::LayoutDayTime )
{
}

void DayTimeLaytout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.logTimestamp != 0 )
    {
        String timestamp;
        DateTime::formatTime(DateTime(msgLog.logTimestamp), timestamp, NEUtilities::TIME_FORMAT_ISO8601_OUTPUT);
        stream.write( reinterpret_cast<const unsigned char *>(timestamp.getString()), timestamp.getLength());
    }
}

//////////////////////////////////////////////////////////////////////////
// ModuleIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ModuleIdLayout::ModuleIdLayout(void)
    : IELayout      ( NELogging::eLayouts::LayoutExecutableId )
{
}

ModuleIdLayout::ModuleIdLayout(const ModuleIdLayout & /*src*/)
    : IELayout ( NELogging::eLayouts::LayoutExecutableId )
{
}

ModuleIdLayout::ModuleIdLayout( ModuleIdLayout && /*src*/ ) noexcept
    : IELayout ( NELogging::eLayouts::LayoutExecutableId )
{
}

void ModuleIdLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    static const ITEM_ID _moduleId{ Process::getInstance().getId() };
#ifdef _BIT64
    static const String  _moduleName{ String::makeString(static_cast<uint64_t>(_moduleId), NEString::eRadix::RadixHexadecimal) };
#else   // _BIT32
    static const String  _moduleName{ String::makeString(static_cast<uint32_t>(_moduleId), NEString::eRadix::RadixHexadecimal) };
#endif  // _BIT64

    if (msgLog.logModuleId != 0)
    {
        if (msgLog.logModuleId == _moduleId)
        {
            stream.write(reinterpret_cast<const unsigned char*>(_moduleName.getBuffer()), _moduleName.getLength());
        }
        else
        {
#ifdef _BIT64
            constexpr char fmt[]{ "0x%llX" };
#else   // _BIT32
            constexpr char fmt[]{ "0x%X" };
#endif  // _BIT64
            char buffer[128];
            int len = String::formatString(buffer, 128, fmt, static_cast<id_type>(msgLog.logModuleId));
            stream.write(reinterpret_cast<const unsigned char*>(buffer), len > 0 ? len : 0);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// MessageLayout class declaration
//////////////////////////////////////////////////////////////////////////

MessageLayout::MessageLayout(void)
    : IELayout ( NELogging::eLayouts::LayoutMessage )
{
}

MessageLayout::MessageLayout(const MessageLayout & /*src*/)
    : IELayout ( NELogging::eLayouts::LayoutMessage )
{
}

MessageLayout::MessageLayout( MessageLayout && /*src*/ ) noexcept
    : IELayout ( NELogging::eLayouts::LayoutMessage )
{
}

void MessageLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    NEString::CharCount count{ NEString::getStringLength<char>(msgLog.logMessage) };
    stream.write(reinterpret_cast<const unsigned char *>(msgLog.logMessage), count);
}

//////////////////////////////////////////////////////////////////////////
// EndOfLineLayout class declaration
//////////////////////////////////////////////////////////////////////////

EndOfLineLayout::EndOfLineLayout(void)
    : IELayout ( NELogging::eLayouts::LayoutEndOfLine )
{
}

EndOfLineLayout::EndOfLineLayout(const EndOfLineLayout & /*src*/)
    : IELayout ( NELogging::eLayouts::LayoutEndOfLine )
{
}

EndOfLineLayout::EndOfLineLayout( EndOfLineLayout && /*src*/ ) noexcept
    : IELayout ( NELogging::eLayouts::LayoutEndOfLine )
{
}

void EndOfLineLayout::logMessage( const NETrace::sLogMessage & /*msgLog*/, IEOutStream & stream ) const
{
    stream.write(reinterpret_cast<const unsigned char*>(&NEString::EndOfLine), 1);
}

//////////////////////////////////////////////////////////////////////////
// PriorityLayout class declaration
//////////////////////////////////////////////////////////////////////////

PriorityLayout::PriorityLayout(void)
    : IELayout ( NELogging::eLayouts::LayoutPriority )
{
}

PriorityLayout::PriorityLayout(const PriorityLayout & /*src*/)
    : IELayout ( NELogging::eLayouts::LayoutPriority )
{
}

PriorityLayout::PriorityLayout( PriorityLayout && /*src*/ ) noexcept
    : IELayout ( NELogging::eLayouts::LayoutPriority )
{
}

void PriorityLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    const String& prio{ NETrace::logPrioToString(msgLog.logMessagePrio) };
    stream.write(reinterpret_cast<const unsigned char *>(prio.getString()), prio.getLength());
}

//////////////////////////////////////////////////////////////////////////
// ScopeIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ScopeIdLayout::ScopeIdLayout(void)
    : IELayout ( NELogging::eLayouts::LaytoutScopeId )
{
}

ScopeIdLayout::ScopeIdLayout(const ScopeIdLayout & /*src*/)
    : IELayout ( NELogging::eLayouts::LaytoutScopeId )
{
}

ScopeIdLayout::ScopeIdLayout( ScopeIdLayout && /*src*/ ) noexcept
    : IELayout ( NELogging::eLayouts::LaytoutScopeId )
{
}

void ScopeIdLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.logScopeId != 0 )
    {
        char buffer[128];
        int len = String::formatString(buffer, 128, "%u", msgLog.logScopeId);
        stream.write( reinterpret_cast<const unsigned char *>(buffer), len > 0 ? len : 0 );
    }
}

//////////////////////////////////////////////////////////////////////////
// ThreadIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ThreadIdLayout::ThreadIdLayout(void)
    : IELayout ( NELogging::eLayouts::LayoutThreadId )
{
}

ThreadIdLayout::ThreadIdLayout(const ThreadIdLayout & /*src*/)
    : IELayout ( NELogging::eLayouts::LayoutThreadId )
{
}

ThreadIdLayout::ThreadIdLayout( ThreadIdLayout && /*src*/ ) noexcept
    : IELayout ( NELogging::eLayouts::LayoutThreadId )
{
}

void ThreadIdLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.logThreadId != 0 )
    {
#ifdef _BIT64
        constexpr char fmt[]{ "%06llu" };
#else   // _BIT32
        constexpr char fmt[]{ "%06u" };
#endif  // _BIT64

        char buffer[128];
        int len = String::formatString(buffer, 128, fmt, static_cast<id_type>(msgLog.logThreadId));
        stream.write( reinterpret_cast<const unsigned char *>(buffer), len > 0 ? len : 0 );
    }
}

//////////////////////////////////////////////////////////////////////////
// ModuleNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ModuleNameLayout::ModuleNameLayout(void)
    : IELayout ( NELogging::eLayouts::LayoutExecutableName )
{
}

ModuleNameLayout::ModuleNameLayout(const ModuleNameLayout & /*src*/)
    : IELayout ( NELogging::eLayouts::LayoutExecutableName )
{
}

ModuleNameLayout::ModuleNameLayout( ModuleNameLayout && /*src*/ ) noexcept
    : IELayout ( NELogging::eLayouts::LayoutExecutableName )
{
}

void ModuleNameLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if (msgLog.logDataType == NETrace::eLogDataType::LogDataLocal)
    {
        static const String& _module{ Process::getInstance().getAppName() };
        stream.write(reinterpret_cast<const unsigned char*>(_module.getString()), _module.getLength());
    }
    else
    {
        if (msgLog.logCookie == NEService::COOKIE_LOCAL)
        {
            static const String& _module{ Process::getInstance().getAppName() };
            stream.write(reinterpret_cast<const unsigned char*>(_module.getString()), _module.getLength());
        }
        else if ((msgLog.logCookie != NEService::COOKIE_UNKNOWN) && (msgLog.logModuleLen != 0))
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

ThreadNameLayout::ThreadNameLayout(void)
    : IELayout ( NELogging::eLayouts::LayoutThreadName )
{
}

ThreadNameLayout::ThreadNameLayout(const ThreadNameLayout & /*src*/)
    : IELayout ( NELogging::eLayouts::LayoutThreadName )
{
}

ThreadNameLayout::ThreadNameLayout( ThreadNameLayout && /*src*/ ) noexcept
    : IELayout ( NELogging::eLayouts::LayoutThreadName )
{
}

void ThreadNameLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    const char* name{ nullptr };
    uint32_t len{ 0 };

    if (msgLog.logDataType == NETrace::eLogDataType::LogDataLocal)
    {
        const String& thread{ Thread::getThreadName(static_cast<id_type>(msgLog.logThreadId)) };
        name = thread.getString();
        len = thread.getLength();
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

ScopeNameLayout::ScopeNameLayout(void)
    : IELayout ( NELogging::eLayouts::LaytoutScopeName )
{
}

ScopeNameLayout::ScopeNameLayout(const ScopeNameLayout & /*src*/)
    : IELayout ( NELogging::eLayouts::LaytoutScopeName )
{
}

ScopeNameLayout::ScopeNameLayout( ScopeNameLayout && /*src*/ ) noexcept
    : IELayout ( NELogging::eLayouts::LaytoutScopeName )
{
}

void ScopeNameLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    stream.write(reinterpret_cast<const unsigned char *>(msgLog.logMessage), msgLog.logMessageLen);
}

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

AnyTextLayout::AnyTextLayout(void)
    : IELayout      ( NELogging::eLayouts::LayoutAnyText )
    , mTextMessage  ( )
{
}

AnyTextLayout::AnyTextLayout(const AnyTextLayout & src)
    : IELayout      ( NELogging::eLayouts::LayoutAnyText )
    , mTextMessage  ( src.mTextMessage )
{
}

AnyTextLayout::AnyTextLayout( AnyTextLayout && src ) noexcept
    : IELayout      ( NELogging::eLayouts::LayoutAnyText )
    , mTextMessage  ( std::move(src.mTextMessage) )
{
}

AnyTextLayout::AnyTextLayout(const String & anyMessage)
    : IELayout      ( NELogging::eLayouts::LayoutAnyText )
    , mTextMessage  ( anyMessage )
{
}

AnyTextLayout::AnyTextLayout(const char * anyMessage)
    : IELayout      ( NELogging::eLayouts::LayoutAnyText )
    , mTextMessage  ( anyMessage != nullptr ? anyMessage : NEString::EmptyStringA )
{
}

void AnyTextLayout::logMessage( const NETrace::sLogMessage & /*msgLog*/, IEOutStream & stream ) const
{
    stream.write( reinterpret_cast<const unsigned char *>(mTextMessage.getString()), mTextMessage.getLength() );
}

//////////////////////////////////////////////////////////////////////////
// CookieIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

CookieIdLayout::CookieIdLayout(void)
    : IELayout(NELogging::eLayouts::LayoutCookieId)
{
}

CookieIdLayout::CookieIdLayout(const CookieIdLayout& src)
    : IELayout(NELogging::eLayouts::LayoutCookieId)
{
}

CookieIdLayout::CookieIdLayout(CookieIdLayout&& src) noexcept
    : IELayout(NELogging::eLayouts::LayoutCookieId)
{
}

void CookieIdLayout::logMessage(const NETrace::sLogMessage& msgLog, IEOutStream& stream) const
{
#ifdef _BIT64
    constexpr char fmt[]{ "%03llu" };
#else   // _BIT32
    constexpr char fmt[]{ "%03u" };
#endif  // _BIT64

    char buffer[128];
    int len = String::formatString(buffer, 128, fmt, static_cast<id_type>(msgLog.logCookie));
    stream.write(reinterpret_cast<const unsigned char*>(buffer), len > 0 ? len : 0);
}

#endif  // AREG_LOGS
