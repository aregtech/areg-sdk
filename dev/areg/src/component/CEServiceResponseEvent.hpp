#ifndef AREG_COMPONENT_CESERVICERESPONSEEVENT_HPP
#define AREG_COMPONENT_CESERVICERESPONSEEVENT_HPP
/************************************************************************
 * \file        areg/src/component/CEServiceResponseEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Response Event.
 *              Base Service Response Event class to send events to
 *              Proxy, trigger updates and notification calls.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEProxyEvent.hpp"
#include "areg/src/component/NEService.hpp"

/************************************************************************
 * Dependencies.
 ************************************************************************/
class CEProxyAddress;

/**
 * \brief       The Service Response class is a base class for all kind
 *              response events sent from Stub to Proxy after processing
 *              request calls. Normally, this is either request call 
 *              response or Attribute update notification. It is derived
 *              from CEProxyEvent.
 * 
 * \details     This class contains response ID, result, service interface
 *              version and executed request sequence number. As well as
 *              this should be response call to notify Proxy about server
 *              connect and disconnect status update.
 *
 **/
//////////////////////////////////////////////////////////////////////////
// CEServiceResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CEServiceResponseEvent    : public CEProxyEvent
{
//////////////////////////////////////////////////////////////////////////
// Internal defines
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \todo        <b>TODO >>></b> remove version object from here and place
     *              in data streaming object, because this parameter is 
     *              needed only on server connect and optional on server
     *              disconnect case. In all other cases version object
     *              might be ignored.
     **/
//////////////////////////////////////////////////////////////////////////
// Declare as runtime event.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CEServiceResponseEvent)

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
    CEServiceResponseEvent( const CEProxyAddress & target
                          , NEService::eResultType result
                          , unsigned int responseId
                          , CEEvent::eEventType eventType
                          , unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );

    /**
     * \brief   Copies all data from given source, except the target proxy address. This is used if proxy needs to clone
     *          event data to send to different target proxy objects.
     * \param   target  The target proxy address
     * \param   src     The service response source to copy data.
     **/
    CEServiceResponseEvent(const CEProxyAddress & target, const CEServiceResponseEvent & src );

    /**
     * \brief   Creates event from streaming object and initializes data
     * \param   stream  The streaming object to read data
     **/
    CEServiceResponseEvent(const IEInStream & stream);

    /**
     * \brief   Destructor. Protected.
     * \remark  Do not call directly, use Destroy() method to clean properly.
     **/
    virtual ~CEServiceResponseEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Get and set response message ID.
     **/
    inline unsigned int GetResponseId( void ) const;

    /**
     * \brief   Returns response call result
     **/
    inline NEService::eResultType GetResult( void ) const;

    /**
     * \brief   Returns sequence number of call.
     **/
    inline unsigned int GetSequenceNumber( void ) const;

    /**
     * \brief   Sets sequence number of call.
     **/
    inline void SetSequenceNumber( unsigned int newSeqNr );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Clones existing service event object.
     *          Copies all data and sets specified target proxy address.
     *          Overwrite this method in every service response specific
     *          class to be able to clone events.
     * \param   target  The target proxy address.
     * \return  Cloned service response event object, which contains specified
     *          target proxy address.
     **/
    virtual CEServiceResponseEvent * CloneEventForTarget(const CEProxyAddress & target) const;

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
    CEServiceResponseEvent( void );
    CEServiceResponseEvent( const CEServiceResponseEvent & /*src*/ );
    const CEServiceResponseEvent & operator = ( const CEServiceResponseEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEServiceResponseEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline unsigned int CEServiceResponseEvent::GetResponseId( void ) const
{   return mResponseId;     }

inline NEService::eResultType CEServiceResponseEvent::GetResult( void ) const
{   return mResult;         }

inline unsigned int CEServiceResponseEvent::GetSequenceNumber( void ) const
{   return mSequenceNr;     }

inline void CEServiceResponseEvent::SetSequenceNumber( unsigned int newSeqNr )
{   mSequenceNr = newSeqNr; }

#endif  // AREG_COMPONENT_CESERVICERESPONSEEVENT_HPP
