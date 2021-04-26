#pragma once

/************************************************************************
 * \file            shared/NECommonSettings.hpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "areg/src/base/GEGlobal.h"
#include "areg/src/trace/GETrace.h"
#include "areg/src/base/CEDateTime.hpp"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/TEArrayList.hpp"
#include "areg/src/base/TEHashMap.hpp"
#include <tchar.h>

namespace NECommonSettings
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
        CEString    nickName;
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

    inline void getWidths( int widthTotal, int columns, int & width1, int & width2 );

    /**
     * \brief   Invalid session ID
     *          NECommonSettings::InvalidSession
     **/
    const uint64_t  InvalidSession      = 0;
    /**
     * \brief   Invalid cookie
     *          NECommonSettings::InvalidCookie
     **/
    const uint32_t  InvalidCookie       = 0;
    /**
     * \brief   The maximum length of message
     *          NECommonSettings::MaxlenNickname
     **/
    const uint32_t  MaxlenNickname      = 64;
    /**
     * \brief   The maximum length of message
     *          NECommonSettings::MaxlenMessage
     **/
    const uint32_t  MaxlenMessage       = 512;
    /**
     * \brief   Invalid Nick-name
     *          NECommonSettings::InvalidNickname
     **/
    const CEString  InvalidNickname     = "";

	/************************************************************************
	 * \brief   NECommonSettings::sConnection
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
        inline sConnection( const uint32_t & _cookie, const CEString & _nickName, const CEDateTime & _connectTime, const CEDateTime & _connectedTime );
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
	    inline const NECommonSettings::sConnection & operator = ( const NECommonSettings::sConnection & src );
	    
	    /**
	     * \brief   Checks equality and returns true if 2 instances of sConnection are equal
	     * \param   other   The instance of sConnection to compare
	     * \return  Returns true if 2 instances are equal
	     **/ 
	    inline bool operator == ( const NECommonSettings::sConnection & other ) const;
	    
        /**
         * \brief   Checks inequality and returns true if 2 instances of sConnection are not equal
         * \param   other   The instance of sConnection to compare
         * \return  Returns true if 2 instances are not equal
         **/ 
	    inline bool operator != ( const NECommonSettings::sConnection & other ) const;

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
	    CEString    nickName;
    
	    /**
	     * \brief   Connection request date-time
	     **/
	    CEDateTime  connectTime;
    
	    /**
	     * \brief   Connection registered / accepted date-time
	     **/
	    CEDateTime  connectedTime;
    
	};

	/**
	 * \brief	NECommonSettings::ListConnections
	 *			Internal custom type definition of TEArrayList<sConnection, const sConnection &> 
	 **/
    typedef	TEArrayList<sConnection, const sConnection &>		                                    ListConnections;

	/**
	 * \brief	NECommonSettings::MapConnections
     *			Internal custom type definition of TEHashMap<uint32_t, sConnection, uint32_t, const sConnection &>
     **/
    typedef	TEHashMap<uint32_t, sConnection, uint32_t, const sConnection &>		                    MapConnections;

	/************************************************************************
	 * \brief   NECommonSettings::sParticipant
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
	    inline const NECommonSettings::sParticipant & operator = ( const NECommonSettings::sParticipant & src );
	    
	    /**
	     * \brief   Checks equality and returns true if 2 instances of sParticipant are equal
	     * \param   other   The instance of sParticipant to compare
	     * \return  Returns true if 2 instances are equal
	     **/ 
	    inline bool operator == ( const NECommonSettings::sParticipant & other ) const;
	    
        /**
         * \brief   Checks inequality and returns true if 2 instances of sParticipant are not equal
         * \param   other   The instance of sParticipant to compare
         * \return  Returns true if 2 instances are not equal
         **/ 
	    inline bool operator != ( const NECommonSettings::sParticipant & other ) const;

	    /**
	     * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
         *          The conversion is a sum of each field of structure
	     **/
	     inline operator size_t ( void ) const;

	    /**
	     * \brief   Declare NECommonSettings::sParticipant instance streamable.
	     **/
	    // DECLARE_STREAMABLE( NECommonSettings::sParticipant );
	    
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
	    CEString    nickName;
    
	};

    typedef sParticipant                                                                            sInitiator;

	/**
	 * \brief	NECommonSettings::ListConnections
	 *			Internal custom type definition of TEArrayList<sConnection, const sConnection &> 
	 **/
	typedef	TEArrayList<sParticipant, const sParticipant &>		                                    ListParticipants;

	/**
	 * \brief	NECommonSettings::MapDirectConnections
	 *			Internal custom type definition of TEHashMap<CEString, PairConnection, const CEString &, const PairConnection &> 
	 **/
	typedef	TEHashMap<sInitiator, ListParticipants, const sInitiator &, const ListParticipants &>   MapParticipants;

}

