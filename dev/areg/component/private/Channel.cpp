/************************************************************************
 * \file        areg/component/private/Channel.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, channel interface. Responsible to deliver
 *              events to target. The Target object is either in the same
 *              process or in the remote PC.
 ************************************************************************/
#include "areg/component/Channel.hpp"

const char  Channel::_CHANNEL_SEPARATOR   = '.';

Channel::Channel( void )
    : mSource( NEService::SOUR_UNKNOWN )
    , mTarget( NEService::TARGET_UNKNOWN )
    , mCookie( NEService::COOKIE_UNKNOWN ) 
{
    ; // do nothing
}

Channel::Channel( ITEM_ID source, ITEM_ID target /*= NEService::TARGET_UNKNOWN*/, ITEM_ID cookie /*= NEService::COOKIE_UNKNOWN*/ )
    : mSource( source )
    , mTarget( target )
    , mCookie( cookie )
{

}

Channel::Channel( const Channel & source )
    : mSource( source.mSource )
    , mTarget( source.mTarget )
    , mCookie( source.mCookie )
{
    ; // do nothing
}

Channel::~Channel(void)
{
    ; // do nothing
}

const Channel & Channel::operator = ( const Channel & source )
{
    if ( static_cast<const Channel *>(this) != &source )
    {
        mSource = source.mSource;
        mTarget = source.mTarget;
        mCookie = source.mCookie;
    }
    return (*this);
}

String Channel::convToString( void ) const
{
    String result;
#if defined(_BIT64)
    return result.formatString("%llu.%llu.%llu", mSource, mTarget, mCookie);
#else
    return result.formatString("%u.%u.%u", mSource, mTarget, mCookie);
#endif
}

const Channel & Channel::convFromString(const char * channel)
{
    mSource = NEService::SOUR_UNKNOWN;
    mTarget = NEService::TARGET_UNKNOWN;
    mCookie = NEService::COOKIE_UNKNOWN;

    String temp(channel), source, target, cookie;
    NEString::CharPos pos = NEString::StartPos;
    pos = temp.substring( source, _CHANNEL_SEPARATOR, pos );
    pos = temp.substring( target, _CHANNEL_SEPARATOR, pos );
    pos = temp.substring( cookie, _CHANNEL_SEPARATOR, pos );

#if defined(_BIT64)
    mSource = static_cast<ITEM_ID>( source.convToUInt64() );
    mTarget = static_cast<ITEM_ID>( target.convToUInt64() );
    mCookie = static_cast<ITEM_ID>( cookie.convToUInt64() );
#else
    mSource = static_cast<ITEM_ID>( source.convToUInt32() );
    mTarget = static_cast<ITEM_ID>( target.convToUInt32() );
    mCookie = static_cast<ITEM_ID>( cookie.convToUInt32() );
#endif

    return (*this);
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, Channel & input )
{
    stream >> input.mSource;
    stream >> input.mTarget;
    stream >> input.mCookie;
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const Channel & output)
{
    stream << output.mSource;
    stream << output.mTarget;
    stream << output.mCookie;
    return stream;
}
