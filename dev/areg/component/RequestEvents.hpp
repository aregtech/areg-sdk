#ifndef AREG_COMPONENT_REQUESTEVENTS_HPP
#define AREG_COMPONENT_REQUESTEVENTS_HPP
/************************************************************************
 * \file        areg/component/RequestEvents.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Local and Remote Request 
 *              Event object for local and remote communication
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/ServiceRequestEvent.hpp"

#include "areg/component/EventData.hpp"

/************************************************************************
 * List of declared classes:
 ************************************************************************/
// ServiceRequestEvent;
    class RequestEvent;
        class LocalRequestEvent;
        class RemoteRequestEvent;

    class NotifyRequestEvent;
        class LocalNotifyRequestEvent;
        class RemoteNotifyRequestEvent;
/************************************************************************
 * \brief   In this file following request event classes are declared:
 *              1. Request function call events:
 *                  a. RequestEvent
 *                  b.      LocalRequestEvent
 *                  c.      RemoteRequestEvent
 *              2. Notification request events:
 *                  a.  NotifyRequestEvent
 *                  b.      LocalNotifyRequestEvent
 *                  c.      RemoteNotifyRequestEvent
 *          These are base classes for communication, used to send
 *          Data in Request objects for local and remote communication.
 *          For more information, see description bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// RequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * RequestEvent class, sends request event
 ************************************************************************/
/**
 * \brief   Generic Request Event object used to trigger request on Stub side.
 *          Request Event is derived from Service Request event and
 *          should be base class for all Service Interface specific
 *          request calls. It is containing data object to transfer
 *          message specific parameter information.
 *
 **/
