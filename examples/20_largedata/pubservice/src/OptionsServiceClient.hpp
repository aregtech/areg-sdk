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
#include "areg/component/Component.hpp"
#include "areg/component/IEEventConsumer.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "generated/src/OptionsClientBase.hpp"

#include "areg/base/Thread.hpp"
#include "areg/component/TEEvent.hpp"
#include "areg/component/Timer.hpp"

class OptionEventData : public NEOptions::sOptionData
{

};

DECLARE_EVENT( OptionEventData, OptionEvent, IEOptionEventConsumer );

class OptionsServiceClient  : public    Component
                            , protected IETimerConsumer
                            , protected IEOptionEventConsumer
                            , protected OptionsClientBase
{
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
     * \param   sentData    Computed rate of sent data per second.
     * \param   missedData  Computed rate of missed data. The data is missed if failed to copy within certain timeshlot.
     *          TODO: remove if useless.
     * \see     requestDataRate
     **/
    virtual void responseDataRate( const NEOptions::sDataRate & sentData, const NEOptions::sDataRate & missedData ) override;

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
     * \param   avSize      The average size of a packet.
     * \see     requestPacketRate
     **/
    virtual void responsePacketRate( unsigned int genPackets, const NEOptions::sDataRate & avSize ) override;

    /**
     * \brief   Overwrite to handle error of PacketRate request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestPacketRateFailed( NEService::eResultType FailureReason ) override;

protected:
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

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
protected:
    inline OptionsServiceClient & self( void );
};

inline OptionsServiceClient & OptionsServiceClient::self( void )
{
    return (*this);
}