inline void NECommonSettings::getWidths( int widthTotal, int columns, int & width1, int & width2 )
{
    width1 = width2 = widthTotal / (columns + 0);
    // width2 = (widthTotal - width1) / (columns - 1);
}

/************************************************************************
 * NECommonSettings::sConnection structure
 ************************************************************************/

/**
 * Every field of structure should have default constructor.
 **/
inline NECommonSettings::sConnection::sConnection( void )
    : cookie          ( InvalidCookie )
    , nickName        (  )
    , connectTime     (  )
    , connectedTime   (  )
{   ;   }
/**
 * \brief   Initialize sConnection data by setting parameters.
 **/
inline NECommonSettings::sConnection::sConnection( const uint32_t & _cookie, const CEString & _nickName, const CEDateTime & _connectTime, const CEDateTime & _connectedTime )
    : cookie          ( _cookie )
    , nickName        ( _nickName )
    , connectTime     ( _connectTime )
    , connectedTime   ( _connectedTime )
{   ;   }
/**
 * Every filed of structure should have copy constructor.
 **/
inline NECommonSettings::sConnection::sConnection( const NECommonSettings::sConnection & src )
    : cookie          ( src.cookie )
    , nickName        ( src.nickName )
    , connectTime     ( src.connectTime )
    , connectedTime   ( src.connectedTime )
{   ;   }
/**
 * No extra function is called, every field of structure should be able to be cleaned.
 **/
inline NECommonSettings::sConnection::~sConnection( void )
{   ;   }
/**
 * Copies data from given source.
 **/
inline const NECommonSettings::sConnection & NECommonSettings::sConnection::operator = ( const NECommonSettings::sConnection & src )
{
    if ( static_cast<const NECommonSettings::sConnection *>(this) != &src )
    {
        this->cookie          = src.cookie;
        this->nickName        = src.nickName;
        this->connectTime     = src.connectTime;
        this->connectedTime   = src.connectedTime;
    }
    return (*this);
}
/**
 * Checks equality of 2 instances of NECommonSettings::sConnection objects.
 **/ 
inline bool NECommonSettings::sConnection::operator == ( const NECommonSettings::sConnection & other ) const
{
    return  ( this == &other ? true : (this->cookie == other.cookie) && (this->nickName == other.nickName) );
}
/**
 * Checks inequality of 2 instances of NECommonSettings::sConnection objects.
 **/ 
inline bool NECommonSettings::sConnection::operator != ( const NECommonSettings::sConnection & other ) const
{
    return  ( this == &other ? false : (this->cookie != other.cookie) || (this->nickName == other.nickName) );
}
/**
 * \brief   Converts data to unsigned int, which might be used as a hash key value in map object.
 *          The conversion is a sum of each field of structure
 **/
 inline NECommonSettings::sConnection::operator size_t ( void ) const
 {
    return  ( static_cast<size_t>( cookie ) + static_cast<size_t>( nickName ) );
 }
