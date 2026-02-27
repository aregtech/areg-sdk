#ifndef AREG_COMPONENT_REQUESTEVENTS_HPP
#define AREG_COMPONENT_REQUESTEVENTS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/RequestEvents.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Generic Local and Remote Request 
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
 * \brief   Service request event sent from proxy to stub; carries request ID and serialized
 *          parameter data.
 **/
class AREG_API RequestEvent    : public ServiceRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(RequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes a request event with source/target addresses and request ID (no
     *          parameters).
     *
     * \param   fromSource      Source proxy address.
     * \param   toTarget        Target stub address.
     * \param   reqId           Request ID.
     * \param   eventType       Event type (local or remote request).
     **/
    RequestEvent( const ProxyAddress & fromSource
                , const StubAddress & toTarget
                , uint32_t reqId
                , Event::EventType eventType);

    /**
     * \brief   Initializes a request event with serialized parameters.
     *
     * \param   args            Serialized request parameters.
     * \param   fromSource      Source proxy address.
     * \param   toTarget        Target stub address.
     * \param   reqId           Request ID.
     * \param   eventType       Event type (local or remote request).
     * \param   name            Optional data stream name.
     **/
    RequestEvent( const EventDataStream & args
                , const ProxyAddress & fromSource
                , const StubAddress & toTarget
                , uint32_t reqId
                , Event::EventType eventType
                , const String & name = String::empty_string());

    /**
     * \brief   Initializes the event from a stream.
     *
     * \param   stream      Input stream to read data.
     **/
    RequestEvent( const InStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~RequestEvent() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the event data object.
     **/
    inline const EventData & data() const;

    /**
     * \brief   Returns the data type of the request.
     **/
    inline NEService::MessageDataType data_type() const;

    /**
     * \brief   Returns the input stream for deserializing request parameters.
     **/
    inline const InStream & read_stream() const;

    /**
     * \brief   Returns the output stream for serializing request parameters.
     **/
    inline OutStream & write_stream();

protected:
    /**
     * \brief   Returns the event data object for modification.
     **/
    inline EventData & data();

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// StreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Reads and initializes event data from a stream.
     *
     * \param   stream      Input stream to read data.
     * \return  The input stream.
     **/
    const InStream & read_stream( const InStream & stream ) override;

    /**
     * \brief   Writes event data to a stream.
     *
     * \param   stream      Output stream to write data.
     * \return  The output stream.
     **/
    OutStream & write_stream( OutStream & stream ) const override;

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
    RequestEvent() = delete;
    AREG_NOCOPY_NOMOVE( RequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// LocalRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * LocalRequestEvent class, sends request event
 ************************************************************************/
/**
 * \brief   Event for triggering a service request on the stub side within the same process.
 **/
class AREG_API LocalRequestEvent  : public    RequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(LocalRequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes a request event with no data. Use when the request has no parameters.
     *
     * \param   fromSource      The address of the source proxy.
     * \param   toTarget        The address of the target stub.
     * \param   reqId           The ID of the request.
     **/
    LocalRequestEvent(const ProxyAddress & fromSource, const StubAddress & toTarget, uint32_t reqId);

    /**
     * \brief   Initializes a request event with data. Use when the request has parameters.
     *
     * \param   args            The shared buffer object containing the request data.
     * \param   fromSource      The address of the source proxy.
     * \param   toTarget        The address of the target stub.
     * \param   reqId           The ID of the request.
     * \param   name            Optional name for the event data; may be null or empty.
     **/
    LocalRequestEvent( const EventDataStream & args
                     , const ProxyAddress & fromSource
                     , const StubAddress & toTarget
                     , uint32_t reqId
                     , const String & name = String::empty_string());

    /**
     * \brief   Initializes the event by deserializing data from the given input stream.
     *
     * \param   stream      The input stream containing serialized event data.
     **/
    LocalRequestEvent( const InStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~LocalRequestEvent() = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalRequestEvent() = delete;
    AREG_NOCOPY_NOMOVE( LocalRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// RemoteRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * RemoteRequestEvent class, sends request event
 ************************************************************************/
/**
 * \brief   Generic Remote Request Event object to trigger requests on Stub side within different
 *          processes via service routing module.
 **/
class AREG_API RemoteRequestEvent : public    RequestEvent
{
    friend class RemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(RemoteRequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates event with no data. Initializes event source, target address, and message
     *          ID. Use if request has no parameters.
     *
     * \param   fromSource      The address of source Proxy.
     * \param   toTarget        The address of Stub target
     * \param   reqId           The ID of request.
     **/
    RemoteRequestEvent(const ProxyAddress & fromSource, const StubAddress & toTarget, uint32_t reqId);

    /**
     * \brief   Creates event with data. Initializes event source, target information, and message
     *          ID. Use if request with parameters should be sent.
     *
     * \param   args            The reference to Shared Buffer object to transfer data.
     * \param   fromSource      The address of Proxy source.
     * \param   toTarget        The address of Stub target.
     * \param   reqId           The ID of request.
     * \param   name            Optional. name for event data. Can be nullptr.
     **/
    RemoteRequestEvent( const EventDataStream & args
                      , const ProxyAddress & fromSource
                      , const StubAddress & toTarget
                      , uint32_t reqId
                      , const String & name = String::empty_string());


    /**
     * \brief   Initializes object data from streaming object.
     *
     * \param   stream      Streaming object, containing initialized data information.
     **/
    RemoteRequestEvent( const InStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteRequestEvent() = default;

//////////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Sets the target communication Channel object.
     *
     * \param   channel     The communication channel object to set.
     **/
    inline void set_target_channel( const Channel & channel );

    /**
     * \brief   Returns target communication channel object.
     **/
    inline const Channel & target_channel() const;

    /**
     * \brief   Sets the source communication Channel object.
     *
     * \param   channel     The communication channel object to set.
     **/
    inline void set_source_channel( const Channel & channel );

    /**
     * \brief   Returns source communication channel object.
     **/
    inline const Channel & source_channel() const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RemoteRequestEvent() = delete;
    AREG_NOCOPY_NOMOVE( RemoteRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// NotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event for sending notification subscription/unsubscription requests from a proxy to a
 *          stub.
 **/
class AREG_API NotifyRequestEvent : public ServiceRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(NotifyRequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes a notification request event for starting or stopping notifications.
     *
     * \param   fromProxy       The address of the source proxy requesting notifications.
     * \param   toStub          The address of the target stub to start or stop sending
     *                          notifications.
     * \param   msgId           The ID of the message (attribute, broadcast, or response). Request
     *                          IDs cannot be notified.
     * \param   reqType         The request type (subscribe or unsubscribe).
     * \param   eventType       The type of event (local or remote request).
     **/
    NotifyRequestEvent( const ProxyAddress & fromProxy
                      , const StubAddress & toStub
                      , uint32_t msgId
                      , NEService::RequestType reqType
                      , Event::EventType eventType );

    /**
     * \brief   Initializes the event by deserializing data from the given input stream.
     *
     * \param   stream      The input stream containing serialized event data.
     **/
    NotifyRequestEvent( const InStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~NotifyRequestEvent() = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    NotifyRequestEvent() = delete;
    AREG_NOCOPY_NOMOVE( NotifyRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// LocalNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event for sending notification subscription/unsubscription requests from a proxy to a
 *          stub within the same process.
 **/
class AREG_API LocalNotifyRequestEvent    : public    NotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(LocalNotifyRequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes a notification request event for starting or stopping notifications.
     *
     * \param   fromProxy       The address of the source proxy requesting notifications.
     * \param   toStub          The address of the target stub to start or stop sending
     *                          notifications.
     * \param   msgId           The ID of the message (attribute, broadcast, or response). Request
     *                          IDs cannot be notified.
     * \param   reqType         The request type (subscribe or unsubscribe).
     **/
    LocalNotifyRequestEvent( const ProxyAddress & fromProxy
                           , const StubAddress & toStub
                           , uint32_t msgId
                           , NEService::RequestType reqType );

    /**
     * \brief   Initializes the event by deserializing data from the given input stream.
     *
     * \param   stream      The input stream containing serialized event data.
     **/
    LocalNotifyRequestEvent( const InStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~LocalNotifyRequestEvent() = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalNotifyRequestEvent() = delete;
    AREG_NOCOPY_NOMOVE( LocalNotifyRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// RemoteNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Remote notification request event sent by a proxy to a stub to start or stop
 *          attribute/broadcast/response notifications.
 **/
class AREG_API RemoteNotifyRequestEvent    : public    NotifyRequestEvent
{
    friend class RemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(RemoteNotifyRequestEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes a notification request event for starting or stopping notifications
     *          between the specified proxy and stub.
     *
     * \param   fromProxy       The address of the source proxy requesting notifications.
     * \param   toStub          The address of the target stub to start or stop sending
     *                          notifications.
     * \param   msgId           The ID of the message (attribute, broadcast, or response). Request
     *                          IDs cannot be notified.
     * \param   reqType         The request type (subscribe or unsubscribe).
     **/
    RemoteNotifyRequestEvent( const ProxyAddress & fromProxy
                            , const StubAddress & toStub
                            , uint32_t msgId
                            , NEService::RequestType reqType );

    /**
     * \brief   Initializes the event by deserializing data from the given input stream.
     *
     * \param   stream      The input stream containing serialized event data.
     **/
    RemoteNotifyRequestEvent( const InStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteNotifyRequestEvent() = default;

//////////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Sets the target communication channel.
     *
     * \param   channel     The communication channel to set.
     **/
    inline void set_target_channel( const Channel & channel );

    /**
     * \brief   Returns the target communication channel.
     **/
    inline const Channel & target_channel() const;

    /**
     * \brief   Sets the source communication channel.
     *
     * \param   channel     The communication channel to set.
     **/
    inline void set_source_channel( const Channel & channel );

    /**
     * \brief   Returns the source communication channel.
     **/
    inline const Channel & source_channel() const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    RemoteNotifyRequestEvent() = delete;
    AREG_NOCOPY_NOMOVE( RemoteNotifyRequestEvent );
};

//////////////////////////////////////////////////////////////////////////
// RequestEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const EventData & RequestEvent::data() const
{
    return mData;
}

inline NEService::MessageDataType RequestEvent::data_type() const
{
    return mData.data_type();
}

inline EventData& RequestEvent::data()
{
    return mData;
}

inline const InStream & RequestEvent::read_stream() const
{
    return mData.read_stream();
}

inline OutStream & RequestEvent::write_stream()
{
    return mData.write_stream();
}

//////////////////////////////////////////////////////////////////////////
// RemoteRequestEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void RemoteRequestEvent::set_target_channel(const Channel & channel)
{
    mTargetStubAddress.set_channel(channel);
}

inline const Channel & RemoteRequestEvent::target_channel() const
{
    return mTargetStubAddress.channel();
}

inline void RemoteRequestEvent::set_source_channel( const Channel & channel )
{
    mProxySource.set_channel(channel);
}

inline const Channel & RemoteRequestEvent::source_channel() const
{
    return mProxySource.channel();
}

//////////////////////////////////////////////////////////////////////////
// RemoteNotifyRequestEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void RemoteNotifyRequestEvent::set_target_channel(const Channel & channel)
{
    mTargetStubAddress.set_channel(channel);
}

inline const Channel & RemoteNotifyRequestEvent::target_channel() const
{
    return mTargetStubAddress.channel();
}

inline void RemoteNotifyRequestEvent::set_source_channel( const Channel & channel )
{
    mProxySource.set_channel(channel);
}

inline const Channel & RemoteNotifyRequestEvent::source_channel() const
{
    return mProxySource.channel();
}

#endif  // AREG_COMPONENT_REQUESTEVENTS_HPP
