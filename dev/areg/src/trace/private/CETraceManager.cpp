/************************************************************************
 * \file        areg/src/trace/private/CETraceManager.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Trace manager. The container of scopes, configuring
 *              tracer, starts and stops tracing.
 ************************************************************************/
#include "areg/src/trace/private/CETraceManager.hpp"

#include "areg/src/trace/CETraceScope.hpp"
#include "areg/src/trace/private/CELogMessage.hpp"
#include "areg/src/trace/private/CETraceEvent.hpp"

#include "areg/src/base/CEProcess.hpp"
#include "areg/src/base/CEFile.hpp"
#include "areg/src/base/CEFileBuffer.hpp"
#include "areg/src/base/EContainers.hpp"
#include "areg/src/base/NEString.hpp"

//////////////////////////////////////////////////////////////////////////
// CETraceManager::CETraceScopeMap class implementation
//////////////////////////////////////////////////////////////////////////

CETraceManager::CETraceScopeMap::CETraceScopeMap(void)
    : TELockResourceMap<unsigned int, CETraceScope, MapTraceScope, ImplTraceScope> ( )
{
    ; // do nothing
}

CETraceManager::CETraceScopeMap::~CETraceScopeMap(void)
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CETraceManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CETraceManager class static members
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Tracing thread name
 **/
const char * const  CETraceManager::TRACER_THREAD_NAME          = "_AREG_TRACER_THREAD_";
const char * const  CETraceManager::DEFAULT_LOG_VERSION         = "log.version          = 1.0.0";
const char * const  CETraceManager::DEFAULT_LOG_ENABLE          = "log.enable           = true";
const char * const  CETraceManager::DEFAULT_LOG_FILE            = "log.file             = ./logs/trace_%time%.log";
const char * const  CETraceManager::DEFAULT_LOG_APPEND          = "log.append           = false";
const char * const  CETraceManager::DEFAULT_LOG_LAYOUT_ENTER    = "log.layout.enter     = %d: [ %t  %x.%z: Enter --> ]%n";
const char * const  CETraceManager::DEFAULT_LOG_LAYOUT_MESSAGE  = "log.layout.message   = %d: [ %t  %p >>> ] %m%n";
const char * const  CETraceManager::DEFAULT_LOG_LAYOUT_EXIT     = "log.layout.exit      = %d: [ %t  %x.%z: Exit <-- ]%n";
const char * const  CETraceManager::DEFAULT_LOG_LAYOUT_DEBUG    = "log.debug            = true";
const char * const  CETraceManager::DEFAULT_SCOPES_ENABLED      = "scope.*              = DEBUG | SCOPE";


//////////////////////////////////////////////////////////////////////////
// CETraceManager class static methods
//////////////////////////////////////////////////////////////////////////

CETraceManager & CETraceManager::GetTraceManager( void )
{
    static CETraceManager _traceManager;
    return _traceManager;
}

unsigned int CETraceManager::MakeScopeId( const char * scopeName )
{
    return  NEMath::Crc32Calculate(scopeName);
}

void CETraceManager::SendLogMessage( CELogMessage & logData )
{
    CETraceManager & tracer = CETraceManager::GetTraceManager();
    logData.SetModuleId( tracer.mModuleId );
    tracer.SendLogEvent( CETraceEventData(CETraceEventData::TraceMessage, logData) );
}

inline void CETraceManager::SendLogEvent( const CETraceEventData & data )
{
    CETraceEvent::SendEvent( data, static_cast<IETraceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );
}

bool CETraceManager::StartLogging( const char * configFile /*= NULL*/ )
{
    CETraceManager & traceManager = CETraceManager::GetTraceManager();
    bool canStart = false;

    do
    {
        CELock lock(traceManager.mLock);
        if ( NEString::isEmpty<char>(configFile) == false )
        {
            canStart = traceManager.IsReady() == false ? traceManager.LoadConfiguration( configFile) : false;
        }
        else if ( CETraceManager::IsLoggingConfigured()== false )
        {
            canStart = traceManager.IsReady() == false ? traceManager.LoadConfiguration( NULL ) : false;
        }
        else
        {
            canStart = traceManager.IsReady() == false;
        }

    } while (false);

    return (canStart ? traceManager.startLoggingThread() : traceManager.mIsStarted);
}

void CETraceManager::StopLogging( unsigned int waitTimeout /*= CEThread::WAIT_INFINITE*/ )
{
    GetTraceManager().stopLoggingThread(waitTimeout);
}

