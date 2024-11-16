/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \file        areg/logging/private/Layouts.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The collection of layouts
 ************************************************************************/
#include "areg/logging/private/Layouts.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/NEUtilities.hpp"

#include "areg/logging/private/LogManager.hpp"

#include <utility>

#if AREG_LOGS

//////////////////////////////////////////////////////////////////////////
// IELayout interface implementation
//////////////////////////////////////////////////////////////////////////

IELayout::IELayout(const NELogOptions::eLayouts layout)
    : mLayout   ( layout )
{
}

//////////////////////////////////////////////////////////////////////////
// TickCountLayout class implementation
//////////////////////////////////////////////////////////////////////////

TickCountLayout::TickCountLayout( void )
    : IELayout     ( NELogOptions::eLayouts::LayoutTickCount )
{
}

TickCountLayout::TickCountLayout( const TickCountLayout & /*src*/ )
    : IELayout     ( NELogOptions::eLayouts::LayoutTickCount )
{
}

TickCountLayout::TickCountLayout( TickCountLayout && /*src*/ ) noexcept
    : IELayout     ( NELogOptions::eLayouts::LayoutTickCount )
{
}

void TickCountLayout::logMessage( const NELogging::sLogMessage & /*msgLog*/, IEOutStream & stream ) const
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
// DayTimeLaytout class declaration
//////////////////////////////////////////////////////////////////////////


DayTimeLaytout::DayTimeLaytout( void )
    : IELayout ( NELogOptions::eLayouts::LayoutDayTime )
{
}

DayTimeLaytout::DayTimeLaytout( const DayTimeLaytout & /*src*/ )
    : IELayout ( NELogOptions::eLayouts::LayoutDayTime )
{
}

DayTimeLaytout::DayTimeLaytout( DayTimeLaytout && /*src*/ ) noexcept
    : IELayout ( NELogOptions::eLayouts::LayoutDayTime )
{
}

void DayTimeLaytout::logMessage( const NELogging::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.logTimestamp != 0 )
    {
        String timestamp;
        DateTime::formatTime(DateTime(msgLog.logTimestamp), timestamp, NEUtilities::TIME_FORMAT_ISO8601_OUTPUT);
        stream.write( reinterpret_cast<const unsigned char *>(timestamp.getString()), static_cast<uint32_t>(timestamp.getLength()));
    }
}

//////////////////////////////////////////////////////////////////////////
// ModuleIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ModuleIdLayout::ModuleIdLayout(void)
    : IELayout      ( NELogOptions::eLayouts::LayoutExecutableId )
{
}

ModuleIdLayout::ModuleIdLayout(const ModuleIdLayout & /*src*/)
    : IELayout ( NELogOptions::eLayouts::LayoutExecutableId )
{
}

ModuleIdLayout::ModuleIdLayout( ModuleIdLayout && /*src*/ ) noexcept
    : IELayout ( NELogOptions::eLayouts::LayoutExecutableId )
{
}

void ModuleIdLayout::logMessage( const NELogging::sLogMessage & msgLog, IEOutStream & stream ) const
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

MessageLayout::MessageLayout(void)
    : IELayout ( NELogOptions::eLayouts::LayoutMessage )
{
}

MessageLayout::MessageLayout(const MessageLayout & /*src*/)
    : IELayout ( NELogOptions::eLayouts::LayoutMessage )
{
}

MessageLayout::MessageLayout( MessageLayout && /*src*/ ) noexcept
    : IELayout ( NELogOptions::eLayouts::LayoutMessage )
{
}

void MessageLayout::logMessage( const NELogging::sLogMessage & msgLog, IEOutStream & stream ) const
{
    uint32_t count{ static_cast<uint32_t>(NEString::getStringLength<char>(msgLog.logMessage)) };
    stream.write(reinterpret_cast<const unsigned char *>(msgLog.logMessage), count);
}

//////////////////////////////////////////////////////////////////////////
// EndOfLineLayout class declaration
//////////////////////////////////////////////////////////////////////////

