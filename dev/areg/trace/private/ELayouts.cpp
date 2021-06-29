/************************************************************************
 * \file        areg/trace/private/ELayouts.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The collection of layouts
 ************************************************************************/
#include "areg/trace/private/ELayouts.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// IELayout interface implementation
//////////////////////////////////////////////////////////////////////////

IELayout::IELayout(const NELogConfig::eLayouts layout)
    : mLayout   ( layout )
{
    ; // do nothing
}

IELayout::~IELayout(void)
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TickCountLayout class implementation
//////////////////////////////////////////////////////////////////////////

TickCountLayout::TickCountLayout( void )
    : IELayout     ( NELogConfig::LayoutTickCount )
{
    ; // do nothing
}

TickCountLayout::TickCountLayout( const TickCountLayout & /*src*/ )
    : IELayout     ( NELogConfig::LayoutTickCount )
{
    ; // do nothing
}

TickCountLayout::~TickCountLayout(void)
{
    ; // do nothing
}

void TickCountLayout::logMessage( const NETrace::sLogMessage & /*msgLog*/, IEOutStream & stream ) const
{
    char buffer[65];    
    String::formatString(buffer, 64, "%llu", static_cast<unsigned int>( DateTime::getProcessTickCount() ));
    stream.write(buffer);
}

//////////////////////////////////////////////////////////////////////////
// DayTimeLaytout class declaration
//////////////////////////////////////////////////////////////////////////


DayTimeLaytout::DayTimeLaytout( void )
    : IELayout ( NELogConfig::LayoutDayTime )
{
    ; // do nothing
}

DayTimeLaytout::DayTimeLaytout( const DayTimeLaytout & /*src*/ )
    : IELayout ( NELogConfig::LayoutDayTime )
{
    ; // do nothing
}

DayTimeLaytout::~DayTimeLaytout(void)
{
    ; // do nothing
}

void DayTimeLaytout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.lmTrace.traceTimestamp != 0 )
    {
        DateTime timestamp( msgLog.lmTrace.traceTimestamp );
        stream.write( timestamp.formatTime( DateTime::TIME_FORMAT_ISO8601_OUTPUT) );
    }
}

//////////////////////////////////////////////////////////////////////////
// ModuleIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ModuleIdLayout::ModuleIdLayout(void)
    : IELayout ( NELogConfig::LayoutExecutableId )
{
    ; // do nothing
}

ModuleIdLayout::ModuleIdLayout(const ModuleIdLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutExecutableId )
{
    ; // do nothing
}

ModuleIdLayout::~ModuleIdLayout(void)
{
    ; // do nothing
}

void ModuleIdLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.lmHeader.logModuleId != 0 )
    {
        char buffer[16];
        String::formatString(buffer, 16, "%p", static_cast<id_type>(msgLog.lmHeader.logModuleId));
        stream.write(buffer);
    }
}

//////////////////////////////////////////////////////////////////////////
// MessageLayout class declaration
//////////////////////////////////////////////////////////////////////////

MessageLayout::MessageLayout(void)
    : IELayout ( NELogConfig::LayoutMessage )
{
    ; // do nothing
}

MessageLayout::MessageLayout(const MessageLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutMessage )
{
    ; // do nothing
}

MessageLayout::~MessageLayout(void)
{
    ; // do nothing
}

void MessageLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.lmTrace.traceMessage != NULL )
        stream.write(msgLog.lmTrace.traceMessage);
}

//////////////////////////////////////////////////////////////////////////
// EndOfLineLayout class declaration
//////////////////////////////////////////////////////////////////////////

EndOfLineLayout::EndOfLineLayout(void)
    : IELayout ( NELogConfig::LayoutEndOfLine )
{
    ; // do nothing
}

EndOfLineLayout::EndOfLineLayout(const EndOfLineLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutEndOfLine )
{
    ; // do nothing
}

EndOfLineLayout::~EndOfLineLayout(void)
{
    ; // do nothing
}

void EndOfLineLayout::logMessage( const NETrace::sLogMessage & /*src*/, IEOutStream & stream ) const
{
    static const char * const   END_OF_LINE = "\n" ;
    stream.write(END_OF_LINE);
}

//////////////////////////////////////////////////////////////////////////
// PriorityLayout class declaration
//////////////////////////////////////////////////////////////////////////

PriorityLayout::PriorityLayout(void)
    : IELayout ( NELogConfig::LayoutPriority )
{
    ; // do nothing
}

PriorityLayout::PriorityLayout(const PriorityLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutPriority )
{
    ; // do nothing
}

