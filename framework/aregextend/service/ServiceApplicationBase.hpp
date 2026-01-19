#ifndef AREG_AREGEXTEND_SERVICE_SERVICEAPPLICATIONBASE_HPP
#define AREG_AREGEXTEND_SERVICE_SERVICEAPPLICATIONBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/ServiceApplicationBase.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service application base class.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/persist/IEConfigurationListener.hpp"
#include "aregextend/service/SystemServiceBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEServiceConnectionProvider;

//////////////////////////////////////////////////////////////////////////
// ServiceApplicationBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Brief class to create a process that may run as console application,
 *          as well as system service. It extends the SystemServiceBase
 *          and implements basic methods. As well as it provides interface
 *          of configuration listener.
 **/
class ServiceApplicationBase    : public    SystemServiceBase
                                , protected IEConfigurationListener
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
    ServiceApplicationBase(ServiceCommunicatonBase& commBase);

    virtual ~ServiceApplicationBase(void) = default;

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
    bool inputConsoleData(char* buffer, unsigned int bufSize);

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
    virtual std::pair<const OptionParser::sOptionSetup*, int> getAppOptions(void) const = 0;

    /**
     * \brief   Returns the UNICODE name of the service application.
     **/
    virtual wchar_t* getServiceNameW(void) const = 0;

     /**
      * \brief   Returns the ASCII name of the service application.
      **/
    virtual char* getServiceNameA(void) const = 0;

    /**
     * \brief   Returns the UNICODE display name of the service application.
     *          This optional display name could be valid only for specific OS.
     *          For example, in Windows this name is displayed in the list of services.
     **/
    virtual wchar_t* getServiceDisplayNameW(void) const = 0;

     /**
      * \brief   Returns the ASCII display name of the service application.
      *          This optional display name could be valid only for specific OS.
      *          For example, in Windows this name is displayed in the list of services.
      **/
    virtual char* getServiceDisplayNameA(void) const = 0;

    /**
     * \brief   Returns the UNICODE description of the service application.
     *          This optional service description could be valid only for specific OS.
     *          For example, in Windows this description is shown in the list of services.
     **/
    virtual wchar_t* getServiceDescriptionW(void) const = 0;

    /**
     * \brief   Returns the ASCII description of the service application.
     *          This optional service description could be valid only for specific OS.
     *          For example, in Windows this description is shown in the list of services.
     **/
    virtual char* getServiceDescriptionA(void) const = 0;

    /**
     * \brief   Returns the type of the remote service.
     *          Valid only for AREG SDK services.
     **/
    virtual NERemoteService::eRemoteServices getServiceType(void) const = 0;

    /**
     * \brief   Returns the type of the connection of the remote services.
     *          Valid only for AREG SDK services.
     **/
    virtual NERemoteService::eConnectionTypes getConnectionType(void) const = 0;

/************************************************************************/
// SystemServiceBase overrides
/************************************************************************/

    /**
     * \brief   Is the main entry point to install, uninstall, register and start service.
     *          Normally, called from the main() method.
     * \param   optStartup  Option that is set to start service. Can be eServiceOption::CMD_Undefined
     *                      if need to run with default option.
     * \param   argument    Option argument. Can be empty or nullptr if no argument is expected
     *                      or need to use default value.
     * \return  The result of execution.
     **/
    virtual int serviceMain(NESystemService::eServiceOption optStartup, const char* argument) override;

    /**
     * \brief   Triggered to initialize the service application.
     * \param   option      The option that was set to run. Can be eServiceOption::CMD_Undefined if unknown or should be ignored.
     * \param   value       The option value as a string. Can be empty string or nullptr if should be ignored.
     * \param   fileConfig  The pointer to the configuration file. Can be empty or nullptr if should be ignored.
     * \return  Returns true if succeeded to initialize application and the application can run.
     *          Otherwise, the application run should be interrupted and the failure code 1 is returned.
     **/
    virtual bool serviceInitialize(NESystemService::eServiceOption option, const char* value, const char* fileConfig) override;

    /**
     * \brief   Triggered when application is going to exit.
     **/
    virtual void serviceRelease( void ) override;

    /**
     * \brief   Call to install (register) message router service in the system.
     * \return  Returns true if registration succeeded.
     **/
    virtual bool serviceInstall( void ) override;

    /**
     * \brief   Call to uninstall (unregister) message router service in the system.
     **/
    virtual void serviceUninstall( void ) override;

    /**
     * \brief   Registers system service in the system.
     **/
    virtual bool registerService( void ) override;

    /**
     * \brief   Opens operating system service DB for further processing.
     * \return  Returns true if succeeded.
     **/
    virtual bool serviceOpen( void ) override;

    /**
     * \brief   Called to start message router service.
     * \return  Returns true, if started with success.
     **/
    virtual bool serviceStart( void ) override;

    /**
     * \brief   Called to pause message router service.
     **/
    virtual void servicePause( void ) override;

    /**
     * \brief   Called to resume paused message router service.
     **/
    virtual bool serviceContinue( void ) override;

    /**
     * \brief   Called to stop message router service.
     **/
    virtual void serviceStop( void ) override;

    /**
     * \brief   Called to shutdown the system service.
     **/
    virtual void serviceShutdown(void) override;

    /**
     * \brief   Sets the state of message router service.
     **/
    virtual bool setState( NESystemService::eSystemServiceState newState ) override;

    /**
     * \brief   Run application as a background process without input or output on console.
     **/
    virtual void runService(void) override;

    /**
     * \brief   Called to setup service and start service dispatcher.
     * \return  Returns value indicating the successful state of the operation.
     *          If returns RESULT_SUCCEEDED, it succeeded to start the service dispatcher (Windows related).
     *          If returns RESULT_IGNORED, the operation is ignored (case for POSIX or if dispatcher started).
     *          In all other cases it should return RESULT_FAILED_INIT.
     **/
    virtual int startServiceDispatcher( void ) override;

