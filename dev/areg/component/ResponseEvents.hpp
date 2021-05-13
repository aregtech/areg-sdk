#ifndef AREG_COMPONENT_RESPONSEEVENTS_HPP
#define AREG_COMPONENT_RESPONSEEVENTS_HPP
/************************************************************************
 * \file        areg/component/ResponseEvents.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Local and Remote Request 
 *              Event object for local and remote communication
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/ServiceResponseEvent.hpp"

#include "areg/component/EventData.hpp"

/************************************************************************
 * List of declared classes:
 ************************************************************************/
// ServiceRequestEvent;
    class ResponseEvent;
        class LocalResponseEvent;
        class RemoteResponseEvent;

/************************************************************************
 * \brief   In this file following classes are declared:
 *              1. ResponseEvent
 *              2.      LocalResponseEvent
 *              3.      RemoteResponseEvent
 *          These are base classes for communication, used to send
 *          Data in Response objects for local and remote communication.
 *          For more information, see description bellow.
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// ResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * ResponseEvent class, used to send responses
 ************************************************************************/
/**
 * \brief   Generic Response Event object used to trigger response or 
 *          attribute update events on Proxy side.
 *          Response Event is derived from Service Response event and
 *          should be base class for all Service Interface specific
 *          response and update calls. It is containing data object to 
 *          transfer message specific parameter information.
 **/
class AREG_API ResponseEvent   : public ServiceResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(ResponseEvent)

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
    ResponseEvent( const ProxyAddress & proxyTarget
                 , const NEService::eResultType result
                 , unsigned int respId
                 , Event::eEventType eventType
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
    ResponseEvent( const EventDataStream & args
                 , const ProxyAddress & proxyTarget
                 , NEService::eResultType result
                 , unsigned int respId
                 , Event::eEventType eventType
                 , unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY
                 , const char* name = NULL );

    /**
     * \brief	Clones existing information, except target Proxy address to send same message to different Proxies.
     * \param	proxyTarget	The address of target Proxy
     * \param	src	        The Event source to copy data.
     **/
    ResponseEvent( const ProxyAddress & proxyTarget, const ResponseEvent & src );

    /**
     * \brief   Creates event from streaming object and initializes data
     * \param   stream  The streaming object to read data
     **/
    ResponseEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~ResponseEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns event data object.
     **/
    inline const EventData & getData( void ) const;

    /**
     * \brief   Returns data type of request data
     **/
    inline NEService::eMessageDataType getDataType( void ) const;

    /**
     * \brief   Returns reference of data input streaming object
     *          to de-serialize parameters.
     **/
    inline const IEInStream & getReadStream( void ) const;

    /**
     * \brief   Returns reference of data output streaming object
     *          to serialize parameters.
     **/
    inline IEOutStream & getWriteStream( void );

protected:
    /**
     * \brief   Returns data object valid for modification.
     **/
    inline EventData & getData( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// StreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Reads and initialize event data from streaming object.
     * \param   stream  The streaming object to read out event data
     * \return  Returns streaming object to read out data.
     **/
    virtual const IEInStream & readStream( const IEInStream & stream );

    /**
     * \brief   Writes event data to streaming object
     * \param   stream  The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    virtual IEOutStream & writeStream( IEOutStream & stream ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Event data object.
     **/
    EventData     mData;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ResponseEvent( void );
    ResponseEvent( const ResponseEvent & /*src*/ );
    const ResponseEvent & operator = ( const ResponseEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// LocalResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * LocalResponseEvent class, used to send responses
 ************************************************************************/
/**
 * \brief   Generic Local Response Event object used to trigger response or 
 *          attribute update events on Proxy side. It is not used
 *          for remote service interface communication
 **/
class AREG_API LocalResponseEvent : public    ResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(LocalResponseEvent)

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
    LocalResponseEvent( const ProxyAddress & proxyTarget
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
    LocalResponseEvent( const EventDataStream & args
                      , const ProxyAddress & proxyTarget
                      , NEService::eResultType result
                      , unsigned int respId
                      , unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY
                      , const char* name = NULL);

    /**
     * \brief	Clones existing information, except target Proxy address to send same message to different Proxies.
     * \param	proxyTarget	The address of target Proxy
     * \param	src	        The Event source to copy data.
     **/
    LocalResponseEvent(const ProxyAddress & proxyTarget, const LocalResponseEvent & src);

    /**
     * \brief   Creates event from streaming object and initializes data
     * \param   stream  The streaming object to read data
     **/
    LocalResponseEvent(const IEInStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~LocalResponseEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalResponseEvent( void );
    LocalResponseEvent( const LocalResponseEvent & /*src*/ );
    const LocalResponseEvent & operator = ( const LocalResponseEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * RemoteResponseEvent class, used to send responses
 ************************************************************************/
/**
 * \brief   Generic Remote Response Event object used to trigger response or 
 *          attribute update events on Proxy side. It is not used
 *          for remote service interface communication
 **/
class AREG_API RemoteResponseEvent: public    ResponseEvent
{
    friend class RemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(RemoteResponseEvent)

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
    RemoteResponseEvent( const ProxyAddress & proxyTarget
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
    RemoteResponseEvent( const EventDataStream & args
                       , const ProxyAddress & proxyTarget
                       , NEService::eResultType result
                       , unsigned int respId
                       , unsigned int seqNr = NEService::SEQUENCE_NUMBER_NOTIFY
                       , const char* name = NULL);

    /**
     * \brief	Clones existing information, except target Proxy address to send same message to different Proxies.
     * \param	proxyTarget	The address of target Proxy
     * \param	src	        The Event source to copy data.
     **/
    RemoteResponseEvent(const ProxyAddress & proxyTarget, const RemoteResponseEvent & src);

    /**
     * \brief   Creates event from streaming object and initializes data
     * \param   stream  The streaming object to read data
     **/
    RemoteResponseEvent(const IEInStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteResponseEvent( void );

//////////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Sets the target communication channel object.
     * \param   channel     The communication channel to set in remote event.
     **/
    inline void setTargetChannel( const Channel & channel );

    /**
     * \brief   Returns the event communication channel object.
     **/
    inline const Channel & getTargetChannel( void ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RemoteResponseEvent( void );
    RemoteResponseEvent( const RemoteResponseEvent & /*src*/ );
    const RemoteResponseEvent & operator = ( const RemoteResponseEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ResponseEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const EventData & ResponseEvent::getData( void ) const
{
    return mData;
}

inline NEService::eMessageDataType ResponseEvent::getDataType( void ) const
{
    return mData.getDataType();
}

inline EventData & ResponseEvent::getData( void )
{
    return mData;
}

inline const IEInStream & ResponseEvent::getReadStream( void ) const
{
    return mData.getReadStream();
}

inline IEOutStream & ResponseEvent::getWriteStream( void )
{
    return mData.getWriteStream();
}

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void RemoteResponseEvent::setTargetChannel(const Channel & channel)
{
    mTargetProxyAddress.setChannel(channel);
}

inline const Channel & RemoteResponseEvent::getTargetChannel( void ) const
{
    return mTargetProxyAddress.getChannel();
}

#endif  // AREG_COMPONENT_RESPONSEEVENTS_HPP
