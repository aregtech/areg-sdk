#ifndef AREG_COMPONENT_CECHANNEL_HPP
#define AREG_COMPONENT_CECHANNEL_HPP
/************************************************************************
 * \file        areg/component/CEChannel.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
// CEChannel class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Channel class, which provides connection of services.
 *          Every connection, independent whether it is local or remote,
 *          has a communication channel. Each registered service gets associated
 *          channel given by system and contains information of communication source and target.
 **/
class AREG_API CEChannel
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
private:
    static const char   _CHANNEL_SEPARATOR  /*= '.'*/;  //!< Channel data separator symbol

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Default constructor
     **/
    CEChannel( void );

    /**
     * \brief   Constructor. Sets channel data, which are the communication source and target IDs, and the cookie set by system.
     * \param   source  The channel communication source ID set by system.
     * \param   target  The channel communication target ID set by system.
     * \param   cookie  The ID assigned by system.
     **/
    CEChannel( ITEM_ID source, ITEM_ID target = NEService::TARGET_UNKNOWN, ITEM_ID cookie = NEService::COOKIE_UNKNOWN );

    /**
     * \brief   Copy constructor.
     * \param   source  The source of data to copy.
     **/
    CEChannel( const CEChannel & source );

    /**
     * \brief   Destructor
     **/
    ~CEChannel( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies channel data from given source
     * \param   source  The source of data to copy.
     **/
    const CEChannel & operator = ( const CEChannel & source );

    /**
     * \brief   Checks equality of 2 channels and returns true if they are equal.
     *          Channels are equal if source, target and cookies are equal.
     * \param   other   The Channel object to compare
     **/
    inline bool operator == ( const CEChannel & other ) const;

    /**
     * \brief   Checks inequality of 2 channels and returns true if they are not equal.
     *          Channels are not equal if source, target or cookies are not equal.
     * \param   other   The Channel object to compare
     **/
    inline bool operator != ( const CEChannel & other ) const;

    /**
     * \brief   Converts channel object to 32-bit unsigned integer value.
     **/
    inline operator ITEM_ID ( void ) const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/

    /**
     * \brief   Streaming operator. Reads from stream and initialize connection channel data.
     * \param   stream  The streaming object to read data.
     * \param   input   Connection channel to initialize.
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEChannel & input );

    /**
     * \brief   Streaming operator. Writes connection channel data into stream.
     * \param   stream  The streaming object to write data.
     * \param   output  Connection channel to stream.
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEChannel & output);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the source ID of channel.
     **/
    inline ITEM_ID GetSource( void ) const;

    /**
     * \brief   Sets the source ID of channel.
     * \param   source  The new source ID to set in channel
     **/
    inline void SetSource( ITEM_ID source );

    /**
     * \brief   Returns the target ID of channel.
     **/
    inline ITEM_ID GetTarget( void ) const;
    /**
     * \brief   Sets the source ID of channel.
     * \param   target  The new target ID to set in channel
     **/

    inline void SetTarget( ITEM_ID target );

    /**
     * \brief   Returns the cookie ID of channel.
     **/
    inline ITEM_ID GetCookie( void ) const;

    /**
     * \brief   Sets the source ID of channel.
     * \param   cookie  The new cookie ID to set in channel
     **/
    inline void SetCookie( ITEM_ID cookie );

    /**
     * \brief   Returns true, if channel data is valid.
     **/
    inline bool IsValid( void ) const;

    /**
     * \brief   Invalidates channel.
     **/
    inline void Invalidate( void );

public:
    /**
     * \brief   Converts channel data to string
     **/
    CEString ToString( void ) const;

    /**
     * \brief   Creates channel data from string
     * \param   channel     Null-terminated string, which contains channel data.
     **/
    const CEChannel & FromString( const char * channel );

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
// CEChannel class inline methods.
//////////////////////////////////////////////////////////////////////////

inline ITEM_ID CEChannel::GetSource(void) const
{   return mSource;                                                                                                         }

inline void CEChannel::SetSource(ITEM_ID source)
{   mSource = source;                                                                                                       }

inline ITEM_ID CEChannel::GetTarget(void) const
{   return mTarget;                                                                                                         }

inline void CEChannel::SetTarget(ITEM_ID target)
{   mTarget = target;                                                                                                       }

inline ITEM_ID CEChannel::GetCookie(void) const
{   return mCookie;                                                                                                         }

inline void CEChannel::SetCookie(ITEM_ID cookie)
{   mCookie = cookie;                                                                                                       }

inline bool CEChannel::operator == ( const CEChannel & other ) const
{   return ( this != & other ? mCookie == other.mCookie && mTarget == other.mTarget && mSource == other.mSource : true );   }

inline bool CEChannel::operator!=(const CEChannel & other) const
{   return ( this != & other ? mCookie != other.mCookie || mTarget != other.mTarget || mSource != other.mSource : false );  }

inline CEChannel::operator ITEM_ID ( void ) const
{   return mSource;                                                                                                         }

inline bool CEChannel::IsValid( void ) const
{   return (mCookie != NEService::COOKIE_UNKNOWN);                                                                          }

inline void CEChannel::Invalidate( void )
{   mSource = NEService::SOURCE_UNKNOWN; mTarget = NEService::TARGET_UNKNOWN; mCookie = NEService::COOKIE_UNKNOWN;          }

#endif  // AREG_COMPONENT_CECHANNEL_HPP
