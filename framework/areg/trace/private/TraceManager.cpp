/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/TraceManager.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
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
#include "areg/base/Containers.hpp"
#include "areg/base/NEString.hpp"

//////////////////////////////////////////////////////////////////////////
// TraceManager::TraceScopeMap class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TraceManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TraceManager class static members
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Tracing thread name
 **/
const TraceManager::sLogEnabling    TraceManager::DEFAULT_LOG_ENABLED_SCOPES[]   = 
{
      { TraceManager::LOG_SCOPES_ALL.data() , static_cast<unsigned int>(NETrace::PrioDebug ) | static_cast<unsigned int>(NETrace::PrioScope)}
    , { TraceManager::LOG_SCOPES_SELF.data(), static_cast<unsigned int>(NETrace::PrioNotset)                                                }
    , { nullptr /* must end with nullptr */ , static_cast<unsigned int>(NETrace::PrioNotset)                                                }
};

//////////////////////////////////////////////////////////////////////////
// TraceManager class static methods
//////////////////////////////////////////////////////////////////////////

TraceManager & TraceManager::getInstance( void )
{
    static TraceManager	_theTraceManager;
    return _theTraceManager;
}

unsigned int TraceManager::makeScopeId( const char * scopeName )
{
    return  NEMath::crc32Calculate(scopeName);
}

void TraceManager::sendLogMessage( LogMessage & logData )
{
    TraceManager & tracer = TraceManager::getInstance();
    logData.setModuleId( tracer.mModuleId );
    tracer._sendLogEvent( TraceEventData(TraceEventData::eTraceAction::TraceMessage, logData) );
}

inline void TraceManager::_sendLogEvent( const TraceEventData & data )
{
    TraceEvent::sendEvent( data, static_cast<IETraceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );
}

bool TraceManager::startLogging( const char * configFile /*= nullptr*/, unsigned int waitTimeout /*= NECommon::WAIT_INFINITE*/ )
{
    TraceManager & traceManager = TraceManager::getInstance();
    bool canStart = false;

    do
    {
        Lock lock(traceManager.mLock);
        if (TraceManager::isLoggingConfigured() == false)
        {
        	if (traceManager.isReady( ) == false)
        	{
                canStart = traceManager.loadConfiguration( configFile);
            }
        }
        else
        {
        	canStart = traceManager.isReady( ) == false;
        }

    } while (false);

    return (canStart ? traceManager.startLoggingThread( waitTimeout ) : traceManager.mIsStarted);
}

void TraceManager::stopLogging( unsigned int waitTimeout /*= NECommon::WAIT_INFINITE*/ )
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

bool TraceManager::configureLogging(const char * configFile /*= nullptr */)
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

