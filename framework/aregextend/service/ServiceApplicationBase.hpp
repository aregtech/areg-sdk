#ifndef AREG_AREGEXTEND_SERVICE_SERVICEAPPLICATIONBASE_HPP
#define AREG_AREGEXTEND_SERVICE_SERVICEAPPLICATIONBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/ServiceApplicationBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service application base class.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/persist/ConfigListener.hpp"
#include "aregextend/service/SystemServiceBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg { class ConnectionProvider; }

//////////////////////////////////////////////////////////////////////////
// ServiceApplicationBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Brief class to create a process that may run as console application,
 *          as well as system service. It extends the SystemServiceBase
 *          and implements basic methods. As well as it provides interface
 *          of configuration listener.
 **/
class ServiceApplicationBase    : public    aregext::SystemServiceBase
                                , protected areg::ConfigListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Constructor, initializes the remote service communication instance.
     * \param   commBase    The instance of the remote service communication object
     *                      that provides the connection implementation, sends and receives messages.
     *                      The object is required by base class of the System Service.
     **/
    ServiceApplicationBase(ServiceCommunicationBase& commBase);

    virtual ~ServiceApplicationBase() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Reads data from the console and sets in the buffer.
     * \param[in,out]   buffer  The buffer to input data from console. Should not be nullptr.
     * \param[in]       bufSize The size of the buffer. Should not be zero.
     * \returns Returns true if succeeded to read data from the console.
     **/
    bool inputConsoleData(char* buffer, uint32_t bufSize);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// ServiceApplicationBase overrides
/************************************************************************/

    /**
     * \brief   Returns list of the options to validate contained in the pair object,
     *          where the first entry is the pointer to the list and second entry is
     *          the number of elements in the list
     **/
    virtual std::pair<const OptionParser::OptionSetup*, int32_t> getAppOptions() const = 0;

    /**
     * \brief   Returns the UNICODE name of the service application.
     **/
    virtual wchar_t* getServiceNameW() const = 0;

     /**
      * \brief   Returns the ASCII name of the service application.
      **/
    virtual char* getServiceNameA() const = 0;

    /**
     * \brief   Returns the UNICODE display name of the service application.
     *          This optional display name could be valid only for specific OS.
     *          For example, in Windows this name is displayed in the list of services.
     **/
    virtual wchar_t* getServiceDisplayNameW() const = 0;

     /**
      * \brief   Returns the ASCII display name of the service application.
      *          This optional display name could be valid only for specific OS.
      *          For example, in Windows this name is displayed in the list of services.
      **/
    virtual char* getServiceDisplayNameA() const = 0;

    /**
     * \brief   Returns the UNICODE description of the service application.
     *          This optional service description could be valid only for specific OS.
     *          For example, in Windows this description is shown in the list of services.
     **/
    virtual wchar_t* getServiceDescriptionW() const = 0;

    /**
     * \brief   Returns the ASCII description of the service application.
     *          This optional service description could be valid only for specific OS.
     *          For example, in Windows this description is shown in the list of services.
     **/
    virtual char* getServiceDescriptionA() const = 0;

    /**
     * \brief   Returns the type of the remote service.
     *          Valid only for Areg SDK services.
     **/
    virtual areg::RemoteServiceKind getServiceType() const = 0;

    /**
     * \brief   Returns the type of the connection of the remote services.
     *          Valid only for Areg SDK services.
     **/
    virtual areg::ConnectionType getConnectionType() const = 0;

