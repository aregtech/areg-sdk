/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Channel.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, channel interface. Responsible to deliver
 *              events to target. The Target object is either in the same
 *              process or in the remote PC.
 ************************************************************************/
#include "areg/component/Channel.hpp"
#include "areg/base/NECommon.hpp"

const Channel & Channel::getInvalidChannel( void )
{
    static const Channel _invalidChannel( NEService::SOURCE_UNKNOWN, NEService::TARGET_UNKNOWN, NEService::COOKIE_UNKNOWN );
    return _invalidChannel;
}

Channel::Channel( void )
    : mSource( NEService::SOURCE_UNKNOWN )
    , mTarget( NEService::TARGET_UNKNOWN )
    , mCookie( NEService::COOKIE_UNKNOWN ) 
{
}

Channel::Channel(const ITEM_ID & source, const ITEM_ID & target /*= NEService::TARGET_UNKNOWN*/, const ITEM_ID & cookie /*= NEService::COOKIE_UNKNOWN*/ )
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
}

Channel::Channel( Channel && source ) noexcept
    : mSource( source.mSource )
    , mTarget( source.mTarget )
    , mCookie( source.mCookie )
{
}

Channel & Channel::operator = ( const Channel & source )
{
    mSource = source.mSource;
    mTarget = source.mTarget;
    mCookie = source.mCookie;
    return (*this);
}

Channel & Channel::operator = ( Channel && source ) noexcept
{
    mSource = source.mSource;
    mTarget = source.mTarget;
    mCookie = source.mCookie;
    return (*this);
}

String Channel::convToString( void ) const
{
    constexpr const char * format{ "%llu.%llu.%llu" };

    char buffer[ 128 ]{ 0 };
    int len = String::formatString( buffer, 128, format, mSource, mTarget, mCookie );
    return (len > 0 ? String( buffer, static_cast<uint32_t>(len) ) : String::getEmptyString());
}

const Channel & Channel::convFromString(const String & channel)
{
    mSource = NEService::SOURCE_UNKNOWN;
    mTarget = NEService::TARGET_UNKNOWN;
    mCookie = NEService::COOKIE_UNKNOWN;

    String source, target, cookie;
    NEString::CharPos pos = NEString::START_POS;
    pos = channel.substring( source, NECommon::OBJECT_SEPARATOR, pos );
    pos = channel.substring( target, NECommon::OBJECT_SEPARATOR, pos );
    channel.substring( cookie, NECommon::OBJECT_SEPARATOR, pos );

    mSource = static_cast<ITEM_ID>( source.toUInt64() );
    mTarget = static_cast<ITEM_ID>( target.toUInt64() );
    mCookie = static_cast<ITEM_ID>( cookie.toUInt64() );

    return (*this);
}
