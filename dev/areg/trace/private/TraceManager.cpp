/************************************************************************
 * \file        areg/trace/private/TraceManager.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Trace manager. The container of scopes, configuring
 *              tracer, starts and stops tracing.
 ************************************************************************/
#include "areg/trace/private/TraceManager.hpp"

#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/LogMessage.hpp"
#include "areg/trace/private/TraceEvent.hpp"

#include "areg/base/Process.hpp"
#include "areg/base/File.hpp"
#include "areg/base/FileBuffer.hpp"
#include "areg/base/EContainers.hpp"
#include "areg/base/NEString.hpp"

//////////////////////////////////////////////////////////////////////////
// TraceManager::TraceScopeMap class implementation
//////////////////////////////////////////////////////////////////////////

TraceManager::TraceScopeMap::TraceScopeMap(void)
    : TELockResourceMap<unsigned int, TraceScope, MapTraceScope, ImplTraceScope> ( )
{
    ; // do nothing
}

TraceManager::TraceScopeMap::~TraceScopeMap(void)
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TraceManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TraceManager class static members
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Tracing thread name
 **/
const char * const  TraceManager::TRACER_THREAD_NAME            = "_AREG_TRACER_THREAD_";
const char * const  TraceManager::DEFAULT_LOG_VERSION           = "log.version          = 1.0.0";
const char * const  TraceManager::DEFAULT_LOG_ENABLE            = "log.enable           = true";
const char * const  TraceManager::DEFAULT_LOG_FILE              = "log.file             = ./logs/trace_%time%.log";
const char * const  TraceManager::DEFAULT_LOG_APPEND            = "log.append           = false";
const char * const  TraceManager::DEFAULT_LOG_LAYOUT_ENTER      = "log.layout.enter     = %d: [ %t  %x.%z: Enter --> ]%n";
const char * const  TraceManager::DEFAULT_LOG_LAYOUT_MESSAGE    = "log.layout.message   = %d: [ %t  %p >>> ] %m%n";
const char * const  TraceManager::DEFAULT_LOG_LAYOUT_EXIT       = "log.layout.exit      = %d: [ %t  %x.%z: Exit <-- ]%n";
const char * const  TraceManager::DEFAULT_LOG_LAYOUT_DEBUG      = "log.debug            = true";
const unsigned int  TraceManager::LOG_START_WAITING_TIME        = IESynchObject::WAIT_1_SEC * 20;

const char * const  TraceManager::LOG_SCOPES_ALL                = "*";
const char * const  TraceManager::LOG_SCOPES_SELF               = "areg_*";

const TraceManager::sLogEnabling    TraceManager::DEFAULT_LOG_ENABLED_SCOPES[]   = 
{
      { TraceManager::LOG_SCOPES_ALL    , static_cast<unsigned int>(NETrace::PrioDebug ) | static_cast<unsigned int>(NETrace::PrioScope)}
    , { TraceManager::LOG_SCOPES_SELF   , static_cast<unsigned int>(NETrace::PrioNotset)                                                }
    , { NULL /* must end with NULL */   , static_cast<unsigned int>(NETrace::PrioNotset)                                                }
};

//////////////////////////////////////////////////////////////////////////
// TraceManager class static methods
//////////////////////////////////////////////////////////////////////////

TraceManager & TraceManager::getInstance( void )
{
    static TraceManager _traceManager;
    return _traceManager;
}

unsigned int TraceManager::makeScopeId( const char * scopeName )
{
    return  NEMath::crc32Calculate(scopeName);
}

void TraceManager::sendLogMessage( LogMessage & logData )
{
    TraceManager & tracer = TraceManager::getInstance();
    logData.setModuleId( tracer.mModuleId );
    tracer._sendLogEvent( TraceEventData(TraceEventData::TraceMessage, logData) );
}

inline void TraceManager::_sendLogEvent( const TraceEventData & data )
{
    TraceEvent::sendEvent( data, static_cast<IETraceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );
}

