#ifndef AREG_AREGEXTEND_SERVICE_SYSTEMSERVICEBASE_HPP
#define AREG_AREGEXTEND_SERVICE_SYSTEMSERVICEBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/SystemServiceBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, base class to create system services.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "aregextend/service/SystemServiceDefs.hpp"
#include "aregextend/service/ServiceCommunicationBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class ConfigListener;

//////////////////////////////////////////////////////////////////////////
// SystemServiceBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The message routing service is a separate process, which routs messages
 *          to components. Applications, developed based on Areg SDK,
 *          should connect to message routing service to send and receive IPC messages.
 *          The business logic of message router to know logical relationship
 *          of running components and know where to redirect messages.
 *          Normally, for every connection channel type there should one instance of 
 *          message routing service also called as Multi-casting router (MCR).
 **/
class SystemServiceBase
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   SystemServiceBase::RESULT_SUCCEEDED
     *          Returns value indicating no error, i.e. operation succeeded.
     **/
    static constexpr int32_t        RESULT_SUCCEEDED        { 0 };

    /**
     * \brief   SystemServiceBase::RESULT_FAILED_INIT
     *          Returns value indicating error during initialization.
     **/
    static constexpr int32_t        RESULT_FAILED_INIT      { 1 };

    /**
     * \brief   SystemServiceBase::RESULT_FAILED_INSTALL
     *          Returns value indicating error during installation of the service.
     *          Valid for Windows OS and ignored in Linux.
     **/
    static constexpr int32_t        RESULT_FAILED_INSTALL   { 2 };

    /**
     * \brief   SystemServiceBase::RESULT_FAILED_RUN
     *          Returns value indicating error during run.
     **/
    static constexpr int32_t        RESULT_FAILED_RUN       { 3 };

    /**
     * \brief   SystemServiceBase::RESULT_IGNORED
     *          Returns value indicating that the operation was ignored.
     **/
    static constexpr int32_t        RESULT_IGNORED          { 4 };

    /**
     * \brief   SystemServiceBase::ServiceControl
     *          The control constants of the service.
     **/
    enum class ServiceControl   : uint8_t
    {
          ServiceStop       = 1 //!< The service received control to stop.
        , ServicePause          //!< The service received control to pause.
        , ServiceContinue       //!< The service received control to resume paused service.
        , ServiceShutdown       //!< The service received control to shutdown.
    };

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes instance of message router service.
     **/
    SystemServiceBase( ServiceCommunicationBase & commBase );
    /**
     * \brief   Destructor.
     **/
    virtual ~SystemServiceBase() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Triggered when the system changes the service control state.
     *          This is operated via operating system and valid for Windows OS.
     *          In Linux OS this has no effect.
     * \param   control     The control triggered by system.
     **/
    void controlService(SystemServiceBase::ServiceControl control);

