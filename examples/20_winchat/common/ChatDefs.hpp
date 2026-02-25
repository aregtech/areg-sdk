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
    const int32_t           DAY_FORMAT_LEN                      = 11;

    struct ChatParticipant
    {
        areg::String      nickName;
        uint32_t    cookie;

    };

    inline bool operator == (const ChatParticipant & lsh, const ChatParticipant & rsh)
    {
        return (lsh.cookie == rsh.cookie ? lsh.nickName == rsh.nickName : false);
    }

    const   int32_t     MAXLEN_NICKNAME = 64;
    const   int32_t     MAXLEN_MESSAGE  = 512;

    struct MessageData
    {
        TCHAR       nickName[MAXLEN_NICKNAME + 1];
        TCHAR       message[ MAXLEN_MESSAGE  + 1];
        uint64_t    dataSave;
        uint64_t    timeSend;
        uint64_t    timeReceived;
    } ;

    /**
     * \brief creates new message data.
     **/
    inline chat:: MessageData * newData();

    inline void getWidths( int32_t widthTotal, int32_t columns, int32_t & width1, int32_t & width2 );

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
    const areg::String  InvalidNickname     = "";

	/************************************************************************
	 * \brief   chat::ConnectionRecord
	 *          Connection data
	 ************************************************************************/
	struct ConnectionRecord
	{

	//////////////////////////////////////////////////////////////////////////
	// Constructor/ destructor
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   Every field of structure should have default constructor.
	     **/
	    inline ConnectionRecord();
        /**
         * \brief   Initialize ConnectionRecord data by setting parameters.
         **/
        inline ConnectionRecord( const uint32_t & _cookie, const areg::String & _nickName, const areg::DateTime & _connectTime, const areg::DateTime & _connectedTime );
	    /**
	     * \brief   Copies data from given source. Every filed of structure should have copy constructor.
	     * \param   src     The source of ConnectionRecord structure to copy data
	     **/
	    inline ConnectionRecord( const ConnectionRecord & src );
	    /**
	     * \brief   Destructor. No extra function is called, every field of structure should be able to be cleaned.
	     **/
	    inline ~ConnectionRecord();

	//////////////////////////////////////////////////////////////////////////
	// Basic operators
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   Copies data from given source
	     * \param   src     The source of ConnectionRecord structure to copy data.
	     **/
	    inline const chat::ConnectionRecord & operator = ( const chat::ConnectionRecord & src );

	    /**
	     * \brief   Checks equality and returns true if 2 instances of ConnectionRecord are equal
	     * \param   other   The instance of ConnectionRecord to compare
	     * \return  Returns true if 2 instances are equal
	     **/
	    inline bool operator == ( const chat::ConnectionRecord & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of ConnectionRecord are not equal
         * \param   other   The instance of ConnectionRecord to compare
         * \return  Returns true if 2 instances are not equal
         **/
	    inline bool operator != ( const chat::ConnectionRecord & other ) const;

	    /**
	     * \brief   Converts data to uint32_t, which might be used as a hash key value in map object.
         *          The conversion is a sum of each field of structure
	     **/
	     inline operator size_t () const;

	//////////////////////////////////////////////////////////////////////////
	// ConnectionRecord fields
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   The cookie set by server application
         *          Default value is InvalidCookie
	     **/
	    uint32_t    cookie;

	    /**
	     * \brief   Connection client nick name, should be unique
	     **/
	    areg::String    nickName;

	    /**
	     * \brief   Connection request date-time
	     **/
	    areg::DateTime  connectTime;

	    /**
	     * \brief   Connection registered / accepted date-time
	     **/
	    areg::DateTime  connectedTime;

	};

	/**
	 * \brief	chat::ListConnections
	 *			Internal custom type definition of ArrayList<ConnectionRecord>
	 **/
    using ListConnections   = areg::ArrayList<ConnectionRecord>;

	/**
	 * \brief	chat::MapConnections
     *			Internal custom type definition of HashMap<uint32_t, ConnectionRecord>
     **/
    using MapConnections    = areg::HashMap<uint32_t, ConnectionRecord >;

	/************************************************************************
	 * \brief   chat::Participant
	 *          DESCRIPTION MISSED
	 ************************************************************************/
	struct Participant
	{

	//////////////////////////////////////////////////////////////////////////
	// Constructor/ destructor
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   Every field of structure should have default constructor.
	     **/
	    inline Participant();
	    /**
	     * \brief   Copies data from given source. Every filed of structure should have copy constructor.
	     * \param   src     The source of Participant structure to copy data
	     **/
	    inline Participant( const Participant & src );
	    /**
	     * \brief   Destructor. No extra function is called, every field of structure should be able to be cleaned.
	     **/
	    inline ~Participant();

	//////////////////////////////////////////////////////////////////////////
	// Basic operators
	///////////////////////////////////////////////////////////////////////////

	    /**
	     * \brief   Copies data from given source
	     * \param   src     The source of Participant structure to copy data.
	     **/
	    inline const chat::Participant & operator = ( const chat::Participant & src );

	    /**
	     * \brief   Checks equality and returns true if 2 instances of Participant are equal
	     * \param   other   The instance of Participant to compare
	     * \return  Returns true if 2 instances are equal
	     **/
	    inline bool operator == ( const chat::Participant & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of Participant are not equal
         * \param   other   The instance of Participant to compare
         * \return  Returns true if 2 instances are not equal
         **/
	    inline bool operator != ( const chat::Participant & other ) const;

	    /**
	     * \brief   Converts data to uint32_t, which might be used as a hash key value in map object.
         *          The conversion is a sum of each field of structure
	     **/
	     inline operator size_t () const;

	//////////////////////////////////////////////////////////////////////////
	// Participant fields
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
	    areg::String    nickName;

	};

    using sInitiator        = Participant;

	/**
	 * \brief	chat::ListConnections
	 *			Internal custom type definition of ArrayList<ConnectionRecord>
	 **/
	using ListParticipants  = areg::ArrayList<Participant>;

	/**
	 * \brief	chat::MapDirectConnections
	 *			Internal custom type definition of HashMap<String, PairConnection>
	 **/
	using MapParticipants   = areg::HashMap<sInitiator, ListParticipants>;

}

inline void chat::getWidths( int32_t widthTotal, int32_t columns, int32_t & width1, int32_t & width2 )
{
    width1 = width2 = widthTotal / (columns + 0);
    // width2 = (widthTotal - width1) / (columns - 1);
}

/************************************************************************
 * chat::ConnectionRecord structure
 ************************************************************************/

/**
 * Every field of structure should have default constructor.
 **/
inline chat::ConnectionRecord::ConnectionRecord()
    : cookie          ( InvalidCookie )
    , nickName        (  )
    , connectTime     (  )
    , connectedTime   (  )
{
}
/**
 * \brief   Initialize ConnectionRecord data by setting parameters.
 **/
inline chat::ConnectionRecord::ConnectionRecord( const uint32_t & _cookie, const areg::String & _nickName, const areg::DateTime & _connectTime, const areg::DateTime & _connectedTime )
    : cookie          ( _cookie )
    , nickName        ( _nickName )
    , connectTime     ( _connectTime )
    , connectedTime   ( _connectedTime )
{
}
/**
 * Every filed of structure should have copy constructor.
 **/
inline chat::ConnectionRecord::ConnectionRecord( const chat::ConnectionRecord & src )
    : cookie          ( src.cookie )
    , nickName        ( src.nickName )
    , connectTime     ( src.connectTime )
    , connectedTime   ( src.connectedTime )
{
}
/**
 * No extra function is called, every field of structure should be able to be cleaned.
 **/
inline chat::ConnectionRecord::~ConnectionRecord()
{
}
/**
 * Copies data from given source.
 **/
inline const chat::ConnectionRecord & chat::ConnectionRecord::operator = ( const chat::ConnectionRecord & src )
{
    if ( static_cast<const chat::ConnectionRecord *>(this) != &src )
    {
        this->cookie          = src.cookie;
        this->nickName        = src.nickName;
        this->connectTime     = src.connectTime;
        this->connectedTime   = src.connectedTime;
    }
    return (*this);
}
/**
 * Checks equality of 2 instances of chat::ConnectionRecord objects.
 **/
inline bool chat::ConnectionRecord::operator == ( const chat::ConnectionRecord & other ) const
{
    return  ( this == &other ? true : (this->cookie == other.cookie) && (this->nickName == other.nickName) );
}
/**
 * Checks inequality of 2 instances of chat::ConnectionRecord objects.
 **/
inline bool chat::ConnectionRecord::operator != ( const chat::ConnectionRecord & other ) const
{
    return  ( this == &other ? false : (this->cookie != other.cookie) || (this->nickName == other.nickName) );
}
/**
 * \brief   Converts data to uint32_t, which might be used as a hash key value in map object.
 *          The conversion is a sum of each field of structure
 **/
 inline chat::ConnectionRecord::operator size_t () const
 {
    return  ( static_cast<size_t>( cookie ) + static_cast<uint32_t>( nickName ) );
 }

/**
 * \brief   Streaming operator. Reads and instantiates chat::ConnectionRecord structure field entries from stream.
 * \param   stream      The streaming object to read fields data of structure
 * \param   input       The instance of chat::ConnectionRecord structure to write data
 * \return  Returns the reference of streaming object.
 **/
inline const areg::InStream & operator >> ( const areg::InStream & stream, chat::ConnectionRecord & input )
{
    stream  >> input.cookie;
    stream  >> input.nickName;
    stream  >> input.connectTime;
    stream  >> input.connectedTime;
    return stream;
}
/**
 * \brief   Streaming operator. Writes chat::ConnectionRecord structure field entries to stream object.
 * \param   stream      The streaming object to write fields data from structure
 * \param   output      The instance of chat::ConnectionRecord structure to read data
 * \return  Returns the reference of streaming object.
 **/
inline areg::OutStream & operator << ( areg::OutStream & stream, const chat::ConnectionRecord & output )
{
    stream  << output.cookie;
    stream  << output.nickName;
    stream  << output.connectTime;
    stream  << output.connectedTime;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// Hasher of chat::ConnectionRecord object
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the chat::ConnectionRecord.
 */
namespace std
{
    template<> struct hash<chat::ConnectionRecord>
    {
        //! A function to convert chat::ConnectionRecord object to uint32_t.
        inline uint32_t operator()(const chat::ConnectionRecord & key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
}

/************************************************************************
 * chat::Participant structure
 ************************************************************************/

/**
 * Every field of structure should have default constructor.
 **/
inline chat::Participant::Participant()
    : sessionId   ( InvalidSession )
    , cookie      ( InvalidCookie )
    , nickName    (  )
{      }
/**
 * Every filed of structure should have copy constructor.
 **/
inline chat::Participant::Participant( const chat::Participant & src )
    : sessionId   ( src.sessionId )
    , cookie      ( src.cookie )
    , nickName    ( src.nickName )
{      }
/**
 * No extra function is called, every field of structure should be able to be cleaned.
 **/
inline chat::Participant::~Participant()
{      }
/**
 * Copies data from given source.
 **/
inline const chat::Participant & chat::Participant::operator = ( const chat::Participant & src )
{
    if ( static_cast<const chat::Participant *>(this) != &src )
    {
        this->sessionId   = src.sessionId;
        this->cookie      = src.cookie;
        this->nickName    = src.nickName;
    }
    return (*this);
}
/**
 * Checks equality of 2 instances of chat::Participant objects.
 **/
inline bool chat::Participant::operator == ( const chat::Participant & other ) const
{
    return  (   this == &other ? true :
                    ( this->sessionId   == other.sessionId )
                &&  ( this->cookie      == other.cookie )
                &&  ( this->nickName    == other.nickName )
            );
}
/**
 * Checks inequality of 2 instances of chat::Participant objects.
 **/
inline bool chat::Participant::operator != ( const chat::Participant & other ) const
{
    return  (   this == &other ? false :
                    ( this->sessionId   != other.sessionId )
                ||  ( this->cookie      != other.cookie )
                ||  ( this->nickName    != other.nickName )
            );
}
/**
 * \brief   Converts data to uint32_t, which might be used as a hash key value in map object.
 *          The conversion is a sum of each field of structure
 **/
 inline chat::Participant::operator size_t () const
 {
    return  ( static_cast<size_t>( sessionId ) + static_cast<size_t>( cookie ) + static_cast<uint32_t>( nickName ) );
 }
/**
 * \brief   Streaming operator. Reads and instantiates chat::Participant structure field entries from stream.
 * \param   stream      The streaming object to read fields data of structure
 * \param   input       The instance of chat::Participant structure to write data
 * \return  Returns the reference of streaming object.
 **/
inline const areg::InStream & operator >> ( const areg::InStream & stream, chat::Participant & input )
{
    stream  >> input.sessionId;
    stream  >> input.cookie;
    stream  >> input.nickName;
    return stream;
}
/**
 * \brief   Streaming operator. Writes chat::Participant structure field entries to stream object.
 * \param   stream      The streaming object to write fields data from structure
 * \param   output      The instance of chat::Participant structure to read data
 * \return  Returns the reference of streaming object.
 **/
inline areg::OutStream & operator << ( areg::OutStream & stream, const chat::Participant & output )
{
    stream  << output.sessionId;
    stream  << output.cookie;
    stream  << output.nickName;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// Hasher of chat::Participant object
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the chat::Participant.
 */
namespace std
{
    template<> struct hash<chat::Participant>
    {
        //! A function to convert chat::Participant object to uint32_t.
        inline uint32_t operator()(const chat::Participant& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
}


inline chat:: MessageData * chat::newData( )
{
    chat:: MessageData * result = DEBUG_NEW chat:: MessageData;
    if ( result != nullptr )
    {
        areg::memZero(reinterpret_cast<void *>(result), sizeof( chat:: MessageData ));
    }

    return result;
}
#endif // !WINCHAT_COMMON_CHATDEFS_HPP
