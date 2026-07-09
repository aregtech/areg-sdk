/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/Channel.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, channel interface. Responsible to deliver
 *              events to target. The Target object is either in the same
 *              process or in the remote PC.
 ************************************************************************/
#include "areg/component/Channel.hpp"
#include "areg/base/CommonDefs.hpp"
namespace areg {

const Channel & Channel::invalid_channel()
{
    static const Channel _invalidChannel( areg::SOURCE_UNKNOWN, areg::TARGET_UNKNOWN, areg::COOKIE_UNKNOWN );
    return _invalidChannel;
}

String Channel::to_string() const
{
    constexpr const char * format{ "%u.%u.%u" };

    char buffer[ 128 ]{ 0 };
    int32_t len = String::format_string( buffer, 128, format, mSource, mTarget, mCookie );
    return (len > 0 ? String( buffer, static_cast<uint32_t>(len) ) : String::empty_string());
}

const Channel & Channel::from_string(const String & channel)
{
    mSource = areg::SOURCE_UNKNOWN;
    mTarget = areg::TARGET_UNKNOWN;
    mCookie = areg::COOKIE_UNKNOWN;

    String source, target, cookie;
    areg::CharPos pos = areg::START_POS;
    pos = channel.substring( source, areg::OBJECT_SEPARATOR, pos );
    pos = channel.substring( target, areg::OBJECT_SEPARATOR, pos );
    channel.substring( cookie, areg::OBJECT_SEPARATOR, pos );

    mSource = static_cast<ITEM_ID>( source.to_uint32() );
    mTarget = static_cast<ITEM_ID>( target.to_uint32() );
    mCookie = static_cast<ITEM_ID>( cookie.to_uint32() );

    return (*this);
}

} // namespace areg
