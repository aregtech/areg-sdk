#ifndef AREG_BASE_CEPROCESS_HPP
#define AREG_BASE_CEPROCESS_HPP
/************************************************************************
 * \file        areg/src/base/CEProcess.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The class to handle process. Get process ID, process handle,
 *              process name, etc.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CEString.hpp"

/************************************************************************
 * Dependencies.
 ************************************************************************/

/**
 * \brief   CEProcess class contain basic functionality of process.
 *          It is a singleton object within process and contains
 *          such methods like getting process name, process path and
 *          extension. The path name does not contain path terminated char.
 **/

//////////////////////////////////////////////////////////////////////////
// CEProcess class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CEProcess
{
//////////////////////////////////////////////////////////////////////////
// Local constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   CEProcess::CURRENT_PROCESS
     *          ID Specifying current process
     **/
    static const ITEM_ID    CURRENT_PROCESS     /*= 0*/;

    /**
     * \brief   CEProcess::UNKNOWN_PROCESS
     *          Invalid process ID.
     **/
    static const ITEM_ID    UNKNOWN_PROCESS     /*= -1*/;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the instance of process object
     **/
    static CEProcess & GetProcess( void );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor, protected
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor. Protected.
     *          To get instance of object, call static method GetProcess()
     *          The object is singleton.
     **/
    CEProcess( void );

    /**
     * \brief   Destructor.
     **/
    ~CEProcess( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Return application name of current process.
     *          The name does not contain files extension.
     **/
    inline const char * GetAppName( void ) const;
    /**
     * \brief   Returns the name of current process. 
     *          If the name may contain file extension as well.
     **/
    inline const char * GetProcessName( void ) const;

    /**
     * \brief   Returns the extension of current process.
     **/
    inline const char * GetProcessExtension( void ) const;

    /**
     * \brief   Returns the path of current process.
     *          The path does not contain path-separation
     *          character '\\' at the end.
     **/
    inline const char * GetProcessPath( void ) const;

    /**
     * \brief   Returns the full path of current process,
     *          including file name and file extension.
     **/
    inline const char * GetProcessFullPath( void ) const;

    /**
     * \brief   Return the ID of current process.
     **/
    inline ITEM_ID GetProcessId( void ) const;

    /**
     * \brief   returns the value of the environment variable var, 
     *          or an empty string if
     *            - either no such environment variable exists (or is empty)
     *            - there is a reason it should not be used 
     *              (e.g. elevated privileges in Linux)
     * \param   var   the environment variable to return
     **/
    CEString GetSafeEnvVariable( const char * var ) const;

private:

    /**
     * \brief   When the method is called, the full path is passed as parameter.
     * \param   fullPath    The full path to executable.
     **/
    void initPaths( const char * fullPath );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The ID of process
     **/
    ITEM_ID         mProcessId;
    /**
     * \brief   The handle of process
     **/
    PROCESSHANDLE   mProcessHandle;
    /**
     * \brief   The name of application without extension.
     **/
    CEString        mAppName;
    /**
     * \brief   The name of process file
     **/
    CEString        mProcessName;
    /**
     * \brief   The extension of process file
     **/
    CEString        mProcessExt;
    /**
     * \brief   The path of process
     **/
    CEString        mProcessPath;
    /**
     * \brief   The full path of process, including name and extension
     **/
    CEString        mProcessFullPath;

//////////////////////////////////////////////////////////////////////////
// FOrbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEProcess( const CEProcess & /*src*/ );
    const CEProcess & operator = ( const CEProcess & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEProcess class inline functions
//////////////////////////////////////////////////////////////////////////

inline const char * CEProcess::GetAppName( void ) const
{   return mAppName;            }

inline const char * CEProcess::GetProcessName(void) const
{   return mProcessName;        }

inline const char * CEProcess::GetProcessExtension(void) const
{   return mProcessExt;         }

inline const char * CEProcess::GetProcessPath(void) const
{   return mProcessPath;        }

inline const char * CEProcess::GetProcessFullPath(void) const
{   return mProcessFullPath;    }

inline ITEM_ID CEProcess::GetProcessId(void) const
{   return mProcessId;          }

#endif  // AREG_BASE_CEPROCESS_HPP
