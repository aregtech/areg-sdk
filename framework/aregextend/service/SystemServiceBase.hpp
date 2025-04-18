#ifndef AREG_AREGEXTEND_SERVICE_SYSTEMSERVICEBASE_HPP
#define AREG_AREGEXTEND_SERVICE_SYSTEMSERVICEBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/service/SystemServiceBase.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, base class to create system services.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "aregextend/service/NESystemService.hpp"
#include "aregextend/service/ServiceCommunicatonBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEConfigurationListener;

//////////////////////////////////////////////////////////////////////////
// SystemServiceBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The message routing service is a separate process, which routs messages
 *          to components. Applications, developed based on AREG SDK,
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
    static constexpr int        RESULT_SUCCEEDED        { 0 };

    /**
     * \brief   SystemServiceBase::RESULT_FAILED_INIT
     *          Returns value indicating error during initialization.
     **/
    static constexpr int        RESULT_FAILED_INIT      { 1 };

    /**
     * \brief   SystemServiceBase::RESULT_FAILED_INSTALL
     *          Returns value indicating error during installation of the service.
     *          Valid for Windows OS and ignored in Linux.
     **/
    static constexpr int        RESULT_FAILED_INSTALL   { 2 };

    /**
     * \brief   SystemServiceBase::RESULT_FAILED_RUN
     *          Returns value indicating error during run.
     **/
    static constexpr int        RESULT_FAILED_RUN       { 3 };

    /**
     * \brief   SystemServiceBase::RESULT_IGNORED
     *          Returns value indicating that the operation was ignored.
     **/
    static constexpr int        RESULT_IGNORED          { 4 };

    /**
     * \brief   SystemServiceBase::eServiceControl
     *          The control constants of the service.
     **/
    typedef enum E_ServiceControl
    {
          ServiceStop       = 1 //!< The service received control to stop.
        , ServicePause          //!< The service received control to pause.
        , ServiceContinue       //!< The service received control to resume paused service.
        , ServiceShutdown       //!< The service received control to shutdown.
    } eServiceControl;

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes instance of message router service.
     **/
    SystemServiceBase( ServiceCommunicatonBase & commBase );
    /**
     * \brief   Destructor.
     **/
    virtual ~SystemServiceBase( void ) = default;

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
    void controlService(SystemServiceBase::eServiceControl control);

