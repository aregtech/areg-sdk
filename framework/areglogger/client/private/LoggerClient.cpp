/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/LoggerClient.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *              The log collector service connection client.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areglogger/client/private/LoggerClient.hpp"

#include "areg/base/File.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/logging/LogConfiguration.hpp"
#include "areglogger/client/LogObserverApi.h"
#include "areglogger/client/LogObserverBase.hpp"

LoggerClient& LoggerClient::getInstance(void)
{
    static LoggerClient _instance;
    return _instance;
}

LoggerClient::LoggerClient(void)
    : ServiceClientConnectionBase( LoggerClient::TargetID
                                 , LoggerClient::ServiceType
                                 , static_cast<uint32_t>(ConnectType)
                                 , LoggerClient::SourceType
                                 , static_cast<IEServiceConnectionConsumer &>(self())
                                 , static_cast<IERemoteMessageHandler &>(self())
                                 , static_cast<DispatcherThread &>(self())
                                 , LoggerClient::ThreadPrefix)
    , IEConfigurationListener    ( )
    , DispatcherThread           ( LoggerClient::ThreadName, NECommon::DEFAULT_BLOCK_SIZE, NECommon::QUEUE_SIZE_MAXIMUM )
    , IEServiceConnectionConsumer( )
    , IERemoteMessageHandler     ( )

    , mCallbacks                 ( nullptr )
    , mMessageProcessor          ( self() )
    , mIsPaused                  ( false )
    , mInstances                 ( )
    , mLogDatabase               ( )
{
}

bool LoggerClient::startLoggerClient(const String & address /*= String::EmptyString*/, uint16_t portNr /*= NESocket::InvalidPort*/)
{
    if ((address.isEmpty() == false) && (portNr != NESocket::InvalidPort))
    {
        Lock lock(mLock);
        mIsPaused = false;
        applyServiceConnectionData(address, portNr);
    }
    else
    {
        Lock lock(mLock);
        mIsPaused = false;
        ConnectionConfiguration config(LoggerClient::ServiceType, LoggerClient::ConnectType);
        if (config.isConfigured())
        {
            applyServiceConnectionData(config.getConnectionAddress(), config.getConnectionPort());
        }
    }

    return connectServiceHost();
}

void LoggerClient::stopLoggerClient(void)
{
    do
    {
        Lock lock(mLock);
        mIsPaused = false;
    } while (false);

    disconnectServiceHost();
}

void LoggerClient::setCallbacks(const sObserverEvents* callbacks)
{
    Lock lock(mLock);
    mCallbacks = callbacks;
}

void LoggerClient::setPaused(bool doPause)
{
    FuncObserverStarted callback{ nullptr };
    bool isStarted{ false };

    do
    {
        Lock lock(mLock);
        mIsPaused = doPause;
        callback = (mCallbacks != nullptr) && isConnectionStarted() ? mCallbacks->evtLoggingStarted : nullptr;
        isStarted = mIsPaused ? false : isConnectionStarted();
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogObserverStarted(isStarted);
    }
    else if (callback != nullptr)
    {
        callback(isStarted);
    }
}

const NESocket::SocketAddress& LoggerClient::getAddress(void) const
{
    Lock lock(mLock);
    return mClientConnection.getAddress();
}

bool LoggerClient::isSqliteEngine(void) const
{
    LogConfiguration config;
    return (config.isDatabaseLoggingEnabled() && (config.getDatabaseEngine() == NELogging::LOGDB_ENGINE_NAME));
}

bool LoggerClient::isConfigLoggerConnectEnabled(void) const
{
    ConnectionConfiguration config(LoggerClient::ServiceType, LoggerClient::ConnectType);
    return (config.isConfigured() && config.getConnectionEnableFlag());
}

String LoggerClient::getConfigLoggerAddress(void) const
{
    ConnectionConfiguration config(LoggerClient::ServiceType, LoggerClient::ConnectType);
    if (config.isConfigured())
    {
        return config.getConnectionAddress();
    }
    else
    {
        return String::EmptyString;
    }
}

uint16_t LoggerClient::getConfigLoggerPort(void) const
{
    ConnectionConfiguration config(LoggerClient::ServiceType, LoggerClient::ConnectType);
    if (config.isConfigured())
    {
        return config.getConnectionPort();
    }
    else
    {
        return NESocket::InvalidPort;
    }
}