const String& TraceManager::getConfigFile(void)
{
    TraceManager& traceManager = TraceManager::getInstance();
    Lock lock(traceManager.mLock);
    return traceManager.mConfigFile;
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

void TraceManager::forceEnableLogging(void)
{
    TraceManager::getInstance().mLogConfig.getStatus().parseProperty( NELogConfig::DEFAULT_LOG_ENABLE.data() );
}

//////////////////////////////////////////////////////////////////////////
// TraceManager class constructor / destructor
//////////////////////////////////////////////////////////////////////////
TraceManager::TraceManager(void)
    : DispatcherThread      ( TraceManager::TRACER_THREAD_NAME.data() )
    , IETraceEventConsumer  ( )

    , mMapTraceScope    ( )
    , mScopesActivated  ( false )
	, mIsStarted		( false )

    , mConfigFile       ( )
    , mLogConfig        ( )

    , mConfigScopeList  ( )
    , mConfigScopeGroup ( )
    , mPropertyList     ( )

    , mModuleId         ( Process::getInstance().getId() )

    , mLoggerFile       ( mLogConfig )
    , mLoggerDebug      ( mLogConfig )

    , mLogStarted       ( false, false )
    , mLock             ( )
{
}

//////////////////////////////////////////////////////////////////////////
// TraceManager class methods
//////////////////////////////////////////////////////////////////////////
void TraceManager::_registerScope( TraceScope & scopeRegister )
{
#ifdef DEBUG
    if ( mMapTraceScope.findResourceObject( static_cast<unsigned int>(scopeRegister)) != nullptr )
    {
        OUTPUT_ERR("The scope [ %s ] is already registered", scopeRegister.getScopeName());
    }
#endif // DEBUG

    ASSERT(mMapTraceScope.findResourceObject( static_cast<unsigned int>(scopeRegister)) == nullptr);
    mMapTraceScope.registerResourceObject( static_cast<unsigned int>(scopeRegister), &scopeRegister);
}

void TraceManager::_unregisterScope(TraceScope & scopeUnregister)
{
    mMapTraceScope.unregisterResourceObject( static_cast<unsigned int>(scopeUnregister));
}

bool TraceManager::_isScopeRegistered(unsigned int scopeId) const
{
    return _getScope( scopeId ) != nullptr ;
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
    if ( scope != nullptr )
    {
        scope->setPriority( newPrio );
    }

    mMapTraceScope.unlock();
}

void TraceManager::addScopePriority(unsigned int scopeId, NETrace::eLogPriority addPrio)
{
    mMapTraceScope.lock();

    TraceScope * scope = mMapTraceScope.findResourceObject(scopeId);
    if ( scope != nullptr )
        scope->addPriority(addPrio);

    mMapTraceScope.unlock();
}

void TraceManager::removeScopePriority(unsigned int scopeId, NETrace::eLogPriority remPrio)
{
    mMapTraceScope.lock();

    TraceScope * scope = mMapTraceScope.findResourceObject(scopeId);
    if ( scope != nullptr )
        scope->removePriority(remPrio);

    mMapTraceScope.unlock();
}

int TraceManager::setScopeGroupPriority(const String & scopeGroupName, unsigned int newPrio)
{
    int result = 0;
    if (scopeGroupName.isEmpty() == false)
    {
        mMapTraceScope.lock();

        unsigned int scopeId    = 0;
        for ( TraceScope * scope = mMapTraceScope.resourceFirstKey(scopeId); scope != nullptr; scope = mMapTraceScope.resourceNextKey(scopeId) )
        {
            if ( scopeGroupName.compare(scope->getScopeName(), false) == NEMath::eCompare::Equal )
            {
                scope->setPriority(newPrio);
                ++ result;
            }
        }

        mMapTraceScope.unlock();
    }
    return result;
}

int TraceManager::addScopeGroupPriority(const String & scopeGroupName, NETrace::eLogPriority addPrio)
{
    int result{ 0 };
    if ( scopeGroupName.isEmpty() == false)
    {
        mMapTraceScope.lock();

        unsigned int scopeId{ 0 };
        for ( TraceScope * scope = mMapTraceScope.resourceFirstKey(scopeId); scope != nullptr; scope = mMapTraceScope.resourceNextKey(scopeId) )
        {
            if ( NEString::compareStrings<char, char>( scopeGroupName, scope->getScopeName( ), NEString::COUNT_ALL, false ) == NEMath::eCompare::Equal)
            {
                scope->addPriority(addPrio);
                ++ result;
            }
        }

        mMapTraceScope.unlock();
    }
    return result;
}

int TraceManager::removeScopeGroupPriority(const String& scopeGroupName, NETrace::eLogPriority remPrio)
{
    int result{ 0 };
    if ( scopeGroupName.isEmpty() == false)
    {
        mMapTraceScope.lock();

        unsigned int scopeId{ 0 };
        for ( TraceScope * scope = mMapTraceScope.resourceFirstKey(scopeId); scope != nullptr; scope = mMapTraceScope.resourceNextKey(scopeId) )
        {
            if (scopeGroupName.compare(scope->getScopeName(), false) == NEMath::eCompare::Equal)
            {
                scope->removePriority(remPrio);
                ++ result;
            }
        }

        mMapTraceScope.unlock();
    }
    return result;
}

bool TraceManager::loadConfiguration( const char * filePath /*= nullptr */ )
{
    Lock lock(mLock);

    mConfigFile = File::getFileFullPath( NEString::isEmpty<char>(filePath) ? NEApplication::DEFAULT_TRACING_CONFIG_FILE.data() : filePath);
    File fileConfig( mConfigFile, FileBase::FO_MODE_EXIST | FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_SHARE_READ );
    fileConfig.open( );

    return loadConfiguration( fileConfig ) && initializeConfig( );
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
                ListProperties::LISTPOS pos = mPropertyList.find(newProperty);
                if ( mPropertyList.isValidPosition(pos) )
                {
                    mPropertyList.setAt( pos, newProperty );
                }
                else
                {
                    mPropertyList.pushLast(newProperty);
                }

                newProperty.clearProperty();
            }
        }
    }

    return (mPropertyList.isEmpty() == false);
}

