/************************************************************************
 * \file        areg/trace/private/CEFileLogger.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       File Logger object to log message into the file
 ************************************************************************/
#include "areg/trace/private/CEFileLogger.hpp"

#include "areg/trace/private/IETraceConfiguration.hpp"
#include "areg/trace/private/CETraceProperty.hpp"
#include "areg/base/CEProcess.hpp"
#include "areg/base/CEDateTime.hpp"

CEFileLogger::CEFileLogger( IETraceConfiguration & tracerConfig )
    : CELoggerBase         ( tracerConfig )

    , mLogFile          ( )
{
    ; // do nothing
}

CEFileLogger::~CEFileLogger(void)
{
    ; // do nothing
}

bool CEFileLogger::OpenLogger( void )
{
    if ( mLogFile.IsOpened() == false )
    {
        const IETraceConfiguration & traceConfig = GetTraceConfiguration();
        ASSERT( static_cast<bool>(traceConfig.PropertyStatus().GetValue()) );
        const CETraceProperty & prop = traceConfig.PropertyLogFile();
        if ( prop.IsValid() )
        {
            CEString fileName = CEFile::NormalizeFileName( static_cast<const char *>(prop.GetValue()) );
            // fileName = CEFile::NormalizeFilePath( fileName );
            if ( fileName.IsEmpty() == false )
            {
                bool newFile    = static_cast<bool>(traceConfig.PropertyAppendData()) == false;
                int mode = CEFile::FO_MODE_WRITE | CEFile::FO_MODE_READ | CEFile::FO_MODE_SHARE_READ | CEFile::FO_MODE_SHARE_WRITE | CEFile::FO_MODE_TEXT;

                if ( CEFile::IsFileExist(fileName) )
                    mode |= newFile ? CEFile::FO_MODE_TRUNCATE : CEFile::FO_MODE_EXIST;
                else
                    mode |= CEFileBase::FO_MODE_CREATE;
                if ( mLogFile.Open( fileName, mode) && CreateLayouts() )
                {
                    
                    CEProcess & curProcess = CEProcess::GetProcess();
                    NETrace::sLogMessage logMsgHello;
                    NEMemory::ZeroData<NETrace::sLogMessage>( logMsgHello );

                    logMsgHello.lmHeader.logLength      = sizeof(NETrace::sLogMessage);
                    logMsgHello.lmHeader.logType        = NETrace::LogMessage;
                    logMsgHello.lmHeader.logModuleId    = 0;

                    logMsgHello.lmTrace.traceThreadId   = 0;
                    logMsgHello.lmTrace.traceScopeId    = 0;
                    logMsgHello.lmTrace.traceTimestamp  = CEDateTime::GetNow();
                    logMsgHello.lmTrace.traceMessagePrio= NETrace::PrioIgnoreLayout;
                    CEString::PrintString(logMsgHello.lmTrace.traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, CELoggerBase::FOMAT_MESSAGE_HELLO, curProcess.GetProcessFullPath(), 0);

                    LogMessage(logMsgHello);
                }
            }
            else
            {
                ; // no file specified
            }
        }
        else
        {
            ; // no property was set
        }
    }

    return mLogFile.IsOpened();
}

void CEFileLogger::CloseLogger(void)
{
    if ( mLogFile.IsOpened() )
    {
        CEProcess & curProcess = CEProcess::GetProcess();
        NETrace::sLogMessage logMsgHello;
        NEMemory::ZeroData<NETrace::sLogMessage>( logMsgHello );

        logMsgHello.lmHeader.logLength      = sizeof(NETrace::sLogMessage);
        logMsgHello.lmHeader.logType        = NETrace::LogMessage;
        logMsgHello.lmHeader.logModuleId    = 0;

        logMsgHello.lmTrace.traceThreadId   = 0;
        logMsgHello.lmTrace.traceScopeId    = 0;
        logMsgHello.lmTrace.traceTimestamp  = CEDateTime::GetNow();
        logMsgHello.lmTrace.traceMessagePrio= NETrace::PrioIgnoreLayout;
        CEString::PrintString(logMsgHello.lmTrace.traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, CELoggerBase::FORMAT_MESSAGE_BYE, curProcess.GetProcessFullPath(), 0);

        LogMessage(logMsgHello);
    }
    ReleaseLayouts();
    mLogFile.Close();
}

bool CEFileLogger::LogMessage( const NETrace::sLogMessage & logMessage )
{
    bool result = false;
    if ( mLogFile.IsOpened() )
    {
        mLogFile.MoveCursorToEnd();

        switch (logMessage.lmHeader.logType)
        {
        case NETrace::LogMessage:
            result = GetLayoutMessage().LogMessage( logMessage, static_cast<IEOutStream &>(mLogFile) );
            break;

        case NETrace::LogScopeEnter:
            result = GetLayoutEnterScope().LogMessage( logMessage, static_cast<IEOutStream &>(mLogFile) );
            break;

        case NETrace::LogScopeExit:
            result = GetLayoutExitScope().LogMessage( logMessage, static_cast<IEOutStream &>(mLogFile) );
            break;

        default:
            ASSERT(false);  // unexpected message to log
            break;
        }
    }
    return result;
}

bool CEFileLogger::IsLoggerOpened(void) const
{
    return mLogFile.IsOpened();
}

void CEFileLogger::FlushLogs(void)
{
    mLogFile.Flush();
}