bool TraceManager::startLogging( const char * configFile /*= NULL*/ )
{
    TraceManager & traceManager = TraceManager::getInstance();
    bool canStart = false;

    do
    {
        Lock lock(traceManager.mLock);
        if ( NEString::isEmpty<char>(configFile) == false )
        {
            canStart = traceManager.isReady() == false ? traceManager.loadConfiguration( configFile) : false;
        }
        else if ( TraceManager::isLoggingConfigured()== false )
        {
            canStart = traceManager.isReady() == false ? traceManager.loadConfiguration( NULL ) : false;
        }
        else
        {
            canStart = traceManager.isReady() == false;
        }

    } while (false);

    return (canStart ? traceManager.startLoggingThread() : traceManager.mIsStarted);
}

void TraceManager::stopLogging( unsigned int waitTimeout /*= Thread::WAIT_INFINITE*/ )
{
    getInstance().stopLoggingThread(waitTimeout);
}

void TraceManager::registerTraceScope( TraceScope & scope )
{
    getInstance()._registerScope(scope);
}

void TraceManager::unregisterTraceScope( TraceScope & scope )
{
    getInstance()._unregisterScope(scope);
}

void TraceManager::activateTraceScope( TraceScope & scope )
{
    getInstance().activateScope(scope);
}

bool TraceManager::configureLogging(const char * configFile /*= NULL */)
{
    return TraceManager::getInstance().loadConfiguration(configFile);
}

bool TraceManager::isLoggingConfigured(void)
{
    bool result = false;
    TraceManager & traceManager = TraceManager::getInstance();
    do 
    {
        Lock lock(traceManager.mLock);
        result = (traceManager.mConfigFile.isEmpty() == false) && (traceManager.mPropertyList.isEmpty() == false);
    } while (false);
    return result;
}

bool TraceManager::isLoggingEnabled(void)
{
    TraceManager & traceManager = TraceManager::getInstance();
    Lock lock(traceManager.mLock);
    return traceManager._isEnabled();
}

