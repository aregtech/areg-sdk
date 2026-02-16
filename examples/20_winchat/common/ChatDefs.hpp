#ifndef WINCHAT_COMMON_CHATDEFS_HPP
#define WINCHAT_COMMON_CHATDEFS_HPP
/************************************************************************
 * \file            generated/ChatDefs.hpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/logging/GELog.h"
#include "areg/base/DateTime.hpp"
#include "areg/base/String.hpp"
#include "areg/base/ArrayList.hpp"
#include "areg/base/HashMap.hpp"
#include "areg/base/MemoryDefs.hpp"
#include <tchar.h>

namespace chat
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
    inline chat::sMessageData * newData();

    inline void getWidths( int widthTotal, int columns, int & width1, int & width2 );

    /**
     * \brief   Invalid session ID
     *          chat::InvalidSession
     **/
    const uint64_t  InvalidSession      = 0;
    /**
     * \brief   Invalid cookie
     *          chat::InvalidCookie
     **/
    const uint32_t  InvalidCookie       = 0;
    /**
     * \brief   The maximum length of message
     *          chat::MaxlenNickname
     **/
    const uint32_t  MaxlenNickname      = 64;
    /**
     * \brief   The maximum length of message
     *          chat::MaxlenMessage
     **/
    const uint32_t  MaxlenMessage       = 512;
    /**
     * \brief   Invalid Nick-name
     *          chat::InvalidNickname
     **/
    const String  InvalidNickname     = "";

	/************************************************************************
	 * \brief   chat::sConnection
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
	    inline sConnection();
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
	    inline ~sConnection();

	//////////////////////////////////////////////////////////////////////////
	// Basic operators
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   Copies data from given source
	     * \param   src     The source of sConnection structure to copy data.
	     **/
	    inline const chat::sConnection & operator = ( const chat::sConnection & src );

	    /**
	     * \brief   Checks equality and returns true if 2 instances of sConnection are equal
	     * \param   other   The instance of sConnection to compare
	     * \return  Returns true if 2 instances are equal
	     **/
	    inline bool operator == ( const chat::sConnection & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of sConnection are not equal
         * \param   other   The instance of sConnection to compare
         * \return  Returns true if 2 instances are not equal
         **/
	    inline bool operator != ( const chat::sConnection & other ) const;

	    /**
	     * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
         *          The conversion is a sum of each field of structure
	     **/
	     inline operator size_t () const;

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
	 * \brief	chat::ListConnections
	 *			Internal custom type definition of ArrayList<sConnection>
	 **/
    using ListConnections   = ArrayList<sConnection>;

	/**
	 * \brief	chat::MapConnections
     *			Internal custom type definition of HashMap<uint32_t, sConnection>
     **/
    using MapConnections    = HashMap<uint32_t, sConnection >;

	/************************************************************************
	 * \brief   chat::sParticipant
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
	    inline sParticipant();
	    /**
	     * \brief   Copies data from given source. Every filed of structure should have copy constructor.
	     * \param   src     The source of sParticipant structure to copy data
	     **/
	    inline sParticipant( const sParticipant & src );
	    /**
	     * \brief   Destructor. No extra function is called, every field of structure should be able to be cleaned.
	     **/
	    inline ~sParticipant();

	//////////////////////////////////////////////////////////////////////////
	// Basic operators
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   Copies data from given source
	     * \param   src     The source of sParticipant structure to copy data.
	     **/
	    inline const chat::sParticipant & operator = ( const chat::sParticipant & src );

	    /**
	     * \brief   Checks equality and returns true if 2 instances of sParticipant are equal
	     * \param   other   The instance of sParticipant to compare
	     * \return  Returns true if 2 instances are equal
	     **/
	    inline bool operator == ( const chat::sParticipant & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of sParticipant are not equal
         * \param   other   The instance of sParticipant to compare
         * \return  Returns true if 2 instances are not equal
         **/
	    inline bool operator != ( const chat::sParticipant & other ) const;

	    /**
	     * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
         *          The conversion is a sum of each field of structure
	     **/
	     inline operator size_t () const;

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
	 * \brief	chat::ListConnections
	 *			Internal custom type definition of ArrayList<sConnection>
	 **/
	using ListParticipants  = ArrayList<sParticipant>;

	/**
	 * \brief	chat::MapDirectConnections
	 *			Internal custom type definition of HashMap<String, PairConnection>
	 **/
	using MapParticipants   = HashMap<sInitiator, ListParticipants>;

}

inline void chat::getWidths( int widthTotal, int columns, int & width1, int & width2 )
{
    width1 = width2 = widthTotal / (columns + 0);
    // width2 = (widthTotal - width1) / (columns - 1);
}

/************************************************************************
 * chat::sConnection structure
 ************************************************************************/

/**
 * Every field of structure should have default constructor.
 **/
inline chat::sConnection::sConnection()
    : cookie          ( InvalidCookie )
    , nickName        (  )
    , connectTime     (  )
    , connectedTime   (  )
{
}
/**
 * \brief   Initialize sConnection data by setting parameters.
 **/
inline chat::sConnection::sConnection( const uint32_t & _cookie, const String & _nickName, const DateTime & _connectTime, const DateTime & _connectedTime )
    : cookie          ( _cookie )
    , nickName        ( _nickName )
    , connectTime     ( _connectTime )
    , connectedTime   ( _connectedTime )
{
}
/**
 * Every filed of structure should have copy constructor.
 **/
inline chat::sConnection::sConnection( const chat::sConnection & src )
    : cookie          ( src.cookie )
    , nickName        ( src.nickName )
    , connectTime     ( src.connectTime )
    , connectedTime   ( src.connectedTime )
{
}
/**
 * No extra function is called, every field of structure should be able to be cleaned.
 **/
inline chat::sConnection::~sConnection()
{
}
/**
 * Copies data from given source.
 **/
inline const chat::sConnection & chat::sConnection::operator = ( const chat::sConnection & src )
{
    if ( static_cast<const chat::sConnection *>(this) != &src )
    {
        this->cookie          = src.cookie;
        this->nickName        = src.nickName;
        this->connectTime     = src.connectTime;
        this->connectedTime   = src.connectedTime;
    }
    return (*this);
}
/**
 * Checks equality of 2 instances of chat::sConnection objects.
 **/
inline bool chat::sConnection::operator == ( const chat::sConnection & other ) const
{
    return  ( this == &other ? true : (this->cookie == other.cookie) && (this->nickName == other.nickName) );
}
/**
 * Checks inequality of 2 instances of chat::sConnection objects.
 **/
inline bool chat::sConnection::operator != ( const chat::sConnection & other ) const
{
    return  ( this == &other ? false : (this->cookie != other.cookie) || (this->nickName == other.nickName) );
}
/**
 * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
 *          The conversion is a sum of each field of structure
 **/
 inline chat::sConnection::operator size_t () const
 {
    return  ( static_cast<size_t>( cookie ) + static_cast<unsigned int>( nickName ) );
 }

/**
 * \brief   Streaming operator. Reads and instantiates chat::sConnection structure field entries from stream.
 * \param   stream      The streaming object to read fields data of structure
 * \param   input       The instance of chat::sConnection structure to write data
 * \return  Returns the reference of streaming object.
 **/
inline const InStream & operator >> ( const InStream & stream, chat::sConnection & input )
{
    stream  >> input.cookie;
    stream  >> input.nickName;
    stream  >> input.connectTime;
    stream  >> input.connectedTime;
    return stream;
}
/**
 * \brief   Streaming operator. Writes chat::sConnection structure field entries to stream object.
 * \param   stream      The streaming object to write fields data from structure
 * \param   output      The instance of chat::sConnection structure to read data
 * \return  Returns the reference of streaming object.
 **/
inline OutStream & operator << ( OutStream & stream, const chat::sConnection & output )
{
    stream  << output.cookie;
    stream  << output.nickName;
    stream  << output.connectTime;
    stream  << output.connectedTime;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// Hasher of chat::sConnection object
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the chat::sConnection.
 */
namespace std
{
    template<> struct hash<chat::sConnection>
    {
        //! A function to convert chat::sConnection object to unsigned int.
        inline unsigned int operator()(const chat::sConnection & key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}

/************************************************************************
 * chat::sParticipant structure
 ************************************************************************/

/**
 * Every field of structure should have default constructor.
 **/
inline chat::sParticipant::sParticipant()
    : sessionId   ( InvalidSession )
    , cookie      ( InvalidCookie )
    , nickName    (  )
{      }
/**
 * Every filed of structure should have copy constructor.
 **/
inline chat::sParticipant::sParticipant( const chat::sParticipant & src )
    : sessionId   ( src.sessionId )
    , cookie      ( src.cookie )
    , nickName    ( src.nickName )
{      }
/**
 * No extra function is called, every field of structure should be able to be cleaned.
 **/
inline chat::sParticipant::~sParticipant()
{      }
/**
 * Copies data from given source.
 **/
inline const chat::sParticipant & chat::sParticipant::operator = ( const chat::sParticipant & src )
{
    if ( static_cast<const chat::sParticipant *>(this) != &src )
    {
        this->sessionId   = src.sessionId;
        this->cookie      = src.cookie;
        this->nickName    = src.nickName;
    }
    return (*this);
}
/**
 * Checks equality of 2 instances of chat::sParticipant objects.
 **/
inline bool chat::sParticipant::operator == ( const chat::sParticipant & other ) const
{
    return  (   this == &other ? true :
                    ( this->sessionId   == other.sessionId )
                &&  ( this->cookie      == other.cookie )
                &&  ( this->nickName    == other.nickName )
            );
}
/**
 * Checks inequality of 2 instances of chat::sParticipant objects.
 **/
inline bool chat::sParticipant::operator != ( const chat::sParticipant & other ) const
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
 inline chat::sParticipant::operator size_t () const
 {
    return  ( static_cast<size_t>( sessionId ) + static_cast<size_t>( cookie ) + static_cast<unsigned int>( nickName ) );
 }
/**
 * \brief   Streaming operator. Reads and instantiates chat::sParticipant structure field entries from stream.
 * \param   stream      The streaming object to read fields data of structure
 * \param   input       The instance of chat::sParticipant structure to write data
 * \return  Returns the reference of streaming object.
 **/
inline const InStream & operator >> ( const InStream & stream, chat::sParticipant & input )
{
    stream  >> input.sessionId;
    stream  >> input.cookie;
    stream  >> input.nickName;
    return stream;
}
/**
 * \brief   Streaming operator. Writes chat::sParticipant structure field entries to stream object.
 * \param   stream      The streaming object to write fields data from structure
 * \param   output      The instance of chat::sParticipant structure to read data
 * \return  Returns the reference of streaming object.
 **/
inline OutStream & operator << ( OutStream & stream, const chat::sParticipant & output )
{
    stream  << output.sessionId;
    stream  << output.cookie;
    stream  << output.nickName;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// Hasher of chat::sParticipant object
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the chat::sParticipant.
 */
namespace std
{
    template<> struct hash<chat::sParticipant>
    {
        //! A function to convert chat::sParticipant object to unsigned int.
        inline unsigned int operator()(const chat::sParticipant& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}


inline chat::sMessageData * chat::newData( )
{
    chat::sMessageData * result = DEBUG_NEW chat::sMessageData;
    if ( result != nullptr )
    {
        NEMemory::memZero(reinterpret_cast<void *>(result), sizeof( chat::sMessageData ));
    }

    return result;
}
#endif // !WINCHAT_COMMON_CHATDEFS_HPP
