#ifndef AREG_COMPONENT_REMOTEEVENTFACTORY_HPP
#define AREG_COMPONENT_REMOTEEVENTFACTORY_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/RemoteEventFactory.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Remote Event Factory.
 *              Single chokepoint for remote (IPC) event serialization and routing.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class Channel;
class Event;
class MessageEnvelope;
class ServiceResponseEvent;

//////////////////////////////////////////////////////////////////////////
// RemoteEventFactory class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Single chokepoint for remote (IPC) event serialization and delivery.
 *
 *          Three directional methods cover the entire IPC data plane:
 *            - route_outgoing_message      : event -> wire  (serialize for transmission)
 *            - route_incoming_message      : wire  -> local (translate and deliver to stub/proxy)
 *            - create_request_failed_event : wire  -> failure response (undeliverable request)
 *
 * \note    ThreadSafe: all methods are stateless static functions.
 **/
class AREG_API RemoteEventFactory
{
//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Stamps the wire routing fields (source/target) on a local remote event in place,
     *          making it ready for transmission. The event payload already carries the serialized
     *          data and the message/sequence fields. Rewrites the process-routing cookies.
     *
     * \param[in,ou]    srcWire     In-out remote event; its header is rewritten to wire form.
     * \param           comChannel  Communication channel providing the sender's process cookie.
     * \return  true if stamped; false for unrecognised event type or empty buffer.
     **/
    static bool route_outgoing_message( Event & srcWire, const Channel & comChannel );

    /**
     * \brief   Translates an inbound wire envelope and delivers it to the target stub or proxy thread.
     *          Must be called directly on the receive thread.
     *
     * \param   wire        Received wire envelope; consumed (moved-from) only when this returns true.
     * \param   comChannel  Communication channel of the receiving Router Client thread.
     * \return  true if delivered;
     *          false when the target stub/proxy is not found (wire left intact).
     **/
    static bool route_incoming_message( MessageEnvelope & wire, const Channel & comChannel );

    /**
     * \brief   Creates a request-failure response Event for an undeliverable request.
     *          Returns an invalid Event when the source proxy cannot be located.
     *
     * \param   wire        The original wire envelope that could not be processed.
     * \param   comChannel  The communication channel for routing correction.
     * \return  An Event value; check is_valid() before delivering.
     **/
    [[nodiscard]]
    static ServiceResponseEvent create_request_failed_event( const MessageEnvelope & wire, const Channel & comChannel );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Hidden
//////////////////////////////////////////////////////////////////////////
private:
    RemoteEventFactory() = default;

    ~RemoteEventFactory() = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( RemoteEventFactory );
};

} // namespace areg
#endif  // AREG_COMPONENT_REMOTEEVENTFACTORY_HPP
