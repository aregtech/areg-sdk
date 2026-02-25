#ifndef AREG_BASE_PROCESS_HPP
#define AREG_BASE_PROCESS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/Process.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
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

 //////////////////////////////////////////////////////////////////////////
 // Process class declaration
 //////////////////////////////////////////////////////////////////////////
/**
 * \brief   Singleton providing process information such as name, path, executable extension, and
 *          environment details.
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
    enum class Bitness : int32_t
    {
          BitsUnknown   = 0
        , Bits32        = sizeof(uint32_t)
        , Bits64        = sizeof(uint64_t)
    };

    /**
     * \brief   Returns the string representation of Process::Bitness value.
     **/
    static inline const char * as_string( Process::Bitness  val );

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the singleton instance of the Process object.
     **/
    static Process & instance();

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor, protected
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Protected constructor. The object is a singleton; use instance() to obtain it.
     **/
    Process();

    /**
     * \brief   Destructor.
     **/
    ~Process() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the application name without file extension.
     **/
    inline const String & app_name() const;
    /**
     * \brief   Returns the name of the current process, including file extension if applicable.
     **/
    inline const String & name() const;

    /**
     * \brief   Returns the file extension of the current process executable.
     **/
    inline const String & extension() const;

    /**
     * \brief   Returns the path to the current process, without trailing path-separator character.
     **/
    inline const String & path() const;

    /**
     * \brief   Returns the full path including filename and extension.
     **/
    inline const String & full_path() const;

    /**
     * \brief   Returns the ID of the current process.
     **/
    inline id_type id() const;

    /**
     * \brief   Returns the process environment bitness: 32-bit or 64-bit.
     **/
    inline Process::Bitness environment() const;

    /**
     * \brief   Returns the process environment bitness: 32-bit or 64-bit.
     **/
    uint32_t bitness() const;

    /**
     * \brief   Returns the environment variable value or an empty string if the variable does not
     *          exist, is empty, or should not be used (e.g., due to elevated privileges).
     *
     * \param   var     The environment variable name.
     **/
    String safe_env_variable( const char * var ) const;

private:

    /**
     * \brief   Initializes process paths from the given full executable path.
     *
     * \param   fullPath    The full path to the executable.
     **/
    void _init_paths( const char * fullPath );

    /**
     * \brief   OS-specific initialization of process internal data.
     **/
    void _os_initilize();

    /**
     * \brief   Returns the environment variable value or an empty string if not found.
     *
     * \param   var     The environment variable name.
     **/
    String _os_env_variable( const char * var ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The process environment
     **/
    const Bitness  mProcEnv;
    /**
     * \brief   The ID of process
     **/
    id_type         mProcessId;
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

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( Process );
};

//////////////////////////////////////////////////////////////////////////
// Process class inline functions
//////////////////////////////////////////////////////////////////////////

inline const String & Process::app_name() const
{
    return mAppName;
}

inline const String & Process::name() const
{
    return mProcessName;
}

inline const String & Process::extension() const
{
    return mProcessExt;
}

inline const String & Process::path() const
{
    return mProcessPath;
}

inline const String & Process::full_path() const
{
    return mProcessFullPath;
}

inline id_type Process::id() const
{
    return mProcessId;
}

inline Process::Bitness Process::environment() const
{
    return mProcEnv;
}

inline const char * Process::as_string( Process::Bitness  val )
{
    switch (val)
    {
    case Process::Bitness::Bits32:
        return "32-bits";
    case Process::Bitness::Bits64:
        return "64-bits";
    case Process::Bitness::BitsUnknown:
        return "unknown bits";
    default:
        return "ERR: Unexpected Process::Bitness value";
    }
}

#endif  // AREG_BASE_PROCESS_HPP