void TraceManager::clearConfigData( void )
{
    Lock lock(mLock);

    mLogConfig.clearProperties();

    mConfigScopeList.clear();
    mConfigScopeGroup.clear();
    mPropertyList.clear();
}

bool TraceManager::activateTracingDefaults( void )
{
    do 
    {
        Lock lock(mLock);

        mLogConfig.setDefaultValues();

        int i = 0;
        do 
        {
            const sLogEnabling & scopes = TraceManager::DEFAULT_LOG_ENABLED_SCOPES[i ++];
            if ( scopes.logScope == nullptr)
                break;

            mConfigScopeGroup.setAt(scopes.logScope, static_cast<int>(scopes.logPrio));
        } while (true);

    } while (false);

    return startLoggingThread();
}

bool TraceManager::initializeConfig(void)
{
    Lock lock(mLock);

    String moduleName      = Process::getInstance().getAppName();
    for (ListProperties::LISTPOS pos = mPropertyList.firstPosition(); mPropertyList.isValidPosition(pos); pos = mPropertyList.nextPosition(pos) )
    {
        const TraceProperty &     prop    = mPropertyList.valueAtPosition(pos);
        const TracePropertyKey &  Key     = prop.getKey();

        // For any property, either global setting is set, or local
        if ( Key.isModuleKeySet(moduleName) && (mLogConfig.updateProperty(prop) == false) )
        {
            if ( Key.getLogConfig() == NELogConfig::eLogConfig::ConfigScope )
            {
                const TracePropertyValue& Value   = prop.getValue( );
                ASSERT( Key.isValidKey( ) );
                unsigned int prio = Value.getPriority( );
                if ( Key.isGlobalKey( ) && Key.getModuleData().isEmpty( ) )
                {
                    ASSERT( Key.getModule( ).isEmpty( ) );
                    mConfigScopeGroup.setAt(NELogConfig::MODULE_SCOPE.data(), prio);
                }
                else if ( Key.getModuleData( ).findLast( NELogConfig::SYNTAX_SCOPE_GROUP ) >= NEString::START_POS )
                {
                    mConfigScopeGroup.setAt( Key.getModuleData( ), prio );
                }
                else
                {
                    mConfigScopeList.setAt( Key.getModuleData( ), prio );
                }
            }
        }
    }

    return _isValid( );
}

bool TraceManager::_isValid(void) const
{
    return (mLogConfig.getVersion().isValid() ? (isFileValid() || isHostValid() || isDebugOutputValid() || isDatabaseValid()) : false);
}

bool TraceManager::_isEnabled(void) const
{
    return mLogConfig.getStatus().getKey().isValidKey() ? static_cast<bool>(mLogConfig.getStatus().getValue()) : NELogConfig::DEFAULT_LOG_ENABLED;
}

bool TraceManager::isHostValid(void) const
{
    return (mLogConfig.getRemoteHost().isValid() || mLogConfig.getRemotePort().isValid());
}

bool TraceManager::isDatabaseValid(void) const
{
    return (mLogConfig.getDatabaseHost().isValid() && mLogConfig.getDatabaseName().isValid() && mLogConfig.getDatabaseDriver().isValid() );
}