bool LoggerClient::setConfigLoggerConnection(const String& address, uint16_t portNr)
{
    bool result{ false };
    ConnectionConfiguration config(LoggerClient::ServiceType, LoggerClient::ConnectType);
    if (config.isConfigured())
    {
        if (address.isEmpty() == false)
            config.setConnectionAddress(address);
        if (portNr != NESocket::InvalidPort)
            config.setConnectionPort(portNr);

        result = true;
    }

    return result;
}

bool LoggerClient::requestConnectedInstances(void)
{
    bool result{ false };
    if (mChannel.getCookie() != NEService::COOKIE_UNKNOWN)
    {
        result = sendMessage(NELogging::messageQueryInstances(mChannel.getCookie(), LoggerClient::TargetID));
    }

    return result;
}

bool LoggerClient::requestScopes(const ITEM_ID& target /*= NEService::TARGET_ALL*/)
{
    bool result{ false };
    Lock lock(mLock);
    if ((mChannel.getCookie() != NEService::COOKIE_UNKNOWN) && (target != NEService::TARGET_UNKNOWN))
    {
        result = sendMessage(NELogging::messageQueryScopes(mChannel.getCookie(), target == NEService::TARGET_ALL ? LoggerClient::TargetID : target));
    }

    return result;
}

bool LoggerClient::requestChangeScopePrio(const NELogging::ScopeNames & scopes, const ITEM_ID& target /*= NEService::TARGET_ALL*/)
{
    bool result{ false };
    Lock lock(mLock);
    if ((mChannel.getCookie() != NEService::COOKIE_UNKNOWN) && (target != NEService::TARGET_UNKNOWN))
    {
        result = sendMessage(NELogging::messageUpdateScopes(mChannel.getCookie(), target == NEService::TARGET_ALL ? LoggerClient::TargetID : target, scopes));
    }

    return result;
}

bool LoggerClient::requestSaveConfiguration(const ITEM_ID& target /*= NEService::TARGET_ALL*/)
{
    bool result{ false };
    Lock lock(mLock);
    if ((mChannel.getCookie() != NEService::COOKIE_UNKNOWN) && (target != NEService::TARGET_UNKNOWN))
    {
        result = sendMessage(NELogging::messageSaveConfiguration(mChannel.getCookie(), target == NEService::TARGET_ALL ? LoggerClient::TargetID : target));
    }

    return result;
}

bool LoggerClient::openLoggingDatabase(const char* dbPath /*= nullptr*/)
{
    String filePath (dbPath);
    if (filePath.isEmpty())
    {
        if (isSqliteEngine())
        {
            LogConfiguration config;
            mLogDatabase.setDatabaseLoggingEnabled(true);
            filePath = File::makeFileFullPath(config.getDatabaseLocation(), config.getDatabaseName());
        }
        else
        {
            mLogDatabase.setDatabaseLoggingEnabled(false);
        }
    }

    bool result{ mLogDatabase.connect(filePath, false) };
    FuncLogDbCreated callback{ mLogDatabase.isOperable() && (mCallbacks != nullptr) ? mCallbacks->evtLogDbCreated  : nullptr};
    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogDbCreated(mLogDatabase.getDatabasePath().getData());
    }
    else if (callback != nullptr)
    {
        callback(mLogDatabase.getDatabasePath().getString());
    }

    return result;
}

void LoggerClient::closeLoggingDatabase(void)
{
    mLogDatabase.disconnect();
}

String LoggerClient::getActiveDatabasePath(void) const
{
    return mLogDatabase.getDatabasePath();
}

String LoggerClient::getInitialDatabasePath(void) const
{
    return mLogDatabase.getInitialDatabasePath();
}

String LoggerClient::getConfigDatabasePath(void) const
{
    String result;
    if (isSqliteEngine())
    {
        LogConfiguration config;
        result = File::makeFileFullPath(config.getDatabaseLocation().getString(), config.getDatabaseName().getString());
    }

    return result;
}

