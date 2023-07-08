/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \file        areg/trace/private/Layouts.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       The collection of layouts
 ************************************************************************/
#include "areg/trace/private/Layouts.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/NEUtilities.hpp"

#include <utility>

//////////////////////////////////////////////////////////////////////////
// IELayout interface implementation
//////////////////////////////////////////////////////////////////////////

IELayout::IELayout(const NELogConfig::eLayouts layout)
    : mLayout   ( layout )
{
}

//////////////////////////////////////////////////////////////////////////
// TickCountLayout class implementation
//////////////////////////////////////////////////////////////////////////

TickCountLayout::TickCountLayout( void )
    : IELayout     ( NELogConfig::eLayouts::LayoutTickCount )
{
}

TickCountLayout::TickCountLayout( const TickCountLayout & /*src*/ )
    : IELayout     ( NELogConfig::eLayouts::LayoutTickCount )
{
}

TickCountLayout::TickCountLayout( TickCountLayout && /*src*/ ) noexcept
    : IELayout     ( NELogConfig::eLayouts::LayoutTickCount )
{
}

void TickCountLayout::logMessage( const NETrace::sLogMessageData & /*msgLog*/, IEOutStream & stream ) const
{
    char buffer[65];    
    String::formatString(buffer, 64, "%llu", static_cast<uint64_t>( DateTime::getProcessTickCount() ));
    stream.write(buffer);
}

//////////////////////////////////////////////////////////////////////////
// DayTimeLaytout class declaration
//////////////////////////////////////////////////////////////////////////


DayTimeLaytout::DayTimeLaytout( void )
    : IELayout ( NELogConfig::eLayouts::LayoutDayTime )
{
}

DayTimeLaytout::DayTimeLaytout( const DayTimeLaytout & /*src*/ )
    : IELayout ( NELogConfig::eLayouts::LayoutDayTime )
{
}

DayTimeLaytout::DayTimeLaytout( DayTimeLaytout && /*src*/ ) noexcept
    : IELayout ( NELogConfig::eLayouts::LayoutDayTime )
{
}

void DayTimeLaytout::logMessage( const NETrace::sLogMessageData & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.dataTimestamp != 0 )
    {
        DateTime timestamp( msgLog.dataTimestamp );
        stream.write( timestamp.formatTime( DateTime::TIME_FORMAT_ISO8601_OUTPUT) );
    }
}

//////////////////////////////////////////////////////////////////////////
// ModuleIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ModuleIdLayout::ModuleIdLayout(void)
    : IELayout ( NELogConfig::eLayouts::LayoutExecutableId )
{
}

ModuleIdLayout::ModuleIdLayout(const ModuleIdLayout & /*src*/)
    : IELayout ( NELogConfig::eLayouts::LayoutExecutableId )
{
}

ModuleIdLayout::ModuleIdLayout( ModuleIdLayout && /*src*/ ) noexcept
    : IELayout ( NELogConfig::eLayouts::LayoutExecutableId )
{
}

void ModuleIdLayout::logMessage( const NETrace::sLogMessageData & msgLog, IEOutStream & stream ) const
{
    if (msgLog.dataModuleId != 0)
    {
        constexpr unsigned int size{ 40 };
        char buffer[size];
        String::formatString(buffer, size, "0x%llX", msgLog.dataModuleId);
        stream.write(buffer);
    }
}

//////////////////////////////////////////////////////////////////////////
// MessageLayout class declaration
//////////////////////////////////////////////////////////////////////////

MessageLayout::MessageLayout(void)
    : IELayout ( NELogConfig::eLayouts::LayoutMessage )
{
}

MessageLayout::MessageLayout(const MessageLayout & /*src*/)
    : IELayout ( NELogConfig::eLayouts::LayoutMessage )
{
}

MessageLayout::MessageLayout( MessageLayout && /*src*/ ) noexcept
    : IELayout ( NELogConfig::eLayouts::LayoutMessage )
{
}

void MessageLayout::logMessage( const NETrace::sLogMessageData & msgLog, IEOutStream & stream ) const
{
    stream.write(msgLog.dataMessage);
}

//////////////////////////////////////////////////////////////////////////
// EndOfLineLayout class declaration
//////////////////////////////////////////////////////////////////////////

EndOfLineLayout::EndOfLineLayout(void)
    : IELayout ( NELogConfig::eLayouts::LayoutEndOfLine )
{
}

EndOfLineLayout::EndOfLineLayout(const EndOfLineLayout & /*src*/)
    : IELayout ( NELogConfig::eLayouts::LayoutEndOfLine )
{
}

EndOfLineLayout::EndOfLineLayout( EndOfLineLayout && /*src*/ ) noexcept
    : IELayout ( NELogConfig::eLayouts::LayoutEndOfLine )
{
}

void EndOfLineLayout::logMessage( const NETrace::sLogMessageData & /*msgLog*/, IEOutStream & stream ) const
{
    constexpr char const END_OF_LINE[] { "\n" };
    stream.write(END_OF_LINE);
}

//////////////////////////////////////////////////////////////////////////
// PriorityLayout class declaration
//////////////////////////////////////////////////////////////////////////

PriorityLayout::PriorityLayout(void)
    : IELayout ( NELogConfig::eLayouts::LayoutPriority )
{
}

PriorityLayout::PriorityLayout(const PriorityLayout & /*src*/)
    : IELayout ( NELogConfig::eLayouts::LayoutPriority )
{
}

PriorityLayout::PriorityLayout( PriorityLayout && /*src*/ ) noexcept
    : IELayout ( NELogConfig::eLayouts::LayoutPriority )
{
}

void PriorityLayout::logMessage( const NETrace::sLogMessageData & msgLog, IEOutStream & stream ) const
{
    stream.write( NETrace::convToString( msgLog.dataMessagePrio ) );
}