/************************************************************************/
// SystemServiceBase overrides
/************************************************************************/

    /**
     * \brief   Parses the NESystemService::ServiceOption options passed
     *          in the command line and returns true if succeeded.
     *          Override and implement this method if the options differ than
     *          NESystemService::ServiceOption
     * \param   argc    The number of options to parse.
     * \param   argv    The options to parse.
     * \return  Returns true if succeeded to parse command line options,
     *          found no failure and the application can continue working.
     *          To interrupt the application, return false.
     **/
    virtual bool parseOptions( int32_t argc, const char ** argv, const OptionParser::sOptionSetup * optSetup, uint32_t optCount );
    virtual bool parseOptions( int32_t argc, char** argv, const OptionParser::sOptionSetup* optSetup, uint32_t optCount);

    /**
     * \brief   Checks the listed options and prepares to dispatch.
     *          In case of failure or request to display manual, prints the
     *          help to use command line options properly.
     *          Override the method if need extra checkups or preparations.
     * \param   opts    The list of options. By default, they are
     *                  type of NESystemService::ServiceOption.
     * \return  Returns true if found no failure and the application can continue working.
     *          To interrupt the application, return false.
     **/
    virtual bool prepareOptions(const OptionParser::InputOptionList& opts);

    /**
     * \brief   Dispatches the option, makes basic initialization like setting configuration file or set verbose flag.
     * \param   opt     The option to dispatch.
     * \return  If operation succeeded, returns true. Otherwise, if the option is not recognized or value is unexpected,
     *          returns false.
     **/
    virtual bool dispatchOption(const OptionParser::sOption& opt);

    /**
     * \brief   Is the main entry point to install, uninstall, register and start service.
     *          Normally, called from the main() method.
     * \param   optStartup  Option that is set to start service. Can be ServiceOption::CMD_Undefined
     *                      if need to run with default option.
     * \param   argument    Option argument. Can be empty or nullptr if no argument is expected
     *                      or need to use default value.
     * \return  The result of execution.
     **/
    virtual int32_t serviceMain(NESystemService::ServiceOption optStartup, const char* argument);

    /**
     * \brief   Sends remote message to the target specified in the message structure.
     * \param   message     The message to send to the target. The message contains information of the source and target.
     **/
    virtual void sendMessageToTarget(const RemoteMessage& message);

    /**
     * \brief   Triggered to initialize the service application.
     * \param   option      The option that was set to run. Can be ServiceOption::CMD_Undefined if unknown or should be ignored.
     * \param   value       The option value as a string. Can be empty string or nullptr if should be ignored.
     * \param   fileConfig  The pointer to the configuration file. Can be empty or nullptr if should be ignored.
     * \return  Returns true if succeeded to initialize application and the application can run.
     *          Otherwise, the application run should be interrupted and the failure code 1 is returned.
     **/
    virtual bool serviceInitialize(NESystemService::ServiceOption option, const char* value, const char* fileConfig) = 0;

    /**
     * \brief   Triggered when service application is going to exit.
     **/
    virtual void serviceRelease() = 0;

    /**
     * \brief   Call to create and install the service in the system.
     * \return  Returns true if registration succeeded.
     **/
    virtual bool serviceInstall() = 0;

    /**
     * \brief   Call to delete and uninstall the service in the system.
     **/
    virtual void serviceUninstall() = 0;

    /**
     * \brief   Registers system service in the system.
     **/
    virtual bool registerService() = 0;

    /**
     * \brief   Opens operating system service for further processing.
     * \return  Returns true if succeeded.
     **/
    virtual bool serviceOpen() = 0;

    /**
     * \brief   Called to start the system service.
     * \return  Returns true, if started with success.
     **/
    virtual bool serviceStart() = 0;

    /**
     * \brief   Called to pause the system service.
     **/
    virtual void servicePause() = 0;

    /**
     * \brief   Called to resume paused system service.
     **/
    virtual bool serviceContinue() = 0;

    /**
     * \brief   Called to stop the system service.
     **/
    virtual void serviceStop() = 0;

    /**
     * \brief   Called to shutdown the system service.
     **/
    virtual void serviceShutdown() = 0;

    /**
     * \brief   Sets the state of the system service.
     **/
    virtual bool setState( NESystemService::ServicePhase newState ) = 0;

    /**
     * \brief   Called to setup service and start service dispatcher.
     * \return  Returns value indicating the successful state of the operation.
     *          If returns RESULT_SUCCEEDED, it succeeded to start the service dispatcher (Windows related).
     *          If returns RESULT_IGNORED, the operation is ignored (case for POSIX or if dispatcher started).
     *          In all other cases it should return RESULT_FAILED_INIT.
     **/
    virtual int32_t startServiceDispatcher() = 0;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns current command of message router service.
     **/
    inline NESystemService::ServiceOption getCurrentOption() const;

    /**
     * \brief   Sets the current command of message router service.
     * \param   optService  The router service command option to set.
     **/
    inline void setCurrentOption( NESystemService::ServiceOption optService );

    /**
     * \brief   Returns the state of message router service.
     **/
    inline NESystemService::ServicePhase getState() const;

    /**
     * \brief   Returns the instance of data rate helper object to use when computing data rate.
     **/
    inline DataRateHelper& getDataRateHelper() const;

    /**
     * \brief   Return the instance of the communication controller object.
     **/
    inline ServiceCommunicationBase& getCommunicationController() const;

    /**
     * \brief   Resets default options.
     **/
    void resetDefaultOptions();

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// SystemServiceBase protected overrides
/************************************************************************/

    /**
     * \brief   Triggered to print the help message on console.
     * \param   isCmdLine   Flag indicating whether it should print the help
     *                      of using service in command line or help of user input commands.
     *                      If 'true', the printing message is about using the service in
     *                      command line. Otherwise, if application expects user inputs, prints
     *                      the help of command options.
     **/
    virtual void printHelp( bool isCmdLine ) = 0;

    /**
     * \brief   Triggered to start the console service.
     **/
    virtual void startConsoleService() = 0;

    /**
     * \brief   Stops the consoler service.
     **/
    virtual void stopConsoleService() = 0;

    /**
     * \brief   Triggered to receive a function to validate and check the input option values.
     **/
    virtual Console::CallBack getOptionCheckCallback() const = 0;

    /**
     * \brief   Triggered if need to run console with extended features.
     *          In extended feature, the console can output message at any position on the screen.
     **/
    virtual void runConsoleInputExtended() = 0;

    /**
     * \brief   Triggered if need to run console with simple (not extended) features.
     **/
    virtual void runConsoleInputSimple() = 0;

    /**
     * \brief   Run application as a background process without input or output on console.
     **/
    virtual void runService() = 0;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    ServiceCommunicationBase &               mCommunication;
    /**
     * \brief   The message router service state.
     **/
    NESystemService::ServicePhase    mSystemServiceState;
    /**
     * \brief   The current command to execute by message router service.
     **/
    NESystemService::ServiceOption         mSystemServiceOption;
    /**
     * \brief   OS specific service handle
     **/
    void *                                  mSvcHandle;
    /**
     * \brief   OS specific service manager handle.
     **/
    void *                                  mSeMHandle;
    /**
     * \brief   The relative or full path to the configuration file. By default, NEApplication::DEFAULT_CONFIG_FILE
     **/
    String                                  mFileConfig;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SystemServiceBase );
};

//////////////////////////////////////////////////////////////////////////
// SystemServiceBase class inline methods.
//////////////////////////////////////////////////////////////////////////

inline NESystemService::ServicePhase SystemServiceBase::getState() const
{
    return mSystemServiceState;
}

inline DataRateHelper& SystemServiceBase::getDataRateHelper() const
{
    return  mCommunication.getDataRateHelper();
}

inline ServiceCommunicationBase& SystemServiceBase::getCommunicationController() const
{
    return const_cast<ServiceCommunicationBase&>(mCommunication);
}

inline NESystemService::ServiceOption SystemServiceBase::getCurrentOption() const
{
    return mSystemServiceOption;
}

inline void SystemServiceBase::setCurrentOption( NESystemService::ServiceOption optService )
{
    mSystemServiceOption = optService;
}

#endif  // AREG_AREGEXTEND_SERVICE_SYSTEMSERVICEBASE_HPP
