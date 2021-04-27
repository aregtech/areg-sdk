#ifndef AREG_COMPONENT_CEREQUESTEVENTS_HPP
#define AREG_COMPONENT_CEREQUESTEVENTS_HPP
/************************************************************************
 * \file        areg/component/CERequestEvents.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Local and Remote Request 
 *              Event object for local and remote communication
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/CEServiceRequestEvent.hpp"

#include "areg/component/CEEventData.hpp"

/************************************************************************
 * List of declared classes:
 ************************************************************************/
// CEServiceRequestEvent;
    class CERequestEvent;
        class CELocalRequestEvent;
        class CERemoteRequestEvent;

    class CENotifyRequestEvent;
        class CELocalNotifyRequestEvent;
        class CERemoteNotifyRequestEvent;
/************************************************************************
 * \brief   In this file following request event classes are declared:
 *              1. Request function call events:
 *                  a. CERequestEvent
 *                  b.      CELocalRequestEvent
 *                  c.      CERemoteRequestEvent
 *              2. Notification request events:
 *                  a.  CENotifyRequestEvent
 *                  b.      CELocalNotifyRequestEvent
 *                  c.      CERemoteNotifyRequestEvent
 *          These are base classes for communication, used to send
 *          Data in Request objects for local and remote communication.
 *          For more information, see description bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// CERequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * CERequestEvent class, sends request event
 ************************************************************************/
/**
 * \brief   Generic Request Event object used to trigger request on Stub side.
 *          Request Event is derived from Service Request event and
 *          should be base class for all Service Interface specific
 *          request calls. It is containing data object to transfer
 *          message specific parameter information.
 *
 **/