void CETraceManager::RegisterTraceScope( CETraceScope & scope )
{
    GetTraceManager().RegisterScope(scope);
}

void CETraceManager::UnregisterTraceScope( CETraceScope & scope )
{
    GetTraceManager().UnregisterScope(scope);
}

void CETraceManager::ActivateTraceScope( CETraceScope & scope )
{
    GetTraceManager().ActivateScope(scope);
}

bool CETraceManager::LoggingConfigure(const char * configFile /*= NULL */)
{
    return CETraceManager::GetTraceManager().LoadConfiguration(configFile);
}

bool CETraceManager::IsLoggingConfigured(void)
{
    bool result = false;
    CETraceManager & traceManager = CETraceManager::GetTraceManager();
    do 
    {
        CELock lock(traceManager.mLock);
        result = (traceManager.mConfigFile.IsEmpty() == false) && (traceManager.mPropertyList.IsEmpty() == false);
    } while (false);
    return result;
}

bool CETraceManager::IsLoggingEnabled(void)
{
    CETraceManager & traceManager = CETraceManager::GetTraceManager();
    CELock lock(traceManager.mLock);
    return traceManager.IsEnabled();
}

bool CETraceManager::ForceActivateLogging(void)
{
    bool result = false;

#ifdef DEBUG
    CETraceManager & traceManager = CETraceManager::GetTraceManager();

    if ( (traceManager.IsLoggingConfigured() == false) && (traceManager.IsLoggingStarted() == false) )
    {
        CELock lock(traceManager.mLock);
        result = traceManager.ActivateTracingDefaults();
    }

#endif // DEBUG

    return result;
}

//////////////////////////////////////////////////////////////////////////
// CETraceManager class constructor / destructor
//////////////////////////////////////////////////////////////////////////
CETraceManager::CETraceManager(void)
    : CEDispatcherThread    ( CETraceManager::TRACER_THREAD_NAME )
    , IETraceEventConsumer ( )
    , IETraceConfiguration ( )

    , mMapTraceScope    ( )
    , mScopesActivated  ( false )

    , mConfigFile       ( )
    , mLogVersion       ( )

    , mLogLayoutEnter   ( )
    , mLogLayoutMessage ( )
    , mLogLayoutExit    ( )
    , mLogStackSize     ( )
    , mLogDebugOutput   ( )
    , mLogStatus        ( )

    , mLogFile          ( )

    , mLogRemoteHost    ( )
    , mLogRemotePort    ( )

    , mLogDbHost        ( )
    , mLogDbName        ( )
    , mLogDbDriver      ( )
    , mLogDbUser        ( )
    , mLogDbPassword    ( )

    , mConfigScopeList  ( )
    , mConfigScopeGroup ( )
    , mPropertyList     ( )

    , mModuleId         ( CEProcess::GetProcess().GetProcessId() )

    , mLoggerFile       ( static_cast<IETraceConfiguration &>(self()) )
    , mLoggerDebug      ( static_cast<IETraceConfiguration &>(self()) )

    , mLogStarted       ( false, false )
    , mLock             ( )
{
    ; // do nothing
}


CETraceManager::~CETraceManager(void)
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CETraceManager class methods
//////////////////////////////////////////////////////////////////////////
void CETraceManager::RegisterScope( CETraceScope & scopeRegister )
{
#ifdef DEBUG
    if ( mMapTraceScope.FindResourceObject( static_cast<unsigned int>(scopeRegister)) != NULL )
    {
        OUTPUT_ERR("The scope [ %s ] is already registered", scopeRegister.GetScopeName());
    }
#endif // DEBUG

    ASSERT(mMapTraceScope.FindResourceObject( static_cast<unsigned int>(scopeRegister)) == NULL);
    mMapTraceScope.RegisterResourceObject( static_cast<unsigned int>(scopeRegister), &scopeRegister);
}

void CETraceManager::UnregisterScope(CETraceScope & scopeUnregister)
{
    mMapTraceScope.UnregisterResourceObject( static_cast<unsigned int>(scopeUnregister));
}

bool CETraceManager::IsScopeRegistered(unsigned int scopeId) const
{
    return GetScope( scopeId ) != NULL ;
}

bool CETraceManager::IsScopeRegistered(const char * scopeName) const
{
    return IsScopeRegistered( CETraceManager::MakeScopeId(scopeName) );
}

bool CETraceManager::IsScopeRegistered(const CETraceScope & scope) const
{
    return IsScopeRegistered( scope.GetScopeId() );
}

