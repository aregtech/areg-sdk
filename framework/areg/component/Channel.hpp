#ifndef AREG_COMPONENT_CHANNEL_HPP
#define AREG_COMPONENT_CHANNEL_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/Channel.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, channel object. Responsible to deliver
 *              events to target. The Target object is either in the same
 *              process or in the remote PC.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/NEService.hpp"

//////////////////////////////////////////////////////////////////////////
// Channel class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Channel class, which provides connection of services.
 *          Every connection, independent whether it is local or remote,
 *          has a communication channel. Each registered service gets associated
 *          channel given by system and contains information of communication source and target.
 **/
class AREG_API Channel
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Channel::INVALID_CHANNEL
     *          Defines invalid channel object.
     **/
    static const Channel    INVALID_CHANNEL;                //!< Invalid channel

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Default constructor
     **/
    Channel( void );

    /**
     * \brief   Constructor. Sets channel data, which are the communication source and target IDs, and the cookie set by system.
     * \param   source  The channel communication source ID set by system.
     * \param   target  The channel communication target ID set by system.
     * \param   cookie  The ID assigned by system.
     **/
    explicit Channel( ITEM_ID source, ITEM_ID target = NEService::TARGET_UNKNOWN, ITEM_ID cookie = NEService::COOKIE_UNKNOWN );

    /**
     * \brief   Copy constructor.
     * \param   source  The source of data to copy.
     **/
    Channel( const Channel & source );

    /**
     * \brief   Move constructor.
     * \param   source  The source of data to move.
     **/
    Channel( Channel && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~Channel( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies channel data from given source
     * \param   source  The source of data to copy.
     **/
    Channel & operator = ( const Channel & source );

    /**
     * \brief   Moves channel data from given source
     * \param   source  The source of data to move.
     **/
    Channel & operator = ( Channel && source ) noexcept;

    /**
     * \brief   Checks equality of 2 channels and returns true if they are equal.
     *          Channels are equal if source, target and cookies are equal.
     * \param   other   The Channel object to compare
     **/
    inline bool operator == ( const Channel & other ) const;

    /**
     * \brief   Checks inequality of 2 channels and returns true if they are not equal.
     *          Channels are not equal if source, target or cookies are not equal.
     * \param   other   The Channel object to compare
     **/
    inline bool operator != ( const Channel & other ) const;

    /**
     * \brief   Converts channel object to 32-bit unsigned integer value.
     **/
    inline explicit operator ITEM_ID ( void ) const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/

    /**
     * \brief   Streaming operator. Reads from stream and initialize connection channel data.
     * \param   stream  The streaming object to read data.
     * \param   input   Connection channel to initialize.
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, Channel & input );

    /**
     * \brief   Streaming operator. Writes connection channel data into stream.
     * \param   stream  The streaming object to write data.
     * \param   output  Connection channel to stream.
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const Channel & output);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the source ID of channel.
     **/
    inline ITEM_ID getSource( void ) const;

    /**
     * \brief   Sets the source ID of channel.
     * \param   source  The new source ID to set in channel
     **/
    inline void setSource( ITEM_ID source );

    /**
     * \brief   Returns the target ID of channel.
     **/
    inline ITEM_ID getTarget( void ) const;
    /**
     * \brief   Sets the source ID of channel.
     * \param   target  The new target ID to set in channel
     **/

    inline void setTarget( ITEM_ID target );

    /**
     * \brief   Returns the cookie ID of channel.
     **/
    inline ITEM_ID getCookie( void ) const;

    /**
     * \brief   Sets the source ID of channel.
     * \param   cookie  The new cookie ID to set in channel
     **/
    inline void setCookie( ITEM_ID cookie );

    /**
     * \brief   Returns true, if channel data is valid.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Invalidates channel.
     **/
    inline void invalidate( void );

public:
    /**
     * \brief   Converts channel data to string
     **/
    String convToString( void ) const;

    /**
     * \brief   Creates channel data from string
     * \param   channel     Null-terminated string, which contains channel data.
     **/
    const Channel & convFromString( const char * channel );


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

inline ITEM_ID Channel::getSource(void) const
{
    return mSource;
}

inline void Channel::setSource(ITEM_ID source)
{
    mSource = source;
}

inline ITEM_ID Channel::getTarget(void) const
{
    return mTarget;
}

inline void Channel::setTarget(ITEM_ID target)
{
    mTarget = target;
}

inline ITEM_ID Channel::getCookie(void) const
{
    return mCookie;
}

inline void Channel::setCookie(ITEM_ID cookie)
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

inline Channel::operator ITEM_ID ( void ) const
{
    return mSource;
}

inline bool Channel::isValid( void ) const
{
    return (mCookie != NEService::COOKIE_UNKNOWN);
}

inline void Channel::invalidate( void )
{
    mSource = NEService::SOURCE_UNKNOWN;
    mTarget = NEService::TARGET_UNKNOWN;
    mCookie = NEService::COOKIE_UNKNOWN;
}

#endif  // AREG_COMPONENT_CHANNEL_HPP