bool TraceManager::isFileValid(void) const
{
    return mLogConfig.getLogFile().isValid();
}

bool TraceManager::isDebugOutputValid(void) const
{
    return mLogConfig.getDebugOutput().isValid();
}

bool TraceManager::startLoggingThread( unsigned int waitTimeout /* = NECommon::WAIT_INFINITE */ )
{
    bool result = false;
    ASSERT((isRunning() == false) && (isReady() == false));
    if ( createThread(NECommon::WAIT_INFINITE) )
    {
        // SetScopesActivity(true);
        if ( waitForDispatcherStart(NECommon::WAIT_INFINITE) )
        {
            mLogStarted.resetEvent();

            _sendLogEvent( TraceEventData(TraceEventData::eTraceAction::TraceStartLogs) );

            result = mLogStarted.lock(waitTimeout);
        }
    }
    else
    {
        OUTPUT_ERR("Failed to create [ %s ] System Timer Thread", TraceManager::TRACER_THREAD_NAME);
    }

    return result;
}

void TraceManager::stopLoggingThread( unsigned int waitTimeout /* = NECommon::WAIT_INFINITE */ )
{
    // RemoveAllEvents(false);
    _sendLogEvent( TraceEventData(TraceEventData::eTraceAction::TraceStopLogs) );
    completionWait( isRunning() ? waitTimeout : NECommon::DO_NOT_WAIT );
}