void CETraceManager::SetScopePriority( unsigned int scopeId, unsigned int newPrio )
{
    mMapTraceScope.ResourceLock();

    CETraceScope * scope = mMapTraceScope.FindResourceObject(scopeId);
    if ( scope != NULL )
        scope->SetPriority(newPrio);

    mMapTraceScope.ResourceUnlock();
}

void CETraceManager::AddScopePriority(unsigned int scopeId, NETrace::eLogPriority addPrio)
{
    mMapTraceScope.ResourceLock();

    CETraceScope * scope = mMapTraceScope.FindResourceObject(scopeId);
    if ( scope != NULL )
        scope->AddPriority(addPrio);

    mMapTraceScope.ResourceUnlock();
}

void CETraceManager::RemoveScopePriority(unsigned int scopeId, NETrace::eLogPriority remPrio)
{
    mMapTraceScope.ResourceLock();

    CETraceScope * scope = mMapTraceScope.FindResourceObject(scopeId);
    if ( scope != NULL )
        scope->RemovePriority(remPrio);

    mMapTraceScope.ResourceUnlock();
}

int CETraceManager::SetScopeGroupPriority(const char * scopeGroupName, unsigned int newPrio)
{
    int result = 0;
    if ( NEString::isEmpty<char>(scopeGroupName) == false )
    {
        mMapTraceScope.ResourceLock();

        unsigned int scopeId    = 0;
        for ( CETraceScope * scope = mMapTraceScope.GetResourceFirstKey(scopeId); scope != NULL; scope = mMapTraceScope.GetResourceNextKey(scopeId) )
        {
            if ( NEString::compareStrings<char, char>(scopeGroupName, scope->GetScopeName(), NEString::CountAll, false ) == 0 )
            {
                scope->SetPriority(newPrio);
                ++ result;
            }
        }

        mMapTraceScope.ResourceUnlock();
    }
    return result;
}

int CETraceManager::AddScopeGroupPriority(const char * scopeGroupName, NETrace::eLogPriority addPrio)
{
    int result = 0;
    if ( NEString::isEmpty<char>(scopeGroupName) == false )
    {
        mMapTraceScope.ResourceLock();

        unsigned int scopeId    = 0;
        for ( CETraceScope * scope = mMapTraceScope.GetResourceFirstKey(scopeId); scope != NULL; scope = mMapTraceScope.GetResourceNextKey(scopeId) )
        {
            if ( NEString::compareStrings<char, char>( scopeGroupName, scope->GetScopeName( ), NEString::CountAll, false ) == 0 )
            {
                scope->AddPriority(addPrio);
                ++ result;
            }
        }

        mMapTraceScope.ResourceUnlock();
    }
    return result;
}

int CETraceManager::RemoveScopeGroupPriority(const char * scopeGroupName, NETrace::eLogPriority remPrio)
{
    int result = 0;
    if ( NEString::isEmpty<char>(scopeGroupName) == false )
    {
        mMapTraceScope.ResourceLock();

        unsigned int scopeId    = 0;
        for ( CETraceScope * scope = mMapTraceScope.GetResourceFirstKey(scopeId); scope != NULL; scope = mMapTraceScope.GetResourceNextKey(scopeId) )
        {
            if ( NEString::compareStrings<char, char>( scopeGroupName, scope->GetScopeName( ), NEString::CountAll, false ) == 0 )
            {
                scope->RemovePriority(remPrio);
                ++ result;
            }
        }

        mMapTraceScope.ResourceUnlock();
    }
    return result;
}

bool CETraceManager::LoadConfiguration( const char * filePath /*= NULL */ )
{
    CELock lock(mLock);

    mConfigFile = CEFile::GetFileFullPath( NEString::isEmpty<char>(filePath) == false ? filePath : NELogConfig::DEFAULT_CONFIG_FILE_NAME );
    CEFile fileConfig( static_cast<const char *>(mConfigFile), CEFileBase::FO_MODE_EXIST | CEFileBase::FO_MODE_READ | CEFileBase::FO_MODE_TEXT | CEFileBase::FO_MODE_SHARE_READ );
    fileConfig.Open( );

    return LoadConfiguration( fileConfig );
}

