/************************************************************************
 * \file        areg/trace/private/FileLogger.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       File Logger object to log message into the file
 ************************************************************************/
#include "areg/trace/private/FileLogger.hpp"

#include "areg/trace/private/IETraceConfiguration.hpp"
#include "areg/trace/private/TraceProperty.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"

FileLogger::FileLogger( IETraceConfiguration & tracerConfig )
    : LoggerBase         ( tracerConfig )

    , mLogFile          ( )
{
    ; // do nothing
}

FileLogger::~FileLogger(void)
{
    ; // do nothing
}

bool FileLogger::openLogger( void )
{
    if ( mLogFile.isOpened() == false )
    {
        const IETraceConfiguration & traceConfig = getTraceConfiguration();
        ASSERT( static_cast<bool>(traceConfig.propertyStatus().getValue()) );
        const TraceProperty & prop = traceConfig.propertyLogFile();
        if ( prop.isValid() )
        {
            String fileName = File::normalizePath( static_cast<const char *>(prop.getValue()) );
            // fileName = File::NormalizeFilePath( fileName );
            if ( fileName.isEmpty() == false )
            {
                bool newFile    = static_cast<bool>(traceConfig.propertyAppendData()) == false;
                int mode = File::FO_MODE_WRITE | File::FO_MODE_READ | File::FO_MODE_SHARE_READ | File::FO_MODE_SHARE_WRITE | File::FO_MODE_TEXT;

                if ( File::existFile(fileName) )
                    mode |= newFile ? File::FO_MODE_TRUNCATE : File::FO_MODE_EXIST;
                else
                    mode |= FileBase::FO_MODE_CREATE;
                if ( mLogFile.open( fileName, mode) && createLayouts() )
                {
                    
                    Process & curProcess = Process::getInstance();
                    NETrace::sLogMessage logMsgHello;
                    NEMemory::zeroData<NETrace::sLogMessage>( logMsgHello );

                    logMsgHello.lmHeader.logLength      = sizeof(NETrace::sLogMessage);
                    logMsgHello.lmHeader.logType        = NETrace::LogMessage;
                    logMsgHello.lmHeader.logModuleId    = 0;

                    logMsgHello.lmTrace.traceThreadId   = 0;
                    logMsgHello.lmTrace.traceScopeId    = 0;
                    logMsgHello.lmTrace.traceTimestamp  = DateTime::getNow();
                    logMsgHello.lmTrace.traceMessagePrio= NETrace::PrioIgnoreLayout;
                    String::formatString(logMsgHello.lmTrace.traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, LoggerBase::FOMAT_MESSAGE_HELLO, curProcess.getFullPath(), 0);

                    logMessage(logMsgHello);
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

    return mLogFile.isOpened();
}

void FileLogger::closeLogger(void)
{
    if ( mLogFile.isOpened() )
    {
        Process & curProcess = Process::getInstance();
        NETrace::sLogMessage logMsgHello;
        NEMemory::zeroData<NETrace::sLogMessage>( logMsgHello );

        logMsgHello.lmHeader.logLength      = sizeof(NETrace::sLogMessage);
        logMsgHello.lmHeader.logType        = NETrace::LogMessage;
        logMsgHello.lmHeader.logModuleId    = 0;

        logMsgHello.lmTrace.traceThreadId   = 0;
        logMsgHello.lmTrace.traceScopeId    = 0;
        logMsgHello.lmTrace.traceTimestamp  = DateTime::getNow();
        logMsgHello.lmTrace.traceMessagePrio= NETrace::PrioIgnoreLayout;
        String::formatString(logMsgHello.lmTrace.traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, LoggerBase::FORMAT_MESSAGE_BYE, curProcess.getFullPath(), 0);

        logMessage(logMsgHello);
    }
    releaseLayouts();
    mLogFile.close();
}

bool FileLogger::logMessage( const NETrace::sLogMessage & logMessage )
{
    bool result = false;
    if ( mLogFile.isOpened() )
    {
        mLogFile.moveToBegin();

        switch (logMessage.lmHeader.logType)
        {
        case NETrace::LogMessage:
            result = getLayoutMessage().logMessage( logMessage, static_cast<IEOutStream &>(mLogFile) );
            break;

        case NETrace::LogScopeEnter:
            result = getLayoutEnterScope().logMessage( logMessage, static_cast<IEOutStream &>(mLogFile) );
            break;

        case NETrace::LogScopeExit:
            result = getLayoutExitScope().logMessage( logMessage, static_cast<IEOutStream &>(mLogFile) );
            break;

        default:
            ASSERT(false);  // unexpected message to log
            break;
        }
    }
    return result;
}

bool FileLogger::isLoggerOpened(void) const
{
    return mLogFile.isOpened();
}

void FileLogger::flushLogs(void)
{
    mLogFile.flush();
}