bool TraceManager::forceActivateLogging(void)
{
    bool result = false;
    TraceManager & traceManager = TraceManager::getInstance();
    if ( (traceManager.isLoggingConfigured() == false) && (traceManager.isLoggingStarted() == false) )
    {
        result = traceManager.activateTracingDefaults();
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// TraceManager class constructor / destructor
//////////////////////////////////////////////////////////////////////////
TraceManager::TraceManager(void)
    : DispatcherThread    ( TraceManager::TRACER_THREAD_NAME )
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

    , mModuleId         ( Process::getInstance().getId() )

    , mLoggerFile       ( static_cast<IETraceConfiguration &>(self()) )
    , mLoggerDebug      ( static_cast<IETraceConfiguration &>(self()) )

    , mLogStarted       ( false, false )
    , mLock             ( )
{
    ; // do nothing
}


TraceManager::~TraceManager(void)
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TraceManager class methods
//////////////////////////////////////////////////////////////////////////
void TraceManager::_registerScope( TraceScope & scopeRegister )
{
#ifdef DEBUG
    if ( mMapTraceScope.findResourceObject( static_cast<unsigned int>(scopeRegister)) != NULL )
    {
        OUTPUT_ERR("The scope [ %s ] is already registered", scopeRegister.getScopeName());
    }
#endif // DEBUG

    ASSERT(mMapTraceScope.findResourceObject( static_cast<unsigned int>(scopeRegister)) == NULL);
    mMapTraceScope.registerResourceObject( static_cast<unsigned int>(scopeRegister), &scopeRegister);
}

void TraceManager::_unregisterScope(TraceScope & scopeUnregister)
{
    mMapTraceScope.unregisterResourceObject( static_cast<unsigned int>(scopeUnregister));
}

bool TraceManager::_isScopeRegistered(unsigned int scopeId) const
{
    return _getScope( scopeId ) != NULL ;
}

bool TraceManager::_isScopeRegistered(const char * scopeName) const
{
    return _isScopeRegistered( TraceManager::makeScopeId(scopeName) );
}

bool TraceManager::_isScopeRegistered(const TraceScope & scope) const
{
    return _isScopeRegistered( scope.getScopeId() );
}

void TraceManager::setScopePriority( unsigned int scopeId, unsigned int newPrio )
{
    mMapTraceScope.lock();

    TraceScope * scope = mMapTraceScope.findResourceObject(scopeId);
    if ( scope != NULL )
        scope->setPriority(newPrio);

    mMapTraceScope.unlock();
}

void TraceManager::addScopePriority(unsigned int scopeId, NETrace::eLogPriority addPrio)
{
    mMapTraceScope.lock();

    TraceScope * scope = mMapTraceScope.findResourceObject(scopeId);
    if ( scope != NULL )
        scope->addPriority(addPrio);

    mMapTraceScope.unlock();
}

void TraceManager::removeScopePriority(unsigned int scopeId, NETrace::eLogPriority remPrio)
{
    mMapTraceScope.lock();

    TraceScope * scope = mMapTraceScope.findResourceObject(scopeId);
    if ( scope != NULL )
        scope->removePriority(remPrio);

    mMapTraceScope.unlock();
}

int TraceManager::setScopeGroupPriority(const char * scopeGroupName, unsigned int newPrio)
{
    int result = 0;
    if ( NEString::isEmpty<char>(scopeGroupName) == false )
    {
        mMapTraceScope.lock();

        unsigned int scopeId    = 0;
        for ( TraceScope * scope = mMapTraceScope.resourceFirstKey(scopeId); scope != NULL; scope = mMapTraceScope.resourceNextKey(scopeId) )
        {
            if ( NEString::compareStrings<char, char>(scopeGroupName, scope->getScopeName(), NEString::CountAll, false ) == 0 )
            {
                scope->setPriority(newPrio);
                ++ result;
            }
        }

        mMapTraceScope.unlock();
    }
    return result;
}

int TraceManager::addScopeGroupPriority(const char * scopeGroupName, NETrace::eLogPriority addPrio)
{
    int result = 0;
    if ( NEString::isEmpty<char>(scopeGroupName) == false )
    {
        mMapTraceScope.lock();

        unsigned int scopeId    = 0;
        for ( TraceScope * scope = mMapTraceScope.resourceFirstKey(scopeId); scope != NULL; scope = mMapTraceScope.resourceNextKey(scopeId) )
        {
            if ( NEString::compareStrings<char, char>( scopeGroupName, scope->getScopeName( ), NEString::CountAll, false ) == 0 )
            {
                scope->addPriority(addPrio);
                ++ result;
            }
        }

        mMapTraceScope.unlock();
    }
    return result;
}

int TraceManager::removeScopeGroupPriority(const char * scopeGroupName, NETrace::eLogPriority remPrio)
{
    int result = 0;
    if ( NEString::isEmpty<char>(scopeGroupName) == false )
    {
        mMapTraceScope.lock();

        unsigned int scopeId    = 0;
        for ( TraceScope * scope = mMapTraceScope.resourceFirstKey(scopeId); scope != NULL; scope = mMapTraceScope.resourceNextKey(scopeId) )
        {
            if ( NEString::compareStrings<char, char>( scopeGroupName, scope->getScopeName( ), NEString::CountAll, false ) == 0 )
            {
                scope->removePriority(remPrio);
                ++ result;
            }
        }

        mMapTraceScope.unlock();
    }
    return result;
}

bool TraceManager::loadConfiguration( const char * filePath /*= NULL */ )
{
    Lock lock(mLock);

    mConfigFile = File::getFileFullPath( NEString::isEmpty<char>(filePath) == false ? filePath : NELogConfig::DEFAULT_CONFIG_FILE_NAME );
    File fileConfig( static_cast<const char *>(mConfigFile), FileBase::FO_MODE_EXIST | FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_SHARE_READ );
    fileConfig.open( );

    return loadConfiguration( fileConfig );
}

bool TraceManager::loadConfiguration( const FileBase & configFile )
{
    Lock lock(mLock);

    clearConfigData();
    if ( configFile.isOpened() )
    {
        mConfigFile = configFile.getName();

        String line;
        TraceProperty newProperty;
        while ( configFile.readLine( line ) > 0 )
        {
            if ( newProperty.parseProperty(line) )
            {
                // search, whether the property is already set
                LISTPOS pos = mPropertyList.find(newProperty, NULL);
                if ( pos != NULL )
                    mPropertyList.setAt(pos, newProperty);
                else
                    mPropertyList.pushLast(newProperty);

                newProperty.clearProperty();
            }
        }
    }

    return (mPropertyList.isEmpty() == false) && initializeConfig();
}

void TraceManager::clearConfigData( void )
{
    Lock lock(mLock);

    mLogVersion.clearProperty();

    mLogLayoutEnter.clearProperty();
    mLogLayoutMessage.clearProperty();
    mLogLayoutExit.clearProperty();
    mLogStackSize.clearProperty();
    mLogDebugOutput.clearProperty();
    mLogStatus.clearProperty();

    mLogFile.clearProperty();

    mLogRemoteHost.clearProperty();
    mLogRemotePort.clearProperty();

    mLogDbHost.clearProperty();
    mLogDbName.clearProperty();
    mLogDbDriver.clearProperty();
    mLogDbUser.clearProperty();
    mLogDbPassword.clearProperty();

    mConfigScopeList.removeAll();
    mConfigScopeGroup.removeAll();
    mPropertyList.removeAll();
}

bool TraceManager::activateTracingDefaults( void )
{
    do 
    {
        Lock lock(mLock);

        mLogVersion.parseProperty( DEFAULT_LOG_VERSION );
        mLogLayoutEnter.parseProperty( DEFAULT_LOG_LAYOUT_ENTER );
        mLogLayoutMessage.parseProperty( DEFAULT_LOG_LAYOUT_MESSAGE );
        mLogLayoutExit.parseProperty( DEFAULT_LOG_LAYOUT_EXIT );
        mLogDebugOutput.parseProperty( DEFAULT_LOG_LAYOUT_DEBUG );
        mLogStatus.parseProperty( DEFAULT_LOG_ENABLE );
        mLogAppendData.parseProperty( DEFAULT_LOG_APPEND );
        mLogFile.parseProperty( DEFAULT_LOG_FILE );

        mLogStackSize.clearProperty();
        mLogRemoteHost.clearProperty();
        mLogRemotePort.clearProperty();
        mLogDbHost.clearProperty();
        mLogDbName.clearProperty();
        mLogDbDriver.clearProperty();
        mLogDbUser.clearProperty();
        mLogDbPassword.clearProperty();

        int i = 0;
        do 
        {
            const sLogEnabling & scopes = TraceManager::DEFAULT_LOG_ENABLED_SCOPES[i ++];
            if ( scopes.logScope == NULL)
                break;

            mConfigScopeGroup.setAt(scopes.logScope, scopes.logPrio);
        } while (true);

    } while (false);

    return startLoggingThread();
}

bool TraceManager::initializeConfig(void)
{
    Lock lock(mLock);

    String moduleName      = Process::getInstance().getAppName();
    for ( LISTPOS pos = mPropertyList.firstPosition(); pos != NULL; pos = mPropertyList.nextPosition(pos) )
    {
        const TraceProperty &     prop    = mPropertyList.getAt(pos);
        const TracePropertyKey &  Key     = prop.getKey();
        const TracePropertyValue& Value   = prop.getValue();

        // For any property, either global setting is set, or local
        if ( Key.isModuleKeySet(moduleName) )
        {
            switch ( Key.getLogConfig() )
            {
            case NELogConfig::ConfigLogVersion:
                mLogVersion = prop;
                break;

            case NELogConfig::ConfigLogFile:
                if ( mLogFile.getKey().isLocalKey() == false || mLogFile.getKey().getModule() == Key.getModule() )
                {
                    mLogFile    = prop;
                    if ( mLogFile.getValue().isValid() == false )
                        mLogFile.getValue() = NELogConfig::DEFAULT_LOG_FILE_NAME;
                }
                break;

            case NELogConfig::ConfigLogRemoteHost:
                if ( mLogRemoteHost.getKey().isLocalKey() == false || mLogRemoteHost.getKey().getModule() == Key.getModule() )
                    mLogRemoteHost  = prop;
                break;

            case NELogConfig::ConfigLogRemotePort:
                if ( mLogRemotePort.getKey().isLocalKey() == false || mLogRemotePort.getKey().getModule() == Key.getModule() )
                    mLogRemotePort  = prop;
                break;

            case NELogConfig::ConfigLogDatabaseDriver:
                if ( mLogDbDriver.getKey().isLocalKey() == false || mLogDbDriver.getKey().getModule() == Key.getModule() )
                    mLogDbDriver = prop;
                break;

            case NELogConfig::ConfigLogDatabaseHost:
                if ( mLogDbHost.getKey().isLocalKey() == false || mLogDbHost.getKey().getModule() == Key.getModule() )
                    mLogDbHost = prop;
                break;

            case NELogConfig::ConfigLogDatabaseUser:
                if ( mLogDbUser.getKey().isLocalKey() == false || mLogDbUser.getKey().getModule() == Key.getModule() )
                    mLogDbUser = prop;
                break;

            case NELogConfig::ConfigLogDatabasePwd:
                if ( mLogDbPassword.getKey().isLocalKey() == false || mLogDbPassword.getKey().getModule() == Key.getModule() )
                    mLogDbPassword = prop;
                break;

            case NELogConfig::ConfigLogDatabaseName:
                if ( mLogDbName.getKey().isLocalKey() == false || mLogDbName.getKey().getModule() == Key.getModule() )
                    mLogDbName = prop;
                break;

            case NELogConfig::ConfigLogDebug:
                if ( mLogDebugOutput.getKey().isLocalKey() == false || mLogDebugOutput.getKey().getModule() == Key.getModule() )
                    mLogDebugOutput = prop;
                break;

            case NELogConfig::ConfigLogAppend:
                if ( mLogAppendData.getKey().isLocalKey() == false || mLogAppendData.getKey().getModule() == Key.getModule() )
                    mLogAppendData = prop;
                break;

            case NELogConfig::ConfigLogStack:
                if ( mLogStackSize.getKey().isLocalKey() == false || mLogStackSize.getKey().getModule() == Key.getModule() )
                    mLogStackSize = prop;
                break;

            case NELogConfig::ConfigLogEnable:
                if ( mLogStatus.getKey().isLocalKey() == false || mLogStatus.getKey().getModule() == Key.getModule() )
                    mLogStatus = prop;
                break;

            case NELogConfig::ConfigLogLayoutEnter:
                if ( mLogLayoutEnter.getKey().isLocalKey() == false || mLogLayoutEnter.getKey().getModule() == Key.getModule() )
                    mLogLayoutEnter = prop;
                break;

            case NELogConfig::ConfigLogLayoutMessage:
                if ( mLogLayoutMessage.getKey().isLocalKey() == false || mLogLayoutMessage.getKey().getModule() == Key.getModule() )
                    mLogLayoutMessage = prop;
                break;

            case NELogConfig::ConfigLogLayoutExit:
                if ( mLogLayoutExit.getKey().isLocalKey() == false || mLogLayoutExit.getKey().getModule() == Key.getModule() )
                    mLogLayoutExit = prop;
                break;

            case NELogConfig::ConfigScope:
                {
                    ASSERT( Key.isValidKey( ) );
                    unsigned int prio = Value.getPriority();
                    if (Key.isGlobalKey())
                    {
                        ASSERT(Key.getModule().isEmpty());
                        mConfigScopeGroup.setAt(NELogConfig::MODULE_SCOPE, prio);
                    }
                    else if ( Key.getModuleData( ).findLastOf( NELogConfig::SYNTAX_SCOPE_GROUP ) != NEString::InvalidPos )
                    {
                        mConfigScopeGroup.setAt( Key.getModuleData( ), prio);
                    }
                    else
                    {
                        mConfigScopeList.setAt( Key.getModuleData( ), prio);
                    }
                }
                break;

            default:
                break;  // do nothing
            }
        }
    }

    return _isValid( );
}

bool TraceManager::_isValid(void) const
{
    return (mLogVersion.isValid() ? (isFileValid() || isHostValid() || isDebugOutputValid() || isDatabaseValid()) : false);
}

bool TraceManager::_isEnabled(void) const
{
    return mLogStatus.getKey().isValidKey() ? static_cast<bool>(mLogStatus.getValue()) : NELogConfig::DEFAULT_LOG_ENABLED;
}

bool TraceManager::isHostValid(void) const
{
    return (mLogRemoteHost.isValid() || mLogRemotePort.isValid());
}

bool TraceManager::isDatabaseValid(void) const
{
    return ( mLogDbHost.isValid() && mLogDbName.isValid() && mLogDbDriver.isValid() );
}

bool TraceManager::isFileValid(void) const
{
    return mLogFile.isValid();
}

bool TraceManager::isDebugOutputValid(void) const
{
    return mLogDebugOutput.isValid();
}

bool TraceManager::startLoggingThread(void)
{
    bool result = false;
    ASSERT((isRunning() == false) && (isReady() == false));
    if ( createThread(Thread::WAIT_INFINITE) )
    {
        // SetScopesActivity(true);
        if ( waitForDispatcherStart(IESynchObject::WAIT_INFINITE) )
        {
            mLogStarted.resetEvent();

            _sendLogEvent( TraceEventData(TraceEventData::TraceStartLogs) );

            // result = mLogStarted.lock(TraceManager::LOG_START_WAITING_TIME);  // <= wait max 10 secs.
            result = mLogStarted.lock(IESynchObject::WAIT_INFINITE);
        }
    }
    else
    {
        OUTPUT_ERR("Failed to create [ %s ] System Timer Thread", TraceManager::TRACER_THREAD_NAME);
    }

    return result;
}

void TraceManager::stopLoggingThread( unsigned int waitTimeout )
{
    // RemoveAllEvents(false);
    _sendLogEvent( TraceEventData(TraceEventData::TraceStopLogs) );
    completionWait( isRunning() ? waitTimeout : Thread::DO_NOT_WAIT );
}

void TraceManager::processEvent(const TraceEventData & data)
{
    const SharedBuffer & stream = data.getReadableStream();

    switch ( data.getTraceAction() )
    {
    case TraceEventData::TraceConfigure:
        {
            loadConfiguration( FileBuffer(stream, "TraceConfigure") );
        }
        break;

    case TraceEventData::TraceChangeConfig:
        {
            ; // not implemented yet
        }
        break;

    case TraceEventData::TraceStartLogs:
        {
            traceStartLogs( );
            mLogStarted.setEvent();
        }
        break;

    case TraceEventData::TraceStopLogs:
        {
            traceStopLogs( );
            triggerExitEvent();
            mLogStarted.resetEvent();
        }
        break;

    case TraceEventData::TraceEnableLogs:
        {
            static_cast<TracePropertyValue &>(mLogStatus.getValue()) = true;
        }
        break;

    case TraceEventData::TraceDisableLogs:
        {
            static_cast<TracePropertyValue &>(mLogStatus.getValue()) = false;
        }
        break;

    case TraceEventData::TraceChangeScopes:
        {
            SortedStringList list;
            stream >> list;
            onUpdateScopes( list );
        }
        break;

    case TraceEventData::TraceMessage:
        {
            traceMessage( LogMessage(static_cast<const IEInStream &>(stream)) );
        }
        break;

    default:
        break; // ignore, do nothing
    }
}

bool TraceManager::runDispatcher(void)
{
    TraceEvent::addListener(static_cast<IETraceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));
    bool result = DispatcherThread::runDispatcher();

    mLoggerFile.closeLogger();
    mLoggerDebug.closeLogger();

    TraceEvent::removeListener(static_cast<IETraceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));

    return result;
}