bool CETraceManager::LoadConfiguration( const CEFileBase & configFile )
{
    CELock lock(mLock);

    ClearConfigurationData();
    if ( configFile.IsOpened() )
    {
        mConfigFile = configFile.GetName();

        CEString line;
        CETraceProperty newProperty;
        while ( configFile.ReadLine( line ) > 0 )
        {
            if ( newProperty.ParseProperty(line) )
            {
                // search, whether the property is already set
                LISTPOS pos = mPropertyList.Find(newProperty, NULL);
                if ( pos != NULL )
                    mPropertyList.SetAt(pos, newProperty);
                else
                    mPropertyList.AddTail(newProperty);
                newProperty.ClearProperty();
            }
        }
    }

    return (mPropertyList.IsEmpty() == false) && InitializeConfiguration();
}

void CETraceManager::ClearConfigurationData( void )
{
    CELock lock(mLock);

    mLogVersion.ClearProperty();

    mLogLayoutEnter.ClearProperty();
    mLogLayoutMessage.ClearProperty();
    mLogLayoutExit.ClearProperty();
    mLogStackSize.ClearProperty();
    mLogDebugOutput.ClearProperty();
    mLogStatus.ClearProperty();

    mLogFile.ClearProperty();

    mLogRemoteHost.ClearProperty();
    mLogRemotePort.ClearProperty();

    mLogDbHost.ClearProperty();
    mLogDbName.ClearProperty();
    mLogDbDriver.ClearProperty();
    mLogDbUser.ClearProperty();
    mLogDbPassword.ClearProperty();

    mConfigScopeList.RemoveAll();
    mConfigScopeGroup.RemoveAll();
    mPropertyList.RemoveAll();
}

bool CETraceManager::ActivateTracingDefaults( void )
{
    mLogVersion.ParseProperty( DEFAULT_LOG_VERSION );
    mLogLayoutEnter.ParseProperty( DEFAULT_LOG_LAYOUT_ENTER );
    mLogLayoutMessage.ParseProperty( DEFAULT_LOG_LAYOUT_MESSAGE );
    mLogLayoutExit.ParseProperty( DEFAULT_LOG_LAYOUT_EXIT );
    mLogDebugOutput.ParseProperty( DEFAULT_LOG_LAYOUT_DEBUG );
    mLogStatus.ParseProperty( DEFAULT_LOG_ENABLE );
    mLogAppendData.ParseProperty( DEFAULT_LOG_APPEND );
    mLogFile.ParseProperty( DEFAULT_LOG_FILE );

    mLogStackSize.ClearProperty();
    mLogRemoteHost.ClearProperty();
    mLogRemotePort.ClearProperty();
    mLogDbHost.ClearProperty();
    mLogDbName.ClearProperty();
    mLogDbDriver.ClearProperty();
    mLogDbUser.ClearProperty();
    mLogDbPassword.ClearProperty();

    mConfigScopeGroup.SetKey( NELogConfig::MODULE_SCOPE, static_cast<unsigned int>(NETrace::PrioDebug) | static_cast<unsigned int>(NETrace::PrioScope));

    return startLoggingThread();
}