/************************************************************************/
// SystemServiceBase overrides
/************************************************************************/

    /**
     * \brief   Parses the NESystemService::eServiceOption options passed
     *          in the command line and returns true if succeeded.
     *          Override and implement this method if the options differ than
     *          NESystemService::eServiceOption
     * \param   argc    The number of options to parse.
     * \param   argv    The options to parse.
     * \return  Returns true if succeeded to parse command line options,
     *          found no failure and the application can continue working.
     *          To interrupt the application, return false.
     **/
    virtual bool parseOptions( int argc, const char ** argv, const OptionParser::sOptionSetup * optSetup, uint32_t optCount );
    virtual bool parseOptions( int argc, char** argv, const OptionParser::sOptionSetup* optSetup, uint32_t optCount);

    /**
     * \brief   Checks the listed options and prepares to dispatch.
     *          In case of failure or request to display manual, prints the
     *          help to use command line options properly.
     *          Override the method if need extra checkups or preparations.
     * \param   opts    The list of options. By default, they are
     *                  type of NESystemService::eServiceOption.
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
     * \param   optStartup  Option that is set to start service. Can be eServiceOption::CMD_Undefined
     *                      if need to run with default option.
     * \param   argument    Option argument. Can be empty or nullptr if no argument is expected
     *                      or need to use default value.
     * \return  The result of execution.
     **/
    virtual int serviceMain(NESystemService::eServiceOption optStartup, const char* argument);

    /**
     * \brief   Sends remote message to the target specified in the message structure.
     * \param   message     The message to send to the target. The message contains information of the source and target.
     **/
    virtual void sendMessageToTarget(const RemoteMessage& message);

    /**
     * \brief   Triggered to initialize the service application.
     * \param   option      The option that was set to run. Can be eServiceOption::CMD_Undefined if unknown or should be ignored.
     * \param   value       The option value as a string. Can be empty string or nullptr if should be ignored.
     * \param   fileConfig  The pointer to the configuration file. Can be empty or nullptr if should be ignored.
     * \return  Returns true if succeeded to initialize application and the application can run.
     *          Otherwise, the application run should be interrupted and the failure code 1 is returned.
     **/
    virtual bool serviceInitialize(NESystemService::eServiceOption option, const char* value, const char* fileConfig) = 0;

    /**
     * \brief   Triggered when service application is going to exit.
     **/
    virtual void serviceRelease( void ) = 0;

    /**
     * \brief   Call to create and install the service in the system.
     * \return  Returns true if registration succeeded.
     **/
    virtual bool serviceInstall( void ) = 0;

    /**
     * \brief   Call to delete and uninstall the service in the system.
     **/
    virtual void serviceUninstall( void ) = 0;

    /**
     * \brief   Registers system service in the system.
     **/
    virtual bool registerService( void ) = 0;

    /**
     * \brief   Opens operating system service for further processing.
     * \return  Returns true if succeeded.
     **/
    virtual bool serviceOpen( void ) = 0;

    /**
     * \brief   Called to start the system service.
     * \return  Returns true, if started with success.
     **/
    virtual bool serviceStart( void ) = 0;

    /**
     * \brief   Called to pause the system service.
     **/
    virtual void servicePause( void ) = 0;

    /**
     * \brief   Called to resume paused system service.
     **/
    virtual bool serviceContinue( void ) = 0;

    /**
     * \brief   Called to stop the system service.
     **/
    virtual void serviceStop( void ) = 0;

    /**
     * \brief   Called to shutdown the system service.
     **/
    virtual void serviceShutdown( void ) = 0;

    /**
     * \brief   Sets the state of the system service.
     **/
    virtual bool setState( NESystemService::eSystemServiceState newState ) = 0;

    /**
     * \brief   Called to setup service and start service dispatcher.
     * \return  Returns value indicating the successful state of the operation.
     *          If returns RESULT_SUCCEEDED, it succeeded to start the service dispatcher (Windows related).
     *          If returns RESULT_IGNORED, the operation is ignored (case for POSIX or if dispatcher started).
     *          In all other cases it should return RESULT_FAILED_INIT.
     **/
    virtual int startServiceDispatcher( void ) = 0;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns current command of message router service.
     **/
    inline NESystemService::eServiceOption getCurrentOption( void ) const;

    /**
     * \brief   Sets the current command of message router service.
     * \param   optService  The router service command option to set.
     **/
    inline void setCurrentOption( NESystemService::eServiceOption optService );

    /**
     * \brief   Returns the state of message router service.
     **/
    inline NESystemService::eSystemServiceState getState( void ) const;

    /**
     * \brief   Returns the instance of data rate helper object to use when computing data rate.
     **/
    inline DataRateHelper& getDataRateHelper(void) const;

    /**
     * \brief   Return the instance of the communication controller object.
     **/
    inline ServiceCommunicatonBase& getCommunicationController(void) const;

    /**
     * \brief   Resets default options.
     **/
    void resetDefaultOptions(void);

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
    virtual void startConsoleService( void ) = 0;

    /**
     * \brief   Stops the consoler service.
     **/
    virtual void stopConsoleService( void ) = 0;

    /**
     * \brief   Triggered to receive a function to validate and check the input option values.
     **/
    virtual Console::CallBack getOptionCheckCallback( void ) const = 0;

    /**
     * \brief   Triggered if need to run console with extended features.
     *          In extended feature, the console can output message at any position on the screen.
     **/
    virtual void runConsoleInputExtended( void ) = 0;

    /**
     * \brief   Triggered if need to run console with simple (not extended) features.
     **/
    virtual void runConsoleInputSimple( void ) = 0;

    /**
     * \brief   Run application as a background process without input or output on console.
     **/
    virtual void runService(void) = 0;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    ServiceCommunicatonBase &               mCommunication;
    /**
     * \brief   The message router service state.
     **/
    NESystemService::eSystemServiceState    mSystemServiceState;
    /**
     * \brief   The current command to execute by message router service.
     **/
    NESystemService::eServiceOption         mSystemServiceOption;
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
    DECLARE_NOCOPY_NOMOVE( SystemServiceBase );
};

//////////////////////////////////////////////////////////////////////////
// SystemServiceBase class inline methods.
//////////////////////////////////////////////////////////////////////////

inline NESystemService::eSystemServiceState SystemServiceBase::getState( void ) const
{
    return mSystemServiceState;
}

inline DataRateHelper& SystemServiceBase::getDataRateHelper(void) const
{
    return  mCommunication.getDataRateHelper();
}

inline ServiceCommunicatonBase& SystemServiceBase::getCommunicationController(void) const
{
    return const_cast<ServiceCommunicatonBase&>(mCommunication);
}

inline NESystemService::eServiceOption SystemServiceBase::getCurrentOption(void) const
{
    return mSystemServiceOption;
}

inline void SystemServiceBase::setCurrentOption( NESystemService::eServiceOption optService )
{
    mSystemServiceOption = optService;
}

#endif  // AREG_AREGEXTEND_SERVICE_SYSTEMSERVICEBASE_HPP
