/************************************************************************
 * \file        areg/src/component/private/CEEventDataStream.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, event data stream implementation
 *
 ************************************************************************/
#include "areg/src/component/CEEventDataStream.hpp"

//////////////////////////////////////////////////////////////////////////
// CEEventDataStream class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEEventDataStream class, static members
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Predefined Empty Data object.
 **/
const CEEventDataStream CEEventDataStream::EmptyData(CEEventDataStream::EventDataInternal, static_cast<const char *>("CEEventDataStream::EmptyData"));

//////////////////////////////////////////////////////////////////////////
// CEEventDataStream class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CEEventDataStream::CEEventDataStream( CEEventDataStream::eEventData evetDataType, const char* name /*= NULL*/ )
    : IEIOStream    ( )

    , mEventDataType(evetDataType)
    , mBufferName   (name != NULL ? name : "CEEventDataStream")
    , mDataBuffer   ( )
    , mSharedList   ( )
{
    ;
}

CEEventDataStream::CEEventDataStream( const CEEventDataStream & buffer, const char* name )
    : IEIOStream    ( )

    , mEventDataType(buffer.mEventDataType)
    , mBufferName   (name != NULL ? name : "CEEventDataStream")
    , mDataBuffer   (buffer.mDataBuffer)
    , mSharedList   (static_cast<const TEStack<CESharedBuffer, const CESharedBuffer &> &>(buffer.mSharedList))
{
    mDataBuffer.MoveToBeginOfData();
}

CEEventDataStream::CEEventDataStream( const CEEventDataStream & src )
    : IEIOStream    ( )

    , mEventDataType(src.mEventDataType)
    , mBufferName   (src.mBufferName)
    , mDataBuffer   (src.mDataBuffer)
    , mSharedList   (static_cast<const TEStack<CESharedBuffer, const CESharedBuffer &> &>(src.mSharedList))
{
    mDataBuffer.MoveToBeginOfData();
}

CEEventDataStream::CEEventDataStream(const IEInStream & stream)
    : IEIOStream    ( )

    , mEventDataType( CEEventDataStream::EventDataExternal)
    , mBufferName   ("CEEventDataStream")
    , mDataBuffer   ( )
    , mSharedList   ( )
{
    stream >> mEventDataType >> mBufferName >> mDataBuffer;
}

CEEventDataStream::~CEEventDataStream( void )
{
    mSharedList.RemoveAll();
    mDataBuffer.InvalidateBuffer();
}

//////////////////////////////////////////////////////////////////////////
// CEEventDataStream class, operators
//////////////////////////////////////////////////////////////////////////
const CEEventDataStream & CEEventDataStream::operator = ( const CEEventDataStream & src )
{
    if (static_cast<const CEEventDataStream *>(this) != &src)
    {
        mSharedList = static_cast<const TEStack<CESharedBuffer, const CESharedBuffer &> &>(src.mSharedList);
        mDataBuffer = src.mDataBuffer;
        mDataBuffer.MoveToBeginOfData();
    }
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// CEEventDataStream class, Methods
//////////////////////////////////////////////////////////////////////////
unsigned int CEEventDataStream::Read( unsigned char* buffer, unsigned int size ) const
{
    return mDataBuffer.Read(buffer, size);
}

unsigned int CEEventDataStream::Read( IEByteBuffer & buffer ) const
{
    unsigned int result = 0;
    if (mEventDataType == CEEventDataStream::EventDataInternal && mSharedList.IsEmpty() == false)
    {
        static_cast<CESharedBuffer &>(buffer) = mSharedList.PopElement();
        result = buffer.GetUsedSize();
    }
    else
    {
        result = mDataBuffer.Read(buffer);
    }
    return result;
}

unsigned int CEEventDataStream::Read( CEString & asciiString ) const
{
    return mDataBuffer.Read(asciiString);
}

unsigned int CEEventDataStream::Read( CEWideString & wideString ) const
{
    return mDataBuffer.Read(wideString);
}

void CEEventDataStream::ResetCursor( void ) const
{
    mDataBuffer.MoveToBeginOfData();
}

unsigned int CEEventDataStream::Write( const unsigned char* buffer, unsigned int size )
{
    return mDataBuffer.Write(buffer, size);
}

unsigned int CEEventDataStream::Write( const IEByteBuffer & buffer )
{
    unsigned int result = 0;
    if (mEventDataType == CEEventDataStream::EventDataInternal)
    {
        mSharedList.PushElement( static_cast<const CESharedBuffer &>(buffer) );
        result = buffer.GetUsedSize();
    }
    else
    {
        result = mDataBuffer.Write(buffer);
    }
    return result;
}

unsigned int CEEventDataStream::Write( const CEString & asciiString )
{
    return mDataBuffer.Write(asciiString);
}

unsigned int CEEventDataStream::Write( const CEWideString & wideString )
{
    return mDataBuffer.Write(wideString);
}

void CEEventDataStream::Flush( void )
{
    ; // do nothing
}

unsigned int CEEventDataStream::GetReadableSize( void ) const
{
    ASSERT(false);
    return 0;
}

unsigned int CEEventDataStream::GetWritableSize( void ) const
{
    ASSERT(false);
    return 0;
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEEventDataStream & input )
{
    stream >> input.mEventDataType;
    stream >> input.mBufferName;
    stream >> input.mDataBuffer;
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEEventDataStream & output )
{
    ASSERT(output.mEventDataType == CEEventDataStream::EventDataExternal);
    stream << CEEventDataStream::EventDataExternal;
    stream << output.mBufferName;
    stream << output.mDataBuffer;
    return stream;
}