bool CETraceManager::InitializeConfiguration(void)
{
    CELock lock(mLock);

    CEString moduleName      = CEProcess::GetProcess().GetAppName();
    for ( LISTPOS pos = mPropertyList.GetHeadPosition(); pos != NULL; pos = mPropertyList.GetNextPosition(pos) )
    {
        const CETraceProperty &     prop    = mPropertyList.GetAt(pos);
        const CETracePropertyKey &  Key     = prop.GetKey();
        const CETracePropertyValue& Value   = prop.GetValue();

        // For any property, either global setting is set, or local
        if ( Key.IsModuleKeySet(moduleName) )
        {
            switch ( Key.GetLogConfig() )
            {
            case NELogConfig::ConfigLogVersion:
                mLogVersion = prop;
                break;

            case NELogConfig::ConfigLogFile:
                if ( mLogFile.GetKey().IsLocalKey() == false || mLogFile.GetKey().GetModule() == Key.GetModule() )
                {
                    mLogFile    = prop;
                    if ( mLogFile.GetValue().IsValid() == false )
                        mLogFile.GetValue() = NELogConfig::DEFAULT_LOG_FILE_NAME;
                }
                break;

            case NELogConfig::ConfigLogRemoteHost:
                if ( mLogRemoteHost.GetKey().IsLocalKey() == false || mLogRemoteHost.GetKey().GetModule() == Key.GetModule() )
                    mLogRemoteHost  = prop;
                break;

            case NELogConfig::ConfigLogRemotePort:
                if ( mLogRemotePort.GetKey().IsLocalKey() == false || mLogRemotePort.GetKey().GetModule() == Key.GetModule() )
                    mLogRemotePort  = prop;
                break;

            case NELogConfig::ConfigLogDatabaseDriver:
                if ( mLogDbDriver.GetKey().IsLocalKey() == false || mLogDbDriver.GetKey().GetModule() == Key.GetModule() )
                    mLogDbDriver = prop;
                break;

            case NELogConfig::ConfigLogDatabaseHost:
                if ( mLogDbHost.GetKey().IsLocalKey() == false || mLogDbHost.GetKey().GetModule() == Key.GetModule() )
                    mLogDbHost = prop;
                break;

            case NELogConfig::ConfigLogDatabaseUser:
                if ( mLogDbUser.GetKey().IsLocalKey() == false || mLogDbUser.GetKey().GetModule() == Key.GetModule() )
                    mLogDbUser = prop;
                break;

            case NELogConfig::ConfigLogDatabasePwd:
                if ( mLogDbPassword.GetKey().IsLocalKey() == false || mLogDbPassword.GetKey().GetModule() == Key.GetModule() )
                    mLogDbPassword = prop;
                break;

            case NELogConfig::ConfigLogDatabaseName:
                if ( mLogDbName.GetKey().IsLocalKey() == false || mLogDbName.GetKey().GetModule() == Key.GetModule() )
                    mLogDbName = prop;
                break;

            case NELogConfig::ConfigLogDebug:
                if ( mLogDebugOutput.GetKey().IsLocalKey() == false || mLogDebugOutput.GetKey().GetModule() == Key.GetModule() )
                    mLogDebugOutput = prop;
                break;

            case NELogConfig::ConfigLogAppend:
                if ( mLogAppendData.GetKey().IsLocalKey() == false || mLogAppendData.GetKey().GetModule() == Key.GetModule() )
                    mLogAppendData = prop;
                break;

            case NELogConfig::ConfigLogStack:
                if ( mLogStackSize.GetKey().IsLocalKey() == false || mLogStackSize.GetKey().GetModule() == Key.GetModule() )
                    mLogStackSize = prop;
                break;

            case NELogConfig::ConfigLogEnable:
                if ( mLogStatus.GetKey().IsLocalKey() == false || mLogStatus.GetKey().GetModule() == Key.GetModule() )
                    mLogStatus = prop;
                break;

            case NELogConfig::ConfigLogLayoutEnter:
                if ( mLogLayoutEnter.GetKey().IsLocalKey() == false || mLogLayoutEnter.GetKey().GetModule() == Key.GetModule() )
                    mLogLayoutEnter = prop;
                break;

            case NELogConfig::ConfigLogLayoutMessage:
                if ( mLogLayoutMessage.GetKey().IsLocalKey() == false || mLogLayoutMessage.GetKey().GetModule() == Key.GetModule() )
                    mLogLayoutMessage = prop;
                break;

            case NELogConfig::ConfigLogLayoutExit:
                if ( mLogLayoutExit.GetKey().IsLocalKey() == false || mLogLayoutExit.GetKey().GetModule() == Key.GetModule() )
                    mLogLayoutExit = prop;
                break;

            case NELogConfig::ConfigScope:
                {
                    ASSERT( Key.IsValidKey( ) );
                    unsigned int prio = Value.GetPriority();
                    if (Key.IsGlobalKey())
                    {
                        ASSERT(Key.GetModule().IsEmpty());
                        mConfigScopeGroup.SetKey(NELogConfig::MODULE_SCOPE, prio);
                    }
                    else if ( Key.GetModuleData( ).FindLastOf( NELogConfig::SYNTAX_SCOPE_GROUP ) != NEString::InvalidPos )
                    {
                        mConfigScopeGroup.SetKey( Key.GetModuleData( ), prio);
                    }
                    else
                    {
                        mConfigScopeList.SetKey( Key.GetModuleData( ), prio);
                    }
                }
                break;

            default:
                break;  // do nothing
            }
        }
    }

    return IsValid( );
}

bool CETraceManager::IsValid(void) const
{
    return (mLogVersion.IsValid() ? (IsFileValid() || IsHostValid() || IsDebugOutputValid() || IsDatabaseValid()) : false);
}

bool CETraceManager::IsEnabled(void) const
{
    return mLogStatus.GetKey().IsValidKey() ? static_cast<bool>(mLogStatus.GetValue()) : NELogConfig::DEFAULT_LOG_ENABLED;
}

bool CETraceManager::IsHostValid(void) const
{
    return (mLogRemoteHost.IsValid() || mLogRemotePort.IsValid());
}

