/************************************************************************
 * \file        areg/trace/private/ELayouts.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The collection of layouts
 ************************************************************************/
#include "areg/trace/private/ELayouts.hpp"

#include "areg/base/CEDateTime.hpp"
#include "areg/base/CEProcess.hpp"
#include "areg/base/CEThread.hpp"
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
// CETickCountLayout class implementation
//////////////////////////////////////////////////////////////////////////

CETickCountLayout::CETickCountLayout( void )
    : IELayout     ( NELogConfig::LayoutTickCount )
{
    ; // do nothing
}

CETickCountLayout::CETickCountLayout( const CETickCountLayout & /*src*/ )
    : IELayout     ( NELogConfig::LayoutTickCount )
{
    ; // do nothing
}

CETickCountLayout::~CETickCountLayout(void)
{
    ; // do nothing
}

void CETickCountLayout::LogMessage( const NETrace::sLogMessage & /*msgLog*/, IEOutStream & stream ) const
{
    char buffer[65];    
    CEString::PrintString(buffer, 64, "%llu", static_cast<unsigned int>( CEDateTime::GetProcessTickClocks() ));
    stream.Write(buffer);
}

//////////////////////////////////////////////////////////////////////////
// CEDayTimeLaytout class declaration
//////////////////////////////////////////////////////////////////////////


CEDayTimeLaytout::CEDayTimeLaytout( void )
    : IELayout ( NELogConfig::LayoutDayTime )
{
    ; // do nothing
}

CEDayTimeLaytout::CEDayTimeLaytout( const CEDayTimeLaytout & /*src*/ )
    : IELayout ( NELogConfig::LayoutDayTime )
{
    ; // do nothing
}

CEDayTimeLaytout::~CEDayTimeLaytout(void)
{
    ; // do nothing
}

void CEDayTimeLaytout::LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.lmTrace.traceTimestamp != 0 )
    {
        CEDateTime timestamp( msgLog.lmTrace.traceTimestamp );
        stream.Write( timestamp.FormatTime( CEDateTime::TIME_FORMAT_ISO8601_OUTPUT) );
    }
}

//////////////////////////////////////////////////////////////////////////
// CEModuleIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

CEModuleIdLayout::CEModuleIdLayout(void)
    : IELayout ( NELogConfig::LayoutExecutableId )
{
    ; // do nothing
}

CEModuleIdLayout::CEModuleIdLayout(const CEModuleIdLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutExecutableId )
{
    ; // do nothing
}

CEModuleIdLayout::~CEModuleIdLayout(void)
{
    ; // do nothing
}

void CEModuleIdLayout::LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.lmHeader.logModuleId != 0 )
    {
        char buffer[16];
        CEString::PrintString(buffer, 16, "%p", msgLog.lmHeader.logModuleId);
        stream.Write(buffer);
    }
}

//////////////////////////////////////////////////////////////////////////
// CEMessageLayout class declaration
//////////////////////////////////////////////////////////////////////////

CEMessageLayout::CEMessageLayout(void)
    : IELayout ( NELogConfig::LayoutMessage )
{
    ; // do nothing
}

CEMessageLayout::CEMessageLayout(const CEMessageLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutMessage )
{
    ; // do nothing
}

CEMessageLayout::~CEMessageLayout(void)
{
    ; // do nothing
}

void CEMessageLayout::LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.lmTrace.traceMessage != NULL )
        stream.Write(msgLog.lmTrace.traceMessage);
}

//////////////////////////////////////////////////////////////////////////
// CEEndOfLineLayout class declaration
//////////////////////////////////////////////////////////////////////////

CEEndOfLineLayout::CEEndOfLineLayout(void)
    : IELayout ( NELogConfig::LayoutEndOfLine )
{
    ; // do nothing
}

CEEndOfLineLayout::CEEndOfLineLayout(const CEEndOfLineLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutEndOfLine )
{
    ; // do nothing
}

CEEndOfLineLayout::~CEEndOfLineLayout(void)
{
    ; // do nothing
}

void CEEndOfLineLayout::LogMessage( const NETrace::sLogMessage & /*src*/, IEOutStream & stream ) const
{
    static const char * const   END_OF_LINE = "\n" ;
    stream.Write(END_OF_LINE);
}

//////////////////////////////////////////////////////////////////////////
// CEPriorityLayout class declaration
//////////////////////////////////////////////////////////////////////////

CEPriorityLayout::CEPriorityLayout(void)
    : IELayout ( NELogConfig::LayoutPriority )
{
    ; // do nothing
}

CEPriorityLayout::CEPriorityLayout(const CEPriorityLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutPriority )
{
    ; // do nothing
}

CEPriorityLayout::~CEPriorityLayout(void)
{
    ; // do nothing
}

void CEPriorityLayout::LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    stream.Write( NETrace::ToString( msgLog.lmTrace.traceMessagePrio ) );
}

//////////////////////////////////////////////////////////////////////////
// CEScopeIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

