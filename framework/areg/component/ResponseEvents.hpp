#ifndef AREG_COMPONENT_RESPONSEEVENTS_HPP
#define AREG_COMPONENT_RESPONSEEVENTS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ResponseEvents.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Generic Local and Remote Request 
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
 * \brief   Service response event sent from stub to proxy; carries serialized response/update data
 *          and target proxy address.
 **/
class AREG_API ResponseEvent   : public ServiceResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(ResponseEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes a response event with target address, result type, and response ID.
     *
     * \param   proxyTarget     Target proxy address.
     * \param   result          Result type indicating success/failure and validation.
     * \param   respId          Response or update ID.
     * \param   eventType       Event type.
     * \param   seqNr           Sequence number for ordering.
     **/
    ResponseEvent( const ProxyAddress & proxyTarget
                 , const NEService::ResultType result
                 , uint32_t respId
                 , Event::EventType eventType
                 , const SequenceNumber & seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );

    /**
     * \brief   Initializes a response event with serialized data.
     *
     * \param   args            Serialized response parameters.
     * \param   proxyTarget     Target proxy address.
     * \param   result          Result type indicating success/failure and validation.
     * \param   respId          Response or update ID.
     * \param   eventType       Event type.
     * \param   seqNr           Sequence number for ordering.
     * \param   name            Optional data stream name.
     **/
    ResponseEvent( const EventDataStream & args
                 , const ProxyAddress & proxyTarget
                 , NEService::ResultType result
                 , uint32_t respId
                 , Event::EventType eventType
                 , const SequenceNumber & seqNr = NEService::SEQUENCE_NUMBER_NOTIFY
                 , const String & name = String::empty_string() );

    /**
     * \brief   Clones an existing response but changes the target proxy address.
     *
     * \param   proxyTarget     New target proxy address.
     * \param   src             Source event to clone.
     **/
    ResponseEvent( const ProxyAddress & proxyTarget, const ResponseEvent & src );

    /**
     * \brief   Initializes the event from a stream.
     *
     * \param   stream      Input stream to read data.
     **/
    ResponseEvent( const InStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~ResponseEvent() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the event data object.
     **/
    inline const EventData & data() const;

    /**
     * \brief   Returns the data type of the response.
     **/
    inline NEService::MessageDataType data_type() const;

    /**
     * \brief   Returns the input stream for deserializing response parameters.
     **/
    inline const InStream & read_stream() const;

    /**
     * \brief   Returns the output stream for serializing response parameters.
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
    ResponseEvent() = delete;
    AREG_NOCOPY_NOMOVE( ResponseEvent );
};

//////////////////////////////////////////////////////////////////////////
// LocalResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * LocalResponseEvent class, used to send responses
 ************************************************************************/
/**
 * \brief   Generic Local Response Event object for triggering response or attribute update events
 *          on Proxy side within same process.
 **/
class AREG_API LocalResponseEvent : public    ResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(LocalResponseEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes local event with target address, result type, and response ID.
     *
     * \param   proxyTarget     The address of target Proxy
     * \param   result          The type of result to indicate whether it is response or update
     *                          event, as well as to specify message validation flag.
     * \param   respId          The ID of response. Can also be update ID.
     * \param   seqNr           The call sequence number.
     **/
    LocalResponseEvent( const ProxyAddress & proxyTarget
                      , NEService::ResultType result
                      , uint32_t respId
                      , const SequenceNumber & seqNr = NEService::SEQUENCE_NUMBER_NOTIFY);

    /**
     * \brief   Initializes local event with serialized arguments, target address, result type, and
     *          response ID.
     *
     * \param   args            Shared Buffer object with information of serialized parameters.
     * \param   proxyTarget     The address of target Proxy
     * \param   result          The type of result to indicate whether it is response or update
     *                          event, as well as to specify message validation flag.
     * \param   respId          The ID of response. Can also be update ID.
     * \param   seqNr           The call sequence number.
     * \param   name            Optional. name for event data. Can be nullptr.
     **/
    LocalResponseEvent( const EventDataStream & args
                      , const ProxyAddress & proxyTarget
                      , NEService::ResultType result
                      , uint32_t respId
                      , const SequenceNumber & seqNr = NEService::SEQUENCE_NUMBER_NOTIFY
                      , const String & name = String::empty_string() );

    /**
     * \brief   Clones existing information, except target Proxy address to send same message to
     *          different Proxies.
     *
     * \param   proxyTarget     The address of target Proxy
     * \param   src             The Event source to copy data.
     **/
    LocalResponseEvent(const ProxyAddress & proxyTarget, const LocalResponseEvent & src);

    /**
     * \brief   Creates event from streaming object and initializes data.
     *
     * \param   stream      The streaming object to read data
     **/
    LocalResponseEvent(const InStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~LocalResponseEvent() = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalResponseEvent() = delete;
    AREG_NOCOPY_NOMOVE( LocalResponseEvent );
};

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * RemoteResponseEvent class, used to send responses
 ************************************************************************/
/**
 * \brief   Generic Remote Response Event object for triggering response or attribute update events
 *          on Proxy side.
 **/
class AREG_API RemoteResponseEvent: public    ResponseEvent
{
    friend class RemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(RemoteResponseEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes remote event with target address, result type, and response ID.
     *
     * \param   proxyTarget     The address of target Proxy
     * \param   result          The type of result to indicate whether it is response or update
     *                          event, as well as to specify message validation flag.
     * \param   respId          The ID of response. Can also be update ID.
     * \param   seqNr           The call sequence number.
     **/
    RemoteResponseEvent( const ProxyAddress & proxyTarget
                       , NEService::ResultType result
                       , uint32_t respId
                       , const SequenceNumber & seqNr = NEService::SEQUENCE_NUMBER_NOTIFY );

    /**
     * \brief   Initializes remote event with serialized arguments, target address, result type, and
     *          response ID.
     *
     * \param   args            Shared Buffer object with information of serialized parameters.
     * \param   proxyTarget     The address of target Proxy
     * \param   result          The type of result to indicate whether it is response or update
     *                          event, as well as to specify message validation flag.
     * \param   respId          The ID of response. Can also be update ID.
     * \param   seqNr           The call sequence number.
     * \param   name            Optional. name for event data. Can be nullptr.
     **/
    RemoteResponseEvent( const EventDataStream & args
                       , const ProxyAddress & proxyTarget
                       , NEService::ResultType result
                       , uint32_t respId
                       , const SequenceNumber & seqNr = NEService::SEQUENCE_NUMBER_NOTIFY
                       , const String & name = String::empty_string() );

    /**
     * \brief   Clones existing information, except target Proxy address to send same message to
     *          different Proxies.
     *
     * \param   proxyTarget     The address of target Proxy
     * \param   src             The Event source to copy data.
     **/
    RemoteResponseEvent(const ProxyAddress & proxyTarget, const RemoteResponseEvent & src);

    /**
     * \brief   Creates event from streaming object and initializes data.
     *
     * \param   stream      The streaming object to read data
     **/
    RemoteResponseEvent(const InStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteResponseEvent() = default;

//////////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Sets the target communication channel object.
     *
     * \param   channel     The communication channel to set in remote event.
     **/
    inline void set_target_channel( const Channel & channel );

    /**
     * \brief   Returns the event communication channel object.
     **/
    inline const Channel & target_channel() const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RemoteResponseEvent() = delete;
    AREG_NOCOPY_NOMOVE( RemoteResponseEvent );
};

//////////////////////////////////////////////////////////////////////////
// ResponseEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const EventData & ResponseEvent::data() const
{
    return mData;
}

inline NEService::MessageDataType ResponseEvent::data_type() const
{
    return mData.data_type();
}

inline EventData & ResponseEvent::data()
{
    return mData;
}

inline const InStream & ResponseEvent::read_stream() const
{
    return mData.read_stream();
}

inline OutStream & ResponseEvent::write_stream()
{
    return mData.write_stream();
}

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void RemoteResponseEvent::set_target_channel(const Channel & channel)
{
    mTargetProxyAddress.set_channel(channel);
}

inline const Channel & RemoteResponseEvent::target_channel() const
{
    return mTargetProxyAddress.channel();
}

#endif  // AREG_COMPONENT_RESPONSEEVENTS_HPP