bool CETraceManager::IsDatabaseValid(void) const
{
    return ( mLogDbHost.IsValid() && mLogDbName.IsValid() && mLogDbDriver.IsValid() );
}

bool CETraceManager::IsFileValid(void) const
{
    return mLogFile.IsValid();
}

bool CETraceManager::IsDebugOutputValid(void) const
{
    return mLogDebugOutput.IsValid();
}

bool CETraceManager::startLoggingThread(void)
{
    bool result = false;
    ASSERT((IsRunning() == false) && (IsReady() == false));
    if ( CreateThread(CEThread::WAIT_INFINITE) )
    {
        // SetScopesActivity(true);
        if ( WaitForDispatcherStart(IESynchObject::WAIT_INFINITE) )
        {
            mLogStarted.ResetEvent();

            SendLogEvent( CETraceEventData(CETraceEventData::TraceStartLogs) );

            result = mLogStarted.Lock(IESynchObject::WAIT_1_SEC * 10);  // <= wait max 10 secs.
        }
    }
    else
    {
        OUTPUT_ERR("Failed to create [ %s ] System Timer Thread", CETraceManager::TRACER_THREAD_NAME);
    }

    return result;
}

void CETraceManager::stopLoggingThread( unsigned int waitTimeout )
{
    // RemoveAllEvents(false);
    SendLogEvent( CETraceEventData(CETraceEventData::TraceStopLogs) );
    CompletionWait( IsRunning() ? waitTimeout : CEThread::DO_NOT_WAIT );
}

void CETraceManager::ProcessEvent(const CETraceEventData & data)
{
    const CESharedBuffer & stream = data.GetReadableStream();

    switch ( data.GetTraceAction() )
    {
    case CETraceEventData::TraceConfigure:
        {
            LoadConfiguration( CEFileBuffer(stream, "TraceConfigure") );
        }
        break;

    case CETraceEventData::TraceChangeConfig:
        {
            ; // not implemented yet
        }
        break;

    case CETraceEventData::TraceStartLogs:
        {
            TraceStartLogs( );
            mLogStarted.SetEvent();
        }
        break;

    case CETraceEventData::TraceStopLogs:
        {
            TraceStopLogs( );
            SetExitEvent();
            mLogStarted.ResetEvent();
        }
        break;

    case CETraceEventData::TraceEnableLogs:
        {
            static_cast<CETracePropertyValue &>(mLogStatus.GetValue()) = true;
        }
        break;

    case CETraceEventData::TraceDisableLogs:
        {
            static_cast<CETracePropertyValue &>(mLogStatus.GetValue()) = false;
        }
        break;

    case CETraceEventData::TraceChangeScopes:
        {
            CESortedStringList list;
            stream >> list;
            TraceChangeScopes( list );
        }
        break;

    case CETraceEventData::TraceMessage:
        {
            TraceMessage( CELogMessage(static_cast<const IEInStream &>(stream)) );
        }
        break;

    default:
        break; // ignore, do nothing
    }
}

bool CETraceManager::RunDispatcher(void)
{
    CETraceEvent::AddListener(static_cast<IETraceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()));
    bool result = CEDispatcherThread::RunDispatcher();

    mLoggerFile.CloseLogger();
    mLoggerDebug.CloseLogger();

    CETraceEvent::RemoveListener(static_cast<IETraceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()));

    return result;
}

void CETraceManager::TraceStartLogs( void )
{
    if ( IsValid() && IsEnabled() )
    {
        SetScopesActivity( true );
        mLoggerFile.OpenLogger();
        mLoggerDebug.OpenLogger();

        mIsStarted = true;
    }
}

void CETraceManager::SetScopesActivity( bool makeActive )
{
    if ( IsValid() )
    {
        mMapTraceScope.ResourceLock();
        if ( IsEnabled() && makeActive && (mScopesActivated == false) )
        {
            unsigned int defaultPrio= NELogConfig::DEFAULT_LOG_PRIORITY;
            mConfigScopeGroup.Find( NELogConfig::MODULE_SCOPE,  defaultPrio);

            for ( MAPPOS pos = mMapTraceScope.GetStartPosition(); pos != NULL; pos = mMapTraceScope.GetNextPosition(pos) )
            {
                ActivateScope(*mMapTraceScope.GetValueAt(pos), defaultPrio);
            }

            mScopesActivated    = true;
        }
        else if ( (makeActive == false) && mScopesActivated )
        {
            for ( MAPPOS pos = mMapTraceScope.GetStartPosition(); pos != NULL; pos = mMapTraceScope.GetNextPosition(pos) )
            {
                mMapTraceScope.GetValueAt(pos)->SetPriority( NETrace::PrioNotset );
            }

            mScopesActivated    = false;
        }

        mMapTraceScope.ResourceUnlock();
    }
}

