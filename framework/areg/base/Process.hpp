#ifndef AREG_BASE_PROCESS_HPP
#define AREG_BASE_PROCESS_HPP
/************************************************************************
 * \file        areg/base/Process.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The class to handle process. Get process ID, process handle,
 *              process name, etc.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"

/************************************************************************
 * Dependencies.
 ************************************************************************/

/**
 * \brief   Process class contain basic functionality of process.
 *          It is a singleton object within process and contains
 *          such methods like getting process name, process path and
 *          extension. The path name does not contain path terminated char.
 **/

//////////////////////////////////////////////////////////////////////////
// Process class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API Process
{
//////////////////////////////////////////////////////////////////////////
// Local constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Process::CURRENT_PROCESS
     *          ID Specifying current process
     **/
    static const ITEM_ID    CURRENT_PROCESS     /*= 0*/;

    /**
     * \brief   Process::UNKNOWN_PROCESS
     *          Invalid process ID.
     **/
    static const ITEM_ID    UNKNOWN_PROCESS     /*= -1*/;

    /**
     * \brief   Process environment. Currently support only 32- or 64-bit processes.
     **/
    typedef enum E_ProcEnv
    {
          ProcEnvUnknown    = 0
        , ProcEnv32Bits     = sizeof(uint32_t)
        , ProcEnv64Bits     = sizeof(uint64_t)
    } eProcEnv;

    /**
     * \brief   Converts and returns the string value of Process::eProcEnv;
     **/
    static inline const char * getString( Process::eProcEnv  val );

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the instance of process object
     **/
    static inline Process & getInstance( void );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor, protected
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor. Protected.
     *          To get instance of object, call static method GetProcess()
     *          The object is singleton.
     **/
    Process( void );

    /**
     * \brief   Destructor.
     **/
    ~Process( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Return application name of current process.
     *          The name does not contain files extension.
     **/
    inline const char * getAppName( void ) const;
    /**
     * \brief   Returns the name of current process. 
     *          The name may contain file extension as well.
     **/
    inline const char * getName( void ) const;

    /**
     * \brief   Returns the extension of current process.
     **/
    inline const char * getExtension( void ) const;

    /**
     * \brief   Returns the path of current process.
     *          The path does not contain path-separation
     *          character '\\' at the end.
     **/
    inline const char * getPath( void ) const;

    /**
     * \brief   Returns the full path of current process,
     *          including file name and file extension.
     **/
    inline const char * getFullPath( void ) const;

    /**
     * \brief   Return the ID of current process.
     **/
    inline ITEM_ID getId( void ) const;

    /**
     * \brief   Returns process environment. It is either 32- or 64-bits.
     **/
    inline Process::eProcEnv getEnvironment( void ) const;

    /**
     * \brief   returns the value of the environment variable var, 
     *          or an empty string if
     *            - either no such environment variable exists (or is empty)
     *            - there is a reason it should not be used 
     *              (e.g. elevated privileges in Linux)
     * \param   var   the environment variable to return
     **/
    String getSafeEnvVariable( const char * var ) const;

private:

    /**
     * \brief   When the method is called, the full path is passed as parameter.
     * \param   fullPath    The full path to executable.
     **/
    void _initPaths( const char * fullPath );

    /**
     * \brief	Initializes process internal data.
     */
    Process & initilize( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The process environment
     **/
    const eProcEnv  mProcEnv;
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
    String          mAppName;
    /**
     * \brief   The name of process file
     **/
    String          mProcessName;
    /**
     * \brief   The extension of process file
     **/
    String          mProcessExt;
    /**
     * \brief   The path of process
     **/
    String          mProcessPath;
    /**
     * \brief   The full path of process, including name and extension
     **/
    String          mProcessFullPath;
    /**
     * \brief	Flag, indicating whether process object is initialized or not.
     **/
    bool 			mIsInitialized;
    /**
     * \brief	The singletone instance of Process object.
     */
    static Process	_theProcess;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    Process( const Process & /*src*/ );
    const Process & operator = ( const Process & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// Process class inline functions
//////////////////////////////////////////////////////////////////////////

inline Process & Process::getInstance( void )
{
    return Process::_theProcess.initilize();
}

inline const char * Process::getAppName( void ) const
{
    return mAppName;
}

inline const char * Process::getName(void) const
{
    return mProcessName;
}

inline const char * Process::getExtension(void) const
{
    return mProcessExt;
}

inline const char * Process::getPath(void) const
{
    return mProcessPath;
}

inline const char * Process::getFullPath(void) const
{
    return mProcessFullPath;
}

inline ITEM_ID Process::getId(void) const
{
    return mProcessId;
}

inline Process::eProcEnv Process::getEnvironment( void ) const
{
    return mProcEnv;
}

inline const char * Process::getString( Process::eProcEnv  val )
{
    switch (val)
    {
    case Process::ProcEnv32Bits:
        return "32-bits";
    case Process::ProcEnv64Bits:
        return "64-bits";
    case Process::ProcEnvUnknown:
        return "unknown bits";
    default:
        return "ERR: Unexpected Process::eProcEnv value";
    }
}

#endif  // AREG_BASE_PROCESS_HPP
