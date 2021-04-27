/************************************************************************
 * \file        areg/component/private/CEEventData.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Request and Response events,
 *              Event Data object containing minimal generic data to
 *              support request and response communication.
 *
 ************************************************************************/
#include "areg/component/CEEventData.hpp"
//////////////////////////////////////////////////////////////////////////
// CEEventData class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEEventData class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CEEventData::CEEventData( unsigned int msgId, CEEventDataStream::eEventData dataType, const char* name /*= NULL*/ )
    : mDataType (NEService::GetDataType(msgId))
    , mData     (dataType, name)
{
    ; // do nothing
}

CEEventData::CEEventData( unsigned int msgId, const CEEventDataStream & args, const char* name /*= NULL*/ )
    : mDataType (NEService::GetDataType(msgId))
    , mData     (args, name)
{
    mData.ResetCursor();
}

CEEventData::CEEventData( const CEEventData& src )
    : mDataType (src.mDataType)
    , mData     (src.mData)
{
    mData.ResetCursor();
}

CEEventData::CEEventData(const IEInStream & stream)
    : mDataType ( NEService::UNDEFINED_DATA_TYPE )
    , mData     ( CEEventDataStream::EventDataExternal )
{
    stream >> mDataType;
    stream >> mData;
    mData.ResetCursor();
}

CEEventData::~CEEventData( void )
{
    ; // do nothing
}

const CEEventData & CEEventData::operator = (const CEEventData & src)
{
    if (this != &src)
    {
        mDataType   = src.mDataType;
        mData       = src.mData;
    }
    return (*this);
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEEventData & input )
{
    stream >> input.mDataType;
    stream >> input.mData;
    input.mData.ResetCursor();
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEEventData & output )
{
    stream << output.mDataType;
    stream << output.mData;
    output.mData.ResetCursor();
    return stream;
}