void CETraceManager::TraceStopLogs(void)
{
    SetScopesActivity( false );
}

void CETraceManager::TraceChangeScopes(CESortedStringList & scopeList)
{
    mMapTraceScope.ResourceLock();

    CEString moduleName = CEProcess::GetProcess().GetAppName();

    while ( scopeList.IsEmpty() == false )
    {
        CETraceProperty scopeProperty;
        CEString line = scopeList.RemoveLast();
        if ( scopeProperty.ParseProperty( line ) )
        {
            const CETracePropertyKey &  Key     = scopeProperty.GetKey();
            const CETracePropertyValue & Value  = scopeProperty.GetValue();
            if ( Key.GetLogConfig() == NELogConfig::ConfigScope)
            {
                // first, save property in property list
                LISTPOS pos = mPropertyList.Find(scopeProperty, NULL);
                if ( pos != NULL )
                {
                    mPropertyList.SetAt(pos, scopeProperty);
                }
                else
                {
                    mPropertyList.AddTail(scopeProperty);
                }

                const CEString & moduleData = Key.GetModuleData();
                // update scope list / or group scope only for this module
                if ( Key.GetModule() == moduleName )
                {
                    NETrace::eLogPriority prio = Value;
                    SetScopesPriority( moduleData, prio);
                }
            }
            else
            {
                ; // not a scope, ignore
            }
        }
        else
        {
            ; // did not parse, ignore
        }
    } // end loop

    mMapTraceScope.ResourceUnlock();
}

void CETraceManager::TraceMessage( const CELogMessage & logMessage )
{
    mLoggerDebug.LogMessage( static_cast<const NETrace::sLogMessage &>(logMessage) );
    mLoggerFile.LogMessage( static_cast<const NETrace::sLogMessage &>(logMessage) );

    if ( HasMoreEvents() == false )
    {
        mLoggerFile.FlushLogs();
    }
}

void CETraceManager::ActivateScope( CETraceScope & traceScope )
{
    CELock lock(mLock);

    unsigned int defaultPrio= NELogConfig::DEFAULT_LOG_PRIORITY;
    if ( mConfigScopeGroup.Find( NELogConfig::MODULE_SCOPE,  defaultPrio) == false )
        defaultPrio = NELogConfig::DEFAULT_LOG_PRIORITY;
    ActivateScope(traceScope, defaultPrio);
}

void CETraceManager::ActivateScope( CETraceScope & traceScope, unsigned int defaultPriority )
{
    CELock lock(mLock);

    traceScope.SetPriority( defaultPriority ); // set first default priority
    const char * scopeName = traceScope.GetScopeName();
    unsigned int scopePrio = defaultPriority;

    if ( mConfigScopeList.Find(scopeName, scopePrio) )
    {
        traceScope.SetPriority( scopePrio ); // exact match. Set priority
    }
    else
    {
        char groupName[NETrace::LOG_MESSAGE_BUFFER_SIZE];
        NEString::copyString<char>(groupName, NETrace::LOG_MESSAGE_BUFFER_SIZE, scopeName);
        NEString::CharPos pos = NEString::searchLastOf<char>(NELogConfig::SYNTAX_SCOPE_SEPARATOR, groupName, NEString::EndPos);
        char * separator = pos != NEString::InvalidPos ? groupName + pos : NULL;
        while ( separator != NULL )
        {
            separator[1] = NELogConfig::SYNTAX_SCOPE_GROUP; // replace group syntax
            separator[2] = '\0';                             // set end of string

            if ( mConfigScopeGroup.Find(groupName, scopePrio) )
            {
                // Found group priority, set it
                traceScope.SetPriority( scopePrio );
                separator = NULL;
            }
            else
            {
                // continue searching
                *separator= '\0';
                pos = NEString::searchLastOf<char>( NELogConfig::SYNTAX_SCOPE_SEPARATOR, groupName, NEString::EndPos );
                separator = pos != NEString::InvalidPos ? groupName + pos : NULL;
            }
        }
    }
}

