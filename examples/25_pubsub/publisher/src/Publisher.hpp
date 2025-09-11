#pragma once

/************************************************************************
 * \file        publisher/src/Publisher.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is demonstration of data PubSub (Publish / Subscribe) features.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "areg/component/Component.hpp"
#include "examples/25_pubsub/services/PubSubStub.hpp"

#include "areg/base/Thread.hpp"
#include "areg/component/Timer.hpp"
#include "aregextend/console/OptionParser.hpp"

/**
 * \brief   This service provider contains attributes (data) that send notifications on update.
 *          There are 2 types of attributes (data)
 *              1.  Update only when value is changed.
 *              2.  Update always when value is set.
 *          The service consumers that connect to the service provider should be able
 *          automatically receive update notifications:
 *              -   When first time subscribe on data update to get the actual value.
 *              -   When data is updated, depending on the update type (always or only on update).
 *              -   When data is invalidated.
 *          All updates must happen automatic, service providers must not receive unnecessary update notifications.
 *          There are 2 types of data updated: on change and always.
 *              - On change: this means to receive update notifications
 *                           only if the value is changed. For example,
 *                           the string should be received only when value changed.
 *              - Always   : this means to receive update notification each
 *                           the value is set even if the value is not updated.
 **/
class Publisher : public    Component
                , protected PubSubStub
                , private   IETimerConsumer
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
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    Publisher( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

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
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired. 
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

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

/************************************************************************/
// StubBase overrides.
/************************************************************************/

    /**
     * \brief   Triggered when proxy client either connected or disconnected to stub.
     * \param   client  The address of proxy client, which connection status is changed.
     * \param   status  The service consumer connection status.
     * \return  Returns true if connected service consumer is relevant to the provider.
     **/
    virtual bool clientConnected(const ProxyAddress & client, NEService::eServiceConnection status) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    //!< Starts updating values.
    void start(void);

    //!< Pauses and stop updating values.
    void stop(void);

    //!< Invalidates the values, on the next start the value should be reset and validated.
    void invalidate(void);

    //!< Quits the service provider application.
    void quit(void);

    //! Outputs message on console
    inline void printMessage(const String & message, eCommands cmd);

    //! Wrapper of the this pointer
    inline Publisher & self(void);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    Timer       mTimerOnChange;     //!< The timer to update values with feature to notify on update
    Timer       mTimerAlways;       //!< The timer to update values with feature to notify always
    int32_t     mClientCount;       //!< The number of connected clients.
    uint32_t    mSeqString;         //!< The sequence number of the string.
    uint16_t    mCountString;       //!< The count number of the string to change.
    uint32_t    mSeqInteger;        //!< The sequence number of the integer.
    uint16_t    mCountInteger;      //!< The count number of the integer to change.
    Thread      mConsoleThread;     //!< The thread to run console to interact with users.
    Mutex       mLock;              //!< Synchronization object for multithreading environment.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    Publisher(void) = delete;
    DECLARE_NOCOPY_NOMOVE(Publisher);
};
