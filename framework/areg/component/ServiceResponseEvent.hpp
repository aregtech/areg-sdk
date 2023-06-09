#ifndef AREG_COMPONENT_SERVICERESPONSEEVENT_HPP
#define AREG_COMPONENT_SERVICERESPONSEEVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/ServiceResponseEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Response Event.
 *              Base Service Response Event class to send events to
 *              Proxy, trigger updates and notification calls.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/ProxyEvent.hpp"
#include "areg/component/NEService.hpp"

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
    DECLARE_RUNTIME_EVENT(ServiceResponseEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Creates service response event. Sets given parameters
     * \param   target      The event target proxy address
     * \param   result      The response result
     * \param   responseId  The response message ID
     * \param   eventType   The type of event.
     * \param   seqNr       The sequence number of call.
     **/
    ServiceResponseEvent( const ProxyAddress & target
                        , NEService::eResultType result
                        , unsigned int responseId
                        , Event::eEventType eventType
                        , unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );

    /**
     * \brief   Copies all data from given source, except the target proxy address. This is used if proxy needs to clone
     *          event data to send to different target proxy objects.
     * \param   target  The target proxy address
     * \param   src     The service response source to copy data.
     **/
    ServiceResponseEvent(const ProxyAddress & target, const ServiceResponseEvent & src );

    /**
     * \brief   Creates event from streaming object and initializes data
     * \param   stream  The streaming object to read data
     **/
    ServiceResponseEvent(const IEInStream & stream);

    /**
     * \brief   Destructor. Protected.
     * \remark  Do not call directly, use Destroy() method to clean properly.
     **/
    virtual ~ServiceResponseEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Get and set response message ID.
     **/
    inline unsigned int getResponseId( void ) const;

    /**
     * \brief   Returns response call result
     **/
    inline NEService::eResultType getResult( void ) const;

    /**
     * \brief   Returns sequence number of call.
     **/
    inline unsigned int getSequenceNumber( void ) const;

    /**
     * \brief   Sets sequence number of call.
     **/
    inline void setSequenceNumber( unsigned int newSeqNr );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Clones existing service event object.
     *          Copies all data and sets specified target proxy address.
     *          Overwrite this method in every service response specific
     *          class to be able to clone events.
     * \param   target  The target proxy address.
     * \return  Cloned service response event object, which contains specified
     *          target proxy address.
     **/
    virtual ServiceResponseEvent * cloneForTarget(const ProxyAddress & target) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Reads and initialize event data from streaming object.
     * \param   stream  The streaming object to read out event data
     * \return  Returns streaming object to read out data.
     **/
    virtual const IEInStream & readStream( const IEInStream & stream ) override;

    /**
     * \brief   Writes event data to streaming object
     * \param   stream  The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    virtual IEOutStream & writeStream( IEOutStream & stream ) const override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The response message ID
     **/
    unsigned int            mResponseId;

    /**
     * \brief   The response result
     **/
    NEService::eResultType mResult;

    /**
     * \brief   The sequence number.
     **/
    unsigned int            mSequenceNr;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceResponseEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceResponseEvent );
};

//////////////////////////////////////////////////////////////////////////
// ServiceResponseEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline unsigned int ServiceResponseEvent::getResponseId( void ) const
{
    return mResponseId;
}

inline NEService::eResultType ServiceResponseEvent::getResult( void ) const
{
    return mResult;
}

inline unsigned int ServiceResponseEvent::getSequenceNumber( void ) const
{
    return mSequenceNr;
}

inline void ServiceResponseEvent::setSequenceNumber( unsigned int newSeqNr )
{
    mSequenceNr = newSeqNr;
}

#endif  // AREG_COMPONENT_SERVICERESPONSEEVENT_HPP