void CETraceManager::SetScopesPriority(const char * scopeName, unsigned int logPriority)
{
    if ( NEString::isEmpty<char>(scopeName) == false )
    {
        NEString::CharPos pos = NEString::searchLastOf<char>( NELogConfig::SYNTAX_SCOPE_SEPARATOR, scopeName, NEString::EndPos );
        const char * syntaxGroup = pos != NEString::InvalidPos ? scopeName + pos : EMPTY_STRING;
        if ( syntaxGroup == NULL )
        {
            // there is a complete scope name, make changes.
            CETraceScope * traceScope   = NULL;
            mConfigScopeList.SetKey(scopeName, static_cast<unsigned int>(logPriority));
            if ( mMapTraceScope.Find(CETraceManager::MakeScopeId(scopeName), traceScope) && (traceScope != NULL) )
                traceScope->SetPriority( logPriority );
        }
        else if ( syntaxGroup == scopeName )
        {
            // change all, it is set group change for complete module
            mConfigScopeGroup.SetKey(scopeName, static_cast<unsigned int>(logPriority));
            for ( MAPPOS mapPos = mMapTraceScope.GetStartPosition(); mapPos != NULL; mapPos = mMapTraceScope.GetNextPosition(mapPos) )
                mMapTraceScope.GetValueAt(mapPos)->SetPriority( logPriority );
        }
        else if ( *(-- syntaxGroup) == NELogConfig::SYNTAX_SCOPE_SEPARATOR )
        {
            // it is a group scope, update all groups
            mConfigScopeGroup.SetKey(scopeName, static_cast<unsigned int>(logPriority));
            int len = static_cast<int>(syntaxGroup - scopeName);
            for ( MAPPOS mapPos = mMapTraceScope.GetStartPosition(); mapPos != NULL; mapPos = mMapTraceScope.GetNextPosition(mapPos) )
            {
                CETraceScope * traceScope = mMapTraceScope.GetValueAt(mapPos);
                if ( NEString::compareStrings<char, char>(scopeName, traceScope->GetScopeName(), len) == 0 )
                    traceScope->SetPriority( logPriority );
            }
        }
        else
        {
            ; // do nothing, wrong syntax
        }
    }
    else
    {
        ; // do nothing, empty scope name
    }
}

const CETraceProperty & CETraceManager::PropertyVersion(void) const
{
    return mLogVersion;
}

const CETraceProperty & CETraceManager::PropertyLayoutEnter(void) const
{
    return mLogLayoutEnter;
}

const CETraceProperty & CETraceManager::PropertyLayoutMessage(void) const
{
    return mLogLayoutMessage;
}

const CETraceProperty & CETraceManager::PropertyLayoutExit(void) const
{
    return mLogLayoutExit;
}

const CETraceProperty & CETraceManager::PropertyStackSize(void) const
{
    return mLogStackSize;
}

const CETraceProperty & CETraceManager::PropertyDebugOutput(void) const
{
    return mLogDebugOutput;
}

const CETraceProperty & CETraceManager::PropertyStatus(void) const
{
    return mLogStatus;
}

const CETraceProperty & CETraceManager::PropertyAppendData(void) const
{
    return mLogAppendData;
}

const CETraceProperty & CETraceManager::PropertyLogFile(void) const
{
    return mLogFile;
}

const CETraceProperty & CETraceManager::PropertyRemoteHost(void) const
{
    return mLogRemoteHost;
}

const CETraceProperty & CETraceManager::PropertyRemotePort(void) const
{
    return mLogRemotePort;
}

const CETraceProperty & CETraceManager::PropertyDatabaseHost(void) const
{
    return mLogDbHost;
}

const CETraceProperty & CETraceManager::PropertyDatabaseName(void) const
{
    return mLogDbName;
}

const CETraceProperty & CETraceManager::PropertyDatabaseDriver(void) const
{
    return mLogDbDriver;
}

const CETraceProperty & CETraceManager::PropertyDatabaseUser(void) const
{
    return mLogDbUser;
}

const CETraceProperty & CETraceManager::PropertyDatabasePassword(void) const
{
    return mLogDbPassword;
}

bool CETraceManager::PostEvent(CEEvent & eventElem)
{
    bool result = false;
    if (RUNTIME_CAST(&eventElem, CETraceEvent) != NULL || RUNTIME_CAST(&eventElem, CETraceEvent) != NULL)
    {
        result = CEEventDispatcher::PostEvent(eventElem);
    }
    else
    {
        OUTPUT_ERR("Not a CETraceEvent type event, cannot Post. Destroying event type [ %s ]", eventElem.GetRuntimeClassName());
        eventElem.Destroy();
    }
    return result;
}
