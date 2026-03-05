#ifndef AREG_COMPONENT_SERVICEREQUESTEVENT_HPP
#define AREG_COMPONENT_SERVICEREQUESTEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ServiceRequestEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Request Event.
 *              Base Service Request event class to send events to Stub
 *              and trigger function call.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/StubEvent.hpp"

#include "areg/component/ServiceDefs.hpp"
namespace areg {

/************************************************************************
 * Dependencies.
 ************************************************************************/
class StubAddress;
class ProxyAddress;
class ComponentAddress;

//////////////////////////////////////////////////////////////////////////
// ServiceRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all requests sent from Proxy to Stub. Instances of StubEvent.
 **/
class AREG_API ServiceRequestEvent : public StubEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare as runtime event class
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(ServiceRequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates Service Event object and sets source Proxy address, target Stub Address, and
     *          event info.
     *
     * \param   proxyAddress    The source Proxy Address, which sent event.
     * \param   target          The target Stub Address, which should process event
     * \param   reqId           The request message ID to process.
     * \param   reqType         The request type.
     * \param   eventType       The type of event.
     **/
    ServiceRequestEvent( const ProxyAddress & proxyAddress
                       , const StubAddress & target
                       , uint32_t reqId
                       , areg::RequestType reqType
                       , Event::EventType eventType );

    /**
     * \brief   Creates event from streaming object and initializes data.
     *
     * \param   stream      The streaming object to read data
     **/
    ServiceRequestEvent(const InStream & stream);

    virtual ~ServiceRequestEvent() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the address of Proxy event source.
     **/
    [[nodiscard]]
    inline const ProxyAddress & event_source() const noexcept;

    /**
     * \brief   Sets the address of Proxy event source.
     *
     * \param   addrProxySource     The address of Proxy of source
     **/
    inline void set_event_source( const ProxyAddress &  addrProxySource );

    /**
     * \brief   Returns request message ID.
     **/
    [[nodiscard]]
    inline uint32_t request_id() const noexcept;

    /**
     * \brief   Returns request type.
     **/
    [[nodiscard]]
    inline areg::RequestType request_type() const noexcept;

    /**
     * \brief   Returns sequence number set in info.
     **/
    [[nodiscard]]
    inline const SequenceNumber & sequence_number() const noexcept;

    /**
     * \brief   Sets new sequence number.
     *
     * \param   newSeqNr    The new sequence number to set.
     **/
    inline void set_sequence_number(const SequenceNumber & newSeqNr) noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Reads and initializes event data from streaming object.
     *
     * \param   stream      The streaming object to read out event data
     * \return  Returns streaming object to read out data.
     **/
    const InStream & read_stream( const InStream & stream ) override;

    /**
     * \brief   Writes event data to streaming object.
     *
     * \param   stream      The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    OutStream & write_stream( OutStream & stream ) const override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Event source Proxy address
     **/
    ProxyAddress              mProxySource;

    /**
     * \brief   Request message ID to trigger service call.
     **/
    uint32_t                mMessageId;

    /**
     * \brief   Request type. Normally, either notification or request call.
     **/
    areg::RequestType    mRequestType;

    /**
     * \brief   Sequence number.
     **/
    SequenceNumber              mSequenceNr;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceRequestEvent() = delete;
    AREG_NOCOPY_NOMOVE( ServiceRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// ServiceRequestEvent class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline const ProxyAddress & ServiceRequestEvent::event_source() const noexcept
{
    return mProxySource;
}

inline void ServiceRequestEvent::set_event_source(const ProxyAddress& addrProxySource)
{
    mProxySource = addrProxySource;
}

inline uint32_t ServiceRequestEvent::request_id() const noexcept
{
    return mMessageId;
}

inline areg::RequestType ServiceRequestEvent::request_type() const noexcept
{
    return mRequestType;
}

inline const SequenceNumber & ServiceRequestEvent::sequence_number() const noexcept
{
    return mSequenceNr;
}

inline void ServiceRequestEvent::set_sequence_number(const SequenceNumber & newSeqNr ) noexcept
{
    mSequenceNr = newSeqNr;
}

} // namespace areg
#endif  // AREG_COMPONENT_SERVICEREQUESTEVENT_HPP