void TraceManager::traceStartLogs( void )
{
    if ( _isValid() && _isEnabled() )
    {
        setScopesActivity( true );
        mLoggerFile.openLogger();
#if defined(_OUTPUT_DEBUG)
        mLoggerDebug.openLogger();
#endif // !defined(_OUTPUT_DEBUG)

        mIsStarted = true;
    }
}

void TraceManager::setScopesActivity( bool makeActive )
{
    if ( _isValid() )
    {
        mMapTraceScope.lock();
        if ( _isEnabled() && makeActive && (mScopesActivated == false) )
        {
            unsigned int defaultPrio= NELogConfig::DEFAULT_LOG_PRIORITY;
            mConfigScopeGroup.find( NELogConfig::MODULE_SCOPE,  defaultPrio);

            for ( MAPPOS pos = mMapTraceScope.firstPosition(); pos != NULL; pos = mMapTraceScope.nextPosition(pos) )
            {
                activateScope(*mMapTraceScope.valueAtPosition(pos), defaultPrio);
            }

            mScopesActivated    = true;
        }
        else if ( (makeActive == false) && mScopesActivated )
        {
            for ( MAPPOS pos = mMapTraceScope.firstPosition(); pos != NULL; pos = mMapTraceScope.nextPosition(pos) )
            {
                mMapTraceScope.valueAtPosition(pos)->setPriority( NETrace::PrioNotset );
            }

            mScopesActivated    = false;
        }

        mMapTraceScope.unlock();
    }
}