void TraceManager::processEvent(const TraceEventData & data)
{
    const SharedBuffer & stream = data.getReadableStream();

    switch ( data.getTraceAction() )
    {
    case TraceEventData::eTraceAction::TraceConfigure:
        {
            loadConfiguration( FileBuffer(stream, "TraceConfigure") );
        }
        break;

    case TraceEventData::eTraceAction::TraceChangeConfig:
        {
            ; // not implemented yet
        }
        break;

    case TraceEventData::eTraceAction::TraceStartLogs:
        {
            traceStartLogs( );
            mLogStarted.setEvent();
        }
        break;

    case TraceEventData::eTraceAction::TraceStopLogs:
        {
            traceStopLogs( );
            triggerExitEvent();
            mLogStarted.resetEvent();
        }
        break;

    case TraceEventData::eTraceAction::TraceEnableLogs:
        {
            static_cast<TracePropertyValue &>(mLogConfig.getStatus().getValue()) = true;
        }
        break;

    case TraceEventData::eTraceAction::TraceDisableLogs:
        {
            static_cast<TracePropertyValue &>(mLogConfig.getStatus().getValue()) = false;
        }
        break;

    case TraceEventData::eTraceAction::TraceChangeScopes:
        {
            SortedStringList list;
            stream >> list;
            onUpdateScopes( list );
        }
        break;

    case TraceEventData::eTraceAction::TraceMessage:
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
            mConfigScopeGroup.find( NELogConfig::MODULE_SCOPE.data(),  defaultPrio);

            for (TraceScopeMap::MAPPOS pos = mMapTraceScope.firstPosition(); mMapTraceScope.isValidPosition(pos); pos = mMapTraceScope.nextPosition(pos) )
            {
                activateScope(*mMapTraceScope.valueAtPosition(pos), defaultPrio);
            }

            mScopesActivated    = true;
        }
        else if ( (makeActive == false) && mScopesActivated )
        {
            for (TraceScopeMap::MAPPOS pos = mMapTraceScope.firstPosition(); mMapTraceScope.isValidPosition(pos); pos = mMapTraceScope.nextPosition(pos) )
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

    String line;
    while (scopeList.removeLast(line))
    {
        TraceProperty scopeProperty(line);
        if ( scopeProperty.isValid() )
        {
            const TracePropertyKey &  Key     = scopeProperty.getKey();
            const TracePropertyValue & Value  = scopeProperty.getValue();
            if ( Key.getLogConfig() == NELogConfig::eLogConfig::ConfigScope)
            {
                // first, save property in property list
                ListProperties::LISTPOS pos = mPropertyList.find(scopeProperty);
                if ( mPropertyList.isValidPosition(pos) )
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
                    NETrace::eLogPriority prio = static_cast<NETrace::eLogPriority>(Value);
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
    {
        defaultPrio = NELogConfig::DEFAULT_LOG_PRIORITY;
    }

    activateScope(traceScope, defaultPrio);
}

void TraceManager::activateScope( TraceScope & traceScope, unsigned int defaultPriority )
{
    Lock lock(mLock);

    traceScope.setPriority( defaultPriority ); // set first default priority
    const String& scopeName = traceScope.getScopeName();
    unsigned int scopePrio  = defaultPriority;

    if ( mConfigScopeList.find(scopeName, scopePrio) )
    {
        traceScope.setPriority( scopePrio ); // exact match. Set priority
    }
    else
    {
        String groupName( scopeName );
        NEString::CharPos pos = NEString::END_POS;
        do
        {
            pos = groupName.findLast(NELogConfig::SYNTAX_SCOPE_SEPARATOR, pos, true);
            if (groupName.isValidPosition(pos))
            {
                // set group syntax
                groupName.setAt(NELogConfig::SYNTAX_SCOPE_GROUP, pos + 1).resize(pos + 2);
                pos -= 1;
            }
            else
            {
                pos = NEString::INVALID_POS;
                groupName = NELogConfig::SYNTAX_SCOPE_GROUP;
            }

            if (mConfigScopeGroup.find(groupName, scopePrio))
            {
                // Found group priority, set it
                traceScope.setPriority(scopePrio);
                break; // found.
            }

        } while (pos != NEString::INVALID_POS);
    }
}

void TraceManager::setScopesPriority(const String & scopeName, unsigned int logPriority)
{
    if (scopeName.isEmpty() == false)
    {
        NEString::CharPos pos = scopeName.findLast(NELogConfig::SYNTAX_SCOPE_SEPARATOR, NEString::END_POS, true);
        if ( pos == NEString::INVALID_POS )
        {
            // there is a complete scope name, make changes.
            TraceScope * traceScope   = nullptr;
            mConfigScopeList.setAt(scopeName, logPriority);
            if (mMapTraceScope.find(TraceManager::makeScopeId(scopeName.getString()), traceScope) && (traceScope != nullptr))
            {
                traceScope->setPriority(logPriority);
            }
        }
        else if ( pos == NEString::START_POS )
        {
            // change all, it is set group change for complete module
            mConfigScopeGroup.setAt(scopeName, logPriority);
            for (TraceScopeMap::MAPPOS mapPos = mMapTraceScope.firstPosition(); mMapTraceScope.isValidPosition(mapPos); mapPos = mMapTraceScope.nextPosition(mapPos))
            {
                mMapTraceScope.valueAtPosition(mapPos)->setPriority(logPriority);
            }
        }
        else if ( (pos > 0) && (scopeName[pos - 1] == NELogConfig::SYNTAX_SCOPE_SEPARATOR))
        {
            // it is a group scope, update all groups
            mConfigScopeGroup.setAt(scopeName, logPriority);
            int len = static_cast<int>(pos - 1);
            for (TraceScopeMap::MAPPOS mapPos = mMapTraceScope.firstPosition(); mMapTraceScope.isValidPosition(mapPos); mapPos = mMapTraceScope.nextPosition(mapPos) )
            {
                TraceScope * traceScope = mMapTraceScope.valueAtPosition(mapPos);
                if (NEString::compareStrings<char, char>(scopeName, traceScope->getScopeName(), len) == NEMath::eCompare::Equal)
                {
                    traceScope->setPriority(logPriority);
                }
            }
        }
    }
}

bool TraceManager::postEvent(Event & eventElem)
{
    bool result = false;
    if (RUNTIME_CAST(&eventElem, TraceEvent) != nullptr)
    {
        result = EventDispatcher::postEvent(eventElem);
    }
    else
    {
        OUTPUT_ERR("Not a TraceEvent type event, cannot Post. Destroying event type [ %s ]", eventElem.getRuntimeClassName().getString());
        eventElem.destroy();
    }
    return result;
}
