/************************************************************************
 * \file        areg/src/component/private/CEChannel.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, channel interface. Responsible to deliver
 *              events to target. The Target object is either in the same
 *              process or in the remote PC.
 ************************************************************************/
#include "areg/src/component/CEChannel.hpp"

const char  CEChannel::_CHANNEL_SEPARATOR   = '.';

CEChannel::CEChannel( void )
    : mSource( NEService::SOURCE_UNKNOWN )
    , mTarget( NEService::TARGET_UNKNOWN )
    , mCookie( NEService::COOKIE_UNKNOWN ) 
{
    ; // do nothing
}

CEChannel::CEChannel( ITEM_ID source, ITEM_ID target /*= NEService::TARGET_UNKNOWN*/, ITEM_ID cookie /*= NEService::COOKIE_UNKNOWN*/ )
    : mSource( source )
    , mTarget( target )
    , mCookie( cookie )
{

}

CEChannel::CEChannel( const CEChannel & source )
    : mSource( source.mSource )
    , mTarget( source.mTarget )
    , mCookie( source.mCookie )
{
    ; // do nothing
}

CEChannel::~CEChannel(void)
{
    ; // do nothing
}

const CEChannel & CEChannel::operator = ( const CEChannel & source )
{
    if ( static_cast<const CEChannel *>(this) != &source )
    {
        mSource = source.mSource;
        mTarget = source.mTarget;
        mCookie = source.mCookie;
    }
    return (*this);
}

CEString CEChannel::ToString( void ) const
{
    CEString result;
#if defined(_BIT64)
    return result.Format("%llu.%llu.%llu", mSource, mTarget, mCookie);
#else
    return result.Format("%u.%u.%u", mSource, mTarget, mCookie);
#endif
}

const CEChannel & CEChannel::FromString(const char * channel)
{
    mSource = NEService::SOURCE_UNKNOWN;
    mTarget = NEService::TARGET_UNKNOWN;
    mCookie = NEService::COOKIE_UNKNOWN;

    CEString temp(channel), source, target, cookie;
    NEString::CharPos pos = NEString::StartPos;
    pos = temp.Substr( source, _CHANNEL_SEPARATOR, pos );
    pos = temp.Substr( target, _CHANNEL_SEPARATOR, pos );
    pos = temp.Substr( cookie, _CHANNEL_SEPARATOR, pos );

#if defined(_BIT64)
    mSource = static_cast<ITEM_ID>( source.ToUInt64() );
    mTarget = static_cast<ITEM_ID>( target.ToUInt64() );
    mCookie = static_cast<ITEM_ID>( cookie.ToUInt64() );
#else
    mSource = static_cast<ITEM_ID>( source.ToUInt32() );
    mTarget = static_cast<ITEM_ID>( target.ToUInt32() );
    mCookie = static_cast<ITEM_ID>( cookie.ToUInt32() );
#endif

    return (*this);
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEChannel & input )
{
    stream >> input.mSource;
    stream >> input.mTarget;
    stream >> input.mCookie;
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEChannel & output)
{
    stream << output.mSource;
    stream << output.mTarget;
    stream << output.mCookie;
    return stream;
}
