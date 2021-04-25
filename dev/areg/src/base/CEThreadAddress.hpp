#ifndef AREG_BASE_CETHREADADDRESS_HPP
#define AREG_BASE_CETHREADADDRESS_HPP
/************************************************************************
 * \file        areg/src/base/CEThreadAddress.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread Address class
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CEString.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;

/**
 * \brief   a class, containing thread address information.
 *          The thread address contains Process ID and Thread ID.
 *          Both of these parameters are unique and used for resource mapping.
 **/
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CEThreadAddress class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CEThreadAddress
{
//////////////////////////////////////////////////////////////////////////
// CEThreadAddress Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   CEThread::INVALID_THREAD_NAME
     *          Invalid thread name
     **/
    static const char* const        INVALID_THREAD_NAME     /*= "INVALID_THREAD_NAME"*/;

public:
    /**
     * \brief   CEThreadAddress::INVALID_THREAD_ADDRESS
     *          Invalid thread address constant.
     **/
    static const CEThreadAddress    INVALID_THREAD_ADDRESS;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Sets current process ID and invalid thread name.
     **/
    CEThreadAddress( void );
    /**
     * \brief   Initialization constructor. Sets current process ID and the thread name.
     * \param   threadID    Thread ID to set.
     **/
    CEThreadAddress( const char * threadName );
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEThreadAddress( const CEThreadAddress & src );
    /**
     * \brief   Initialization constructor. Initialize variables from given stream
     * \param   stream  Input Streaming object that contains Thread Address data.
     **/
    CEThreadAddress( const IEInStream & stream );
    /**
     * \brief   Destructor
     **/
    ~CEThreadAddress( void );

//////////////////////////////////////////////////////////////////////////
// CEThreadAddress operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   This operator converts thread address data to unsigned int value
     **/
    operator unsigned int ( void ) const;
    /**
     * \brief   This operator assigns data from source. 
     *          The process ID cannot be changed. Only Thread ID is copied.
     **/
    inline const CEThreadAddress & operator = (const CEThreadAddress & src);
    /**
     * \brief   Compares 2 thread address objects and if they are
     *          identical, returns true
     **/
    bool operator == (const CEThreadAddress & other) const;
    /**
     * \brief   Compares 2 thread address objects and if they are not
     *          identical, returns true
     **/
    bool operator != (const CEThreadAddress & other) const;

/************************************************************************/
// Friend global operators to make thread address streamable
/************************************************************************/
    /**
     * \brief	Read data from streaming object and initialize Thread Address object.
     * \param	stream	The streaming object to read data.
     * \param	input	The reference to Thread Address object to initialize data.
     * \return	Reference to streaming object.
     **/
    friend inline const IEInStream & operator >> ( const IEInStream & stream, CEThreadAddress & input );

    /**
     * \brief	Write data to streaming object and copies Thread Address object.
     * \param	stream	The streaming object to write data
     * \param	output	The reference to Thread Address object to get data
     * \return	Reference to streaming object.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const CEThreadAddress & output);

//////////////////////////////////////////////////////////////////////////
// CEThreadAddress operations and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Return thread name.
     **/
    inline const CEString & GetThreadName( void ) const;
    /**
     * \brief   Return true if current object is not equal to INVALID_THREAD_ADDRESS
     **/
    bool IsValid( void ) const;

    /**
     * \brief	Converts Thread Address object to the thread path string value.
     *          the thread path has following format: "<process ID>::<thread name>::",
     *          where <process ID> is an integer value of process and
     *          <string name> is the name of string.
     * \param	threadAddress	The thread address object to convert.
     * \return	Returns string of thread path
     **/
    static CEString ConvertAddressToPath( const CEThreadAddress & threadAddress );

    /**
     * \brief	Parses passed path and converts passed path to the Thread Address object.
     *          The path should contain at least following format: "<process ID>::<thread name>".
     *          The first item should be integer.
     *          If Thread Address was parsed and properly retrieved, on output, parameter 'out_nextPart' 
     *          will contains address of string in 'threadPath' after Thread Address part.
     *          For example: if path is "<process ID>::<thread name>::<component name>::<service name>::",
     *          on output, out_nextPart will contain "<component name>::<service name>::" it is not NULL
     * \param	threadPath	    The path of thread to convert.
     * \param   out_nextPart    If successfully retrieved thread address and it is not NULL, 
     *                          on output will contain address of string after retrieving Thread address.
     *                          If parsing fails, it will contain same address as 'threadPart'.
     *                          If parameter is NULL, it will be ignored.
     * \return	Return created from path thread address object.
     **/
    static CEThreadAddress ConvertPathToAddress( const char* const threadPath, const char** out_nextPart = NULL );

//////////////////////////////////////////////////////////////////////////
// CEThreadAddress member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The thread name.
     **/
    CEString        mThreadName;
};

//////////////////////////////////////////////////////////////////////////
// CEThreadAddress class inline function implementation
//////////////////////////////////////////////////////////////////////////
inline const CEThreadAddress & CEThreadAddress::operator = ( const CEThreadAddress & src )
{
    if (static_cast<const CEThreadAddress *>(this) != &src)
    { this->mThreadName = src.mThreadName;}
    return (*this);
}

inline const CEString & CEThreadAddress::GetThreadName( void ) const
{   return mThreadName;                                         }

//////////////////////////////////////////////////////////////////////////
// Global operators for CEThreadAddress class
//////////////////////////////////////////////////////////////////////////
inline const IEInStream & operator >> (const IEInStream & stream, CEThreadAddress & input)
{   return ( stream >> input.mThreadName );     }

inline IEOutStream & operator << (IEOutStream & stream, const CEThreadAddress & output)
{   return ( stream << output.mThreadName );    }

#endif  // AREG_BASE_CETHREADADDRESS_HPP
