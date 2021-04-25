#ifndef AREG_COMPONENT_CERESPONSEEVENTS_HPP
#define AREG_COMPONENT_CERESPONSEEVENTS_HPP
/************************************************************************
 * \file        areg/src/component/CEResponseEvents.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Local and Remote Request 
 *              Event object for local and remote communication
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEServiceResponseEvent.hpp"

#include "areg/src/component/CEEventData.hpp"

/************************************************************************
 * List of declared classes:
 ************************************************************************/
// CEServiceRequestEvent;
    class CEResponseEvent;
        class CELocalResponseEvent;
        class CERemoteResponseEvent;

/************************************************************************
 * \brief   In this file following classes are declared:
 *              1. CEResponseEvent
 *              2.      CELocalResponseEvent
 *              3.      CERemoteResponseEvent
 *          These are base classes for communication, used to send
 *          Data in Response objects for local and remote communication.
 *          For more information, see description bellow.
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// CEResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * CEResponseEvent class, used to send responses
 ************************************************************************/
/**
 * \brief   Generic Response Event object used to trigger response or 
 *          attribute update events on Proxy side.
 *          Response Event is derived from Service Response event and
 *          should be base class for all Service Interface specific
 *          response and update calls. It is containing data object to 
 *          transfer message specific parameter information.
 **/
class AREG_API CEResponseEvent   : public CEServiceResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CEResponseEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Initializes event. Sets target address, result type and response ID, etc.
     * \param	proxyTarget	The address of target Proxy
     * \param	result	    The type of result to indicate whether it is response or update event, as well as to specify
     *                      message validation flag.
     * \param	respId	    The ID of response. Can also be update ID.
     * \param   eventType   The event type.
     * \param   seqNr       The call sequence number.
     **/
    CEResponseEvent( const CEProxyAddress & proxyTarget
                   , const NEService::eResultType result
                   , unsigned int respId
                   , CEEvent::eEventType eventType
                   , unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );

    /**
     * \brief	Initializes event. Sets buffer of serialized arguments, event target address, result type and response ID.
     * \param	args	    Shared Buffer object with information of serialized parameters.
     * \param	proxyTarget	The address of target Proxy
     * \param	result	    The type of result to indicate whether it is response or update event, as well as to specify
     *                      message validation flag.
     * \param	respId	    The ID of response. Can also be update ID.
     * \param   seqNr       The call sequence number.
     * \param	name	    Optional. Name for event data. Can be NULL.
     **/
    CEResponseEvent( const CEEventDataStream & args
                   , const CEProxyAddress & proxyTarget
                   , NEService::eResultType result
                   , unsigned int respId
                   , CEEvent::eEventType eventType
                   , unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY
                   , const char* name = NULL );

    /**
     * \brief	Clones existing information, except target Proxy address to send same message to different Proxies.
     * \param	proxyTarget	The address of target Proxy
     * \param	src	        The Event source to copy data.
     **/
    CEResponseEvent( const CEProxyAddress & proxyTarget, const CEResponseEvent & src );

    /**
     * \brief   Creates event from streaming object and initializes data
     * \param   stream  The streaming object to read data
     **/
    CEResponseEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEResponseEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns event data object.
     **/
    inline const CEEventData & GetData( void ) const;

    /**
     * \brief   Returns data type of request data
     **/
    inline NEService::eMessageDataType GetDataType( void ) const;

    /**
     * \brief   Returns reference of data input streaming object
     *          to de-serialize parameters.
     **/
    inline const IEInStream & GetReadStream( void ) const;

    /**
     * \brief   Returns reference of data output streaming object
     *          to serialize parameters.
     **/
    inline IEOutStream & GetWriteStream( void );