protected:

/************************************************************************/
// IEConfigurationListener protected overrides
/************************************************************************/

    /**
     * \brief   Called by configuration manager before the configuration is saved in the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void prepareSaveConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager after the configuration is saved in the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void postSaveConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager before the configuration is loaded from the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void prepareReadConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager when configuration is completed to load data from the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void postReadConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager after setting read-only and writable properties.
     *          For example, when the default configuration is set.
     * \param   listReadonly    The list of read-only properties to set in the configuration.
     * \param   listWritable    The list of module / process specific properties to set in the configuration;
     * \param   config          The instance of configuration manager.
     **/
    virtual void onSetupConfiguration(const NEPersistence::ListProperties& listReadonly, const NEPersistence::ListProperties& listWritable, ConfigManager& config) override;

//////////////////////////////////////////////////////////////////////////
// OS specific hidden methods.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS specific validity check of the service.
     **/
    bool _osIsValid( void ) const;

    /**
     * \brief   Called to free engaged resources.
     **/
    void _osFreeResources( void );

    /**
     * \brief   OS specific initialization of the service.
     **/
    bool _osInitializeService( void );

    /**
     * \brief   OS specific implementation to open service.
     **/
    bool _osOpenService( void );

    /**
     * \brief   OS specific implementation to create service.
     **/
    bool _osCreateService( void );

    /**
     * \brief   OS specific implementation of deleting service.
     **/
    void _osDeleteService( void );

    /**
     * \brief   Registers service and returns true if handle is valid.
     *          The method is valid for Windows OS.
     **/
    bool _osRegisterService( void );

    /**
     * \brief   OS specific implementation of changing the state of the log collector service.
     **/
    bool _osSetState( NESystemService::eSystemServiceState newState );

    /**
     * \brief   OS specific implementation of waiting for user input on console.
     * \param   buffer  The allocated buffer to stream input from console.
     * \param   bufSize The size of allocated bugger.
     * \return  Returns true if succeeded to get user input.
     **/
    bool _osWaitUserInput(char* buffer, unsigned int bufSize);

    /**
     * \brief   OS specific implementation to setup the service and start the dispatcher.
     * \return  Returns value indicating the successful state of the operation.
     *          If returns RESULT_SUCCEEDED, it succeeded to start the service dispatcher (Windows related).
     *          If returns RESULT_IGNORED, the operation is ignored (case for POSIX or if dispatcher started).
     *          In all other cases it should return RESULT_FAILED_INIT.
     **/
    int _osStartServiceDispatcher( void );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    bool    mServiceSetup;  //!< Flag, indicating whether service si setup or not.

//////////////////////////////////////////////////////////////////////////
// Hidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(ServiceApplicationBase);
};

#endif  // AREG_AREGEXTEND_SERVICE_SERVICEAPPLICATIONBASE_HPP
