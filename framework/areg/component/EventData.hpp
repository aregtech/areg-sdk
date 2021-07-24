#ifndef AREG_COMPONENT_EVENTDATA_HPP
#define AREG_COMPONENT_EVENTDATA_HPP
/************************************************************************
 * \file        areg/component/EventData.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Data object containing data to
 *              support request and response communication.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/EventDataStream.hpp"
#include "areg/component/NEService.hpp"

//////////////////////////////////////////////////////////////////////////
// EventData class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * EventData class, used in request and response events
 ************************************************************************/
/**
 * \brief   Event Data object is used in request and response events
 *          to transfer data. It contains information like message ID, 
 *          data type and data serialized in binary buffer. De-serialization 
 *          of parameters depends on parameter type and specific for every call.
 **/
class AREG_API EventData
{
//////////////////////////////////////////////////////////////////////////
// List of friend classes to access protected members
//////////////////////////////////////////////////////////////////////////
    friend class LocalRequestEvent;
    friend class RemoteRequestEvent;
    friend class LocalResponseEvent;
    friend class RemoteResponseEvent;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Constructor.
     * \param	msgId	The ID of communication message. 
     *                  Data type will be set according of 
     *                  message ID type.
     * \param	name	Optional. Used to name data streaming object.
     *                  Can be NULL if there is no need to name streaming object.
     **/
    EventData(unsigned int msgId, EventDataStream::eEventData dataType, const char* name = NULL);

    /**
     * \brief	Constructor.
     * \param	msgId	The ID of communication message. 
     *                  Data type will be set according of 
     *                  message ID type.
     * \param	args	Streaming object containing serialized information
     *                  of parameters.
     * \param	name	Optional. Used to name data streaming object.
     *                  Can be NULL if there is no need to name streaming object.
     **/
    EventData(unsigned int msgId, const EventDataStream & args, const char* name = NULL);

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    EventData(const EventData & src);
    
    /**
     * \brief   Initialization constructor.
     *          Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    EventData( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    ~EventData( void );

public:
/************************************************************************/
// Friend global operators to stream Event Data Buffer
/************************************************************************/

    /**
     * \brief   Copies event data from given source.
     * \param   src     The source of data to copy.
     **/
    const EventData & operator = ( const EventData & src );

    /**
     * \brief	Friend global operator to initialize data from streaming.
     * \param	stream	The data streaming object to read data.
     * \param	input	The Event Data Buffer object to write data.
     * \return	Reference to Streaming object.
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, EventData & input );

    /**
     * \brief	Friend global operator to write object into streaming buffer.
     * \param	stream	The data streaming object to write data.
     * \param	output	The Event Data Buffer object of data source.
     * \return	Reference to Streaming object.
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const EventData & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    
    /**
     * \brief   Returns type of data. There are 2 types of data defined.
     *          It is either request or response. The update messages
     *          are classified as response.
     **/
    inline NEService::eMessageDataType getDataType( void ) const;
    
    /**
     * \brief   Returns reference of data input streaming object
     *          to de-serialize message parameters.
     **/
    inline const IEInStream & getReadStream( void ) const;
    
    /**
     * \brief   Returns reference of data output streaming object
     *          to serialize message parameters
     **/
    inline IEOutStream & getWriteStream( void );

    /**
     * \brief   Returns reference of data container object,
     *          which is a streaming object.
     **/
    inline const EventDataStream & getDataStream( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The type of data
     **/
    NEService::eMessageDataType    mDataType;
    /**
     * \brief   Streaming object, containing data in binary format.
     **/
    EventDataStream                 mData;
};

//////////////////////////////////////////////////////////////////////////
// EventData class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline NEService::eMessageDataType EventData::getDataType( void ) const
{
    return mDataType;
}

inline const IEInStream& EventData::getReadStream( void ) const
{
    return mData.getStreamForRead();
}

inline IEOutStream & EventData::getWriteStream( void )
{
    return mData.getStreamForWrite();
}

inline const EventDataStream & EventData::getDataStream( void ) const
{
    return mData;
}

#endif  // AREG_COMPONENT_EVENTDATA_HPP
