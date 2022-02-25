#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/Process.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       The class to handle process. Get process ID, process handle,
 *              process name, etc.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include <filesystem>
#include <string>

/************************************************************************
 * Dependencies.
 ************************************************************************/

 //////////////////////////////////////////////////////////////////////////
 // Process class declaration
 //////////////////////////////////////////////////////////////////////////
 /**
 * \brief   Process class contain basic functionality of process.
 *          It is a singleton object within process and contains
 *          such methods like getting process name, process path and
 *          extension. The path name does not contain path terminated char.
 **/

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
    static constexpr id_type    CURRENT_PROCESS  { 0 };

    /**
     * \brief   Process::UNKNOWN_PROCESS
     *          Unknown / invalid process ID.
     **/
    static constexpr id_type    UNKNOWN_PROCESS { static_cast<id_type>(~0) };

    /**
     * \brief   Process environment. Currently support only 32- or 64-bit processes.
     **/
    typedef enum class E_ProcEnv : int
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
    ~Process( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Return application name of current process.
     *          The name does not contain files extension.
     **/
    inline std::string getAppName() const;
    /**
     * \brief   Returns the name of current process. 
     *          The name may contain file extension as well.
     **/
    inline std::string getName() const;

    /**
     * \brief   Returns the extension of current process.
     **/
    inline std::string getExtension() const;

    /**
     * \brief   Returns the path of current process.
     *          The path does not contain path-separation
     *          character '\\' at the end.
     **/
    inline std::string getPath() const;

    /**
     * \brief   Returns the full path of current process,
     *          including file name and file extension.
     **/
    inline std::string getFullPath() const;

    /**
     * \brief   Return the ID of current process.
     **/
    inline id_type getId( void ) const;

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
    std::string getSafeEnvVariable( const char * var ) const;

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
    id_type         mProcessId;
    /**
     * \brief   The handle of process
     **/
    PROCESSHANDLE   mProcessHandle;

    /**
     * \brief   The full path of process, including name and extension
     **/
    std::filesystem::path   mProcessFullPath;

    /**
     * \brief   Flag, indicating whether process object is initialized or not.
     **/
    bool            mIsInitialized;
    /**
     * \brief   The singletone instance of Process object.
     */
    static Process  _theProcess;


//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( Process );
};

//////////////////////////////////////////////////////////////////////////
// Process class inline functions
//////////////////////////////////////////////////////////////////////////

inline Process & Process::getInstance( void )
{
    return Process::_theProcess.initilize();
}

inline std::string Process::getAppName() const
{
    return mProcessFullPath.stem().string();
}

inline std::string Process::getName() const
{
    return mProcessFullPath.filename().string();
}

inline std::string Process::getExtension() const
{
    return mProcessFullPath.extension().string();
}

inline std::string Process::getPath() const
{
    return mProcessFullPath.parent_path().string();
}

inline std::string Process::getFullPath() const
{
    return mProcessFullPath.string();
}

inline id_type Process::getId(void) const
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
    case Process::eProcEnv::ProcEnv32Bits:
        return "32-bits";
    case Process::eProcEnv::ProcEnv64Bits:
        return "64-bits";
    case Process::eProcEnv::ProcEnvUnknown:
        return "unknown bits";
    default:
        return "ERR: Unexpected Process::eProcEnv value";
    }
}