//////////////////////////////////////////////////////////////////////////
// ScopeIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ScopeIdLayout::ScopeIdLayout(void)
    : IELayout ( NELogConfig::eLayouts::LaytoutScopeId )
{
}

ScopeIdLayout::ScopeIdLayout(const ScopeIdLayout & /*src*/)
    : IELayout ( NELogConfig::eLayouts::LaytoutScopeId )
{
}

ScopeIdLayout::ScopeIdLayout( ScopeIdLayout && /*src*/ ) noexcept
    : IELayout ( NELogConfig::eLayouts::LaytoutScopeId )
{
}

void ScopeIdLayout::logMessage( const NETrace::sLogMessageData & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.dataScopeId != 0 )
    {
        char buffer[16];
        String::formatString(buffer, 16, "%u", msgLog.dataScopeId);
        stream.write(buffer);
    }
}

//////////////////////////////////////////////////////////////////////////
// ThreadIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ThreadIdLayout::ThreadIdLayout(void)
    : IELayout ( NELogConfig::eLayouts::LayoutThreadId )
{
}

ThreadIdLayout::ThreadIdLayout(const ThreadIdLayout & /*src*/)
    : IELayout ( NELogConfig::eLayouts::LayoutThreadId )
{
}

ThreadIdLayout::ThreadIdLayout( ThreadIdLayout && /*src*/ ) noexcept
    : IELayout ( NELogConfig::eLayouts::LayoutThreadId )
{
}

void ThreadIdLayout::logMessage( const NETrace::sLogMessageData & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.dataThreadId != 0 )
    {
        char buffer[32];

#ifdef _BIT64
        String::formatString(buffer, 32, "0x%016llX", static_cast<uint64_t>(msgLog.dataThreadId));
#else   // _BIT32
        String::formatString(buffer, 32, "0x%08X", static_cast<uint32_t>(msgLog.lmTrace.dataThreadId));
#endif  // _BIT64

        stream.write(buffer);
    }
}

//////////////////////////////////////////////////////////////////////////
// ModuleNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ModuleNameLayout::ModuleNameLayout(void)
    : IELayout ( NELogConfig::eLayouts::LayoutExecutableName )
{
}

ModuleNameLayout::ModuleNameLayout(const ModuleNameLayout & /*src*/)
    : IELayout ( NELogConfig::eLayouts::LayoutExecutableName )
{
}

ModuleNameLayout::ModuleNameLayout( ModuleNameLayout && /*src*/ ) noexcept
    : IELayout ( NELogConfig::eLayouts::LayoutExecutableName )
{
}

void ModuleNameLayout::logMessage( const NETrace::sLogMessageData & /*msgLog*/, IEOutStream & stream ) const
{
    stream.write( Process::getInstance().getAppName() );
}

//////////////////////////////////////////////////////////////////////////
// ThreadNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ThreadNameLayout::ThreadNameLayout(void)
    : IELayout ( NELogConfig::eLayouts::LayoutThreadName )
{
}

ThreadNameLayout::ThreadNameLayout(const ThreadNameLayout & /*src*/)
    : IELayout ( NELogConfig::eLayouts::LayoutThreadName )
{
}

ThreadNameLayout::ThreadNameLayout( ThreadNameLayout && /*src*/ ) noexcept
    : IELayout ( NELogConfig::eLayouts::LayoutThreadName )
{
}

void ThreadNameLayout::logMessage( const NETrace::sLogMessageData & msgLog, IEOutStream & stream ) const
{
    static const String _unknownThread("Unknown Thread");

    id_type id = static_cast<id_type>(msgLog.dataThreadId);
    Thread * thread = Thread::findThreadById( id );
    stream.write( thread != nullptr ? thread->getName() : _unknownThread );
}

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ScopeNameLayout::ScopeNameLayout(void)
    : IELayout ( NELogConfig::eLayouts::LaytoutScopeName )
{
}

ScopeNameLayout::ScopeNameLayout(const ScopeNameLayout & /*src*/)
    : IELayout ( NELogConfig::eLayouts::LaytoutScopeName )
{
}

ScopeNameLayout::ScopeNameLayout( ScopeNameLayout && /*src*/ ) noexcept
    : IELayout ( NELogConfig::eLayouts::LaytoutScopeName )
{
}

void ScopeNameLayout::logMessage( const NETrace::sLogMessageData & msgLog, IEOutStream & stream ) const
{
    stream.write( msgLog.dataMessage );
}

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

AnyTextLayout::AnyTextLayout(void)
    : IELayout      ( NELogConfig::eLayouts::LayoutAnyText )
    , mTextMessage  ( )
{
}

AnyTextLayout::AnyTextLayout(const AnyTextLayout & src)
    : IELayout      ( NELogConfig::eLayouts::LayoutAnyText )
    , mTextMessage  ( src.mTextMessage )
{
}

AnyTextLayout::AnyTextLayout( AnyTextLayout && src ) noexcept
    : IELayout      ( NELogConfig::eLayouts::LayoutAnyText )
    , mTextMessage  ( std::move(src.mTextMessage) )
{
}

AnyTextLayout::AnyTextLayout(const String & anyMessage)
    : IELayout      ( NELogConfig::eLayouts::LayoutAnyText )
    , mTextMessage  ( anyMessage )
{
}

AnyTextLayout::AnyTextLayout(const char * anyMessage)
    : IELayout      ( NELogConfig::eLayouts::LayoutAnyText )
    , mTextMessage  ( anyMessage != nullptr ? anyMessage : NEString::EmptyStringA )
{
}

void AnyTextLayout::logMessage( const NETrace::sLogMessageData & /*msgLog*/, IEOutStream & stream ) const
{
    stream.write( mTextMessage.getString() );
}
