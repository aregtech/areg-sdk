/************************************************************************
 * \file        areg/trace/private/CEDebugOutputLogger.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Debug Output Logger object to log message into the
 *              system Output console object
 ************************************************************************/
#include "areg/trace/private/CEDebugOutputLogger.hpp"

#include "areg/trace/private/IETraceConfiguration.hpp"
#include "areg/trace/private/CETraceProperty.hpp"
#include "areg/base/CEProcess.hpp"
#include "areg/base/CEDateTime.hpp"
#include "areg/base/IEByteBuffer.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/CEWideString.hpp"
#include "areg/base/CEString.hpp"

#include "areg/base/private/NEDebug.hpp"

CEDebugOutputLogger::CEDebugOutputLogger( IETraceConfiguration & tracerConfig )
    : CELoggerBase     ( tracerConfig )
    , IEOutStream      ( )

    , mIsOpened         ( false )
    , mOutputMessageA   ( )
{
    ; // do nothing
}

CEDebugOutputLogger::~CEDebugOutputLogger(void)
{
    ; // do nothing
}

bool CEDebugOutputLogger::OpenLogger(void)
{
    if ( mIsOpened == false )
    {
        const IETraceConfiguration & traceConfig = GetTraceConfiguration();
        ASSERT( static_cast<bool>(traceConfig.PropertyStatus().GetValue()) );
        
        const CETraceProperty & prop = traceConfig.PropertyDebugOutput();
        if ( prop.IsValid() && static_cast<bool>(prop.GetValue()) )
        {
#ifdef  _DEBUG

            mIsOpened = CreateLayouts( );

            if ( mIsOpened )
            {
                CEProcess & curProcess = CEProcess::GetProcess();
                NETrace::sLogMessage logMsgHello;
                NEMemory::ZeroData<NETrace::sLogMessage>(logMsgHello);

                logMsgHello.lmHeader.logLength      = sizeof(NETrace::sLogMessage);
                logMsgHello.lmHeader.logType        = NETrace::LogMessage;
                logMsgHello.lmHeader.logModuleId    = 0;

                logMsgHello.lmTrace.traceThreadId   = 0;
                logMsgHello.lmTrace.traceScopeId    = 0;
                logMsgHello.lmTrace.traceTimestamp  = CEDateTime::GetNow();
                logMsgHello.lmTrace.traceMessagePrio= NETrace::PrioIgnoreLayout;
                CEString::PrintString(logMsgHello.lmTrace.traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, CELoggerBase::FOMAT_MESSAGE_HELLO, curProcess.GetProcessFullPath(), curProcess.GetProcessId());

                LogMessage(logMsgHello);
            }
#endif  // _DEBUG
        }
        else
        {
            ; // no property was set
        }
    }

    return mIsOpened;
}

void CEDebugOutputLogger::CloseLogger(void)
{
    if ( mIsOpened )
    {
#ifdef  _DEBUG
        CEProcess & curProcess = CEProcess::GetProcess();
        NETrace::sLogMessage logMsgHello;
        NEMemory::ZeroData<NETrace::sLogMessage>(logMsgHello);

        logMsgHello.lmHeader.logLength      = sizeof(NETrace::sLogMessage);
        logMsgHello.lmHeader.logType        = NETrace::LogMessage;
        logMsgHello.lmHeader.logModuleId    = 0;

        logMsgHello.lmTrace.traceThreadId   = 0;
        logMsgHello.lmTrace.traceScopeId    = 0;
        logMsgHello.lmTrace.traceTimestamp  = CEDateTime::GetNow();
        logMsgHello.lmTrace.traceMessagePrio= NETrace::PrioIgnoreLayout;
        CEString::PrintString(logMsgHello.lmTrace.traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, CELoggerBase::FORMAT_MESSAGE_BYE, curProcess.GetProcessFullPath(), curProcess.GetProcessId());

        LogMessage(logMsgHello);
#endif  // _DEBUG
    }

    ReleaseLayouts();
    mIsOpened = false;
}

bool CEDebugOutputLogger::LogMessage(const NETrace::sLogMessage & logMessage)
{
    bool result = false;
    if ( mIsOpened )
    {
        switch (logMessage.lmHeader.logType)
        {
        case NETrace::LogMessage:
            result = GetLayoutMessage().LogMessage(logMessage, static_cast<IEOutStream &>(*this));
            break;

        case NETrace::LogScopeEnter:
            result = GetLayoutEnterScope().LogMessage( logMessage, static_cast<IEOutStream &>(*this) );
            break;

        case NETrace::LogScopeExit:
            result = GetLayoutExitScope().LogMessage( logMessage, static_cast<IEOutStream &>(*this) );
            break;

        default:
            ASSERT(false);  // unexpected message to log
            break;
        }

        Flush();
    }
    return result;
}

bool CEDebugOutputLogger::IsLoggerOpened(void) const
{
    return mIsOpened;
}

unsigned int CEDebugOutputLogger::Write(const unsigned char * /*buffer*/, unsigned int size)
{
    return size;
}

unsigned int CEDebugOutputLogger::Write(const IEByteBuffer & buffer)
{
    return buffer.GetUsedSize();
}

unsigned int CEDebugOutputLogger::Write( const CEString & asciiString )
{
#ifdef _DEBUG
    mOutputMessageA += asciiString;
#endif  // WIN32
    return asciiString.GetOccupiedBytes();
}

unsigned int CEDebugOutputLogger::Write( const CEWideString & wideString )
{
// #ifdef  _DEBUG
//     mOutputMessageW += wideString;
// #endif  // _DEBUG
    return wideString.GetOccupiedBytes();
}

void CEDebugOutputLogger::Flush(void)
{
    NEDebug::outputMessageOS(mOutputMessageA.GetBuffer());
    mOutputMessageA.Clear();
}

unsigned int CEDebugOutputLogger::GetWritableSize(void) const
{
    return static_cast<unsigned int>(0xFFFF);
}

void CEDebugOutputLogger::FlushLogs(void)
{
    Flush( );
}
