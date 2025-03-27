/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
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

#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/logging/LogConfiguration.hpp"
#include "areglogger/client/LogObserverApi.h"

LoggerClient& LoggerClient::getInstance(void)
{
    static LoggerClient _instance;
    return _instance;
}

LoggerClient::LoggerClient(void)
    : ServiceClientConnectionBase( TargetID
                                 , ServiceType
                                 , static_cast<uint32_t>(ConnectType)
                                 , SourceType
                                 , static_cast<IEServiceConnectionConsumer &>(self())
                                 , static_cast<IERemoteMessageHandler &>(self())
                                 , static_cast<DispatcherThread &>(self())
                                 , ThreadPrefix)
    , IEConfigurationListener    ( )
    , DispatcherThread           ( ThreadName )
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
    FuncObserverStarted evtStart{ nullptr };
    bool isStarted{ false };

    do
    {
        Lock lock(mLock);
        mIsPaused = doPause;
        if (mCallbacks != nullptr)
        {
            evtStart = isConnectionStarted() ? mCallbacks->evtLoggingStarted : nullptr;
            isStarted = mIsPaused;
        }
    } while (false);

    if (evtStart != nullptr)
    {
        evtStart(isStarted);
    }
}

const NESocket::SocketAddress& LoggerClient::getAddress(void) const
{
    Lock lock(mLock);
    return mClientConnection.getAddress();
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

bool LoggerClient::requestConnectedInstances(void)
{
    bool result{ false };
    if (mChannel.getCookie() != NEService::COOKIE_UNKNOWN)
    {
        result = sendMessage(NELogging::messageQueryInstances(mChannel.getCookie(), LoggerClient::TargetID));
    }

    return result;
}

bool LoggerClient::requestScopes(const ITEM_ID& target /*= NEService::COOKIE_ANY*/)
{
    bool result{ false };
    Lock lock(mLock);
    if ((mChannel.getCookie() != NEService::COOKIE_UNKNOWN) && (target != NEService::COOKIE_UNKNOWN))
    {
        result = sendMessage(NELogging::messageQueryScopes(mChannel.getCookie(), target == NEService::COOKIE_ANY ? LoggerClient::TargetID : target));
    }

    return result;
}

bool LoggerClient::requestChangeScopePrio(const NELogging::ScopeNames & scopes, const ITEM_ID& target /*= NEService::COOKIE_ANY*/)
{
    bool result{ false };
    Lock lock(mLock);
    if ((mChannel.getCookie() != NEService::COOKIE_UNKNOWN) && (target != NEService::COOKIE_UNKNOWN))
    {
        result = sendMessage(NELogging::messageUpdateScopes(mChannel.getCookie(), target == NEService::COOKIE_ANY ? LoggerClient::TargetID : target, scopes));
    }

    return result;
}

bool LoggerClient::requestSaveConfiguration(const ITEM_ID& target /*= NEService::COOKIE_ANY*/)
{
    bool result{ false };
    Lock lock(mLock);
    if ((mChannel.getCookie() != NEService::COOKIE_UNKNOWN) && (target != NEService::COOKIE_UNKNOWN))
    {
        result = sendMessage(NELogging::messageSaveConfiguration(mChannel.getCookie(), target == NEService::COOKIE_ANY ? LoggerClient::TargetID : target));
    }

    return result;
}

bool LoggerClient::openLoggingDatabase(const char* dbPath /*= nullptr*/)
{
    String filePath (dbPath);
    if (filePath.isEmpty())
    {
        LogConfiguration config;
        if (config.isDatabaseLoggingEnabled() && (config.getDatabaseName() == NELogging::LOGDB_NAME_SQLITE3))
        {
            mLogDatabase.setDatabaseLoggingEnabled(true);
            filePath = config.getDatabaseLocation();
        }
        else
        {
            mLogDatabase.setDatabaseLoggingEnabled(false);
        }
    }

    bool result{ mLogDatabase.connect(filePath) };
    if (mLogDatabase.isOperable() && (mCallbacks != nullptr) && (mCallbacks->evtLogDbCreated != nullptr))
    {
        mCallbacks->evtLogDbCreated(mLogDatabase.getDatabasePath().getString());
    }

    return result;
}

void LoggerClient::closeLoggingDatabase(void)
{
    mLogDatabase.disconnect();
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
    FuncObserverConfigured evtConfig{ nullptr };
    FuncLogDbConfigured evtLogConfig{ nullptr };
    String address;
    uint16_t port{0};
    String dbName;
    String dbLocation;
    String dbUser;

    config.setLogEnabled(NELogging::eLogingTypes::LogTypeFile, true, true);
    config.setLogEnabled(NELogging::eLogingTypes::LogTypeRemote, false, true);

    do
    {
        Lock lock(mLock);
        if (mCallbacks != nullptr)
        {
            evtConfig   = mCallbacks->evtObserverConfigured;
            evtLogConfig= mCallbacks->evtLogDbConfigured;
            address     = config.getRemoteServiceAddress(LoggerClient::ServiceType, LoggerClient::ConnectType);
            port        = config.getRemoteServicePort(LoggerClient::ServiceType, LoggerClient::ConnectType);
            dbName      = config.getLogDatabaseProperty(NEPersistence::getLogDatabaseName().position);
            dbLocation  = config.getLogDatabaseProperty(NEPersistence::getLogDatabaseLocation().position);
            dbUser      = config.getLogDatabaseProperty(NEPersistence::getLogDatabaseUser().position);
        }
    } while (false);

    if (evtConfig != nullptr)
    {
        evtConfig(true, address.getString(), port);
    }

    if (evtLogConfig != nullptr)
    {
        evtLogConfig(config.getLogEnabled(NELogging::eLogingTypes::LogTypeDatabase), dbName.getString(), dbLocation.getString(), dbUser.getString());
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
        FuncInstancesDisconnect evtDisconnect{ mCallbacks != nullptr ? mCallbacks->evtInstDisconnected : nullptr };
        if (evtDisconnect != nullptr)
        {

            for (const auto& entry : mInstances.getData())
            {
                evtDisconnect(&entry.first, 1);
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
    FuncServiceConnected evtConnect{ nullptr };
    FuncObserverStarted evtStart{ nullptr };
    String address;
    uint16_t port{ NESocket::InvalidPort };
    bool isStarted{ false };

    do
    {
        Lock lock(mLock);
        if (mCallbacks != nullptr)
        {
            evtConnect = mCallbacks->evtServiceConnected;
            evtStart = mCallbacks->evtLoggingStarted;
            const NESocket::SocketAddress& addr{ mClientConnection.getAddress() };
            address = addr.getHostAddress();
            port = addr.getHostPort();
            isStarted = mIsPaused = false;
        }
    } while (false);

    sendMessage(NELogging::messageQueryInstances(channel.getCookie(), LoggerClient::TargetID));
    if (evtConnect != nullptr)
    {
        evtConnect(true, address.getString(), port);
    }

    if (evtStart != nullptr)
    {
        evtStart(isStarted);
    }
}

void LoggerClient::disconnectedRemoteServiceChannel(const Channel& /* channel */)
{
    FuncServiceConnected evtConnect{ nullptr };
    FuncObserverStarted evtStart{ nullptr };
    String address;
    uint16_t port{ NESocket::InvalidPort };

    do
    {
        Lock lock(mLock);
        if (mCallbacks != nullptr)
        {
            evtConnect = mCallbacks->evtServiceConnected;
            evtStart = mCallbacks->evtLoggingStarted;
            const NESocket::SocketAddress& addr{ mClientConnection.getAddress() };
            address = addr.getHostAddress();
            port = addr.getHostPort();
        }
    } while (false);

    if (evtStart != nullptr)
    {
        evtStart(false);
    }

    if (evtConnect != nullptr)
    {
        evtConnect(false, address.getString(), port);
    }
}

void LoggerClient::lostRemoteServiceChannel(const Channel& /* channel */)
{
    FuncObserverStarted evtStart{ nullptr };

    do
    {
        Lock lock(mLock);
        if (mCallbacks != nullptr)
        {
            evtStart = mCallbacks->evtLoggingStarted;
        }

        mInstances.clear();
    } while (false);

    if (evtStart != nullptr)
    {
        evtStart(false);
    }
}

void LoggerClient::failedSendMessage(const RemoteMessage& /* msgFailed */, Socket& /* whichTarget */)
{
    FuncMessagingFailed evtFailed{ nullptr };
    do
    {
        Lock lock(mLock);
        if (mCallbacks != nullptr)
        {
            evtFailed = mCallbacks->evtMessagingFailed;
        }
    } while (false);


    if (evtFailed)
    {
        evtFailed();
    }
}

void LoggerClient::failedReceiveMessage(Socket& /* whichSource */)
{
    FuncMessagingFailed evtFailed{ nullptr };
    do
    {
        Lock lock(mLock);
        if (mCallbacks != nullptr)
        {
            evtFailed = mCallbacks->evtMessagingFailed;
        }
    } while (false);


    if (evtFailed)
    {
        evtFailed();
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