/**
 * \brief   Streaming operator. Reads and instantiates NECommonSettings::sConnection structure field entries from stream.
 * \param   stream      The streaming object to read fields data of structure
 * \param   input       The instance of NECommonSettings::sConnection structure to write data
 * \return  Returns the reference of streaming object.
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NECommonSettings::sConnection & input )
{
    stream  >> input.cookie;
    stream  >> input.nickName;
    stream  >> input.connectTime;
    stream  >> input.connectedTime;
    return stream;
}
/**
 * \brief   Streaming operator. Writes NECommonSettings::sConnection structure field entries to stream object.
 * \param   stream      The streaming object to write fields data from structure
 * \param   output      The instance of NECommonSettings::sConnection structure to read data
 * \return  Returns the reference of streaming object.
 **/
inline IEOutStream & operator << ( IEOutStream & stream, const NECommonSettings::sConnection & output )
{
    stream  << output.cookie;
    stream  << output.nickName;
    stream  << output.connectTime;
    stream  << output.connectedTime;
    return stream;
}

/************************************************************************
 * NECommonSettings::sParticipant structure
 ************************************************************************/

/**
 * Every field of structure should have default constructor.
 **/
inline NECommonSettings::sParticipant::sParticipant( void )
    : sessionId   ( InvalidSession )
    , cookie      ( InvalidCookie )
    , nickName    (  )
{   ;   }
/**
 * Every filed of structure should have copy constructor.
 **/
inline NECommonSettings::sParticipant::sParticipant( const NECommonSettings::sParticipant & src )
    : sessionId   ( src.sessionId )
    , cookie      ( src.cookie )
    , nickName    ( src.nickName )
{   ;   }
/**
 * No extra function is called, every field of structure should be able to be cleaned.
 **/
inline NECommonSettings::sParticipant::~sParticipant( void )
{   ;   }
/**
 * Copies data from given source.
 **/
inline const NECommonSettings::sParticipant & NECommonSettings::sParticipant::operator = ( const NECommonSettings::sParticipant & src )
{
    if ( static_cast<const NECommonSettings::sParticipant *>(this) != &src )
    {
        this->sessionId   = src.sessionId;
        this->cookie      = src.cookie;
        this->nickName    = src.nickName;
    }
    return (*this);
}
/**
 * Checks equality of 2 instances of NECommonSettings::sParticipant objects.
 **/ 
inline bool NECommonSettings::sParticipant::operator == ( const NECommonSettings::sParticipant & other ) const
{
    return  (   this == &other ? true :
                    ( this->sessionId   == other.sessionId )
                &&  ( this->cookie      == other.cookie )
                &&  ( this->nickName    == other.nickName )
            );
}
/**
 * Checks inequality of 2 instances of NECommonSettings::sParticipant objects.
 **/ 
inline bool NECommonSettings::sParticipant::operator != ( const NECommonSettings::sParticipant & other ) const
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
 inline NECommonSettings::sParticipant::operator size_t ( void ) const
 {
    return  ( static_cast<size_t>( sessionId ) + static_cast<size_t>( cookie ) + static_cast<size_t>( nickName ) );
 }
/**
 * \brief   Streaming operator. Reads and instantiates NECommonSettings::sParticipant structure field entries from stream.
 * \param   stream      The streaming object to read fields data of structure
 * \param   input       The instance of NECommonSettings::sParticipant structure to write data
 * \return  Returns the reference of streaming object.
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NECommonSettings::sParticipant & input )
{
    stream  >> input.sessionId;
    stream  >> input.cookie;
    stream  >> input.nickName;
    return stream;
}
/**
 * \brief   Streaming operator. Writes NECommonSettings::sParticipant structure field entries to stream object.
 * \param   stream      The streaming object to write fields data from structure
 * \param   output      The instance of NECommonSettings::sParticipant structure to read data
 * \return  Returns the reference of streaming object.
 **/
inline IEOutStream & operator << ( IEOutStream & stream, const NECommonSettings::sParticipant & output )
{
    stream  << output.sessionId;
    stream  << output.cookie;
    stream  << output.nickName;
    return stream;
}
