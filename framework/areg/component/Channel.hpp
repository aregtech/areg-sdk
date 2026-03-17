#ifndef AREG_COMPONENT_CHANNEL_HPP
#define AREG_COMPONENT_CHANNEL_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/Channel.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, channel object. Responsible to deliver
 *              events to target. The Target object is either in the same
 *              process or in the remote PC.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/component/ServiceDefs.hpp"
#include "areg/base/IOStream.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// Channel class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Represents a communication channel between services; contains source and target
 *          identifiers and a system-assigned cookie.
 **/
class AREG_API Channel
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns a predefined invalid channel object.
     **/
    static const Channel & invalid_channel();                //!< Invalid channel

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    constexpr Channel() noexcept = default;

    constexpr Channel( const Channel & source ) noexcept = default;

    constexpr Channel( Channel && source ) noexcept = default;

    ~Channel() = default;

    /**
     * \brief   Initializes the channel with source and target identifiers and an optional cookie.
     *
     * \param   source      The channel communication source ID.
     * \param   target      The channel communication target ID.
     * \param   cookie      The system-assigned ID.
     **/
    explicit inline constexpr Channel( const ITEM_ID & source, const ITEM_ID & target = areg::TARGET_UNKNOWN, const ITEM_ID & cookie = areg::COOKIE_UNKNOWN ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    inline constexpr Channel & operator = ( const Channel & source ) noexcept = default;

    inline constexpr Channel & operator = ( Channel && source ) noexcept = default;

    [[nodiscard]]
    inline constexpr bool operator == ( const Channel & other ) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator != ( const Channel & other ) const noexcept;

    /**
     * \brief   Converts the channel to its ITEM_ID representation.
     **/
    [[nodiscard]]
    inline explicit constexpr operator const ITEM_ID & () const noexcept;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/

    /**
     * \brief   Reads channel data from stream.
     *
     * \param       stream  The stream object to read from.
     * \param[out]  input   The channel to initialize with data from the stream.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, Channel & input );

    /**
     * \brief   Writes channel data to stream.
     *
     * \param[out]  stream  The stream object to write to.
     * \param       output  The channel to write.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const Channel & output);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the source identifier of the channel.
     **/
    [[nodiscard]]
    inline constexpr const ITEM_ID& source() const noexcept;

    /**
     * \brief   Sets the source identifier of the channel.
     *
     * \param   source      The new source identifier.
     **/
    inline constexpr void set_source(const ITEM_ID& source ) noexcept;

    /**
     * \brief   Returns the target identifier of the channel.
     **/
    [[nodiscard]]
    inline constexpr const ITEM_ID& target() const noexcept;
    /**
     * \brief   Sets the target identifier of the channel.
     *
     * \param   target      The new target identifier.
     **/

    inline constexpr void set_target(const ITEM_ID & target ) noexcept;

    /**
     * \brief   Returns the cookie identifier of the channel.
     **/
    [[nodiscard]]
    inline constexpr const ITEM_ID & cookie() const noexcept;

    /**
     * \brief   Sets the cookie identifier of the channel.
     *
     * \param   cookie      The new cookie identifier.
     **/
    inline constexpr void set_cookie(const ITEM_ID & cookie ) noexcept;

    /**
     * \brief   Returns true if the channel data is valid.
     **/
    [[nodiscard]]
    inline constexpr bool is_valid() const noexcept;

    /**
     * \brief   Invalidates the channel.
     **/
    inline constexpr void invalidate() noexcept;

public:
    /**
     * \brief   Converts the channel data to a string representation.
     **/
    [[nodiscard]]
    String to_string() const;

    /**
     * \brief   Initializes the channel from a string representation.
     *
     * \param   channel     The string containing channel data.
     **/
    const Channel & from_string( const String & channel );


//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Channel source ID
     **/
    ITEM_ID     mSource{ areg::SOURCE_UNKNOWN };
    /**
     * \brief   Channel Target ID
     **/
    ITEM_ID     mTarget{ areg::TARGET_UNKNOWN };
    /**
     * \brief   Channel Cookie assigned by system
     **/
    ITEM_ID     mCookie{ areg::COOKIE_UNKNOWN };
};

//////////////////////////////////////////////////////////////////////////
// Channel class inline methods.
//////////////////////////////////////////////////////////////////////////

inline constexpr Channel::Channel(const ITEM_ID& source, const ITEM_ID& target /*= areg::TARGET_UNKNOWN*/, const ITEM_ID& cookie /*= areg::COOKIE_UNKNOWN*/) noexcept
    : mSource(source)
    , mTarget(target)
    , mCookie(cookie)
{
}

inline constexpr bool Channel::operator == (const Channel& other) const noexcept
{
    return (this != &other ? (mCookie == other.mCookie) && (mTarget == other.mTarget) && (mSource == other.mSource) : true);
}

inline constexpr bool Channel::operator != (const Channel& other) const noexcept
{
    return (this != &other ? (mCookie != other.mCookie) || (mTarget != other.mTarget) || (mSource != other.mSource) : false);
}

inline constexpr Channel::operator const ITEM_ID& () const noexcept
{
    return mSource;
}

inline constexpr const ITEM_ID & Channel::source() const noexcept
{
    return mSource;
}

inline constexpr void Channel::set_source(const ITEM_ID & source) noexcept
{
    mSource = source;
}

inline constexpr const ITEM_ID & Channel::target() const noexcept
{
    return mTarget;
}

inline constexpr void Channel::set_target(const ITEM_ID & target) noexcept
{
    mTarget = target;
}

inline constexpr const ITEM_ID & Channel::cookie() const noexcept
{
    return mCookie;
}

inline constexpr void Channel::set_cookie(const ITEM_ID & cookie) noexcept
{
    mCookie = cookie;
}

inline constexpr bool Channel::is_valid() const noexcept
{
    return (mCookie != areg::COOKIE_UNKNOWN);
}

inline constexpr void Channel::invalidate() noexcept
{
    mSource = areg::SOURCE_UNKNOWN;
    mTarget = areg::TARGET_UNKNOWN;
    mCookie = areg::COOKIE_UNKNOWN;
}

inline const InStream & operator >> ( const InStream & stream, Channel & input )
{
    stream >> input.mSource;
    stream >> input.mTarget;
    stream >> input.mCookie;
    return stream;
}

inline OutStream & operator << ( OutStream & stream, const Channel & output)
{
    stream << output.mSource;
    stream << output.mTarget;
    stream << output.mCookie;
    return stream;
}

} // namespace areg
#endif  // AREG_COMPONENT_CHANNEL_HPP
