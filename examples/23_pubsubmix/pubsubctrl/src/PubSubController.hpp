#pragma once

/************************************************************************
 * \file        pubsubctrl/src/PubSubController.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is demonstration of data PubSub (Publish / Subscribe) features.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/component/Component.hpp"

#include "areg/base/Thread.hpp"
#include "extend/console/OptionParser.hpp"
#include "common/src/Publisher.hpp"
#include "common/src/Subscriber.hpp"

/**
 * \brief   This service provider is a the main Publisher that in addition outputs
 *          the information or options and waits for user input. It starts a separate
 *          thread and waits for input the command. If user inputs the command to quit,
 *          it changes the state of the service, so that all processes quit as well.
 **/
class PubSubController  : protected Component
                        , private   Publisher
                        , private   IEThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// The list of internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    //!< The commands of PubSub
    enum class eCommands : int
    {
          CMD_Undefined     //!< Undefined command, no command is entered
        , CMD_Error         //!< Error happened
        , CMD_Invalidate    //!< Invalidate data
        , CMD_Pause         //!< Pause updating data
        , CMD_Start         //!< Start time to update data
        , CMD_Quit          //!< Quit application
        , CMD_Help          //!< Print help on console
    };

    //!< The list of valid options
    static const OptionParser::sOptionSetup ValidOptions[];

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    PubSubController( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~PubSubController( void );

//////////////////////////////////////////////////////////////////////////
// Overrides.
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// Component overrides.
/************************************************************************/

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to start up. Set listeners and make
     *          initialization in this function call.
     * \param	comThread	The component thread, which triggered startup command
     **/
    virtual void startupComponent(ComponentThread & comThread) override;

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    virtual void shutdownComponent( ComponentThread & comThread ) override;

/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   This callback function is called from Thread object, when it is 
     *          running and fully operable. If thread needs run in loop, the loop 
     *          should be implemented here. When consumer exits this function, 
     *          the thread will complete work. To restart thread running, 
     *          createThread() method should be called again.
     **/
    virtual void onThreadRuns( void ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    //! Outputs message on console
    inline void printMessage(const String & message, eCommands cmd);

    //! Wrapper of the this pointer
    inline PubSubController & self(void);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    Subscriber  mSubscriber;        //!< A subscriber to this publisher that runs in the same thread. Made for testing purpose.
    Thread      mConsoleThread;     //!< The thread to run console to interact with users.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PubSubController(void) = delete;
    DECLARE_NOCOPY_NOMOVE(PubSubController);
};