void TraceManager::traceStopLogs(void)
{
    setScopesActivity( false );
}

void TraceManager::onUpdateScopes(SortedStringList & scopeList)
{
    mMapTraceScope.lock();

    String moduleName = Process::getInstance().getAppName();

    while ( scopeList.isEmpty() == false )
    {
        TraceProperty scopeProperty;
        String line = scopeList.removeLast();
        if ( scopeProperty.parseProperty( line ) )
        {
            const TracePropertyKey &  Key     = scopeProperty.getKey();
            const TracePropertyValue & Value  = scopeProperty.getValue();
            if ( Key.getLogConfig() == NELogConfig::ConfigScope)
            {
                // first, save property in property list
                LISTPOS pos = mPropertyList.find(scopeProperty, NULL);
                if ( pos != NULL )
                {
                    mPropertyList.setAt(pos, scopeProperty);
                }
                else
                {
                    mPropertyList.pushLast(scopeProperty);
                }

                const String & moduleData = Key.getModuleData();
                // update scope list / or group scope only for this module
                if ( Key.getModule() == moduleName )
                {
                    NETrace::eLogPriority prio = Value;
                    setScopesPriority( moduleData, prio);
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

    mMapTraceScope.unlock();
}

void TraceManager::traceMessage( const LogMessage & logMessage )
{
    mLoggerFile.logMessage( static_cast<const NETrace::sLogMessage &>(logMessage) );
    mLoggerDebug.logMessage( static_cast<const NETrace::sLogMessage &>(logMessage) );

    if ( hasMoreEvents() == false )
    {
        mLoggerFile.flushLogs();
    }
}

void TraceManager::activateScope( TraceScope & traceScope )
{
    Lock lock(mLock);

    unsigned int defaultPrio= NELogConfig::DEFAULT_LOG_PRIORITY;
    if ( mConfigScopeGroup.find( NELogConfig::MODULE_SCOPE,  defaultPrio) == false )
        defaultPrio = NELogConfig::DEFAULT_LOG_PRIORITY;
    activateScope(traceScope, defaultPrio);
}

void TraceManager::activateScope( TraceScope & traceScope, unsigned int defaultPriority )
{
    Lock lock(mLock);

    traceScope.setPriority( defaultPriority ); // set first default priority
    const char * scopeName = traceScope.getScopeName();
    unsigned int scopePrio = defaultPriority;

    if ( mConfigScopeList.find(scopeName, scopePrio) )
    {
        traceScope.setPriority( scopePrio ); // exact match. Set priority
    }
    else
    {

        char groupName[NETrace::LOG_MESSAGE_BUFFER_SIZE];
        NEString::CharCount end = NEString::copyString<char>(groupName, NETrace::LOG_MESSAGE_BUFFER_SIZE, scopeName);

        char * separator = groupName + end;
        do 
        {
            *separator= '\0';
            NEString::CharPos pos = NEString::findLastOf<char>(NELogConfig::SYNTAX_SCOPE_SEPARATOR, groupName, NEString::EndPos);
            if (pos != NEString::InvalidPos)
            {
                separator = groupName + pos;
                separator[1] = NELogConfig::SYNTAX_SCOPE_GROUP; // replace group syntax
                separator[2] = NEString::EndOfString;           // set end of string
            }
            else
            {
                // Check whether the entire module scopes are enabled
                groupName[0] = NELogConfig::SYNTAX_SCOPE_GROUP; // set in the first position
                groupName[1] = NEString::EndOfString;           // set end of string
                separator    = NULL;                            // set NULL to escape loop if needed.
            }

            if ( mConfigScopeGroup.find(groupName, scopePrio) )
            {
                // Found group priority, set it
                traceScope.setPriority( scopePrio );
                separator = NULL;
            }

        } while (separator != NULL);

    }
}

void TraceManager::setScopesPriority(const char * scopeName, unsigned int logPriority)
{
    if ( NEString::isEmpty<char>(scopeName) == false )
    {
        NEString::CharPos pos = NEString::findLastOf<char>( NELogConfig::SYNTAX_SCOPE_SEPARATOR, scopeName, NEString::EndPos );
        const char * syntaxGroup = pos != NEString::InvalidPos ? scopeName + pos : EMPTY_STRING;
        if ( syntaxGroup == NULL )
        {
            // there is a complete scope name, make changes.
            TraceScope * traceScope   = NULL;
            mConfigScopeList.setAt(scopeName, static_cast<unsigned int>(logPriority));
            if ( mMapTraceScope.find(TraceManager::makeScopeId(scopeName), traceScope) && (traceScope != NULL) )
                traceScope->setPriority( logPriority );
        }
        else if ( syntaxGroup == scopeName )
        {
            // change all, it is set group change for complete module
            mConfigScopeGroup.setAt(scopeName, static_cast<unsigned int>(logPriority));
            for ( MAPPOS mapPos = mMapTraceScope.firstPosition(); mapPos != NULL; mapPos = mMapTraceScope.nextPosition(mapPos) )
                mMapTraceScope.valueAtPosition(mapPos)->setPriority( logPriority );
        }
        else if ( *(-- syntaxGroup) == NELogConfig::SYNTAX_SCOPE_SEPARATOR )
        {
            // it is a group scope, update all groups
            mConfigScopeGroup.setAt(scopeName, static_cast<unsigned int>(logPriority));
            int len = static_cast<int>(syntaxGroup - scopeName);
            for ( MAPPOS mapPos = mMapTraceScope.firstPosition(); mapPos != NULL; mapPos = mMapTraceScope.nextPosition(mapPos) )
            {
                TraceScope * traceScope = mMapTraceScope.valueAtPosition(mapPos);
                if ( NEString::compareStrings<char, char>(scopeName, traceScope->getScopeName(), len) == 0 )
                    traceScope->setPriority( logPriority );
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

const TraceProperty & TraceManager::propertyVersion(void) const
{
    return mLogVersion;
}

const TraceProperty & TraceManager::propertyLayoutEnter(void) const
{
    return mLogLayoutEnter;
}

const TraceProperty & TraceManager::propertyLayoutMessage(void) const
{
    return mLogLayoutMessage;
}

const TraceProperty & TraceManager::propertyLayoutExit(void) const
{
    return mLogLayoutExit;
}

const TraceProperty & TraceManager::propertyStackSize(void) const
{
    return mLogStackSize;
}

const TraceProperty & TraceManager::propertyDebugOutput(void) const
{
    return mLogDebugOutput;
}

const TraceProperty & TraceManager::propertyStatus(void) const
{
    return mLogStatus;
}

const TraceProperty & TraceManager::propertyAppendData(void) const
{
    return mLogAppendData;
}

const TraceProperty & TraceManager::propertyLogFile(void) const
{
    return mLogFile;
}

const TraceProperty & TraceManager::propertyRemoteHost(void) const
{
    return mLogRemoteHost;
}

const TraceProperty & TraceManager::propertyRemotePort(void) const
{
    return mLogRemotePort;
}

const TraceProperty & TraceManager::propertyDatabaseHost(void) const
{
    return mLogDbHost;
}

const TraceProperty & TraceManager::propertyDatabaseName(void) const
{
    return mLogDbName;
}

const TraceProperty & TraceManager::propertyDatabaseDriver(void) const
{
    return mLogDbDriver;
}

const TraceProperty & TraceManager::propertyDatabaseUser(void) const
{
    return mLogDbUser;
}

const TraceProperty & TraceManager::propertyDatabasePassword(void) const
{
    return mLogDbPassword;
}

bool TraceManager::postEvent(Event & eventElem)
{
    bool result = false;
    if (RUNTIME_CAST(&eventElem, TraceEvent) != NULL || RUNTIME_CAST(&eventElem, TraceEvent) != NULL)
    {
        result = EventDispatcher::postEvent(eventElem);
    }
    else
    {
        OUTPUT_ERR("Not a TraceEvent type event, cannot Post. Destroying event type [ %s ]", eventElem.getRuntimeClassName());
        eventElem.destroy();
    }
    return result;
}
