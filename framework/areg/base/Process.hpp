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

namespace areg
{
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
        enum class Bitness : int32_t
        {
            BitsUnknown   = 0
            , Bits32        = sizeof(uint32_t)
            , Bits64        = sizeof(uint64_t)
        };

        /**
         * \brief   Converts and returns the string value of Process::Bitness;
         **/
        static inline const char * getString( Process::Bitness  val );

    //////////////////////////////////////////////////////////////////////////
    // Static members
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Returns the instance of process object
         **/
        static Process & getInstance();

    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor, protected
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Default constructor. Protected.
         *          To get instance of object, call static method GetProcess()
         *          The object is singleton.
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
         * \brief   Return application name of current process.
         *          The name does not contain files extension.
         **/
        inline const areg::String & getAppName() const;
        /**
         * \brief   Returns the name of current process. 
         *          The name may contain file extension as well.
         **/
        inline const areg::String & getName() const;

        /**
         * \brief   Returns the extension of current process.
         **/
        inline const areg::String & getExtension() const;

        /**
         * \brief   Returns the path of current process.
         *          The path does not contain path-separation
         *          character '\\' at the end.
         **/
        inline const areg::String & getPath() const;

        /**
         * \brief   Returns the full path of current process,
         *          including file name and file extension.
         **/
        inline const areg::String & getFullPath() const;

        /**
         * \brief   Return the ID of current process.
         **/
        inline id_type getId() const;

        /**
         * \brief   Returns process environment. It is either 32- or 64-bits.
         **/
        inline Process::Bitness getEnvironment() const;

        uint32_t getBitness() const;

        /**
         * \brief   returns the value of the environment variable var, 
         *          or an empty string if:
         *              -   No such environment variable exists or it is empty.
         *              -   There is a reason that it should not be used 
         *                  (e.g. elevated privileges in Linux).
         * \param   var   The environment variable to return value.
         **/
        areg::String getSafeEnvVariable( const char * var ) const;

    private:

        /**
         * \brief   When the method is called, the full path is passed as parameter.
         * \param   fullPath    The full path to executable.
         **/
        void _initPaths( const char * fullPath );

        /**
         * \brief	OS specific initialization of process internal data.
         */
        void _osInitilize();

        /**
         * \brief   Returns the value of environment variable or empty strig.
         * \param   var     The environment variable to return value.
         **/
        areg::String _osGetEnvVariable( const char * var ) const;

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
        areg::String          mAppName;
        /**
         * \brief   The name of process file
         **/
        areg::String          mProcessName;
        /**
         * \brief   The extension of process file
         **/
        areg::String          mProcessExt;
        /**
         * \brief   The path of process
         **/
        areg::String          mProcessPath;
        /**
         * \brief   The full path of process, including name and extension
         **/
        areg::String          mProcessFullPath;
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

    inline const areg::String & Process::getAppName() const
    {
        return mAppName;
    }

    inline const areg::String & Process::getName() const
    {
        return mProcessName;
    }

    inline const areg::String & Process::getExtension() const
    {
        return mProcessExt;
    }

    inline const areg::String & Process::getPath() const
    {
        return mProcessPath;
    }

    inline const areg::String & Process::getFullPath() const
    {
        return mProcessFullPath;
    }

    inline id_type Process::getId() const
    {
        return mProcessId;
    }

    inline Process::Bitness Process::getEnvironment() const
    {
        return mProcEnv;
    }

    inline const char * Process::getString( Process::Bitness  val )
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

} // namespace areg
#endif  // AREG_BASE_PROCESS_HPP
