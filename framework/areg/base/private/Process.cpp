/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Process.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The class to handle process. Get process ID, process handle, process name, etc.
 *              OS independent implementation
 ************************************************************************/
#include "areg/base/Process.hpp"

#include "areg/base/File.hpp"
#include "areg/component/ServiceDefs.hpp"
#include <filesystem>
#include <iostream>

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // Process class implementation
    //////////////////////////////////////////////////////////////////////////

    Process & Process::getInstance()
    {
        static Process _theProcess;
        return _theProcess;
    }


    Process::Process()
        : mProcEnv          ( static_cast<Process::Bitness>(sizeof(id_type)) )
        , mProcessId        ( Process::UNKNOWN_PROCESS )
        , mProcessHandle    ( nullptr )
        , mAppName          ( )
        , mProcessName      ( )
        , mProcessExt       ( )
        , mProcessPath      ( )
        , mProcessFullPath  ( )
        , mIsInitialized    ( false )
    {
        _osInitilize();
    }

    void Process::_initPaths( const char * fullPath )
    {
        ASSERT(fullPath != nullptr);
        mProcessFullPath = fullPath;
        std::filesystem::path procPath(mProcessFullPath.getData());

        if (procPath.empty() == false)
        {
            mProcessPath = procPath.parent_path().empty() ? areg::String::getEmptyString() : procPath.parent_path().string();
            mProcessName = procPath.filename().empty()    ? areg::String::getEmptyString() : procPath.filename().string();
            mAppName     = procPath.stem().empty()        ? areg::String::getEmptyString() : procPath.stem().string();
            mProcessExt  = procPath.extension().empty()   ? areg::String::getEmptyString() : procPath.extension().string();
        }
    }

    uint32_t Process::getBitness() const
    {
        if ((static_cast<uint16_t>(mProcEnv) & static_cast<uint16_t>(Process::Bitness::Bits32)) != 0)
        {
            return static_cast<uint32_t>(areg::InstanceBitness::Bitness32);
        }
        else if ((static_cast<uint16_t>(mProcEnv) & static_cast<uint16_t>(Process::Bitness::Bits64)) != 0)
        {
            return static_cast<uint32_t>(areg::InstanceBitness::Bitness64);
        }
        else
        {
            return static_cast<uint32_t>(areg::InstanceBitness::BitnessUnknown);
        }
    }

    areg::String Process::getSafeEnvVariable( const char * var ) const
    {
        return _osGetEnvVariable( var );
    }

} // namespace areg