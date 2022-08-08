#pragma once

/************************************************************************
 * \file            generated/NECommon.hpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/trace/GETrace.h"
#include "areg/base/DateTime.hpp"
#include "areg/base/String.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/TEHashMap.hpp"
#include "areg/base/NEMemory.hpp"
#include <tchar.h>

namespace NECommon
{
    const char * const  MODEL_NAME_DISTRIBUTED_CLIENT       = "ModelDistributedClient";
    const char * const  MODEL_NAME_CENTRAL_SERVER           = "ModelCentralServer";

    const char * const  COMP_NAME_CENTRAL_SERVER            = "CompCentralServer";
    const char * const  COMP_NAME_DISTRIBUTED_CLIENT        = "CompDistributedClient";

    const TCHAR* const  SERVER_NAME                         = _T( "<Register>" );

    const char * const  DAY_FORMAT                          = "YYYY-MM-DD ";
    const int           DAY_FORMAT_LEN                      = 11;

    typedef struct S_ChatParticipant
    {
        String      nickName;
        uint32_t    cookie;

    } sChatParticipant;

    inline bool operator == (const sChatParticipant & lsh, const sChatParticipant & rsh)
    {
        return (lsh.cookie == rsh.cookie ? lsh.nickName == rsh.nickName : false);
    }

    const   int     MAXLEN_NICKNAME = 64;
    const   int     MAXLEN_MESSAGE  = 512;

    typedef struct S_MessageData
    {
        TCHAR       nickName[MAXLEN_NICKNAME + 1];
        TCHAR       message[ MAXLEN_MESSAGE  + 1];
        uint64_t    dataSave;
        uint64_t    timeSend;
        uint64_t    timeReceived;
    } sMessageData;

    /**
     * \brief creates new message data.
     **/
    inline NECommon::sMessageData * newData();

    inline void getWidths( int widthTotal, int columns, int & width1, int & width2 );

    /**
     * \brief   Invalid session ID
     *          NECommon::InvalidSession
     **/
    const uint64_t  InvalidSession      = 0;
    /**
     * \brief   Invalid cookie
     *          NECommon::InvalidCookie
     **/
    const uint32_t  InvalidCookie       = 0;
    /**
     * \brief   The maximum length of message
     *          NECommon::MaxlenNickname
     **/
    const uint32_t  MaxlenNickname      = 64;
    /**
     * \brief   The maximum length of message
     *          NECommon::MaxlenMessage
     **/
    const uint32_t  MaxlenMessage       = 512;
    /**
     * \brief   Invalid Nick-name
     *          NECommon::InvalidNickname
     **/
    const String  InvalidNickname     = "";

	/************************************************************************
	 * \brief   NECommon::sConnection
	 *          Connection data
	 ************************************************************************/
	struct sConnection
	{

	//////////////////////////////////////////////////////////////////////////
	// Constructor/ destructor
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   Every field of structure should have default constructor.
	     **/
	    inline sConnection( void );
        /**
         * \brief   Initialize sConnection data by setting parameters.
         **/
        inline sConnection( const uint32_t & _cookie, const String & _nickName, const DateTime & _connectTime, const DateTime & _connectedTime );
	    /**
	     * \brief   Copies data from given source. Every filed of structure should have copy constructor.
	     * \param   src     The source of sConnection structure to copy data
	     **/
	    inline sConnection( const sConnection & src );
	    /**
	     * \brief   Destructor. No extra function is called, every field of structure should be able to be cleaned.
	     **/
	    inline ~sConnection( void );

	//////////////////////////////////////////////////////////////////////////
	// Basic operators
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   Copies data from given source
	     * \param   src     The source of sConnection structure to copy data.
	     **/
	    inline const NECommon::sConnection & operator = ( const NECommon::sConnection & src );

	    /**
	     * \brief   Checks equality and returns true if 2 instances of sConnection are equal
	     * \param   other   The instance of sConnection to compare
	     * \return  Returns true if 2 instances are equal
	     **/
	    inline bool operator == ( const NECommon::sConnection & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of sConnection are not equal
         * \param   other   The instance of sConnection to compare
         * \return  Returns true if 2 instances are not equal
         **/
	    inline bool operator != ( const NECommon::sConnection & other ) const;

	    /**
	     * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
         *          The conversion is a sum of each field of structure
	     **/
	     inline operator size_t ( void ) const;

	//////////////////////////////////////////////////////////////////////////
	// sConnection fields
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   The cookie set by server application
         *          Default value is InvalidCookie
	     **/
	    uint32_t    cookie;

	    /**
	     * \brief   Connection client nick name, should be unique
	     **/
	    String    nickName;

	    /**
	     * \brief   Connection request date-time
	     **/
	    DateTime  connectTime;

	    /**
	     * \brief   Connection registered / accepted date-time
	     **/
	    DateTime  connectedTime;

	};

	/**
	 * \brief	NECommon::ListConnections
	 *			Internal custom type definition of TEArrayList<sConnection>
	 **/
    using ListConnections   = TEArrayList<sConnection>;

	/**
	 * \brief	NECommon::MapConnections
     *			Internal custom type definition of TEHashMap<uint32_t, sConnection>
     **/
    using MapConnections    = TEHashMap<uint32_t, sConnection >;

	/************************************************************************
	 * \brief   NECommon::sParticipant
	 *          DESCRIPTION MISSED
	 ************************************************************************/
	struct sParticipant
	{

	//////////////////////////////////////////////////////////////////////////
	// Constructor/ destructor
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   Every field of structure should have default constructor.
	     **/
	    inline sParticipant( void );
	    /**
	     * \brief   Copies data from given source. Every filed of structure should have copy constructor.
	     * \param   src     The source of sParticipant structure to copy data
	     **/
	    inline sParticipant( const sParticipant & src );
	    /**
	     * \brief   Destructor. No extra function is called, every field of structure should be able to be cleaned.
	     **/
	    inline ~sParticipant( void );

	//////////////////////////////////////////////////////////////////////////
	// Basic operators
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   Copies data from given source
	     * \param   src     The source of sParticipant structure to copy data.
	     **/
	    inline const NECommon::sParticipant & operator = ( const NECommon::sParticipant & src );

	    /**
	     * \brief   Checks equality and returns true if 2 instances of sParticipant are equal
	     * \param   other   The instance of sParticipant to compare
	     * \return  Returns true if 2 instances are equal
	     **/
	    inline bool operator == ( const NECommon::sParticipant & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of sParticipant are not equal
         * \param   other   The instance of sParticipant to compare
         * \return  Returns true if 2 instances are not equal
         **/
	    inline bool operator != ( const NECommon::sParticipant & other ) const;

	    /**
	     * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
         *          The conversion is a sum of each field of structure
	     **/
	     inline operator size_t ( void ) const;

	//////////////////////////////////////////////////////////////////////////
	// sParticipant fields
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   DESCRIPTION MISSED
         *          Default value is InvalidSession
	     **/
	    uint64_t    sessionId;

	    /**
	     * \brief   DESCRIPTION MISSED
         *          Default value is InvalidCookie
	     **/
	    uint32_t    cookie;

	    /**
	     * \brief   DESCRIPTION MISSED
	     **/
	    String    nickName;

	};

    using sInitiator        = sParticipant;

	/**
	 * \brief	NECommon::ListConnections
	 *			Internal custom type definition of TEArrayList<sConnection>
	 **/
	using ListParticipants  = TEArrayList<sParticipant>;

	/**
	 * \brief	NECommon::MapDirectConnections
	 *			Internal custom type definition of TEHashMap<String, PairConnection>
	 **/
	using MapParticipants   = TEHashMap<sInitiator, ListParticipants>;

}

inline void NECommon::getWidths( int widthTotal, int columns, int & width1, int & width2 )
{
    width1 = width2 = widthTotal / (columns + 0);
    // width2 = (widthTotal - width1) / (columns - 1);
}

/************************************************************************
 * NECommon::sConnection structure
 ************************************************************************/

/**
 * Every field of structure should have default constructor.
 **/
inline NECommon::sConnection::sConnection( void )
    : cookie          ( InvalidCookie )
    , nickName        (  )
    , connectTime     (  )
    , connectedTime   (  )
{
}
/**
 * \brief   Initialize sConnection data by setting parameters.
 **/
inline NECommon::sConnection::sConnection( const uint32_t & _cookie, const String & _nickName, const DateTime & _connectTime, const DateTime & _connectedTime )
    : cookie          ( _cookie )
    , nickName        ( _nickName )
    , connectTime     ( _connectTime )
    , connectedTime   ( _connectedTime )
{
}
/**
 * Every filed of structure should have copy constructor.
 **/
inline NECommon::sConnection::sConnection( const NECommon::sConnection & src )
    : cookie          ( src.cookie )
    , nickName        ( src.nickName )
    , connectTime     ( src.connectTime )
    , connectedTime   ( src.connectedTime )
{
}
/**
 * No extra function is called, every field of structure should be able to be cleaned.
 **/
inline NECommon::sConnection::~sConnection( void )
{
}
/**
 * Copies data from given source.
 **/
inline const NECommon::sConnection & NECommon::sConnection::operator = ( const NECommon::sConnection & src )
{
    if ( static_cast<const NECommon::sConnection *>(this) != &src )
    {
        this->cookie          = src.cookie;
        this->nickName        = src.nickName;
        this->connectTime     = src.connectTime;
        this->connectedTime   = src.connectedTime;
    }
    return (*this);
}
/**
 * Checks equality of 2 instances of NECommon::sConnection objects.
 **/
inline bool NECommon::sConnection::operator == ( const NECommon::sConnection & other ) const
{
    return  ( this == &other ? true : (this->cookie == other.cookie) && (this->nickName == other.nickName) );
}
/**
 * Checks inequality of 2 instances of NECommon::sConnection objects.
 **/
inline bool NECommon::sConnection::operator != ( const NECommon::sConnection & other ) const
{
    return  ( this == &other ? false : (this->cookie != other.cookie) || (this->nickName == other.nickName) );
}
/**
 * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
 *          The conversion is a sum of each field of structure
 **/
 inline NECommon::sConnection::operator size_t ( void ) const
 {
    return  ( static_cast<size_t>( cookie ) + static_cast<unsigned int>( nickName ) );
 }

/**
 * \brief   Streaming operator. Reads and instantiates NECommon::sConnection structure field entries from stream.
 * \param   stream      The streaming object to read fields data of structure
 * \param   input       The instance of NECommon::sConnection structure to write data
 * \return  Returns the reference of streaming object.
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NECommon::sConnection & input )
{
    stream  >> input.cookie;
    stream  >> input.nickName;
    stream  >> input.connectTime;
    stream  >> input.connectedTime;
    return stream;
}
/**
 * \brief   Streaming operator. Writes NECommon::sConnection structure field entries to stream object.
 * \param   stream      The streaming object to write fields data from structure
 * \param   output      The instance of NECommon::sConnection structure to read data
 * \return  Returns the reference of streaming object.
 **/
inline IEOutStream & operator << ( IEOutStream & stream, const NECommon::sConnection & output )
{
    stream  << output.cookie;
    stream  << output.nickName;
    stream  << output.connectTime;
    stream  << output.connectedTime;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// Hasher of NECommon::sConnection object
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the NECommon::sConnection.
 */
namespace std
{
    template<> struct hash<NECommon::sConnection>
    {
        //! A function to convert NECommon::sConnection object to unsigned int.
        inline unsigned int operator()(const NECommon::sConnection & key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}

/************************************************************************
 * NECommon::sParticipant structure
 ************************************************************************/

/**
 * Every field of structure should have default constructor.
 **/
inline NECommon::sParticipant::sParticipant( void )
    : sessionId   ( InvalidSession )
    , cookie      ( InvalidCookie )
    , nickName    (  )
{   ;   }
/**
 * Every filed of structure should have copy constructor.
 **/
inline NECommon::sParticipant::sParticipant( const NECommon::sParticipant & src )
    : sessionId   ( src.sessionId )
    , cookie      ( src.cookie )
    , nickName    ( src.nickName )
{   ;   }
/**
 * No extra function is called, every field of structure should be able to be cleaned.
 **/
inline NECommon::sParticipant::~sParticipant( void )
{   ;   }
/**
 * Copies data from given source.
 **/
inline const NECommon::sParticipant & NECommon::sParticipant::operator = ( const NECommon::sParticipant & src )
{
    if ( static_cast<const NECommon::sParticipant *>(this) != &src )
    {
        this->sessionId   = src.sessionId;
        this->cookie      = src.cookie;
        this->nickName    = src.nickName;
    }
    return (*this);
}
/**
 * Checks equality of 2 instances of NECommon::sParticipant objects.
 **/
inline bool NECommon::sParticipant::operator == ( const NECommon::sParticipant & other ) const
{
    return  (   this == &other ? true :
                    ( this->sessionId   == other.sessionId )
                &&  ( this->cookie      == other.cookie )
                &&  ( this->nickName    == other.nickName )
            );
}
/**
 * Checks inequality of 2 instances of NECommon::sParticipant objects.
 **/
inline bool NECommon::sParticipant::operator != ( const NECommon::sParticipant & other ) const
{
    return  (   this == &other ? false :
                    ( this->sessionId   != other.sessionId )
                ||  ( this->cookie      != other.cookie )
                ||  ( this->nickName    != other.nickName )
            );
}
/**
 * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
 *          The conversion is a sum of each field of structure
 **/
 inline NECommon::sParticipant::operator size_t ( void ) const
 {
    return  ( static_cast<size_t>( sessionId ) + static_cast<size_t>( cookie ) + static_cast<unsigned int>( nickName ) );
 }
/**
 * \brief   Streaming operator. Reads and instantiates NECommon::sParticipant structure field entries from stream.
 * \param   stream      The streaming object to read fields data of structure
 * \param   input       The instance of NECommon::sParticipant structure to write data
 * \return  Returns the reference of streaming object.
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NECommon::sParticipant & input )
{
    stream  >> input.sessionId;
    stream  >> input.cookie;
    stream  >> input.nickName;
    return stream;
}
/**
 * \brief   Streaming operator. Writes NECommon::sParticipant structure field entries to stream object.
 * \param   stream      The streaming object to write fields data from structure
 * \param   output      The instance of NECommon::sParticipant structure to read data
 * \return  Returns the reference of streaming object.
 **/
inline IEOutStream & operator << ( IEOutStream & stream, const NECommon::sParticipant & output )
{
    stream  << output.sessionId;
    stream  << output.cookie;
    stream  << output.nickName;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// Hasher of NECommon::sParticipant object
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the NECommon::sParticipant.
 */
namespace std
{
    template<> struct hash<NECommon::sParticipant>
    {
        //! A function to convert NECommon::sParticipant object to unsigned int.
        inline unsigned int operator()(const NECommon::sParticipant& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}


inline NECommon::sMessageData * NECommon::newData( )
{
    NECommon::sMessageData * result = DEBUG_NEW NECommon::sMessageData;
    if ( result != nullptr )
    {
        NEMemory::memZero(reinterpret_cast<void *>(result), sizeof( NECommon::sMessageData ));
    }

    return result;
}
