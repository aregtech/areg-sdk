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

#include "areg/base/IOStream.hpp"
#include "areg/base/String.hpp"
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
     * \param   source      CRC32(thread_name) of the source thread.
     * \param   target      CRC32(thread_name) of the target thread, or 0 if unknown.
     * \param   cookie      Process identity cookie (0=unknown, 1=local, >=256 IPC-assigned).
     **/
    explicit inline constexpr Channel( uint32_t source, uint32_t target = 0u, uint32_t cookie = 0u ) noexcept;

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
     * \brief   Returns the source as a scalar channel identity.
     **/
    [[nodiscard]]
    inline explicit constexpr operator uint32_t () const noexcept;

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
     * \brief   Returns the source CRC32(thread_name) of this channel.
     **/
    [[nodiscard]]
    inline constexpr uint32_t source() const noexcept;

    /**
     * \brief   Sets the source thread magic.
     *
     * \param   source      CRC32(thread_name) of the source thread.
     **/
    inline constexpr void set_source( uint32_t source ) noexcept;

    /**
     * \brief   Returns the target CRC32(thread_name) of this channel, or 0 if unknown.
     **/
    [[nodiscard]]
    inline constexpr uint32_t target() const noexcept;

    /**
     * \brief   Sets the target thread magic.
     *
     * \param   target      CRC32(thread_name) of the target thread.
     **/
    inline constexpr void set_target( uint32_t target ) noexcept;

    /**
     * \brief   Returns the process identity cookie.
     **/
    [[nodiscard]]
    inline constexpr uint32_t cookie() const noexcept;

    /**
     * \brief   Sets the process identity cookie.
     *
     * \param   cookie      Process cookie (0=unknown, 1=local, >=256 IPC-assigned).
     **/
    inline constexpr void set_cookie( uint32_t cookie ) noexcept;

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
    uint32_t    mSource { 0u };  //!< CRC32(thread_name) of the source thread
    uint32_t    mTarget { 0u };  //!< CRC32(thread_name) of the target thread; 0 = unknown
    uint32_t    mCookie { 0u };  //!< process identity cookie (0=unknown, 1=local, >=256 IPC)
};

//////////////////////////////////////////////////////////////////////////
// Channel class inline methods.
//////////////////////////////////////////////////////////////////////////

inline constexpr Channel::Channel( uint32_t source, uint32_t target, uint32_t cookie ) noexcept
    : mSource ( source )
    , mTarget ( target )
    , mCookie ( cookie )
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

inline constexpr Channel::operator uint32_t () const noexcept
{
    return mSource;
}

inline constexpr uint32_t Channel::source() const noexcept
{
    return mSource;
}

inline constexpr void Channel::set_source( uint32_t source ) noexcept
{
    mSource = source;
}

inline constexpr uint32_t Channel::target() const noexcept
{
    return mTarget;
}

inline constexpr void Channel::set_target( uint32_t target ) noexcept
{
    mTarget = target;
}

inline constexpr uint32_t Channel::cookie() const noexcept
{
    return mCookie;
}

inline constexpr void Channel::set_cookie( uint32_t cookie ) noexcept
{
    mCookie = cookie;
}

inline constexpr bool Channel::is_valid() const noexcept
{
    return (mCookie != 0u);
}

inline constexpr void Channel::invalidate() noexcept
{
    mSource = 0u;
    mTarget = 0u;
    mCookie = 0u;
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
