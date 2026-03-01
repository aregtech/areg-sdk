#ifndef AREG_COMPONENT_EVENTDATA_HPP
#define AREG_COMPONENT_EVENTDATA_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/EventData.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Data object containing data to
 *              support request and response communication.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/IOStream.hpp"
#include "areg/component/EventDataStream.hpp"
#include "areg/component/ServiceDefs.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventData class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * EventData class, used in request and response events
 ************************************************************************/
/**
 * \brief   Container for message data (request or response); holds message ID and serialized
 *          parameter buffer.
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
     * \brief   Initializes event data with a message ID and data type.
     *
     * \param   msgId       Message ID.
     * \param   dataType    Data type determined by the message ID.
     * \param   name        Optional name for the data stream.
     **/
    EventData(uint32_t msgId, EventDataStream::EventDataKind dataType, const String & name = String::empty_string());

    /**
     * \brief   Initializes event data with a message ID and serialized parameters.
     *
     * \param   msgId       Message ID.
     * \param   args        Stream containing serialized parameters.
     * \param   name        Optional name for the data stream.
     **/
    EventData(uint32_t msgId, const EventDataStream & args, const String & name = String::empty_string());

    /**
     * \brief   Copies event data from another instance.
     *
     * \param   src     Source to copy.
     **/
    EventData(const EventData & src);
    
    /**
     * \brief   Moves event data from another instance.
     *
     * \param   src     Source to move.
     * \note    Move overload. Takes ownership of the source data.
     **/
    EventData( EventData && src ) noexcept;

    /**
     * \brief   Initializes event data from a stream.
     *
     * \param   stream      Stream containing event data.
     **/
    EventData( const InStream & stream );

    /**
     * \brief   Destructor.
     **/
    ~EventData() = default;

public:
/************************************************************************/
// Friend global operators to stream Event Data Buffer
/************************************************************************/

    /**
     * \brief   Copies event data from another instance.
     *
     * \param   src     Source to copy.
     **/
    EventData & operator = ( const EventData & src );

    /**
     * \brief   Moves event data from another instance.
     *
     * \param   src     Source to move.
     * \note    Move overload. Takes ownership of the source data.
     **/
    EventData & operator = ( EventData && src ) noexcept;

    /**
     * \brief   Initializes event data from a stream.
     *
     * \param   stream      Input stream.
     * \param   input       Event data object to initialize.
     * \return  The input stream.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, EventData & input );

    /**
     * \brief   Writes event data to a stream.
     *
     * \param   stream      Output stream.
     * \param   output      Event data object to write.
     * \return  The output stream.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const EventData & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    
    /**
     * \brief   Returns the data type (request or response).
     **/
    inline areg::MessageDataType data_type() const;
    
    /**
     * \brief   Returns the input stream for deserializing message parameters.
     **/
    inline const InStream & read_stream() const;
    
    /**
     * \brief   Returns the output stream for serializing message parameters.
     **/
    inline OutStream & write_stream();

    /**
     * \brief   Returns the underlying data stream container.
     **/
    inline const EventDataStream & data_stream() const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The type of data
     **/
    areg::MessageDataType    mDataType;
    /**
     * \brief   Streaming object, containing data in binary format.
     **/
    EventDataStream                 mData;
};

//////////////////////////////////////////////////////////////////////////
// EventData class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline areg::MessageDataType EventData::data_type() const
{
    return mDataType;
}

inline const InStream& EventData::read_stream() const
{
    return mData.stream_for_read();
}

inline OutStream & EventData::write_stream()
{
    return mData.stream_for_write();
}

inline const EventDataStream & EventData::data_stream() const
{
    return mData;
}

inline const InStream & operator >> ( const InStream & stream, EventData & input )
{
    stream >> input.mDataType;
    stream >> input.mData;
    input.mData.reset();
    return stream;
}

inline OutStream & operator << ( OutStream & stream, const EventData & output )
{
    stream << output.mDataType;
    stream << output.mData;
    output.mData.reset();
    return stream;
}

} // namespace areg
#endif  // AREG_COMPONENT_EVENTDATA_HPP