class AREG_API RequestEvent    : public ServiceRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(RequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Creates event with no data. Initializes event source and target address and message ID.
     *          Use if request has no parameter.
     * \param	fromSource	The address of source Proxy.
     * \param	toTarget	The address of Stub target
     * \param	reqId	    The ID of request.
     * \param   eventType   The type of event. It should be either 
     *                      local request or remote request type.
     **/
    RequestEvent( const ProxyAddress & fromSource
                , const StubAddress & toTarget
                , unsigned int reqId
                , Event::eEventType eventType);

    /**
     * \brief	Creates event with data. Initializes event source, target information,  and message ID.
     *          Use if request with parameter should be sent.
     * \param	args	    The reference to Shared Buffer object to transfer data.
     * \param	fromSource	The address of Proxy source.
     * \param	toTarget	The address of Stub target.
     * \param	reqId	    The ID of request.
     * \param   eventType   The type of event. It should be either 
     *                      local request or remote request type.
     * \param	name	    Optional. Name for event data. Can be NULL.
     **/
    RequestEvent( const EventDataStream & args
                , const ProxyAddress & fromSource
                , const StubAddress & toTarget
                , unsigned int reqId
                , Event::eEventType eventType
                , const char* name = NULL);

    /**
     * \brief   Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    RequestEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~RequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
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
    RequestEvent( void );
    RequestEvent( const RequestEvent & /*src*/ );
    const RequestEvent & operator = ( const RequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// LocalRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * LocalRequestEvent class, sends request event
 ************************************************************************/
/**
 * \brief   Generic Local Request Event object used to trigger request on Stub
 *          side within same process. It is not used for remote service interface
 *          communication.
 **/
class AREG_API LocalRequestEvent  : public    RequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(LocalRequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Creates event with no data. Initializes event source and target address and message ID.
     *          Use if request has no parameter.
     * \param	fromSource	The address of source Proxy.
     * \param	toTarget	The address of Stub target
     * \param	reqId	    The ID of request.
     **/
    LocalRequestEvent(const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId);

    /**
     * \brief	Creates event with data. Initializes event source, target information and message ID.
     *          Use if request with parameter should be sent.
     * \param	args	    The reference to Shared Buffer object to transfer data.
     * \param	fromSource	The address of Proxy source.
     * \param	toTarget	The address of Stub target.
     * \param	reqId	    The ID of request.
     * \param	name	    Optional. Name for event data. Can be NULL.
     **/
    LocalRequestEvent( const EventDataStream & args
                     , const ProxyAddress & fromSource
                     , const StubAddress & toTarget
                     , unsigned int reqId
                     , const char* name = NULL);

    /**
     * \brief   Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    LocalRequestEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~LocalRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalRequestEvent( void );
    LocalRequestEvent( const LocalRequestEvent & /*src*/ );
    const LocalRequestEvent & operator = ( const LocalRequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// RemoteRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * RemoteRequestEvent class, sends request event
 ************************************************************************/
/**
 * \brief   Generic Remote Request Event object used to trigger request on Stub
 *          side within different processes via service routing module. 
 *          It is used for remote service interface communication.
 **/
class AREG_API RemoteRequestEvent : public    RequestEvent
{
    friend class RemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(RemoteRequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Creates event with no data. Initializes event source and target address and message ID.
     *          Use if request has no parameter.
     * \param	fromSource	The address of source Proxy.
     * \param	toTarget	The address of Stub target
     * \param	reqId	    The ID of request.
     **/
    RemoteRequestEvent(const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId);

    /**
     * \brief	Creates event with data. Initializes event source, target information and message ID.
     *          Use if request with parameter should be sent.
     * \param	args	    The reference to Shared Buffer object to transfer data.
     * \param	fromSource	The address of Proxy source.
     * \param	toTarget	The address of Stub target.
     * \param	reqId	    The ID of request.
     * \param	name	    Optional. Name for event data. Can be NULL.
     **/
    RemoteRequestEvent( const EventDataStream & args
                      , const ProxyAddress & fromSource
                      , const StubAddress & toTarget
                      , unsigned int reqId
                      , const char* name = NULL);


    /**
     * \brief   Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    RemoteRequestEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteRequestEvent( void );

//////////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Sets the target communication Channel object
     * \param   channel     The communication channel object to set.
     **/
    inline void setTargetChannel( const Channel & channel );

    /**
     * \brief   Returns target communication channel object
     **/
    inline const Channel & getTargetChannel( void ) const;

    /**
     * \brief   Sets the source communication Channel object.
     * \param   channel     The communication channel object to set.
     **/
    inline void setSourceChannel( const Channel & channel );

    /**
     * \brief   Returns source communication channel object
     **/
    inline const Channel & getSourceChannel( void ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RemoteRequestEvent( void );
    RemoteRequestEvent( const RemoteRequestEvent & /*src*/ );
    const RemoteRequestEvent & operator = ( const RemoteRequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The notify request event is sent to stub object, when requesting
 *          to send notification messages like attribute change
 *          or response notification
 **/
class AREG_API NotifyRequestEvent : public ServiceRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(NotifyRequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates notification event used to send start / stop notification request from Proxy to Stub.
     * \param   fromProxy   The source Proxy address, requesting start / stop notifications.
     * \param   toStub      The target Stub address to start / stop sending notifications.
     * \param   msgId       The ID of message. Normally either Attribute ID, Broadcast ID or Response ID. The Request IDs cannot be notified.
     * \param   reqType     The type of request.
     * \param   eventType   The type of event. It should be either local request or remote request type.
     **/
    NotifyRequestEvent( const ProxyAddress & fromProxy
                      , const StubAddress & toStub
                      , unsigned int msgId
                      , NEService::eRequestType reqType
                      , Event::eEventType eventType );

    /**
     * \brief   Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    NotifyRequestEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~NotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    NotifyRequestEvent( void );
    NotifyRequestEvent( const NotifyRequestEvent & /*src*/ );
    const NotifyRequestEvent & operator = ( const NotifyRequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// LocalNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The notify request event of Local Service Interface.
 *          It is used to send notification request to stub object 
 *          within same process.
 **/
class AREG_API LocalNotifyRequestEvent    : public    NotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(LocalNotifyRequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates notification local event used to send start / stop notification request from Proxy to Stub.
     * \param   fromProxy   The source Proxy address, requesting start / stop notifications.
     * \param   toStub      The target Stub address to start / stop sending notifications.
     * \param   msgId       The ID of message. Normally either Attribute ID, Broadcast ID or Response ID. The Request IDs cannot be notified.
     * \param   reqType     The type of request.
     **/
    LocalNotifyRequestEvent( const ProxyAddress & fromProxy
                           , const StubAddress & toStub
                           , unsigned int msgId
                           , NEService::eRequestType reqType );

    /**
     * \brief   Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    LocalNotifyRequestEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~LocalNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalNotifyRequestEvent( void );
    LocalNotifyRequestEvent( const LocalNotifyRequestEvent & /*src*/ );
    const LocalNotifyRequestEvent & operator = ( const LocalNotifyRequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// RemoteNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The notify request event of Remote Service Interface.
 *          It is used to send notification request to stub object 
 *          between remote processes.
 **/
class AREG_API RemoteNotifyRequestEvent    : public    NotifyRequestEvent
{
    friend class RemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(RemoteNotifyRequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates notification remote event used to send start / stop notification request from Proxy to Stub.
     * \param   fromProxy   The source Proxy address, requesting start / stop notifications.
     * \param   toStub      The target Stub address to start / stop sending notifications.
     * \param   msgId       The ID of message. Normally either Attribute ID, Broadcast ID or Response ID. The Request IDs cannot be notified.
     * \param   reqType     The type of request.
     **/
    RemoteNotifyRequestEvent( const ProxyAddress & fromProxy
                            , const StubAddress & toStub
                            , unsigned int msgId
                            , NEService::eRequestType reqType );

    /**
     * \brief   Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    RemoteNotifyRequestEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Sets the target communication Channel object
     * \param   channel     The communication channel object to set.
     **/
    inline void setTargetChannel( const Channel & channel );

    /**
     * \brief   Returns target communication channel object
     **/
    inline const Channel & getTargetChannel( void ) const;

    /**
     * \brief   Sets the source communication Channel object.
     * \param   channel     The communication channel object to set.
     **/
    inline void setSourceChannel( const Channel & channel );

    /**
     * \brief   Returns source communication channel object
     **/
    inline const Channel & getSourceChannel( void ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    RemoteNotifyRequestEvent( void );
    RemoteNotifyRequestEvent( const RemoteNotifyRequestEvent & /*src*/ );
    const RemoteNotifyRequestEvent & operator = ( const RemoteNotifyRequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// RequestEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const EventData & RequestEvent::getData( void ) const
{
    return mData;
}

inline NEService::eMessageDataType RequestEvent::getDataType( void ) const
{
    return mData.getDataType();
}

inline EventData& RequestEvent::getData( void )
{
    return mData;
}

inline const IEInStream & RequestEvent::getReadStream( void ) const
{
    return mData.getReadStream();
}

inline IEOutStream & RequestEvent::getWriteStream( void )
{
    return mData.getWriteStream();
}

//////////////////////////////////////////////////////////////////////////
// RemoteRequestEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void RemoteRequestEvent::setTargetChannel(const Channel & channel)
{
    mTargetStubAddress.setChannel(channel);
}

inline const Channel & RemoteRequestEvent::getTargetChannel( void ) const
{
    return mTargetStubAddress.getChannel();
}

inline void RemoteRequestEvent::setSourceChannel( const Channel & channel )
{
    mProxySource.setChannel(channel);
}

inline const Channel & RemoteRequestEvent::getSourceChannel( void ) const
{
    return mProxySource.getChannel();
}

//////////////////////////////////////////////////////////////////////////
// RemoteNotifyRequestEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void RemoteNotifyRequestEvent::setTargetChannel(const Channel & channel)
{
    mTargetStubAddress.setChannel(channel);
}

inline const Channel & RemoteNotifyRequestEvent::getTargetChannel( void ) const
{
    return mTargetStubAddress.getChannel();
}

inline void RemoteNotifyRequestEvent::setSourceChannel( const Channel & channel )
{
    mProxySource.setChannel(channel);
}

inline const Channel & RemoteNotifyRequestEvent::getSourceChannel( void ) const
{
    return mProxySource.getChannel();
}

#endif  // AREG_COMPONENT_REQUESTEVENTS_HPP