class AREG_API CERequestEvent    : public CEServiceRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CERequestEvent)

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
    CERequestEvent( const CEProxyAddress & fromSource
                  , const CEStubAddress & toTarget
                  , unsigned int reqId
                  , CEEvent::eEventType eventType);

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
    CERequestEvent( const CEEventDataStream & args
                  , const CEProxyAddress & fromSource
                  , const CEStubAddress & toTarget
                  , unsigned int reqId
                  , CEEvent::eEventType eventType
                  , const char* name = NULL);

    /**
     * \brief   Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    CERequestEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~CERequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
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
    CERequestEvent( void );
    CERequestEvent( const CERequestEvent & /*src*/ );
    const CERequestEvent & operator = ( const CERequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CELocalRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * CELocalRequestEvent class, sends request event
 ************************************************************************/
/**
 * \brief   Generic Local Request Event object used to trigger request on Stub
 *          side within same process. It is not used for remote service interface
 *          communication.
 **/
class AREG_API CELocalRequestEvent  : public    CERequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CELocalRequestEvent)

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
    CELocalRequestEvent(const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId);

    /**
     * \brief	Creates event with data. Initializes event source, target information and message ID.
     *          Use if request with parameter should be sent.
     * \param	args	    The reference to Shared Buffer object to transfer data.
     * \param	fromSource	The address of Proxy source.
     * \param	toTarget	The address of Stub target.
     * \param	reqId	    The ID of request.
     * \param	name	    Optional. Name for event data. Can be NULL.
     **/
    CELocalRequestEvent( const CEEventDataStream & args
                       , const CEProxyAddress & fromSource
                       , const CEStubAddress & toTarget
                       , unsigned int reqId
                       , const char* name = NULL);

    /**
     * \brief   Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    CELocalRequestEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~CELocalRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CELocalRequestEvent( void );
    CELocalRequestEvent( const CELocalRequestEvent & /*src*/ );
    const CELocalRequestEvent & operator = ( const CELocalRequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CERemoteRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * CERemoteRequestEvent class, sends request event
 ************************************************************************/
/**
 * \brief   Generic Remote Request Event object used to trigger request on Stub
 *          side within different processes via broker component. 
 *          It is used for remote service interface communication.
 **/
class AREG_API CERemoteRequestEvent : public    CERequestEvent
{
    friend class CERemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CERemoteRequestEvent)

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
    CERemoteRequestEvent(const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId);

    /**
     * \brief	Creates event with data. Initializes event source, target information and message ID.
     *          Use if request with parameter should be sent.
     * \param	args	    The reference to Shared Buffer object to transfer data.
     * \param	fromSource	The address of Proxy source.
     * \param	toTarget	The address of Stub target.
     * \param	reqId	    The ID of request.
     * \param	name	    Optional. Name for event data. Can be NULL.
     **/
    CERemoteRequestEvent( const CEEventDataStream & args
                        , const CEProxyAddress & fromSource
                        , const CEStubAddress & toTarget
                        , unsigned int reqId
                        , const char* name = NULL);


    /**
     * \brief   Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    CERemoteRequestEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~CERemoteRequestEvent( void );

//////////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Sets the target communication Channel object
     * \param   channel     The communication channel object to set.
     **/
    inline void SetTargetChannel( const CEChannel & channel );

    /**
     * \brief   Returns target communication channel object
     **/
    inline const CEChannel & GetTargetChannel( void ) const;

    /**
     * \brief   Sets the source communication Channel object.
     * \param   channel     The communication channel object to set.
     **/
    inline void SetSourceChannel( const CEChannel & channel );

    /**
     * \brief   Returns source communication channel object
     **/
    inline const CEChannel & GetSourceChannel( void ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CERemoteRequestEvent( void );
    CERemoteRequestEvent( const CERemoteRequestEvent & /*src*/ );
    const CERemoteRequestEvent & operator = ( const CERemoteRequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CENotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The notify request event is sent to stub object, when requesting
 *          to send notification messages like attribute change
 *          or response notification
 **/
class AREG_API CENotifyRequestEvent : public CEServiceRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CENotifyRequestEvent)

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
    CENotifyRequestEvent( const CEProxyAddress & fromProxy
                        , const CEStubAddress & toStub
                        , unsigned int msgId
                        , NEService::eRequestType reqType
                        , CEEvent::eEventType eventType );

    /**
     * \brief   Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    CENotifyRequestEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~CENotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CENotifyRequestEvent( void );
    CENotifyRequestEvent( const CENotifyRequestEvent & /*src*/ );
    const CENotifyRequestEvent & operator = ( const CENotifyRequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CELocalNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The notify request event of Local Service Interface.
 *          It is used to send notification request to stub object 
 *          within same process.
 **/
class AREG_API CELocalNotifyRequestEvent    : public    CENotifyRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CELocalNotifyRequestEvent)

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
    CELocalNotifyRequestEvent( const CEProxyAddress & fromProxy
                             , const CEStubAddress & toStub
                             , unsigned int msgId
                             , NEService::eRequestType reqType );

    /**
     * \brief   Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    CELocalNotifyRequestEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~CELocalNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CELocalNotifyRequestEvent( void );
    CELocalNotifyRequestEvent( const CELocalNotifyRequestEvent & /*src*/ );
    const CELocalNotifyRequestEvent & operator = ( const CELocalNotifyRequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CERemoteNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The notify request event of Remote Service Interface.
 *          It is used to send notification request to stub object 
 *          between remote processes.
 **/
class AREG_API CERemoteNotifyRequestEvent    : public    CENotifyRequestEvent
{
    friend class CERemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Declare event as runtime to support runtime casting.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CERemoteNotifyRequestEvent)

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
    CERemoteNotifyRequestEvent( const CEProxyAddress & fromProxy
                              , const CEStubAddress & toStub
                              , unsigned int msgId
                              , NEService::eRequestType reqType );

    /**
     * \brief   Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    CERemoteNotifyRequestEvent( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~CERemoteNotifyRequestEvent( void );

//////////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Sets the target communication Channel object
     * \param   channel     The communication channel object to set.
     **/
    inline void SetTargetChannel( const CEChannel & channel );

    /**
     * \brief   Returns target communication channel object
     **/
    inline const CEChannel & GetTargetChannel( void ) const;

    /**
     * \brief   Sets the source communication Channel object.
     * \param   channel     The communication channel object to set.
     **/
    inline void SetSourceChannel( const CEChannel & channel );

    /**
     * \brief   Returns source communication channel object
     **/
    inline const CEChannel & GetSourceChannel( void ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    CERemoteNotifyRequestEvent( void );
    CERemoteNotifyRequestEvent( const CERemoteNotifyRequestEvent & /*src*/ );
    const CERemoteNotifyRequestEvent & operator = ( const CERemoteNotifyRequestEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CERequestEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const CEEventData & CERequestEvent::GetData( void ) const
{   return mData;                       }

inline NEService::eMessageDataType CERequestEvent::GetDataType( void ) const
{   return mData.GetMessageDataType();         }

inline CEEventData& CERequestEvent::GetData( void )
{   return mData;                       }

inline const IEInStream & CERequestEvent::GetReadStream( void ) const
{   return mData.GetReadStream();       }

inline IEOutStream & CERequestEvent::GetWriteStream( void )
{   return mData.GetWriteStream();      }

//////////////////////////////////////////////////////////////////////////
// CERemoteRequestEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void CERemoteRequestEvent::SetTargetChannel(const CEChannel & channel)
{   mTargetStubAddress.SetChannel(channel); }

inline const CEChannel & CERemoteRequestEvent::GetTargetChannel( void ) const
{   return mTargetStubAddress.GetChannel(); }

inline void CERemoteRequestEvent::SetSourceChannel( const CEChannel & channel )
{   mProxySource.SetChannel(channel);       }

inline const CEChannel & CERemoteRequestEvent::GetSourceChannel( void ) const
{   return mProxySource.GetChannel();       }

//////////////////////////////////////////////////////////////////////////
// CERemoteNotifyRequestEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void CERemoteNotifyRequestEvent::SetTargetChannel(const CEChannel & channel)
{   mTargetStubAddress.SetChannel(channel); }

inline const CEChannel & CERemoteNotifyRequestEvent::GetTargetChannel( void ) const
{   return mTargetStubAddress.GetChannel(); }

inline void CERemoteNotifyRequestEvent::SetSourceChannel( const CEChannel & channel )
{   mProxySource.SetChannel(channel);       }

inline const CEChannel & CERemoteNotifyRequestEvent::GetSourceChannel( void ) const
{   return mProxySource.GetChannel();       }

#endif  // AREG_COMPONENT_CEREQUESTEVENTS_HPP
