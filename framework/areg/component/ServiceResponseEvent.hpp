#ifndef AREG_COMPONENT_SERVICERESPONSEEVENT_HPP
#define AREG_COMPONENT_SERVICERESPONSEEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ServiceResponseEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Response Event.
 *              Base Service Response Event class to send events to
 *              Proxy, trigger updates and notification calls.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/ProxyEvent.hpp"
#include "areg/component/ServiceDefs.hpp"
namespace areg {

/************************************************************************
 * Dependencies.
 ************************************************************************/
class ProxyAddress;

/**
 * \brief       The Service Response class is a base class for all kind
 *              response events sent from Stub to Proxy after processing
 *              request calls. Normally, this is either request call 
 *              response or Attribute update notification. It is derived
 *              from ProxyEvent.
 * 
 * \details     This class contains response ID, result, service interface
 *              version and executed request sequence number. As well as
 *              this should be response call to notify Proxy about server
 *              connect and disconnect status update.
 *
 **/
//////////////////////////////////////////////////////////////////////////
// ServiceResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all response events sent from Stub to Proxy after processing request
 *          calls. Handles response delivery and attribute update notifications.
 **/
class AREG_API ServiceResponseEvent    : public ProxyEvent
{
//////////////////////////////////////////////////////////////////////////
// Internal defines
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \todo        <b>TODO :</b> remove version object from here and place
     *              in data streaming object, because this parameter is 
     *              needed only on server connect and optional on server
     *              disconnect case. In all other cases version object
     *              might be ignored.
     **/
//////////////////////////////////////////////////////////////////////////
// Declare as runtime event.
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(ServiceResponseEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Creates service response event and sets parameters.
     *
     * \param   target          The event target proxy address
     * \param   result          The response result
     * \param   responseId      The response message ID
     * \param   eventType       The type of event.
     * \param   seqNr           The sequence number of call.
     **/
    ServiceResponseEvent( const ProxyAddress & target
                        , areg::ResultType result
                        , uint32_t responseId
                        , areg::EventType eventType
                        , const SequenceNumber & seqNr = areg::SEQUENCE_NUMBER_NOTIFY );

    /**
     * \brief   Copies all data from given source, except the target proxy address. This is used if
     *          proxy needs to clone event data to send to different target proxy objects.
     *
     * \param   target      The target proxy address
     * \param   src         The service response source to copy data.
     **/
    ServiceResponseEvent(const ProxyAddress & target, const ServiceResponseEvent & src );

    /**
     * \brief   Creates event from streaming object and initializes data.
     *
     * \param   stream      The streaming object to read data
     **/
    ServiceResponseEvent(const InStream & stream);

    virtual ~ServiceResponseEvent() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns response message ID.
     **/
    inline uint32_t response_id() const noexcept;

    /**
     * \brief   Returns response call result.
     **/
    [[nodiscard]]
    inline areg::ResultType result() const noexcept;

    /**
     * \brief   Returns sequence number of call.
     **/
    inline const SequenceNumber & sequence_number() const noexcept;

    /**
     * \brief   Sets sequence number of call.
     *
     * \param   newSeqNr    The new sequence number to set.
     **/
    inline void set_sequence_number( const SequenceNumber & newSeqNr ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Clones existing service event object with specified target proxy address. Override
     *          this method in each service response specific class.
     *
     * \param   target      The target proxy address.
     * \return  Cloned service response event object, which contains specified target proxy address.
     **/
    virtual ServiceResponseEvent * clone_for_target(const ProxyAddress & target) const;

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
     * \brief   The response message ID
     **/
    uint32_t            mResponseId;

    /**
     * \brief   The response result
     **/
    areg::ResultType mResult;

    /**
     * \brief   The sequence number.
     **/
    SequenceNumber          mSequenceNr;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceResponseEvent() = delete;
    AREG_NOCOPY_NOMOVE( ServiceResponseEvent );
};

//////////////////////////////////////////////////////////////////////////
// ServiceResponseEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline uint32_t ServiceResponseEvent::response_id() const noexcept
{
    return mResponseId;
}

inline areg::ResultType ServiceResponseEvent::result() const noexcept
{
    return mResult;
}

inline const SequenceNumber & ServiceResponseEvent::sequence_number() const noexcept
{
    return mSequenceNr;
}

inline void ServiceResponseEvent::set_sequence_number( const SequenceNumber & newSeqNr ) noexcept
{
    mSequenceNr = newSeqNr;
}

} // namespace areg
#endif  // AREG_COMPONENT_SERVICERESPONSEEVENT_HPP
