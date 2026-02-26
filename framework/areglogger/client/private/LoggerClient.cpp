/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/LoggerClient.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Observer library API.
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

LoggerClient& LoggerClient::getInstance()
{
    static LoggerClient _instance;
    return _instance;
}

LoggerClient::LoggerClient()
    : areg::ServiceClientConnectionBase( LoggerClient::TARGET_ID
                                 , LoggerClient::SERVICE_TYPE
                                 , static_cast<uint32_t>(CONNECT_TYPE)
                                 , LoggerClient::SOURCE_TYPE
                                 , static_cast<areg::ConnectionConsumer &>(self())
                                 , static_cast<areg::RemoteMessageHandler &>(self())
                                 , static_cast<areg::DispatcherThread &>(self())
                                 , LoggerClient::THREAD_PREFIX)
    , areg::ConfigListener    ( )
    , areg::DispatcherThread           ( LoggerClient::THREAD_NAME, areg::DEFAULT_BLOCK_SIZE, areg::QUEUE_SIZE_MAXIMUM )
    , areg::ConnectionConsumer( )
    , areg::RemoteMessageHandler     ( )

    , mCallbacks                 ( nullptr )
    , mMessageProcessor          ( self() )
    , mIsPaused                  ( false )
    , mInstances                 ( )
    , mLogDatabase               ( )
{
}

