#ifndef AREG_COMPONENT_SERVICEREQUESTEVENT_HPP
#define AREG_COMPONENT_SERVICEREQUESTEVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/ServiceRequestEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Request Event.
 *              Base Service Request event class to send events to Stub
 *              and trigger function call.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/StubEvent.hpp"

#include "areg/component/NEService.hpp"

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
 * \brief       The Service Request Event class is a base class for all
 *              requests to send from Proxy to Stub. This is runtime class
 *              and instance of StubEvent class.
 * 
 * \details     To trigger a function call or starting update notification
 *              Stub object is receiving Service Request event, which is
 *              containing message ID (request or attribute ID), request
 *              type (notification or function call). As well as the source
 *              Proxy object and target Stub should be specified.
 *              Derive this class to define custom request objects and/or
 *              set additional data parameter.
 *
 **/
class AREG_API ServiceRequestEvent : public StubEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare as runtime event class
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(ServiceRequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates Service Event object, set source Proxy address, target Stub Address and event info.
     * \param   proxyAddress    The source Proxy Address, which sent event.
     * \param   target          The target Stub Address, which should process event
     * \param   reqId           The request message ID to process.
     * \param   reqType         The request type.
     * \param   eventType       The type of event.
     **/
    ServiceRequestEvent( const ProxyAddress & proxyAddress
                       , const StubAddress & target
                       , unsigned int reqId
                       , NEService::eRequestType reqType
                       , Event::eEventType eventType );

    /**
     * \brief   Creates event from streaming object and initializes data
     * \param   stream  The streaming object to read data
     **/
    ServiceRequestEvent(const IEInStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~ServiceRequestEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Return the address of Proxy of event source
     **/
    inline const ProxyAddress & getEventSource( void ) const;

    /**
     * \brief   Sets the address of Proxy of event source
     * \param   addrProxySource The address of Proxy of source
     **/
    inline void setEventSource( const ProxyAddress &  addrProxySource );

    /**
     * \brief   Returns request message ID stored in service event
     **/
    inline unsigned int getRequestId( void ) const;

    /**
     * \brief   Returns request type to process.
     **/
    inline NEService::eRequestType getRequestType( void ) const;

    /**
     * \brief   Returns sequence number set in info.
     **/
    inline unsigned int getSequenceNumber( void ) const;

    /**
     * \brief   Sets new sequence number.
     **/
    inline void setSequenceNumber(unsigned int newSeqNr);

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
     * \brief   Event source Proxy address
     **/
    ProxyAddress              mProxySource;

    /**
     * \brief   Request message ID to trigger service call.
     **/
    unsigned int                mMessageId;

    /**
     * \brief   Request type. Normally, either notification or request call.
     **/
    NEService::eRequestType    mRequestType;

    /**
     * \brief   Sequence number.
     **/
    unsigned int                mSequenceNr;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceRequestEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// ServiceRequestEvent class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline const ProxyAddress & ServiceRequestEvent::getEventSource( void ) const
{
    return mProxySource;
}

inline void ServiceRequestEvent::setEventSource(const ProxyAddress& addrProxySource)
{
    mProxySource = addrProxySource;
}

inline unsigned int ServiceRequestEvent::getRequestId( void ) const
{
    return mMessageId;
}

inline NEService::eRequestType ServiceRequestEvent::getRequestType( void ) const
{
    return mRequestType;
}

inline unsigned int ServiceRequestEvent::getSequenceNumber( void ) const
{
    return mSequenceNr;
}

inline void ServiceRequestEvent::setSequenceNumber( unsigned int newSeqNr )
{
    mSequenceNr = newSeqNr;
}

#endif  // AREG_COMPONENT_SERVICEREQUESTEVENT_HPP