bool LoggerClient::setConfigDatabasePath(const String& dbPath, bool enable)
{
    bool result{ false };
    LogConfiguration config;
    if (config.getDatabaseEngine() == NELogging::LOGDB_ENGINE_NAME)
    {
        String dbLocation = File::getFileDirectory(dbPath.getString());
        String dbName = File::getFileNameWithExtension(dbPath.getString());
        config.setDatabaseEnable(enable, false);
        config.setDatabaseLocation(dbLocation, false);
        config.setDatabaseName(dbName, false);
        result = true;
    }

    return result;
}

String LoggerClient::getConfigDatabaseLocation(void) const
{
    String result;
    if (isSqliteEngine())
    {
        LogConfiguration config;
        result = File::getFileFullPath(config.getDatabaseLocation().getString());
    }

    return result;
}

bool LoggerClient::setConfigDatabaseLocation(const String& dbLocation)
{
    bool result{ false };
    if (isSqliteEngine())
    {
        LogConfiguration config;
        config.setDatabaseLocation(dbLocation, false);
        result = true;
    }

    return result;
}

String LoggerClient::getConfigDatabaseName(void) const
{
    String result;
    if (isSqliteEngine())
    {
        LogConfiguration config;
        result = config.getDatabaseName().getString();
    }

    return result;
}

bool LoggerClient::setConfigDatabaseName(const String& dbName)
{
    bool result{ false };
    if (isSqliteEngine())
    {
        LogConfiguration config;
        config.setDatabaseName(dbName, false);
        result = true;
    }

    return result;
}

bool LoggerClient::setConfigLoggerConnectEnabled(bool isEnabled)
{
    bool result{ false };
    LogConfiguration config;
    if (config.getDatabaseEngine() == NELogging::LOGDB_ENGINE_NAME)
    {
        config.setDatabaseEnable(isEnabled, false);
        result = true;
    }

    return result;
}

void LoggerClient::saveConfiguration(void)
{
    LogConfiguration config;
    config.saveConfiguration();
}

void LoggerClient::prepareSaveConfiguration(ConfigManager& /* config */)
{
}

void LoggerClient::postSaveConfiguration(ConfigManager& /* config */)
{
}

void LoggerClient::prepareReadConfiguration(ConfigManager& /* config */)
{
}

void LoggerClient::postReadConfiguration(ConfigManager& config)
{
    FuncObserverConfigured callbackConf{ nullptr };
    FuncLogDbConfigured callbackConfDb{ nullptr };
    String address;
    uint16_t port{0};
    String dbName;
    String dbLocation;
    String dbUser;

    config.setLogEnabled(NELogging::eLogingTypes::LogTypeFile, true, true);
    config.setLogEnabled(NELogging::eLogingTypes::LogTypeRemote, true, true);

    do
    {
        Lock lock(mLock);
        if (mCallbacks != nullptr)
        {
            callbackConf    = mCallbacks->evtObserverConfigured;
            callbackConfDb  = mCallbacks->evtLogDbConfigured;
            address         = config.getRemoteServiceAddress(LoggerClient::ServiceType, LoggerClient::ConnectType);
            port            = config.getRemoteServicePort(LoggerClient::ServiceType, LoggerClient::ConnectType);
            dbName          = config.getLogDatabaseProperty(NEPersistence::getLogDatabaseName().position);
            dbLocation      = config.getLogDatabaseProperty(NEPersistence::getLogDatabaseLocation().position);
            dbUser          = config.getLogDatabaseProperty(NEPersistence::getLogDatabaseUser().position);
        }
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogObserverConfigured(true, address.getData(), port);
        LogObserverBase::_theLogObserver->onLogDbConfigured(config.getLogEnabled(NELogging::eLogingTypes::LogTypeDatabase), dbName.getData(), dbLocation.getData(), dbUser.getData());
    }
    else
    {
        if (callbackConf != nullptr)
            callbackConf(true, address.getString(), port);
        if (callbackConfDb != nullptr)
            callbackConfDb(config.getLogEnabled(NELogging::eLogingTypes::LogTypeDatabase), dbName.getString(), dbLocation.getString(), dbUser.getString());
    }
}

void LoggerClient::onSetupConfiguration( const NEPersistence::ListProperties&  /* listReadonly */
                                        , const NEPersistence::ListProperties& /* listWritable */
                                        , ConfigManager& /* config */)
{
}

