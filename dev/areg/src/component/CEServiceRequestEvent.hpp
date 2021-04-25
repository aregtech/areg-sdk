#ifndef AREG_COMPONENT_CESERVICEREQUESTEVENT_HPP
#define AREG_COMPONENT_CESERVICEREQUESTEVENT_HPP
/************************************************************************
 * \file        areg/src/component/CEServiceRequestEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Request Event.
 *              Base Service Request event class to send events to Stub
 *              and trigger function call.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEStubEvent.hpp"

#include "areg/src/component/NEService.hpp"

/************************************************************************
 * Dependencies.
 ************************************************************************/
class CEStubAddress;
class CEProxyAddress;
class CEComponentAddress;

//////////////////////////////////////////////////////////////////////////
// CEServiceRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The Service Request Event class is a base class for all
 *              requests to send from Proxy to Stub. This is runtime class
 *              and instance of CEStubEvent class.
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
class AREG_API CEServiceRequestEvent : public CEStubEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare as runtime event class
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CEServiceRequestEvent)

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
    CEServiceRequestEvent( const CEProxyAddress & proxyAddress
                         , const CEStubAddress & target
                         , unsigned int reqId
                         , NEService::eRequestType reqType
                         , CEEvent::eEventType eventType );

    /**
     * \brief   Creates event from streaming object and initializes data
     * \param   stream  The streaming object to read data
     **/
    CEServiceRequestEvent(const IEInStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~CEServiceRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Return the address of Proxy of event source
     **/
    inline const CEProxyAddress & GetEventSource( void ) const;

    /**
     * \brief   Sets the address of Proxy of event source
     * \param   addrProxySource The address of Proxy of source
     **/
    inline void SetEventSource( const CEProxyAddress &  addrProxySource );

    /**
     * \brief   Returns request message ID stored in service event
     **/
    inline unsigned int GetRequestId( void ) const;

    /**
     * \brief   Returns request type to process.
     **/
    inline NEService::eRequestType GetRequestType( void ) const;

    /**
     * \brief   Returns sequence number set in info.
     **/
    inline unsigned int GetSequenceNumber( void ) const;

    /**
     * \brief   Sets new sequence number.
     **/
    inline void SetSequenceNumber(unsigned int newSeqNr);

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// CEStreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Reads and initialize event data from streaming object.
     * \param   stream  The streaming object to read out event data
     * \return  Returns streaming object to read out data.
     **/
    virtual const IEInStream & ReadFromStream( const IEInStream & stream );

    /**
     * \brief   Writes event data to streaming object
     * \param   stream  The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    virtual IEOutStream & WriteToStream( IEOutStream & stream ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Event source Proxy address
     **/
    CEProxyAddress              mProxySource;

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
    CEServiceRequestEvent( void );
    CEServiceRequestEvent(const CEServiceRequestEvent & /*src*/ );
    const CEServiceRequestEvent& operator = (const CEServiceRequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEServiceRequestEvent class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline const CEProxyAddress & CEServiceRequestEvent::GetEventSource( void ) const
{   return mProxySource;            }

inline void CEServiceRequestEvent::SetEventSource(const CEProxyAddress& addrProxySource)
{   mProxySource = addrProxySource; }

inline unsigned int CEServiceRequestEvent::GetRequestId( void ) const
{   return mMessageId;              }

inline NEService::eRequestType CEServiceRequestEvent::GetRequestType( void ) const
{   return mRequestType;            }

inline unsigned int CEServiceRequestEvent::GetSequenceNumber( void ) const
{   return mSequenceNr;             }

inline void CEServiceRequestEvent::SetSequenceNumber( unsigned int newSeqNr )
{   mSequenceNr = newSeqNr;         }

#endif  // AREG_COMPONENT_CESERVICEREQUESTEVENT_HPP