protected:
    /**
     * \brief   Returns data object valid for modification.
     **/
    inline CEEventData & GetData( void );

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
private:
    /**
     * \brief   Event data object.
     **/
    CEEventData     mData;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEResponseEvent( void );
    CEResponseEvent( const CEResponseEvent & /*src*/ );
    const CEResponseEvent & operator = ( const CEResponseEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CELocalResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * CELocalResponseEvent class, used to send responses
 ************************************************************************/
/**
 * \brief   Generic Local Response Event object used to trigger response or 
 *          attribute update events on Proxy side. It is not used
 *          for remote service interface communication
 **/
class AREG_API CELocalResponseEvent : public    CEResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CELocalResponseEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Initializes local event. Sets target address, result type and response ID, etc.
     * \param	proxyTarget	The address of target Proxy
     * \param	result	    The type of result to indicate whether it is response or update event, as well as to specify
     *                      message validation flag.
     * \param	respId	    The ID of response. Can also be update ID.
     * \param   seqNr       The call sequence number.
     **/
    CELocalResponseEvent( const CEProxyAddress & proxyTarget
                        , NEService::eResultType result
                        , unsigned int respId
                        , unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY);

    /**
     * \brief	Initializes local event. Sets buffer of serialized arguments, event target address, result type and response ID.
     * \param	args	    Shared Buffer object with information of serialized parameters.
     * \param	proxyTarget	The address of target Proxy
     * \param	result	    The type of result to indicate whether it is response or update event, as well as to specify
     *                      message validation flag.
     * \param	respId	    The ID of response. Can also be update ID.
     * \param   seqNr       The call sequence number.
     * \param	name	    Optional. Name for event data. Can be NULL.
     **/
    CELocalResponseEvent( const CEEventDataStream & args
                        , const CEProxyAddress & proxyTarget
                        , NEService::eResultType result
                        , unsigned int respId
                        , unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY
                        , const char* name = NULL);

    /**
     * \brief	Clones existing information, except target Proxy address to send same message to different Proxies.
     * \param	proxyTarget	The address of target Proxy
     * \param	src	        The Event source to copy data.
     **/
    CELocalResponseEvent(const CEProxyAddress & proxyTarget, const CELocalResponseEvent & src);

    /**
     * \brief   Creates event from streaming object and initializes data
     * \param   stream  The streaming object to read data
     **/
    CELocalResponseEvent(const IEInStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~CELocalResponseEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CELocalResponseEvent( void );
    CELocalResponseEvent( const CELocalResponseEvent & /*src*/ );
    const CELocalResponseEvent & operator = ( const CELocalResponseEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CERemoteResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * CERemoteResponseEvent class, used to send responses
 ************************************************************************/
/**
 * \brief   Generic Remote Response Event object used to trigger response or 
 *          attribute update events on Proxy side. It is not used
 *          for remote service interface communication
 **/
class AREG_API CERemoteResponseEvent: public    CEResponseEvent
{
    friend class CERemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CERemoteResponseEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Initializes remote event. Sets target address, result type and response ID, etc.
     * \param	proxyTarget	The address of target Proxy
     * \param	result	    The type of result to indicate whether it is response or update event, as well as to specify
     *                      message validation flag.
     * \param	respId	    The ID of response. Can also be update ID.
     * \param   seqNr       The call sequence number.
     **/
    CERemoteResponseEvent( const CEProxyAddress & proxyTarget
                         , NEService::eResultType result
                         , unsigned int respId
                         , unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );

    /**
     * \brief	Initializes remote event. Sets buffer of serialized arguments, event target address, result type and response ID.
     * \param	args	    Shared Buffer object with information of serialized parameters.
     * \param	proxyTarget	The address of target Proxy
     * \param	result	    The type of result to indicate whether it is response or update event, as well as to specify
     *                      message validation flag.
     * \param	respId	    The ID of response. Can also be update ID.
     * \param   seqNr       The call sequence number.
     * \param	name	    Optional. Name for event data. Can be NULL.
     **/
    CERemoteResponseEvent( const CEEventDataStream & args
                         , const CEProxyAddress & proxyTarget
                         , NEService::eResultType result
                         , unsigned int respId
                         , unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY
                         , const char* name = NULL);

    /**
     * \brief	Clones existing information, except target Proxy address to send same message to different Proxies.
     * \param	proxyTarget	The address of target Proxy
     * \param	src	        The Event source to copy data.
     **/
    CERemoteResponseEvent(const CEProxyAddress & proxyTarget, const CERemoteResponseEvent & src);

    /**
     * \brief   Creates event from streaming object and initializes data
     * \param   stream  The streaming object to read data
     **/
    CERemoteResponseEvent(const IEInStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~CERemoteResponseEvent( void );

//////////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Sets the target communication channel object.
     * \param   channel     The communication channel to set in remote event.
     **/
    inline void SetTargetChannel( const CEChannel & channel );

    /**
     * \brief   Returns the event communication channel object.
     **/
    inline const CEChannel & GetTargetChannel( void ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CERemoteResponseEvent( void );
    CERemoteResponseEvent( const CERemoteResponseEvent & /*src*/ );
    const CERemoteResponseEvent & operator = ( const CERemoteResponseEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEResponseEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const CEEventData & CEResponseEvent::GetData( void ) const
{   return mData;                       }

inline NEService::eMessageDataType CEResponseEvent::GetDataType( void ) const
{   return mData.GetMessageDataType();         }

inline CEEventData & CEResponseEvent::GetData( void )
{   return mData;                       }

inline const IEInStream & CEResponseEvent::GetReadStream( void ) const
{   return mData.GetReadStream();       }

inline IEOutStream & CEResponseEvent::GetWriteStream( void )
{   return mData.GetWriteStream();      }

//////////////////////////////////////////////////////////////////////////
// CERemoteResponseEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void CERemoteResponseEvent::SetTargetChannel(const CEChannel & channel)
{   mTargetProxyAddress.SetChannel(channel);    }

inline const CEChannel & CERemoteResponseEvent::GetTargetChannel( void ) const
{   return mTargetProxyAddress.GetChannel();    }

#endif  // AREG_COMPONENT_CERESPONSEEVENTS_HPP