bool LoggerClient::postEvent(Event& eventElem)
{
    return EventDispatcher::postEvent(eventElem);
}

void LoggerClient::readyForEvents(bool isReady)
{
    if (isReady)
    {
        registerForServiceClientCommands();
        DispatcherThread::readyForEvents(true);
        setConnectionState(ServiceClientConnectionBase::eConnectionState::DisconnectState);
    }
    else
    {
        DispatcherThread::readyForEvents(false);
        setConnectionState(ServiceClientConnectionBase::eConnectionState::ConnectionStopped);
        unregisterForServiceClientCommands();
    }
}

bool LoggerClient::connectServiceHost(void)
{
    bool result{ false };
    if (isRunning() == false)
    {
        if (createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE))
        {
            result = ServiceClientConnectionBase::connectServiceHost();
        }
        else
        {
            shutdownThread(NECommon::WAIT_INFINITE);
        }
    }
    else
    {
        result = mClientConnection.isValid();
    }

    return result;
}

void LoggerClient::disconnectServiceHost(void)
{
    if (isRunning())
    {
        FuncInstancesDisconnect callback{ mCallbacks != nullptr ? mCallbacks->evtInstDisconnected : nullptr };
        if (LogObserverBase::_theLogObserver != nullptr)
        {
            LogObserverBase::_theLogObserver->onLogServiceDisconnected();
        }
        else if (callback != nullptr)
        {

            for (const auto& entry : mInstances.getData())
            {
                callback(&entry.first, 1);
            }
        }

        mInstances.clear();

        ServiceClientConnectionBase::disconnectServiceHost();
        completionWait(NECommon::WAIT_INFINITE);
        shutdownThread(NECommon::DO_NOT_WAIT);
    }
}

void LoggerClient::onServiceExit(void)
{
    ServiceClientConnectionBase::onServiceExit();
    triggerExit();
}

void LoggerClient::connectedRemoteServiceChannel(const Channel& channel)
{
    FuncServiceConnected callbackConnect{ nullptr };
    FuncObserverStarted callbackStart{ nullptr };
    String address;
    uint16_t port{ NESocket::InvalidPort };
    bool isStarted{ false };

    do
    {
        Lock lock(mLock);
        const NESocket::SocketAddress& addr{ mClientConnection.getAddress() };
        address = addr.getHostAddress();
        port = addr.getHostPort();
        isStarted = mIsPaused ? false : isConnectionStarted();

        if (mCallbacks != nullptr)
        {
            callbackConnect = mCallbacks->evtServiceConnected;
            callbackStart = mCallbacks->evtLoggingStarted;
        }
    } while (false);

    sendMessage(NELogging::messageQueryInstances(channel.getCookie(), LoggerClient::TargetID));

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogServiceConnected(true, address.getData(), port);
        LogObserverBase::_theLogObserver->onLogObserverStarted(isStarted);
    }
    else
    {
        if (callbackConnect != nullptr)
            callbackConnect(true, address.getString(), port);
        if (callbackStart != nullptr)
            callbackStart(isStarted);
    }
}

void LoggerClient::disconnectedRemoteServiceChannel(const Channel& /* channel */)
{
    FuncServiceConnected callbackConnect{ nullptr };
    FuncObserverStarted callbackStart{ nullptr };
    String address;
    uint16_t port{ NESocket::InvalidPort };

    do
    {
        Lock lock(mLock);
        const NESocket::SocketAddress& addr{ mClientConnection.getAddress() };
        address = addr.getHostAddress();
        port = addr.getHostPort();

        if (mCallbacks != nullptr)
        {
            callbackConnect = mCallbacks->evtServiceConnected;
            callbackStart = mCallbacks->evtLoggingStarted;
        }
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogObserverStarted(false);
        LogObserverBase::_theLogObserver->onLogServiceConnected(false, address.getData(), port);
    }
    else
    {
        if (callbackStart != nullptr)
            callbackStart(false);
        if (callbackConnect != nullptr)
            callbackConnect(false, address.getString(), port);
    }
}

