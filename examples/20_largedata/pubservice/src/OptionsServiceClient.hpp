#pragma once

/************************************************************************
 * \file        pubservice/src/OptionsServiceClient.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/IEEventConsumer.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "generated/src/OptionsClientBase.hpp"

#include "areg/appbase/Console.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/TEEvent.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/SynchObjects.hpp"

#include <atomic>

class OptionEventData : public NEOptions::sOptionData
{
};

DECLARE_EVENT( OptionEventData, OptionEvent, IEOptionEventConsumer );

class OptionsServiceClient  : public    Component
                            , protected IEThreadConsumer
                            , protected IETimerConsumer
                            , protected IEOptionEventConsumer
                            , protected OptionsClientBase
{
//////////////////////////////////////////////////////////////////////////
// Internal constants and static members
//////////////////////////////////////////////////////////////////////////

    static constexpr Console::Coord     COORD_DATA_RATE { 0, 0 };

    static constexpr Console::Coord     COORD_ITEM_RATE { 0, 1 };

    static constexpr Console::Coord     COORD_INFO_SLEEP{ 0, 2 };

    static constexpr Console::Coord     COORD_OPTIONS   { 0, 3 };

    static constexpr Console::Coord     COORD_ERROR_INFO{ 0, 4 };

    static constexpr Console::Coord     COORD_APP_INFO  { 0, 5 };

    static constexpr Console::Coord     COORD_OPT_INFO  { 0, 5 };

    static constexpr std::string_view   ConsoleThread   { "Console Thread" };

    static constexpr std::string_view   FMT_DATA_RATE   { "Data Rate: generated [ % 4.02f ] %s / sec, sent [ % 4.02f ] %s / sec, lost [ % 4.02f ] %s / sec." };

    static constexpr std::string_view   FMT_ITEM_RATE   { "Item Rate: generated [ %u ], sent [ %u ], average size [ %u ] bytes." };

    static constexpr std::string_view   FMT_GEN_INFO    { "One block per [ %u ] ns ( %.02f ms), slept [ %u ] and ignored [ %u ] times."}

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
    OptionsServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor.
     **/
    virtual ~OptionsServiceClient(void) = default;


/************************************************************************
 * Response DataRate
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Response of data rate
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   genData     Computed rate of generated data per second.
     * \param   sentData    Computed rate of sent data per second.
     * \param   missedData  Computed rate of missed data. The data is missed if failed to copy within certain time-slot.
     *          TODO: remove if useless.
     * \see     requestDataRate
     **/
    virtual void responseDataRate( const NEOptions::sDataRate & genData, const NEOptions::sDataRate & sentData, const NEOptions::sDataRate & missedData ) override;

    /**
     * \brief   Overwrite to handle error of DataRate request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestDataRateFailed( NEService::eResultType FailureReason ) override;

/************************************************************************
 * Response PacketRate
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Response of packet rate.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   genPackets  Number of generate packet per second
     * \param   sentPackets Number of sent packet per second
     * \param   avSize      The average size of a packet in bytes.
     * \see     requestPacketRate
     **/
    virtual void responsePacketRate( unsigned int genPackets, unsigned int sentPackets, unsigned int avSize ) override;

    /**
     * \brief   Overwrite to handle error of PacketRate request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestPacketRateFailed( NEService::eResultType FailureReason ) override;

protected:
/************************************************************************/
// IEProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered by Proxy, when gets service connected event.
     *          Make client initializations in this function. No request
     *          will be processed until this function is not called for
     *          client object. Also set listeners here if client is interested
     *          to receive update notifications.
     * \param   isConnected     The flag, indicating whether service is connected
     *                          or disconnected. Make cleanups and stop sending
     *                          requests or assigning for notification if
     *                          this flag is false. No event to Stub target will
     *                          be sent, until no service connected event is
     *                          received.
     * \param   proxy           The Service Interface Proxy object, which is
     *                          notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object,
     *          i.e. if passed Proxy address is equal to the Proxy object that client has.
     *          If Proxy objects are not equal, it should return false;
     **/
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy ) override;

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
// IEOptionEventConsumer interface overrides.
/************************************************************************/

    /**
     * \brief  Override operation. Implement this function to receive events and make processing.
     * \param  data    The data, which was passed as an event.
     **/
    virtual void processEvent(const OptionEventData& data) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    NEOptions::sOptionData  mOptions;
    Thread                  mConsoleThread;
    Mutex                   mLock;
    std::atomic_bool        mQuitThread;
//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline OptionsServiceClient & self( void );
};

inline OptionsServiceClient & OptionsServiceClient::self( void )
{
    return (*this);
}
