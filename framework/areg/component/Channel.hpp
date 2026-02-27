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
#include "areg/base/GEGlobal.h"

#include "areg/component/ServiceDefs.hpp"
#include "areg/base/IOStream.hpp"

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

    Channel();

    /**
     * \brief   Initializes the channel with source and target identifiers and an optional cookie.
     *
     * \param   source      The channel communication source ID.
     * \param   target      The channel communication target ID.
     * \param   cookie      The system-assigned ID.
     **/
    explicit Channel( const ITEM_ID & source, const ITEM_ID & target = NEService::TARGET_UNKNOWN, const ITEM_ID & cookie = NEService::COOKIE_UNKNOWN );

    /**
     * \brief   Copies values from the given source.
     *
     * \param   source      The source of data to copy.
     **/
    Channel( const Channel & source );

    /**
     * \brief   Moves values from the given source.
     *
     * \param   source      The source of data to move.
     **/
    Channel( Channel && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~Channel() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies channel data from the given source.
     *
     * \param   source      The source of data to copy.
     **/
    Channel & operator = ( const Channel & source );

    /**
     * \brief   Moves channel data from the given source.
     *
     * \param   source      The source of data to move.
     **/
    Channel & operator = ( Channel && source ) noexcept;

    /**
     * \brief   Returns true if both channels have identical source, target, and cookie identifiers.
     *
     * \param   other       The channel object to compare.
     **/
    inline bool operator == ( const Channel & other ) const;

    /**
     * \brief   Returns true if the channels differ in source, target, or cookie identifiers.
     *
     * \param   other       The channel object to compare.
     **/
    inline bool operator != ( const Channel & other ) const;

    /**
     * \brief   Converts the channel to its ITEM_ID representation.
     **/
    inline explicit operator const ITEM_ID & () const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/

    /**
     * \brief   Reads channel data from stream.
     *
     * \param   stream      The stream object to read from.
     * \param[out] input       The channel to initialize with data from the stream.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, Channel & input );

    /**
     * \brief   Writes channel data to stream.
     *
     * \param[out] stream      The stream object to write to.
     * \param   output      The channel to write.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const Channel & output);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the source identifier of the channel.
     **/
    inline const ITEM_ID & source() const;

    /**
     * \brief   Sets the source identifier of the channel.
     *
     * \param   source      The new source identifier.
     **/
    inline void set_source(const ITEM_ID & source );

    /**
     * \brief   Returns the target identifier of the channel.
     **/
    inline const ITEM_ID & target() const;
    /**
     * \brief   Sets the target identifier of the channel.
     *
     * \param   target      The new target identifier.
     **/

    inline void set_target(const ITEM_ID & target );

    /**
     * \brief   Returns the cookie identifier of the channel.
     **/
    inline const ITEM_ID & cookie() const;

    /**
     * \brief   Sets the cookie identifier of the channel.
     *
     * \param   cookie      The new cookie identifier.
     **/
    inline void set_cookie(const ITEM_ID & cookie );

    /**
     * \brief   Returns true if the channel data is valid.
     **/
    inline bool is_valid() const;

    /**
     * \brief   Invalidates the channel.
     **/
    inline void invalidate();

public:
    /**
     * \brief   Converts the channel data to a string representation.
     **/
    String to_string() const;

    /**
     * \brief   Initializes the channel from a string representation.
     *
     * \param   channel     The string containing channel data.
     **/
    const Channel & conv_from_string( const String & channel );


//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Channel source ID
     **/
    ITEM_ID     mSource;
    /**
     * \brief   Channel Target ID
     **/
    ITEM_ID     mTarget;
    /**
     * \brief   Channel Cookie assigned by system
     **/
    ITEM_ID     mCookie;
};

//////////////////////////////////////////////////////////////////////////
// Channel class inline methods.
//////////////////////////////////////////////////////////////////////////

inline const ITEM_ID & Channel::source() const
{
    return mSource;
}

inline void Channel::set_source(const ITEM_ID & source)
{
    mSource = source;
}

inline const ITEM_ID & Channel::target() const
{
    return mTarget;
}

inline void Channel::set_target(const ITEM_ID & target)
{
    mTarget = target;
}

inline const ITEM_ID & Channel::cookie() const
{
    return mCookie;
}

inline void Channel::set_cookie(const ITEM_ID & cookie)
{
    mCookie = cookie;
}

inline bool Channel::operator == ( const Channel & other ) const
{
    return ( this != & other ? (mCookie == other.mCookie) && (mTarget == other.mTarget) && (mSource == other.mSource) : true );
}

inline bool Channel::operator!=(const Channel & other) const
{
    return ( this != & other ? (mCookie != other.mCookie) || (mTarget != other.mTarget) || (mSource != other.mSource) : false );
}

inline Channel::operator const ITEM_ID & () const
{
    return mSource;
}

inline bool Channel::is_valid() const
{
    return (mCookie != NEService::COOKIE_UNKNOWN);
}

inline void Channel::invalidate()
{
    mSource = NEService::SOURCE_UNKNOWN;
    mTarget = NEService::TARGET_UNKNOWN;
    mCookie = NEService::COOKIE_UNKNOWN;
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

#endif  // AREG_COMPONENT_CHANNEL_HPP
