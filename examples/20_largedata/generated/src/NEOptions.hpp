//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/NEOptions.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_NEOPTIONS_HPP
#define  GENERATED_SRC_NEOPTIONS_HPP

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.09.2022  23:32:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source Options.
 *
 * \file            generated/src/NEOptions.hpp
 * \ingroup         Options Service Interface
 * \brief           This is an automatic generated code of Options
 *                  Service Interface namespace declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// Dependency includes
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mandatory includes
//////////////////////////////////////////////////////////////////////////
#include "areg/base/IEIOStream.hpp"
#include "areg/base/Version.hpp"
#include "areg/component/NEService.hpp"

#include <utility>

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// NEOptions namespace declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 *
 * \brief   Options Service Interface defined constants,
 *          types and structures message IDs, global static methods.
 *
 *          This SI sets options to generate data
 *
 **/
namespace   NEOptions
{

/************************************************************************/
// Service Interface constants
/************************************************************************/
    /**
     * \brief   Service Interface Name.
     *          NEOptions::ServiceName
     **/
    constexpr   char    ServiceName[]   { "Options" };
    /**
     * \brief   Implemented Version.
     *          NEOptions::InterfaceVersion
     **/
    const       Version InterfaceVersion{ 1, 0, 0 };

/************************************************************************/
// Service Interface data types
/************************************************************************/


    /**
     * \brief   Bit settings of option flag.
     *          NEOptions::eOptionFlags
     **/
    enum class eOptionFlags
    {
        /**
         * \brief   No command.
         **/
          CmdNothing          = 0
        /**
         * \brief   The command to change image width
         **/
        , CmdWidth            = 1
        /**
         * \brief   The command to change image height
         **/
        , CmdHeight           = 2
        /**
         * \brief   The command to change lines per image block
         **/
        , CmdLinesPerBlock    = 4
        /**
         * \brief   The command to set image pixel time in nanoseconds.
         **/
        , CmdPixelTime        = 8
        /**
         * \brief   The command to set number of channels.
         **/
        , CmdChannels         = 16
        /**
         * \brief   The command to set data rate.
         **/
        , CmdDataRate         = 32
        /**
         * \brief   The command to set packet rate
         **/
        , CmdPacketRate       = 64
    };
    /**
     * \brief   Returns string value of NEOptions::eOptionFlags
     **/
    inline const char * getString( NEOptions::eOptionFlags enumValue );

    /**
     * \brief   A structure to generate a bitmap image and send, so that we can calculate the data rate
     *          Every field of structure should have default constructor.
     *          NEOptions::sOptionData
     **/
    struct sOptionData
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor/ destructor
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Default constructor. Every field of structure should have default constructor.
         **/
        inline sOptionData( void );

        /**
         * \brief   Initialize sOptionData data by setting parameters.
         **/
        inline sOptionData( const unsigned int & _flags, const unsigned int & _width, const unsigned int & _height, const unsigned int & _linesPerBlock, const unsigned int & _pixelTime, const unsigned int & _channels, const double & _dataRate, const unsigned int & _packetRate );

        /**
         * \brief   Copies data from given source. Every filed of structure should have copy constructor.
         * \param   src     The source of NEOptions::sOptionData structure to copy data.
         **/
        inline sOptionData( const NEOptions::sOptionData & src );

        /**
         * \brief   Moves data from given source. Every filed of structure should have move constructor.
         * \param   src     The source of NEOptions::sOptionData structure to move data.
         **/
        inline sOptionData( NEOptions::sOptionData && src ) noexcept;