PriorityLayout::~PriorityLayout(void)
{
    ; // do nothing
}

void PriorityLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    stream.write( NETrace::convToString( msgLog.lmTrace.traceMessagePrio ) );
}

//////////////////////////////////////////////////////////////////////////
// ScopeIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ScopeIdLayout::ScopeIdLayout(void)
    : IELayout ( NELogConfig::LaytoutScopeId )
{
    ; // do nothing
}

ScopeIdLayout::ScopeIdLayout(const ScopeIdLayout & /*src*/)
    : IELayout ( NELogConfig::LaytoutScopeId )
{
    ; // do nothing
}

ScopeIdLayout::~ScopeIdLayout(void)
{
    ; // do nothing
}

void ScopeIdLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.lmTrace.traceScopeId != 0 )
    {
        char buffer[16];
        String::formatString(buffer, 16, "%u", msgLog.lmTrace.traceScopeId);
        stream.write(buffer);
    }
}

//////////////////////////////////////////////////////////////////////////
// ThreadIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

ThreadIdLayout::ThreadIdLayout(void)
    : IELayout ( NELogConfig::LayoutThreadId )
{
    ; // do nothing
}

ThreadIdLayout::ThreadIdLayout(const ThreadIdLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutThreadId )
{
    ; // do nothing
}

ThreadIdLayout::~ThreadIdLayout(void)
{
    ; // do nothing
}

void ThreadIdLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.lmTrace.traceThreadId != 0 )
    {
        char buffer[32];
        String::formatString(buffer, 32, "%p", static_cast<id_type>(msgLog.lmTrace.traceThreadId));
        stream.write(buffer);
    }
}

//////////////////////////////////////////////////////////////////////////
// ModuleNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ModuleNameLayout::ModuleNameLayout(void)
    : IELayout ( NELogConfig::LayoutExecutableName )
{
    ; // do nothing
}

ModuleNameLayout::ModuleNameLayout(const ModuleNameLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutExecutableName )
{
    ; // do nothing
}

ModuleNameLayout::~ModuleNameLayout(void)
{
    ; // do nothing
}

void ModuleNameLayout::logMessage( const NETrace::sLogMessage & /*msgLog*/, IEOutStream & stream ) const
{
    stream.write( Process::getInstance().getAppName() );
}

//////////////////////////////////////////////////////////////////////////
// ThreadNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ThreadNameLayout::ThreadNameLayout(void)
    : IELayout ( NELogConfig::LayoutThreadName )
{
    ; // do nothing
}

ThreadNameLayout::ThreadNameLayout(const ThreadNameLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutThreadName )
{
    ; // do nothing
}

ThreadNameLayout::~ThreadNameLayout(void)
{
    ; // do nothing
}

void ThreadNameLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    Thread * thread = Thread::findThreadById(msgLog.lmTrace.traceThreadId);
    stream.write( thread != NULL ? thread->getName() : "Unknown Thread" );
}

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

ScopeNameLayout::ScopeNameLayout(void)
    : IELayout ( NELogConfig::LaytoutScopeName )
{
    ; // do nothing
}

ScopeNameLayout::ScopeNameLayout(const ScopeNameLayout & /*src*/)
    : IELayout ( NELogConfig::LaytoutScopeName )
{
    ; // do nothing
}

ScopeNameLayout::~ScopeNameLayout(void)
{
    ; // do nothing
}

void ScopeNameLayout::logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    stream.write( msgLog.lmTrace.traceMessage );
}

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

AnyTextLayout::AnyTextLayout(void)
    : IELayout     ( NELogConfig::LayoutAnyText )
    , mTextMessage  ( )
{
    ; // do nothing
}

AnyTextLayout::AnyTextLayout(const AnyTextLayout & src)
    : IELayout     ( NELogConfig::LayoutAnyText )
    , mTextMessage  ( src.mTextMessage )
{
    ; // do nothing
}

AnyTextLayout::AnyTextLayout(const String & anyMessage)
    : IELayout     ( NELogConfig::LayoutAnyText )
    , mTextMessage  ( anyMessage )
{
    ; // do nothing
}

AnyTextLayout::AnyTextLayout(const char * anyMessage)
    : IELayout     ( NELogConfig::LayoutAnyText )
    , mTextMessage  ( anyMessage != NULL ? anyMessage : "" )
{
    ; // do nothing
}

AnyTextLayout::~AnyTextLayout(void)
{
    ; // do nothing
}

void AnyTextLayout::logMessage( const NETrace::sLogMessage & /*msgLog*/, IEOutStream & stream ) const
{
    stream.write( mTextMessage.getString() );
}
