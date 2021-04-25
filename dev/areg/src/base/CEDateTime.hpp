#ifndef AREG_BASE_CEDATETIME_HPP
#define AREG_BASE_CEDATETIME_HPP

/************************************************************************
 * \file        areg/src/base/CEDateTime.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Date and Time class.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/NEUtilities.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
struct tm;
class IEInStream;
class IEOutStream;
//////////////////////////////////////////////////////////////////////////
// CEDateTime class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Date and Time class. It saves date and time value as 
 *          64-bit value and makes basic operations with date and time value.
 **/
class AREG_API CEDateTime
{
//////////////////////////////////////////////////////////////////////////
// Internal constants
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Hidden constants
/************************************************************************/
    /**
     * \brief   Output format of milliseconds
     **/
    static const char * const       FORMAT_MILLISECOND                  /*= "%l"*/;

public:
/************************************************************************/
// Public constants
/************************************************************************/

    /**
     * \brief   Format of timestamp to display logs
     **/
    static const char * const       TIME_FORMAT_ISO8601_OUTPUT          /*= "%Y-%m-%d %H:%M:%S,%l"*/;

    /**
     * \brief   Absolute time format of timestamp
     **/
    static const char * const       TIME_FORMAT_ABSOLUTE_OUTPUT         /*= "%H:%M:%S,%l"*/;

    /**
     * \brief   Format only data of timestamp
     **/
    static const char * const       TIME_FORMAT_DATE_OUTPUT             /*= "%d %b %Y %H:%M:%S,%l"*/;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Sets zero value
     **/
    CEDateTime( void );

    /**
     * \brief   Initialization constructor. Sets date and time value in microseconds passed since January 1 1970.
     * \param   dateTime    Microseconds passed since January 1 1970.
     **/
    CEDateTime( const TIME64 & dateTime );

    /**
     * \brief   Initialization constructor. Sets date and time value
     *          from given system time structure.
     * \param   sysTime     System time structure to get date and time values.
     **/
    CEDateTime( const NEUtilities::sSystemTime & sysTime );

    /**
     * \brief   Initialization constructor. Sets date and time value
     *          from given file time structure.
     * \param   fileTime    File time structure to get date and time values.
     **/
    CEDateTime( const NEUtilities::sFileTime & fileTime );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEDateTime( const CEDateTime & dateTime );

    /**
     * \brief   Initializes data from streaming object
     **/
    CEDateTime( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    ~CEDateTime(void);

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts and returns date and time value as 64-bit unsigned integer value
     **/
    inline operator TIME64 ( void ) const;

    /**
     * \brief   Assigning operator.
     *          Sets date and time value from given source
     * \param   src     The source of date and time value
     **/
    inline const CEDateTime & operator = ( const CEDateTime & src );

    /**
     * \brief   Comparing operator
     *          Returns true, if 2 date and time objects are equal.
     *          Otherwise, returns false.
     * \param   other   The date and time object to compare
     * \return  Returns true if 2 date and time objects are equal
     **/
    bool operator == ( const CEDateTime & other ) const;

    /**
     * \brief   Comparing operator
     *          Returns true, if 2 date and time objects are not equal.
     *          Otherwise, returns false.
     * \param   other   The date and time object to compare
     * \return  Returns true if 2 date and time objects are not equal
     **/
    bool operator != ( const CEDateTime & other ) const;

    /**
     * \brief   Comparing operator
     *          Returns true, if one date and time object is greater than other.
     *          Otherwise, returns false.
     * \param   other   The date and time object to compare
     * \return  Returns true if date and time value is greater than other
     **/
    bool operator > ( const CEDateTime & other ) const;

    /**
     * \brief   Comparing operator
     *          Returns true, if one date and time object is smaller than other.
     *          Otherwise, returns false.
     * \param   other   The date and time object to compare
     * \return  Returns true if date and time value is smaller than other
     **/
    bool operator < ( const CEDateTime & other ) const;

    /**
     * \brief   Comparing operator
     *          Returns true, if one date and time object is greater or equal than other.
     *          Otherwise, returns false.
     * \param   other   The date and time object to compare
     * \return  Returns true if date and time value is greater or equal than other
     **/
    bool operator >= ( const CEDateTime & other ) const;

    /**
     * \brief   Comparing operator
     *          Returns true, if one date and time object is smaller or equal than other.
     *          Otherwise, returns false.
     * \param   other   The date and time object to compare
     * \return  Returns true if date and time value is smaller or equal than other
     **/
    bool operator <= ( const CEDateTime & other ) const;

    /**
     * \brief   Streaming operator.
     *          Reads (de-serializes) date and time value from streaming object.
     * \param   stream  Streaming object, which contains serialized date and time value
     * \param   input   Date and time object, which is initialized by de-serializing 
     *                  date and time value from stream
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEDateTime & input );

    /**
     * \brief   Streaming operator.
     *          Writes (serializes) date and time value to streaming object.
     * \param   stream  Streaming object to serialized date and time value
     * \param   input   Date and time object, which is contains date and time value and
     *                  should be serialized to streaming object
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEDateTime & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Static operations
/************************************************************************/
    /**
     * \brief   Retrieves and returns current system time in UTC (Coordinated Universal Time).
     * \return  Returns current date and time value.
     **/
    static CEDateTime GetNow( void );

    /**
     * \brief   Retrieves and returns current local time.
     * \return  Returns current date and time value.
     **/
    static void GetNow( NEUtilities::sSystemTime & outSysTime );

    /**
     * \brief   Retrieves the number of milliseconds that have elapsed since the system was started.
     **/
    static uint64_t GetTickCount( void );

    /**
     * \brief   Retrieves the number of milliseconds that have elapsed since current process has been started.
     **/
    static uint64_t GetProcessTickClocks( void );

/************************************************************************/
// Non-static operations
/************************************************************************/
    /**
     * \brief   Converts existing date and time value to system time structure
     * \param   out_sysTime The System Time structure to feel data
     **/
    void ToSystemTime( NEUtilities::sSystemTime & out_sysTime ) const;

    /**
     * \brief   Retrieves date and time value from system time structure and sets value
     * \param   sysTime     The System Time structure as a source of data.
     **/
    void FromSystemTime( const NEUtilities::sSystemTime & sysTime );

    /**
     * \brief   Formats time and outputs as a string. The caller should specify the
     *          the time format name for output
     **/
    CEString FormatTime( const char * formatName = CEDateTime::TIME_FORMAT_ISO8601_OUTPUT ) const;

    /**
     * \brief   Returns the time data.
     **/
    inline const TIME64 & GetTime( void ) const;

    /**
     * \brief   Returns true, if time value is not zero
     **/
    inline bool IsValid( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Date and time value
     **/
    TIME64  mDateTime;
};

//////////////////////////////////////////////////////////////////////////
// CEDateTime class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CEDateTime::operator TIME64 ( void ) const
{   return mDateTime;                                   }

inline const CEDateTime & CEDateTime::operator = ( const CEDateTime & src )
{   mDateTime = src.mDateTime; return (*this);          }

inline const TIME64 & CEDateTime::GetTime( void ) const
{   return mDateTime;                                   }

inline bool CEDateTime::IsValid( void ) const
{   return (mDateTime != 0);                            }

#endif  // AREG_BASE_CEDATETIME_HPP