EndOfLineLayout::EndOfLineLayout(void)
    : IELayout ( NELogOptions::eLayouts::LayoutEndOfLine )
{
}

EndOfLineLayout::EndOfLineLayout(const EndOfLineLayout & /*src*/)
    : IELayout ( NELogOptions::eLayouts::LayoutEndOfLine )
{
}

EndOfLineLayout::EndOfLineLayout( EndOfLineLayout && /*src*/ ) noexcept
    : IELayout ( NELogOptions::eLayouts::LayoutEndOfLine )
{
}

void EndOfLineLayout::logMessage( const NELogging::sLogMessage & /*msgLog*/, IEOutStream & stream ) const
{
    stream.write(reinterpret_cast<const unsigned char*>(&NEString::EndOfLine), 1);
}

//////////////////////////////////////////////////////////////////////////
// PriorityLayout class declaration
//////////////////////////////////////////////////////////////////////////

PriorityLayout::PriorityLayout(void)
    : IELayout ( NELogOptions::eLayouts::LayoutPriority )
{
}

PriorityLayout::PriorityLayout(const PriorityLayout & /*src*/)
    : IELayout ( NELogOptions::eLayouts::LayoutPriority )
{
}

PriorityLayout::PriorityLayout( PriorityLayout && /*src*/ ) noexcept
    : IELayout ( NELogOptions::eLayouts::LayoutPriority )
{
}

void PriorityLayout::logMessage( const NELogging::sLogMessage & msgLog, IEOutStream & stream ) const
{
    const String& prio{ NELogging::logPrioToString(msgLog.logMessagePrio) };
    stream.write(reinterpret_cast<const unsigned char *>(prio.getString()), static_cast<uint32_t>(prio.getLength()));
}

//////////////////////////////////////////////////////////////////////////
// ScopeIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ScopeIdLayout::ScopeIdLayout(void)
    : IELayout ( NELogOptions::eLayouts::LaytoutScopeId )
{
}

ScopeIdLayout::ScopeIdLayout(const ScopeIdLayout & /*src*/)
    : IELayout ( NELogOptions::eLayouts::LaytoutScopeId )
{
}

ScopeIdLayout::ScopeIdLayout( ScopeIdLayout && /*src*/ ) noexcept
    : IELayout ( NELogOptions::eLayouts::LaytoutScopeId )
{
}

void ScopeIdLayout::logMessage( const NELogging::sLogMessage & msgLog, IEOutStream & stream ) const
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

ThreadIdLayout::ThreadIdLayout(void)
    : IELayout ( NELogOptions::eLayouts::LayoutThreadId )
{
}

ThreadIdLayout::ThreadIdLayout(const ThreadIdLayout & /*src*/)
    : IELayout ( NELogOptions::eLayouts::LayoutThreadId )
{
}

ThreadIdLayout::ThreadIdLayout( ThreadIdLayout && /*src*/ ) noexcept
    : IELayout ( NELogOptions::eLayouts::LayoutThreadId )
{
}

void ThreadIdLayout::logMessage( const NELogging::sLogMessage & msgLog, IEOutStream & stream ) const
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

ModuleNameLayout::ModuleNameLayout(void)
    : IELayout ( NELogOptions::eLayouts::LayoutExecutableName )
{
}

ModuleNameLayout::ModuleNameLayout(const ModuleNameLayout & /*src*/)
    : IELayout ( NELogOptions::eLayouts::LayoutExecutableName )
{
}

ModuleNameLayout::ModuleNameLayout( ModuleNameLayout && /*src*/ ) noexcept
    : IELayout ( NELogOptions::eLayouts::LayoutExecutableName )
{
}