CEScopeIdLayout::CEScopeIdLayout(void)
    : IELayout ( NELogConfig::LaytoutScopeId )
{
    ; // do nothing
}

CEScopeIdLayout::CEScopeIdLayout(const CEScopeIdLayout & /*src*/)
    : IELayout ( NELogConfig::LaytoutScopeId )
{
    ; // do nothing
}

CEScopeIdLayout::~CEScopeIdLayout(void)
{
    ; // do nothing
}

void CEScopeIdLayout::LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.lmTrace.traceScopeId != 0 )
    {
        char buffer[16];
        CEString::PrintString(buffer, 16, "%u", msgLog.lmTrace.traceScopeId);
        stream.Write(buffer);
    }
}

//////////////////////////////////////////////////////////////////////////
// CEThreadIdLayout class declaration
//////////////////////////////////////////////////////////////////////////

CEThreadIdLayout::CEThreadIdLayout(void)
    : IELayout ( NELogConfig::LayoutThreadId )
{
    ; // do nothing
}

CEThreadIdLayout::CEThreadIdLayout(const CEThreadIdLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutThreadId )
{
    ; // do nothing
}

CEThreadIdLayout::~CEThreadIdLayout(void)
{
    ; // do nothing
}

void CEThreadIdLayout::LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    if ( msgLog.lmTrace.traceThreadId != 0 )
    {
        char buffer[16];
        CEString::PrintString(buffer, 16, "%p", msgLog.lmTrace.traceThreadId);
        stream.Write(buffer);
    }
}

//////////////////////////////////////////////////////////////////////////
// CEModuleNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

CEModuleNameLayout::CEModuleNameLayout(void)
    : IELayout ( NELogConfig::LayoutExecutableName )
{
    ; // do nothing
}

CEModuleNameLayout::CEModuleNameLayout(const CEModuleNameLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutExecutableName )
{
    ; // do nothing
}

CEModuleNameLayout::~CEModuleNameLayout(void)
{
    ; // do nothing
}

void CEModuleNameLayout::LogMessage( const NETrace::sLogMessage & /*msgLog*/, IEOutStream & stream ) const
{
    stream.Write( CEProcess::GetProcess().GetAppName() );
}

//////////////////////////////////////////////////////////////////////////
// CEThreadNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

CEThreadNameLayout::CEThreadNameLayout(void)
    : IELayout ( NELogConfig::LayoutThreadName )
{
    ; // do nothing
}

CEThreadNameLayout::CEThreadNameLayout(const CEThreadNameLayout & /*src*/)
    : IELayout ( NELogConfig::LayoutThreadName )
{
    ; // do nothing
}

CEThreadNameLayout::~CEThreadNameLayout(void)
{
    ; // do nothing
}

void CEThreadNameLayout::LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    CEThread * thread = CEThread::FindThreadById(msgLog.lmTrace.traceThreadId);
    stream.Write( thread != NULL ? thread->GetThreadName() : "Unknown Thread" );
}

//////////////////////////////////////////////////////////////////////////
// CEScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

CEScopeNameLayout::CEScopeNameLayout(void)
    : IELayout ( NELogConfig::LaytoutScopeName )
{
    ; // do nothing
}

CEScopeNameLayout::CEScopeNameLayout(const CEScopeNameLayout & /*src*/)
    : IELayout ( NELogConfig::LaytoutScopeName )
{
    ; // do nothing
}

CEScopeNameLayout::~CEScopeNameLayout(void)
{
    ; // do nothing
}

void CEScopeNameLayout::LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const
{
    stream.Write( msgLog.lmTrace.traceMessage );
}

//////////////////////////////////////////////////////////////////////////
// CEScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////

CEAnyTextLayout::CEAnyTextLayout(void)
    : IELayout     ( NELogConfig::LayoutAnyText )
    , mTextMessage  ( )
{
    ; // do nothing
}

CEAnyTextLayout::CEAnyTextLayout(const CEAnyTextLayout & src)
    : IELayout     ( NELogConfig::LayoutAnyText )
    , mTextMessage  ( src.mTextMessage )
{
    ; // do nothing
}

CEAnyTextLayout::CEAnyTextLayout(const CEString & anyMessage)
    : IELayout     ( NELogConfig::LayoutAnyText )
    , mTextMessage  ( anyMessage )
{
    ; // do nothing
}

CEAnyTextLayout::CEAnyTextLayout(const char * anyMessage)
    : IELayout     ( NELogConfig::LayoutAnyText )
    , mTextMessage  ( anyMessage != NULL ? anyMessage : "" )
{
    ; // do nothing
}

CEAnyTextLayout::~CEAnyTextLayout(void)
{
    ; // do nothing
}

void CEAnyTextLayout::LogMessage( const NETrace::sLogMessage & /*msgLog*/, IEOutStream & stream ) const
{
    stream.Write( mTextMessage.GetBuffer() );
}