/************************************************************************/
// SystemServiceBase overrides
/************************************************************************/

    /**
     * \brief   Is the main entry point to install, uninstall, register and start service.
     *          Normally, called from the main() method.
     * \param   optStartup  Option that is set to start service. Can be ServiceOption::CMD_Undefined
     *                      if need to run with default option.
     * \param   argument    Option argument. Can be empty or nullptr if no argument is expected
     *                      or need to use default value.
     * \return  The result of execution.
     **/
    int32_t serviceMain(aregext::ServiceOption optStartup, const char* argument) override;

    /**
     * \brief   Triggered to initialize the service application.
     * \param   option      The option that was set to run. Can be ServiceOption::CMD_Undefined if unknown or should be ignored.
     * \param   value       The option value as a string. Can be empty string or nullptr if should be ignored.
     * \param   fileConfig  The pointer to the configuration file. Can be empty or nullptr if should be ignored.
     * \return  Returns true if succeeded to initialize application and the application can run.
     *          Otherwise, the application run should be interrupted and the failure code 1 is returned.
     **/
    bool serviceInitialize(aregext::ServiceOption option, const char* value, const char* fileConfig) override;

    /**
     * \brief   Triggered when application is going to exit.
     **/
    void serviceRelease() override;

    /**
     * \brief   Call to install (register) message router service in the system.
     * \return  Returns true if registration succeeded.
     **/
    bool serviceInstall() override;

    /**
     * \brief   Call to uninstall (unregister) message router service in the system.
     **/
    void serviceUninstall() override;

    /**
     * \brief   Registers system service in the system.
     **/
    bool registerService() override;

    /**
     * \brief   Opens operating system service DB for further processing.
     * \return  Returns true if succeeded.
     **/
    bool serviceOpen() override;

    /**
     * \brief   Called to start message router service.
     * \return  Returns true, if started with success.
     **/
    bool serviceStart() override;

    /**
     * \brief   Called to pause message router service.
     **/
    void servicePause() override;

    /**
     * \brief   Called to resume paused message router service.
     **/
    bool serviceContinue() override;

    /**
     * \brief   Called to stop message router service.
     **/
    void serviceStop() override;

    /**
     * \brief   Called to shutdown the system service.
     **/
    void serviceShutdown() override;

    /**
     * \brief   Sets the state of message router service.
     **/
    bool setState( aregext::ServicePhase newState ) override;

    /**
     * \brief   Run application as a background process without input or output on console.
     **/
    void runService() override;

    /**
     * \brief   Called to setup service and start service dispatcher.
     * \return  Returns value indicating the successful state of the operation.
     *          If returns RESULT_SUCCEEDED, it succeeded to start the service dispatcher (Windows related).
     *          If returns RESULT_IGNORED, the operation is ignored (case for POSIX or if dispatcher started).
     *          In all other cases it should return RESULT_FAILED_INIT.
     **/
    int32_t startServiceDispatcher() override;

protected:

/************************************************************************/
// ConfigListener protected overrides
/************************************************************************/

    /**
     * \brief   Called by configuration manager before the configuration is saved in the file.
     * \param   config  The instance of configuration manager.
     **/
    void prepareSaveConfiguration(areg::ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager after the configuration is saved in the file.
     * \param   config  The instance of configuration manager.
     **/
    void postSaveConfiguration(areg::ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager before the configuration is loaded from the file.
     * \param   config  The instance of configuration manager.
     **/
    void prepareReadConfiguration(areg::ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager when configuration is completed to load data from the file.
     * \param   config  The instance of configuration manager.
     **/
    void postReadConfiguration(areg::ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager after setting read-only and writable properties.
     *          For example, when the default configuration is set.
     * \param   listReadonly    The list of read-only properties to set in the configuration.
     * \param   listWritable    The list of module / process specific properties to set in the configuration;
     * \param   config          The instance of configuration manager.
     **/
    void onSetupConfiguration(const areg::ListProperties& listReadonly, const areg::ListProperties& listWritable, areg::ConfigManager& config) override;

//////////////////////////////////////////////////////////////////////////
// OS specific hidden methods.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS specific validity check of the service.
     **/
    bool _osIsValid() const;

    /**
     * \brief   Called to free engaged resources.
     **/
    void _osFreeResources();

    /**
     * \brief   OS specific initialization of the service.
     **/
    bool _osInitializeService();

    /**
     * \brief   OS specific implementation to open service.
     **/
    bool _osOpenService();

    /**
     * \brief   OS specific implementation to create service.
     **/
    bool _osCreateService();

    /**
     * \brief   OS specific implementation of deleting service.
     **/
    void _osDeleteService();

    /**
     * \brief   Registers service and returns true if handle is valid.
     *          The method is valid for Windows OS.
     **/
    bool _osRegisterService();

    /**
     * \brief   OS specific implementation of changing the state of the log collector service.
     **/
    bool _osSetState( aregext::ServicePhase newState );

    /**
     * \brief   OS specific implementation to setup the service and start the dispatcher.
     * \return  Returns value indicating the successful state of the operation.
     *          If returns RESULT_SUCCEEDED, it succeeded to start the service dispatcher (Windows related).
     *          If returns RESULT_IGNORED, the operation is ignored (case for POSIX or if dispatcher started).
     *          In all other cases it should return RESULT_FAILED_INIT.
     **/
    int32_t _osStartServiceDispatcher();

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    bool    mServiceSetup;  //!< Flag, indicating whether service si setup or not.

//////////////////////////////////////////////////////////////////////////
// Hidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(ServiceApplicationBase);
};

#endif  // AREG_AREGEXTEND_SERVICE_SERVICEAPPLICATIONBASE_HPP