void ModuleNameLayout::logMessage( const NELogging::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if (msgLog.logDataType == NELogging::eLogDataType::LogDataLocal)
    {
        static const String& _module{ Process::getInstance().getAppName() };
        stream.write(reinterpret_cast<const unsigned char*>(_module.getString()), static_cast<uint32_t>(_module.getLength()));
    }
    else
    {
        if (msgLog.logCookie == NEService::COOKIE_LOCAL)
        {
            static const String& _module{ Process::getInstance().getAppName() };
            stream.write(reinterpret_cast<const unsigned char*>(_module.getString()), static_cast<uint32_t>(_module.getLength()));
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
    : IELayout ( NELogOptions::eLayouts::LayoutThreadName )
{
}

ThreadNameLayout::ThreadNameLayout(const ThreadNameLayout & /*src*/)
    : IELayout ( NELogOptions::eLayouts::LayoutThreadName )
{
}

ThreadNameLayout::ThreadNameLayout( ThreadNameLayout && /*src*/ ) noexcept
    : IELayout ( NELogOptions::eLayouts::LayoutThreadName )
{
}

void ThreadNameLayout::logMessage( const NELogging::sLogMessage & msgLog, IEOutStream & stream ) const
{
    const char* name{ nullptr };
    uint32_t len{ 0 };

    if (msgLog.logDataType == NELogging::eLogDataType::LogDataLocal)
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

ScopeNameLayout::ScopeNameLayout(void)
    : IELayout ( NELogOptions::eLayouts::LaytoutScopeName )
{
}

ScopeNameLayout::ScopeNameLayout(const ScopeNameLayout & /*src*/)
    : IELayout ( NELogOptions::eLayouts::LaytoutScopeName )
{
}

ScopeNameLayout::ScopeNameLayout( ScopeNameLayout && /*src*/ ) noexcept
    : IELayout ( NELogOptions::eLayouts::LaytoutScopeName )
{
}

void ScopeNameLayout::logMessage( const NELogging::sLogMessage & msgLog, IEOutStream & stream ) const
{
    stream.write(reinterpret_cast<const unsigned char *>(msgLog.logMessage), msgLog.logMessageLen);
}

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

AnyTextLayout::AnyTextLayout(void)
    : IELayout      ( NELogOptions::eLayouts::LayoutAnyText )
    , mTextMessage  ( )
{
}

AnyTextLayout::AnyTextLayout(const AnyTextLayout & src)
    : IELayout      ( NELogOptions::eLayouts::LayoutAnyText )
    , mTextMessage  ( src.mTextMessage )
{
}

AnyTextLayout::AnyTextLayout( AnyTextLayout && src ) noexcept
    : IELayout      ( NELogOptions::eLayouts::LayoutAnyText )
    , mTextMessage  ( std::move(src.mTextMessage) )
{
}

AnyTextLayout::AnyTextLayout(const String & anyMessage)
    : IELayout      ( NELogOptions::eLayouts::LayoutAnyText )
    , mTextMessage  ( anyMessage )
{
}

AnyTextLayout::AnyTextLayout(const char * anyMessage)
    : IELayout      ( NELogOptions::eLayouts::LayoutAnyText )
    , mTextMessage  ( anyMessage != nullptr ? anyMessage : NEString::EmptyStringA )
{
}

void AnyTextLayout::logMessage( const NELogging::sLogMessage & /*msgLog*/, IEOutStream & stream ) const
{
    stream.write( reinterpret_cast<const unsigned char *>(mTextMessage.getString()), static_cast<uint32_t>(mTextMessage.getLength()) );
}

//////////////////////////////////////////////////////////////////////////
// CookieIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

CookieIdLayout::CookieIdLayout(void)
    : IELayout(NELogOptions::eLayouts::LayoutCookieId)
{
}

CookieIdLayout::CookieIdLayout(const CookieIdLayout& /* src */)
    : IELayout(NELogOptions::eLayouts::LayoutCookieId)
{
}

CookieIdLayout::CookieIdLayout(CookieIdLayout&& /* src */) noexcept
    : IELayout(NELogOptions::eLayouts::LayoutCookieId)
{
}

void CookieIdLayout::logMessage(const NELogging::sLogMessage& msgLog, IEOutStream& stream) const
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