bool LoggerClient::startLoggerClient(const areg::String & address /*= areg::String::EmptyString*/, uint16_t portNr /*= areg::InvalidPort*/)
{
    if ((address.isEmpty() == false) && (portNr != areg::InvalidPort))
    {
        areg::Lock lock(mLock);
        mIsPaused = false;
        applyServiceConnectionData(address, portNr);
    }
    else
    {
        areg::Lock lock(mLock);
        mIsPaused = false;
        areg::ConnectionConfiguration config(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
        if (config.isConfigured())
        {
            applyServiceConnectionData(config.getConnectionAddress(), config.getConnectionPort());
        }
    }

    return connectServiceHost();
}

void LoggerClient::stopLoggerClient()
{
    do
    {
        areg::Lock lock(mLock);
        mIsPaused = false;
    } while (false);

    disconnectServiceHost();
}

void LoggerClient::setCallbacks(const ObserverEvents* callbacks)
{
    areg::Lock lock(mLock);
    mCallbacks = callbacks;
}

void LoggerClient::setPaused(bool doPause)
{
    FuncObserverStarted callback{ nullptr };
    bool isStarted{ false };

    do
    {
        areg::Lock lock(mLock);
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

const areg::SocketAddress& LoggerClient::getAddress() const
{
    areg::Lock lock(mLock);
    return mClientConnection.getAddress();
}

bool LoggerClient::isSqliteEngine() const
{
    areg::LogConfiguration config;
    return (config.isDatabaseLoggingEnabled() && (config.getDatabaseEngine() == areg::LOGDB_ENGINE_NAME));
}

bool LoggerClient::isConfigLoggerConnectEnabled() const
{
    areg::ConnectionConfiguration config(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
    return (config.isConfigured() && config.getConnectionEnableFlag());
}

areg::String LoggerClient::getConfigLoggerAddress() const
{
    areg::ConnectionConfiguration config(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
    if (config.isConfigured())
    {
        return config.getConnectionAddress();
    }
    else
    {
        return areg::String::EmptyString;
    }
}

uint16_t LoggerClient::getConfigLoggerPort() const
{
    areg::ConnectionConfiguration config(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
    if (config.isConfigured())
    {
        return config.getConnectionPort();
    }
    else
    {
        return areg::InvalidPort;
    }
}

bool LoggerClient::setConfigLoggerConnection(const areg::String& address, uint16_t portNr)
{
    bool result{ false };
    areg::ConnectionConfiguration config(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
    if (config.isConfigured())
    {
        if (address.isEmpty() == false)
            config.setConnectionAddress(address);
        if (portNr != areg::InvalidPort)
            config.setConnectionPort(portNr);

        result = true;
    }

    return result;
}

bool LoggerClient::requestConnectedInstances()
{
    bool result{ false };
    if (mChannel.getCookie() != areg::COOKIE_UNKNOWN)
    {
        result = sendMessage(areg::messageQueryInstances(mChannel.getCookie(), LoggerClient::TARGET_ID));
    }

    return result;
}

bool LoggerClient::requestScopes(const ITEM_ID& target /*= areg::TARGET_ALL*/)
{
    bool result{ false };
    areg::Lock lock(mLock);
    if ((mChannel.getCookie() != areg::COOKIE_UNKNOWN) && (target != areg::TARGET_UNKNOWN))
    {
        result = sendMessage(areg::messageQueryScopes(mChannel.getCookie(), target == areg::TARGET_ALL ? LoggerClient::TARGET_ID : target));
    }

    return result;
}

bool LoggerClient::requestChangeScopePrio(const areg::ScopeNames & scopes, const ITEM_ID& target /*= areg::TARGET_ALL*/)
{
    bool result{ false };
    areg::Lock lock(mLock);
    if ((mChannel.getCookie() != areg::COOKIE_UNKNOWN) && (target != areg::TARGET_UNKNOWN))
    {
        result = sendMessage(areg::messageUpdateScopes(mChannel.getCookie(), target == areg::TARGET_ALL ? LoggerClient::TARGET_ID : target, scopes));
    }

    return result;
}

bool LoggerClient::requestSaveConfiguration(const ITEM_ID& target /*= areg::TARGET_ALL*/)
{
    bool result{ false };
    areg::Lock lock(mLock);
    if ((mChannel.getCookie() != areg::COOKIE_UNKNOWN) && (target != areg::TARGET_UNKNOWN))
    {
        result = sendMessage(areg::messageSaveConfiguration(mChannel.getCookie(), target == areg::TARGET_ALL ? LoggerClient::TARGET_ID : target));
    }

    return result;
}

bool LoggerClient::openLoggingDatabase(const char* dbPath /*= nullptr*/)
{
    areg::String filePath (dbPath);
    if (filePath.isEmpty())
    {
        if (isSqliteEngine())
        {
            areg::LogConfiguration config;
            mLogDatabase.setDatabaseLoggingEnabled(true);
            filePath = areg::File::makeFileFullPath(config.getDatabaseLocation(), config.getDatabaseName());
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

void LoggerClient::closeLoggingDatabase()
{
    mLogDatabase.disconnect();
}

areg::String LoggerClient::getActiveDatabasePath() const
{
    return mLogDatabase.getDatabasePath();
}

areg::String LoggerClient::getInitialDatabasePath() const
{
    return mLogDatabase.getInitialDatabasePath();
}

areg::String LoggerClient::getConfigDatabasePath() const
{
    areg::String result;
    if (isSqliteEngine())
    {
        areg::LogConfiguration config;
        result = areg::File::makeFileFullPath(config.getDatabaseLocation().getString(), config.getDatabaseName().getString());
    }

    return result;
}

bool LoggerClient::setConfigDatabasePath(const areg::String& dbPath, bool enable)
{
    bool result{ false };
    areg::LogConfiguration config;
    if (config.getDatabaseEngine() == areg::LOGDB_ENGINE_NAME)
    {
        areg::String dbLocation = areg::File::getFileDirectory(dbPath.getString());
        areg::String dbName = areg::File::getFileNameWithExtension(dbPath.getString());
        config.setDatabaseEnable(enable, false);
        config.setDatabaseLocation(dbLocation, false);
        config.setDatabaseName(dbName, false);
        result = true;
    }

    return result;
}

areg::String LoggerClient::getConfigDatabaseLocation() const
{
    areg::String result;
    if (isSqliteEngine())
    {
        areg::LogConfiguration config;
        result = areg::File::getFileFullPath(config.getDatabaseLocation().getString());
    }

    return result;
}

bool LoggerClient::setConfigDatabaseLocation(const areg::String& dbLocation)
{
    bool result{ false };
    if (isSqliteEngine())
    {
        areg::LogConfiguration config;
        config.setDatabaseLocation(dbLocation, false);
        result = true;
    }

    return result;
}

areg::String LoggerClient::getConfigDatabaseName() const
{
    areg::String result;
    if (isSqliteEngine())
    {
        areg::LogConfiguration config;
        result = config.getDatabaseName().getString();
    }

    return result;
}

bool LoggerClient::setConfigDatabaseName(const areg::String& dbName)
{
    bool result{ false };
    if (isSqliteEngine())
    {
        areg::LogConfiguration config;
        config.setDatabaseName(dbName, false);
        result = true;
    }

    return result;
}

bool LoggerClient::setConfigLoggerConnectEnabled(bool isEnabled)
{
    bool result{ false };
    areg::LogConfiguration config;
    if (config.getDatabaseEngine() == areg::LOGDB_ENGINE_NAME)
    {
        config.setDatabaseEnable(isEnabled, false);
        result = true;
    }

    return result;
}

void LoggerClient::saveConfiguration()
{
    areg::LogConfiguration config;
    config.saveConfiguration();
}

void LoggerClient::prepareSaveConfiguration(areg::ConfigManager& /* config */)
{
}

void LoggerClient::postSaveConfiguration(areg::ConfigManager& /* config */)
{
}

void LoggerClient::prepareReadConfiguration(areg::ConfigManager& /* config */)
{
}

void LoggerClient::postReadConfiguration(areg::ConfigManager& config)
{
    FuncObserverConfigured callbackConf{ nullptr };
    FuncLogDbConfigured callbackConfDb{ nullptr };
    areg::String address;
    uint16_t port{0};
    areg::String dbName;
    areg::String dbLocation;
    areg::String dbUser;

    config.setLogEnabled(areg::LogTarget::File, true, true);
    config.setLogEnabled(areg::LogTarget::Remote, true, true);

    do
    {
        areg::Lock lock(mLock);
        if (mCallbacks != nullptr)
        {
            callbackConf    = mCallbacks->evtObserverConfigured;
            callbackConfDb  = mCallbacks->evtLogDbConfigured;
            address         = config.getRemoteServiceAddress(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
            port            = config.getRemoteServicePort(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
            dbName          = config.getLogDatabaseProperty(areg::getLogDatabaseName().position);
            dbLocation      = config.getLogDatabaseProperty(areg::getLogDatabaseLocation().position);
            dbUser          = config.getLogDatabaseProperty(areg::getLogDatabaseUser().position);
        }
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogObserverConfigured(true, address.getData(), port);
        LogObserverBase::_theLogObserver->onLogDbConfigured(config.getLogEnabled(areg::LogTarget::Database), dbName.getData(), dbLocation.getData(), dbUser.getData());
    }
    else
    {
        if (callbackConf != nullptr)
            callbackConf(true, address.getString(), port);
        if (callbackConfDb != nullptr)
            callbackConfDb(config.getLogEnabled(areg::LogTarget::Database), dbName.getString(), dbLocation.getString(), dbUser.getString());
    }
}

void LoggerClient::onSetupConfiguration( const areg::ListProperties&  /* listReadonly */
                                        , const areg::ListProperties& /* listWritable */
                                        , areg::ConfigManager& /* config */)
{
}

bool LoggerClient::postEvent(areg::Event& eventElem)
{
    return areg::EventDispatcher::postEvent(eventElem);
}

void LoggerClient::readyForEvents(bool isReady)
{
    if (isReady)
    {
        registerForServiceClientCommands();
        areg::DispatcherThread::readyForEvents(true);
        setConnectionState(areg::ServiceClientConnectionBase::ConnectionPhase::DisconnectState);
    }
    else
    {
        areg::DispatcherThread::readyForEvents(false);
        setConnectionState(areg::ServiceClientConnectionBase::ConnectionPhase::ConnectionStopped);
        unregisterForServiceClientCommands();
    }
}

bool LoggerClient::connectServiceHost()
{
    bool result{ false };
    if (isRunning() == false)
    {
        if (createThread(areg::WAIT_INFINITE) && waitForDispatcherStart(areg::WAIT_INFINITE))
        {
            result = areg::ServiceClientConnectionBase::connectServiceHost();
        }
        else
        {
            shutdownThread(areg::WAIT_INFINITE);
        }
    }
    else
    {
        result = mClientConnection.isValid();
    }

    return result;
}

void LoggerClient::disconnectServiceHost()
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

        areg::ServiceClientConnectionBase::disconnectServiceHost();
        completionWait(areg::WAIT_INFINITE);
        shutdownThread(areg::DO_NOT_WAIT);
    }
}

void LoggerClient::onServiceExit()
{
    areg::ServiceClientConnectionBase::onServiceExit();
    triggerExit();
}

void LoggerClient::connectedRemoteServiceChannel(const areg::Channel& channel)
{
    FuncServiceConnected callbackConnect{ nullptr };
    FuncObserverStarted callbackStart{ nullptr };
    areg::String address;
    uint16_t port{ areg::InvalidPort };
    bool isStarted{ false };

    do
    {
        areg::Lock lock(mLock);
        const areg::SocketAddress& addr{ mClientConnection.getAddress() };
        address = addr.getHostAddress();
        port = addr.getHostPort();
        isStarted = mIsPaused ? false : isConnectionStarted();

        if (mCallbacks != nullptr)
        {
            callbackConnect = mCallbacks->evtServiceConnected;
            callbackStart = mCallbacks->evtLoggingStarted;
        }
    } while (false);

    sendMessage(areg::messageQueryInstances(channel.getCookie(), LoggerClient::TARGET_ID));

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

void LoggerClient::disconnectedRemoteServiceChannel(const areg::Channel& /* channel */)
{
    FuncServiceConnected callbackConnect{ nullptr };
    FuncObserverStarted callbackStart{ nullptr };
    areg::String address;
    uint16_t port{ areg::InvalidPort };

    do
    {
        areg::Lock lock(mLock);
        const areg::SocketAddress& addr{ mClientConnection.getAddress() };
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

void LoggerClient::lostRemoteServiceChannel(const areg::Channel& /* channel */)
{
    FuncObserverStarted callback{ nullptr };

    do
    {
        areg::Lock lock(mLock);
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

void LoggerClient::failedSendMessage(const areg::RemoteMessage& /* msgFailed */, areg::Socket& /* whichTarget */)
{
    FuncMessagingFailed callback{ nullptr };
    do
    {
        areg::Lock lock(mLock);
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

void LoggerClient::failedReceiveMessage(areg::Socket& /* whichSource */)
{
    FuncMessagingFailed callback{ nullptr };
    do
    {
        areg::Lock lock(mLock);
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

void LoggerClient::failedProcessMessage(const areg::RemoteMessage& /* msgUnprocessed */)
{
}

void LoggerClient::processReceivedMessage(const areg::RemoteMessage& msgReceived, areg::Socket& whichSource)
{
    if (msgReceived.isValid() && whichSource.isValid())
    {
        areg::FuncIdRange msgId = static_cast<areg::FuncIdRange>(msgReceived.getMessageId());
        switch (msgId)
        {
        case areg::FuncIdRange::SystemServiceNotifyConnection:
            mMessageProcessor.notifyServiceConnection(msgReceived);
            serviceConnectionEvent(msgReceived);
            break;

        case areg::FuncIdRange::SystemServiceNotifyInstances:
            mMessageProcessor.notifyConnectedClients(msgReceived);
            break;

        case areg::FuncIdRange::ServiceLogRegisterScopes:
            mMessageProcessor.notifyLogRegisterScopes(msgReceived);
            break;

        case areg::FuncIdRange::ServiceLogScopesUpdated:
            mMessageProcessor.notifyLogUpdateScopes(msgReceived);
            break;

        case areg::FuncIdRange::ServiceLogMessage:
            if (mIsPaused == false)
            {
                mMessageProcessor.notifyLogMessage(msgReceived);
            }
            break;

        case areg::FuncIdRange::SystemServiceNotifyRegister:      // fall through
        case areg::FuncIdRange::ServiceLastId:                    // fall through
        case areg::FuncIdRange::SystemServiceQueryInstances:      // fall through
        case areg::FuncIdRange::SystemServiceRequestRegister:     // fall through
        case areg::FuncIdRange::SystemServiceDisconnect:          // fall through
        case areg::FuncIdRange::SystemServiceConnect:             // fall through
        case areg::FuncIdRange::ResponseServiceProviderConnection:// fall through
        case areg::FuncIdRange::RequestServiceProviderConnection: // fall through
        case areg::FuncIdRange::ResponseServiceProviderVersion:   // fall through
        case areg::FuncIdRange::RequestServiceProviderVersion:    // fall through
        case areg::FuncIdRange::RequestRegisterService:           // fall through
        case areg::FuncIdRange::ComponentCleanup:                 // fall through
        case areg::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case areg::FuncIdRange::AttributeLastId:                  // fall through
        case areg::FuncIdRange::AttributeFirstId:                 // fall through
        case areg::FuncIdRange::ResponseLastId:                   // fall through
        case areg::FuncIdRange::ResponseFirstId:                  // fall through
        case areg::FuncIdRange::RequestLastId:                    // fall through
        case areg::FuncIdRange::RequestFirstId:                   // fall through
        case areg::FuncIdRange::EmptyFunctionId:                  // fall through
        case areg::FuncIdRange::ServiceLogUpdateScopes:           // fall through
        case areg::FuncIdRange::ServiceLogQueryScopes:            // fall through
        case areg::FuncIdRange::ServiceSaveLogConfiguration:      // fall through
        default:
            ASSERT(false);
        }
    }
}