void LoggerClient::lostRemoteServiceChannel(const Channel& /* channel */)
{
    FuncObserverStarted callback{ nullptr };

    do
    {
        Lock lock(mLock);
        callback = mCallbacks != nullptr ? mCallbacks->evtLoggingStarted : nullptr;
        mInstances.clear();
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogObserverStarted(false);
    }
    else if (callback != nullptr)
    {
        callback(false);
    }
}

void LoggerClient::failedSendMessage(const RemoteMessage& /* msgFailed */, Socket& /* whichTarget */)
{
    FuncMessagingFailed callback{ nullptr };
    do
    {
        Lock lock(mLock);
        callback = mCallbacks != nullptr ? mCallbacks->evtMessagingFailed : nullptr;
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogMessagingFailed();
    }
    else if (callback != nullptr)
    {
        callback();
    }
}

void LoggerClient::failedReceiveMessage(Socket& /* whichSource */)
{
    FuncMessagingFailed callback{ nullptr };
    do
    {
        Lock lock(mLock);
        callback = mCallbacks != nullptr ? mCallbacks->evtMessagingFailed : nullptr;
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogMessagingFailed();
    }
    else if (callback != nullptr)
    {
        callback();
    }
}

void LoggerClient::failedProcessMessage(const RemoteMessage& /* msgUnprocessed */)
{
}

void LoggerClient::processReceivedMessage(const RemoteMessage& msgReceived, Socket& whichSource)
{
    if (msgReceived.isValid() && whichSource.isValid())
    {
        NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>(msgReceived.getMessageId());
        switch (msgId)
        {
        case NEService::eFuncIdRange::SystemServiceNotifyConnection:
            mMessageProcessor.notifyServiceConnection(msgReceived);
            serviceConnectionEvent(msgReceived);
            break;

        case NEService::eFuncIdRange::SystemServiceNotifyInstances:
            mMessageProcessor.notifyConnectedClients(msgReceived);
            break;

        case NEService::eFuncIdRange::ServiceLogRegisterScopes:
            mMessageProcessor.notifyLogRegisterScopes(msgReceived);
            break;

        case NEService::eFuncIdRange::ServiceLogScopesUpdated:
            mMessageProcessor.notifyLogUpdateScopes(msgReceived);
            break;

        case NEService::eFuncIdRange::ServiceLogMessage:
            if (mIsPaused == false)
            {
                mMessageProcessor.notifyLogMessage(msgReceived);
            }
            break;

        case NEService::eFuncIdRange::SystemServiceNotifyRegister:      // fall through
        case NEService::eFuncIdRange::ServiceLastId:                    // fall through
        case NEService::eFuncIdRange::SystemServiceQueryInstances:      // fall through
        case NEService::eFuncIdRange::SystemServiceRequestRegister:     // fall through
        case NEService::eFuncIdRange::SystemServiceDisconnect:          // fall through
        case NEService::eFuncIdRange::SystemServiceConnect:             // fall through
        case NEService::eFuncIdRange::ResponseServiceProviderConnection:// fall through
        case NEService::eFuncIdRange::RequestServiceProviderConnection: // fall through
        case NEService::eFuncIdRange::ResponseServiceProviderVersion:   // fall through
        case NEService::eFuncIdRange::RequestServiceProviderVersion:    // fall through
        case NEService::eFuncIdRange::RequestRegisterService:           // fall through
        case NEService::eFuncIdRange::ComponentCleanup:                 // fall through
        case NEService::eFuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case NEService::eFuncIdRange::AttributeLastId:                  // fall through
        case NEService::eFuncIdRange::AttributeFirstId:                 // fall through
        case NEService::eFuncIdRange::ResponseLastId:                   // fall through
        case NEService::eFuncIdRange::ResponseFirstId:                  // fall through
        case NEService::eFuncIdRange::RequestLastId:                    // fall through
        case NEService::eFuncIdRange::RequestFirstId:                   // fall through
        case NEService::eFuncIdRange::EmptyFunctionId:                  // fall through
        case NEService::eFuncIdRange::ServiceLogUpdateScopes:           // fall through
        case NEService::eFuncIdRange::ServiceLogQueryScopes:            // fall through
        case NEService::eFuncIdRange::ServiceSaveLogConfiguration:      // fall through
        default:
            ASSERT(false);
        }
    }
}
