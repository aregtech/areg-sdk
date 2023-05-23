#ifndef AREG_COMPONENT_EVENTDATA_HPP
#define AREG_COMPONENT_EVENTDATA_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/EventData.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Event Data object containing data to
 *              support request and response communication.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/IEIOStream.hpp"
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
     *                  Can be nullptr if there is no need to name streaming object.
     **/
    EventData(unsigned int msgId, EventDataStream::eEventData dataType, const String & name = String::getEmptyString());

    /**
     * \brief	Constructor.
     * \param	msgId	The ID of communication message.
     *                  Data type will be set according of
     *                  message ID type.
     * \param	args	Streaming object containing serialized information
     *                  of parameters.
     * \param	name	Optional. Used to name data streaming object.
     *                  Can be nullptr if there is no need to name streaming object.
     **/
    EventData(unsigned int msgId, const EventDataStream & args, const String & name = String::getEmptyString());

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    EventData(const EventData & src);
    
    /**
     * \brief   Move constructor.
     * \param   src     The source of data to move.
     **/
    EventData( EventData && src ) noexcept;

    /**
     * \brief   Initialization constructor.
     *          Initializes object data from streaming object.
     * \param   stream  Streaming object, containing initialized data information.
     **/
    EventData( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    ~EventData( void ) = default;

public:
/************************************************************************/
// Friend global operators to stream Event Data Buffer
/************************************************************************/

    /**
     * \brief   Copies event data from given source.
     * \param   src     The source of data to copy.
     **/
    EventData & operator = ( const EventData & src );

    /**
     * \brief   Moves event data from given source.
     * \param   src     The source of data to move.
     **/
    EventData & operator = ( EventData && src ) noexcept;

    /**
     * \brief	Friend global operator to initialize data from streaming.
     * \param	stream	The data streaming object to read data.
     * \param	input	The Event Data Buffer object to write data.
     * \return	Reference to Streaming object.
     **/
    friend inline const IEInStream & operator >> ( const IEInStream & stream, EventData & input );

    /**
     * \brief	Friend global operator to write object into streaming buffer.
     * \param	stream	The data streaming object to write data.
     * \param	output	The Event Data Buffer object of data source.
     * \return	Reference to Streaming object.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const EventData & output );

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
     *          to deserialize message parameters.
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

inline const IEInStream & operator >> ( const IEInStream & stream, EventData & input )
{
    stream >> input.mDataType;
    stream >> input.mData;
    input.mData.resetCursor();
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const EventData & output )
{
    stream << output.mDataType;
    stream << output.mData;
    output.mData.resetCursor();
    return stream;
}

#endif  // AREG_COMPONENT_EVENTDATA_HPP