        /**
         * \brief   Default destructor.
         **/
        ~sOptionData( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Basic operators
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies data from given source. Every field of structure should have copy constructor.
         * \param   src     The source of sOptionData structure to copy data.
         **/
        inline NEOptions::sOptionData & operator = ( const NEOptions::sOptionData & src );

        /**
         * \brief   Moves data from given source. Every field of structure should have move constructor.
         * \param   src     The source of sOptionData structure to move data.
         **/
        inline NEOptions::sOptionData & operator = ( NEOptions::sOptionData && src ) noexcept;

        /**
         * \brief   Checks equality and returns true if 2 instances of sOptionData are equal
         * \param   other   The instance of sOptionData to compare
         **/ 
        inline bool operator == ( const NEOptions::sOptionData & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of sOptionData are not equal
         * \param   other   The instance of sOptionData to compare
         **/ 
        inline bool operator != ( const NEOptions::sOptionData & other ) const;
    //////////////////////////////////////////////////////////////////////////
    // sOptionData fields
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Flag, identifying valid or changed fileds of options
         **/
         unsigned int    flags           /* = 0 */;
        /**
         * \brief   Image width in pixels
         **/
         unsigned int    width           /* = 0 */;
        /**
         * \brief   IMage height in pixels
         **/
         unsigned int    height          /* = 0 */;
        /**
         * \brief   Lines per image block
         **/
         unsigned int    linesPerBlock   /* = 0 */;
        /**
         * \brief   Pixel time in nanoseconds.
         **/
         unsigned int    pixelTime       /* = 0 */;
        /**
         * \brief   Number of image source channels
         **/
         unsigned int    channels        /* = 0 */;
        /**
         * \brief   Tata rate in Kilobytes per second
         **/
         double          dataRate        /* = 0.0 */;
        /**
         * \brief   Packet rates per second
         **/
         unsigned int    packetRate      /* = 0 */;
    };

    /**
     * \brief   Defines the unit of the value
     *          NEOptions::eUnit
     **/
    enum class eUnit
    {
        /**
         * \brief   Invalid unit, should not be used.
         **/
          UnitUndefined
        /**
         * \brief   The value is in bytes
         **/
        , UnitBytes
        /**
         * \brief   The value is in Kilobytes
         **/
        , UnitKilobytes
        /**
         * \brief   The value is in megabytes
         **/
        , UnitMegabytes
    };
    /**
     * \brief   Returns string value of NEOptions::eUnit
     **/
    inline const char * getString( NEOptions::eUnit enumValue );

    /**
     * \brief   The structure of computed data rate per second
     *          Every field of structure should have default constructor.
     *          NEOptions::sDataRate
     **/
    struct sDataRate
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor/ destructor
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Default constructor. Every field of structure should have default constructor.
         **/
        inline sDataRate( void );

        /**
         * \brief   Initialize sDataRate data by setting parameters.
         **/
        inline sDataRate( const double & _dataRate, const NEOptions::eUnit & _valueUnit );

        /**
         * \brief   Copies data from given source. Every filed of structure should have copy constructor.
         * \param   src     The source of NEOptions::sDataRate structure to copy data.
         **/
        inline sDataRate( const NEOptions::sDataRate & src );

        /**
         * \brief   Moves data from given source. Every filed of structure should have move constructor.
         * \param   src     The source of NEOptions::sDataRate structure to move data.
         **/
        inline sDataRate( NEOptions::sDataRate && src ) noexcept;

        /**
         * \brief   Default destructor.
         **/
        ~sDataRate( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Basic operators
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies data from given source. Every field of structure should have copy constructor.
         * \param   src     The source of sDataRate structure to copy data.
         **/
        inline NEOptions::sDataRate & operator = ( const NEOptions::sDataRate & src );

        /**
         * \brief   Moves data from given source. Every field of structure should have move constructor.
         * \param   src     The source of sDataRate structure to move data.
         **/
        inline NEOptions::sDataRate & operator = ( NEOptions::sDataRate && src ) noexcept;

        /**
         * \brief   Checks equality and returns true if 2 instances of sDataRate are equal
         * \param   other   The instance of sDataRate to compare
         **/ 
        inline bool operator == ( const NEOptions::sDataRate & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of sDataRate are not equal
         * \param   other   The instance of sDataRate to compare
         **/ 
        inline bool operator != ( const NEOptions::sDataRate & other ) const;
    //////////////////////////////////////////////////////////////////////////
    // sDataRate fields
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   The data rate value
         **/
         double              dataRate    /* = 0.0 */;
        /**
         * \brief   The unit of data rate -- bytes, kilobytes or megabytes.
         **/
         NEOptions::eUnit    valueUnit   /* = UnitUndefined */;
    };

    /**
     * \brief   The structure of computed packet rate per second
     *          Every field of structure should have default constructor.
     *          NEOptions::sPacketRate
     **/
    struct sPacketRate
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor/ destructor
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Default constructor. Every field of structure should have default constructor.
         **/
        inline sPacketRate( void );

        /**
         * \brief   Initialize sPacketRate data by setting parameters.
         **/
        inline sPacketRate( const unsigned int & _numPackets );

        /**
         * \brief   Copies data from given source. Every filed of structure should have copy constructor.
         * \param   src     The source of NEOptions::sPacketRate structure to copy data.
         **/
        inline sPacketRate( const NEOptions::sPacketRate & src );

        /**
         * \brief   Moves data from given source. Every filed of structure should have move constructor.
         * \param   src     The source of NEOptions::sPacketRate structure to move data.
         **/
        inline sPacketRate( NEOptions::sPacketRate && src ) noexcept;

        /**
         * \brief   Default destructor.
         **/
        ~sPacketRate( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Basic operators
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies data from given source. Every field of structure should have copy constructor.
         * \param   src     The source of sPacketRate structure to copy data.
         **/
        inline NEOptions::sPacketRate & operator = ( const NEOptions::sPacketRate & src );

        /**
         * \brief   Moves data from given source. Every field of structure should have move constructor.
         * \param   src     The source of sPacketRate structure to move data.
         **/
        inline NEOptions::sPacketRate & operator = ( NEOptions::sPacketRate && src ) noexcept;

        /**
         * \brief   Checks equality and returns true if 2 instances of sPacketRate are equal
         * \param   other   The instance of sPacketRate to compare
         **/ 
        inline bool operator == ( const NEOptions::sPacketRate & other ) const;

        /**
         * \brief   Checks inequality and returns true if 2 instances of sPacketRate are not equal
         * \param   other   The instance of sPacketRate to compare
         **/ 
        inline bool operator != ( const NEOptions::sPacketRate & other ) const;
    //////////////////////////////////////////////////////////////////////////
    // sPacketRate fields
    ///////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Number of packets generated per second.
         **/
         unsigned int    numPackets  /* = 0 */;
    };

/************************************************************************
 * Service Message IDs
 ************************************************************************/
    /**
     * \brief   NEOptions::eMessageIDs
     *          Options Service interface defined message IDs
     **/
    enum class eMessageIDs : msg_id
    {
    /************************************************************************
     * Request IDs
     ************************************************************************/
        /**
         * \brief   Request ID: MsgId_requestDataRate
         *          Request to get data rate values
         **/
          MsgId_requestDataRate           = NEService::REQUEST_ID_FIRST   // void requestDataRate( void );
        /**
         * \brief   Request ID: MsgId_requestPacketRate
         *          Request to get packetrate, i.e. the numberof generated image blocks per second
         **/
        , MsgId_requestPacketRate                                           // void requestPacketRate( void );
        /**
         * \brief   Request ID: MsgId_requestChangeOptions
         *          Request to change the options to generate images.
         **/
        , MsgId_requestChangeOptions                                        // void requestChangeOptions( const NEOptions::sOptionData & Options );
        /**
         * \brief   Request ID: MsgId_requestQuitApplication
         *          DESCRIPTION MISSED
         **/
        , MsgId_requestQuitApplication                                      // void requestQuitApplication( void );

    /************************************************************************
     * Response IDs
     ************************************************************************/
        /**
         * \brief   Response ID: MsgId_responseDataRate
         *          Response of data rate
         **/
        , MsgId_responseDataRate          = NEService::RESPONSE_ID_FIRST  // void responseDataRate( const NEOptions::sDataRate & sentData, const NEOptions::sDataRate & missedData );
        /**
         * \brief   Response ID: MsgId_responsePacketRate
         *          Response of packet rate.
         **/
        , MsgId_responsePacketRate                                          // void responsePacketRate( unsigned int genPackets, const NEOptions::sDataRate & avSize );
    /************************************************************************
     * Broadcast IDs
     ************************************************************************/

    /************************************************************************
     * Attributes IDs
     ************************************************************************/
        /**
         * \brief   Attribute ID: MsgId_OptionData
         *          The options
         **/
        , MsgId_OptionData                = NEService::ATTRIBUTE_ID_FIRST // NEOptions::sOptionData mOptionData;

    /************************************************************************
     * Reserved constant IDs
     ************************************************************************/
        /**
         * \brief   ID of empty function
         **/
        , MsgId_NotProcessed              = NEService::INVALID_MESSAGE_ID
        /**
         * \brief   ID of invalid call
         **/
        , MsgId_Invalid                   = NEService::RESPONSE_ID_NONE

    };
    /**
     * \brief   Returns string value of NEOptions::eMessageIDs
     **/
    inline const char * getString( NEOptions::eMessageIDs msgId );

/************************************************************************
 * Service Interface standard functions and variables
 ************************************************************************/

    /**
     * \brief   Creates and returns Service Interface Data
     **/
    const NEService::SInterfaceData & getInterfaceData( void );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEOptions::eMessageIDs getResponseId( NEOptions::eMessageIDs reqId );

    /**
     * \brief   Returns the mapped Response ID of give Request ID.
     **/
    NEOptions::eMessageIDs getRequestId( NEOptions::eMessageIDs respId );
}

//////////////////////////////////////////////////////////////////////////
// NEOptions inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   Declare and implement NEOptions methods
 ************************************************************************/

IMPLEMENT_STREAMABLE( NEOptions::eMessageIDs );

/************************************************************************
 * NEOptions::eOptionFlags enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NEOptions::eOptionFlags )

/**
 * Return string value of NEOptions::eOptionFlags
 **/
inline const char * NEOptions::getString( NEOptions::eOptionFlags enumValue )
{
    switch ( enumValue )
    {
    case    NEOptions::eOptionFlags::CmdNothing:
        return "NEOptions::eOptionFlags::CmdNothing";
    case    NEOptions::eOptionFlags::CmdWidth:
        return "NEOptions::eOptionFlags::CmdWidth";
    case    NEOptions::eOptionFlags::CmdHeight:
        return "NEOptions::eOptionFlags::CmdHeight";
    case    NEOptions::eOptionFlags::CmdLinesPerBlock:
        return "NEOptions::eOptionFlags::CmdLinesPerBlock";
    case    NEOptions::eOptionFlags::CmdPixelTime:
        return "NEOptions::eOptionFlags::CmdPixelTime";
    case    NEOptions::eOptionFlags::CmdChannels:
        return "NEOptions::eOptionFlags::CmdChannels";
    case    NEOptions::eOptionFlags::CmdDataRate:
        return "NEOptions::eOptionFlags::CmdDataRate";
    case    NEOptions::eOptionFlags::CmdPacketRate:
        return "NEOptions::eOptionFlags::CmdPacketRate";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NEOptions::eOptionFlags value!";
    }
}

/************************************************************************
 * NEOptions::sOptionData strcuture inline implementation
 ************************************************************************/
/**
 * Every field of structure should have default constructor.
 **/
inline NEOptions::sOptionData::sOptionData( void )
    : flags           ( 0 )
    , width           ( 0 )
    , height          ( 0 )
    , linesPerBlock   ( 0 )
    , pixelTime       ( 0 )
    , channels        ( 0 )
    , dataRate        ( 0.0 )
    , packetRate      ( 0 )
{   /* do nothing */    }

inline NEOptions::sOptionData::sOptionData( const unsigned int & _flags, const unsigned int & _width, const unsigned int & _height, const unsigned int & _linesPerBlock, const unsigned int & _pixelTime, const unsigned int & _channels, const double & _dataRate, const unsigned int & _packetRate )
    : flags           ( _flags )
    , width           ( _width )
    , height          ( _height )
    , linesPerBlock   ( _linesPerBlock )
    , pixelTime       ( _pixelTime )
    , channels        ( _channels )
    , dataRate        ( _dataRate )
    , packetRate      ( _packetRate )
{   /* do nothing */    }

inline NEOptions::sOptionData::sOptionData( const NEOptions::sOptionData & src )
    : flags           ( src.flags )
    , width           ( src.width )
    , height          ( src.height )
    , linesPerBlock   ( src.linesPerBlock )
    , pixelTime       ( src.pixelTime )
    , channels        ( src.channels )
    , dataRate        ( src.dataRate )
    , packetRate      ( src.packetRate )
{   /* do nothing */    }

inline NEOptions::sOptionData::sOptionData( NEOptions::sOptionData && src ) noexcept
    : flags           ( std::move(src.flags) )
    , width           ( std::move(src.width) )
    , height          ( std::move(src.height) )
    , linesPerBlock   ( std::move(src.linesPerBlock) )
    , pixelTime       ( std::move(src.pixelTime) )
    , channels        ( std::move(src.channels) )
    , dataRate        ( std::move(src.dataRate) )
    , packetRate      ( std::move(src.packetRate) )
{   /* do nothing */    }

inline NEOptions::sOptionData & NEOptions::sOptionData::operator = ( const NEOptions::sOptionData & src )
{
    if ( static_cast<const NEOptions::sOptionData *>(this) != &src )
    {
            this->flags           = src.flags;
            this->width           = src.width;
            this->height          = src.height;
            this->linesPerBlock   = src.linesPerBlock;
            this->pixelTime       = src.pixelTime;
            this->channels        = src.channels;
            this->dataRate        = src.dataRate;
            this->packetRate      = src.packetRate;
    }

    return (*this);
}

inline NEOptions::sOptionData & NEOptions::sOptionData::operator = ( NEOptions::sOptionData && src ) noexcept
{
    if ( static_cast<const NEOptions::sOptionData *>(this) != &src )
    {
            this->flags           = std::move(src.flags);
            this->width           = std::move(src.width);
            this->height          = std::move(src.height);
            this->linesPerBlock   = std::move(src.linesPerBlock);
            this->pixelTime       = std::move(src.pixelTime);
            this->channels        = std::move(src.channels);
            this->dataRate        = std::move(src.dataRate);
            this->packetRate      = std::move(src.packetRate);
    }
    return (*this);
}

inline bool NEOptions::sOptionData::operator == ( const NEOptions::sOptionData & other ) const
{
    if ( static_cast<const NEOptions::sOptionData *>(this) != &other )
    {
        return (
                   (this->flags           == other.flags)
                && (this->width           == other.width)
                && (this->height          == other.height)
                && (this->linesPerBlock   == other.linesPerBlock)
                && (this->pixelTime       == other.pixelTime)
                && (this->channels        == other.channels)
                && (this->dataRate        == other.dataRate)
                && (this->packetRate      == other.packetRate)
                );
    }
    else
    {
        return true;
    }
}

inline bool NEOptions::sOptionData::operator != ( const NEOptions::sOptionData & other ) const
{
    if ( static_cast<const NEOptions::sOptionData *>(this) != &other )
    {
        return (
                   (this->flags           != other.flags)
                || (this->width           != other.width)
                || (this->height          != other.height)
                || (this->linesPerBlock   != other.linesPerBlock)
                || (this->pixelTime       != other.pixelTime)
                || (this->channels        != other.channels)
                || (this->dataRate        != other.dataRate)
                || (this->packetRate      != other.packetRate)
                );
    }
    else
    {
        return false;
    }
}


/**
 * Encode and decode NEOptions::sOptionData structure
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NEOptions::sOptionData & input )
{
    stream >> input.flags;
    stream >> input.width;
    stream >> input.height;
    stream >> input.linesPerBlock;
    stream >> input.pixelTime;
    stream >> input.channels;
    stream >> input.dataRate;
    stream >> input.packetRate;
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const NEOptions::sOptionData & output )
{
    stream << output.flags;
    stream << output.width;
    stream << output.height;
    stream << output.linesPerBlock;
    stream << output.pixelTime;
    stream << output.channels;
    stream << output.dataRate;
    stream << output.packetRate;
    return stream;
}

/************************************************************************
 * NEOptions::eUnit enum declare streamable
 ************************************************************************/
IMPLEMENT_STREAMABLE( NEOptions::eUnit )

/**
 * Return string value of NEOptions::eUnit
 **/
inline const char * NEOptions::getString( NEOptions::eUnit enumValue )
{
    switch ( enumValue )
    {
    case    NEOptions::eUnit::UnitUndefined:
        return "NEOptions::eUnit::UnitUndefined";
    case    NEOptions::eUnit::UnitBytes:
        return "NEOptions::eUnit::UnitBytes";
    case    NEOptions::eUnit::UnitKilobytes:
        return "NEOptions::eUnit::UnitKilobytes";
    case    NEOptions::eUnit::UnitMegabytes:
        return "NEOptions::eUnit::UnitMegabytes";
    default:
        ASSERT(false);
        return "ERROR: Unexpected NEOptions::eUnit value!";
    }
}

/************************************************************************
 * NEOptions::sDataRate strcuture inline implementation
 ************************************************************************/
/**
 * Every field of structure should have default constructor.
 **/
inline NEOptions::sDataRate::sDataRate( void )
    : dataRate    ( 0.0 )
    , valueUnit   ( eUnit::UnitUndefined )
{   /* do nothing */    }

inline NEOptions::sDataRate::sDataRate( const double & _dataRate, const NEOptions::eUnit & _valueUnit )
    : dataRate    ( _dataRate )
    , valueUnit   ( _valueUnit )
{   /* do nothing */    }

inline NEOptions::sDataRate::sDataRate( const NEOptions::sDataRate & src )
    : dataRate    ( src.dataRate )
    , valueUnit   ( src.valueUnit )
{   /* do nothing */    }

inline NEOptions::sDataRate::sDataRate( NEOptions::sDataRate && src ) noexcept
    : dataRate    ( std::move(src.dataRate) )
    , valueUnit   ( std::move(src.valueUnit) )
{   /* do nothing */    }

inline NEOptions::sDataRate & NEOptions::sDataRate::operator = ( const NEOptions::sDataRate & src )
{
    if ( static_cast<const NEOptions::sDataRate *>(this) != &src )
    {
            this->dataRate    = src.dataRate;
            this->valueUnit   = src.valueUnit;
    }

    return (*this);
}

inline NEOptions::sDataRate & NEOptions::sDataRate::operator = ( NEOptions::sDataRate && src ) noexcept
{
    if ( static_cast<const NEOptions::sDataRate *>(this) != &src )
    {
            this->dataRate    = std::move(src.dataRate);
            this->valueUnit   = std::move(src.valueUnit);
    }
    return (*this);
}

inline bool NEOptions::sDataRate::operator == ( const NEOptions::sDataRate & other ) const
{
    if ( static_cast<const NEOptions::sDataRate *>(this) != &other )
    {
        return (
                   (this->dataRate    == other.dataRate)
                && (this->valueUnit   == other.valueUnit)
                );
    }
    else
    {
        return true;
    }
}

inline bool NEOptions::sDataRate::operator != ( const NEOptions::sDataRate & other ) const
{
    if ( static_cast<const NEOptions::sDataRate *>(this) != &other )
    {
        return (
                   (this->dataRate    != other.dataRate)
                || (this->valueUnit   != other.valueUnit)
                );
    }
    else
    {
        return false;
    }
}


/**
 * Encode and decode NEOptions::sDataRate structure
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NEOptions::sDataRate & input )
{
    stream >> input.dataRate;
    stream >> input.valueUnit;
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const NEOptions::sDataRate & output )
{
    stream << output.dataRate;
    stream << output.valueUnit;
    return stream;
}

/************************************************************************
 * NEOptions::sPacketRate strcuture inline implementation
 ************************************************************************/
/**
 * Every field of structure should have default constructor.
 **/
inline NEOptions::sPacketRate::sPacketRate( void )
    : numPackets  ( 0 )
{   /* do nothing */    }

inline NEOptions::sPacketRate::sPacketRate( const unsigned int & _numPackets )
    : numPackets  ( _numPackets )
{   /* do nothing */    }

inline NEOptions::sPacketRate::sPacketRate( const NEOptions::sPacketRate & src )
    : numPackets  ( src.numPackets )
{   /* do nothing */    }

inline NEOptions::sPacketRate::sPacketRate( NEOptions::sPacketRate && src ) noexcept
    : numPackets  ( std::move(src.numPackets) )
{   /* do nothing */    }

inline NEOptions::sPacketRate & NEOptions::sPacketRate::operator = ( const NEOptions::sPacketRate & src )
{
    if ( static_cast<const NEOptions::sPacketRate *>(this) != &src )
    {
            this->numPackets  = src.numPackets;
    }

    return (*this);
}

inline NEOptions::sPacketRate & NEOptions::sPacketRate::operator = ( NEOptions::sPacketRate && src ) noexcept
{
    if ( static_cast<const NEOptions::sPacketRate *>(this) != &src )
    {
            this->numPackets  = std::move(src.numPackets);
    }
    return (*this);
}

inline bool NEOptions::sPacketRate::operator == ( const NEOptions::sPacketRate & other ) const
{
    if ( static_cast<const NEOptions::sPacketRate *>(this) != &other )
    {
        return (
                   (this->numPackets  == other.numPackets)
                );
    }
    else
    {
        return true;
    }
}

inline bool NEOptions::sPacketRate::operator != ( const NEOptions::sPacketRate & other ) const
{
    if ( static_cast<const NEOptions::sPacketRate *>(this) != &other )
    {
        return (
                   (this->numPackets  != other.numPackets)
                );
    }
    else
    {
        return false;
    }
}


/**
 * Encode and decode NEOptions::sPacketRate structure
 **/
inline const IEInStream & operator >> ( const IEInStream & stream, NEOptions::sPacketRate & input )
{
    stream >> input.numPackets;
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const NEOptions::sPacketRate & output )
{
    stream << output.numPackets;
    return stream;
}

/**
 * Returns string value of NEOptions::eMessageIDs
 **/
inline const char * NEOptions::getString( NEOptions::eMessageIDs msgId )
{
    switch ( msgId )
    {
    case    NEOptions::eMessageIDs::MsgId_requestDataRate:
        return "NEOptions::eMessageIDs::MsgId_requestDataRate";
    case    NEOptions::eMessageIDs::MsgId_requestPacketRate:
        return "NEOptions::eMessageIDs::MsgId_requestPacketRate";
    case    NEOptions::eMessageIDs::MsgId_requestChangeOptions:
        return "NEOptions::eMessageIDs::MsgId_requestChangeOptions";
    case    NEOptions::eMessageIDs::MsgId_requestQuitApplication:
        return "NEOptions::eMessageIDs::MsgId_requestQuitApplication";
    case    NEOptions::eMessageIDs::MsgId_responseDataRate:
        return "NEOptions::eMessageIDs::MsgId_responseDataRate";
    case    NEOptions::eMessageIDs::MsgId_responsePacketRate:
        return "NEOptions::eMessageIDs::MsgId_responsePacketRate";
    case    NEOptions::eMessageIDs::MsgId_OptionData:
        return "NEOptions::eMessageIDs::MsgId_OptionData";

    case NEOptions::eMessageIDs::MsgId_NotProcessed:
        return "NEOptions::eMessageIDs::MsgId_NotProcessed";
    case NEOptions::eMessageIDs::MsgId_Invalid:
        return "NEOptions::eMessageIDs::MsgId_Invalid";
    default:
        ASSERT(false);
        return "ERROR: Undefined NEOptions::eMessageIDs value!";
    }
}

//////////////////////////////////////////////////////////////////////////
// NEOptions namespace declaration End
//////////////////////////////////////////////////////////////////////////

#endif  // GENERATED_SRC_NEOPTIONS_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/NEOptions.hpp file
//////////////////////////////////////////////////////////////////////////
