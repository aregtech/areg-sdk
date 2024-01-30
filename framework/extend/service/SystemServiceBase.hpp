#ifndef AREG_EXTEND_SERVICE_SYSTEMSERVICEBASE_HPP
#define AREG_EXTEND_SERVICE_SYSTEMSERVICEBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/service/private/SystemServiceBase.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, base class to create system services.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "extend/service/NESystemService.hpp"
#include "extend/service/ServiceCommunicatonBase.hpp"

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
    virtual bool parseOptions( int argc, const char ** argv, const OptionParser::sOptionSetup * optSetup, int optCount );
    virtual bool parseOptions( int argc, char** argv, const OptionParser::sOptionSetup* optSetup, int optCount);

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
     * \brief   Called from main to start execution of  message router service.
     * \param   argc    The 'argc' parameter passed from 'main', indicates the number of parameters passed to executable.
     * \param   argv    The 'argv' parameter passed from 'main', indicated parameters passed to executable.
     **/
    virtual void serviceMain( int argc, char ** argv );

    /**
     * \brief   Sends remote message to the target specified in the message structure.
     * \param   message     The message to send to the target. The message contains information of the source and target.
     **/
    virtual void sendMessageToTarget(const RemoteMessage& message);

    /**
     * \brief   Call to install (register) message router service in the system.
     * \return  Returns true if registration succeeded.
     **/
    virtual bool serviceInstall( void ) = 0;

    /**
     * \brief   Call to uninstall (unregister) message router service in the system.
     **/
    virtual void serviceUninstall( void ) = 0;

    /**
     * \brief   Registers system service in the system.
     **/
    virtual bool registerService( void ) = 0;

    /**
     * \brief   Opens operating system service DB for further processing.
     * \return  Returns true if succeeded.
     **/
    virtual bool serviceOpen( void ) = 0;

    /**
     * \brief   Called to start message router service.
     * \return  Returns true, if started with success.
     **/
    virtual bool serviceStart( void ) = 0;

    /**
     * \brief   Called to pause message router service.
     **/
    virtual void servicePause( void ) = 0;

    /**
     * \brief   Called to resume paused message router service.
     **/
    virtual bool serviceContinue( void ) = 0;

    /**
     * \brief   Called to stop message router service.
     **/
    virtual void serviceStop( void ) = 0;

    /**
     * \brief   Sets the state of message router service.
     **/
    virtual bool setState( NESystemService::eSystemServiceState newState ) = 0;

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
     * \param   cmdService  The message router service command to set.
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

inline NESystemService::eServiceOption SystemServiceBase::getCurrentOption(void) const
{
    return mSystemServiceOption;
}

inline void SystemServiceBase::setCurrentOption( NESystemService::eServiceOption optService )
{
    mSystemServiceOption = optService;
}

#endif  // AREG_EXTEND_SERVICE_SYSTEMSERVICEBASE_HPP
