/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Process.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The class to handle process. Get process ID, process handle, process name, etc.
 *              OS independent implementation
 ************************************************************************/
#include "areg/base/Process.hpp"

#include "areg/base/File.hpp"
#include "areg/component/NEService.hpp"
#include <filesystem>
#include <iostream>

//////////////////////////////////////////////////////////////////////////
// Process class implementation
//////////////////////////////////////////////////////////////////////////

Process & Process::getInstance( void )
{
    static Process _theProcess;
    return _theProcess;
}


Process::Process( void )
    : mProcEnv          ( static_cast<Process::eProcEnv>(sizeof(id_type)) )
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
        mProcessPath = procPath.parent_path().empty() ? String::getEmptyString() : procPath.parent_path().string();
        mProcessName = procPath.filename().empty()    ? String::getEmptyString() : procPath.filename().string();
        mAppName     = procPath.stem().empty()        ? String::getEmptyString() : procPath.stem().string();
        mProcessExt  = procPath.extension().empty()   ? String::getEmptyString() : procPath.extension().string();
    }
}

unsigned int Process::getBitness(void) const
{
    if ((static_cast<uint16_t>(mProcEnv) & static_cast<uint16_t>(Process::eProcEnv::ProcEnv32Bits)) != 0)
    {
        return static_cast<unsigned int>(NEService::eInstanceBitness::Bitness32);
    }
    else if ((static_cast<uint16_t>(mProcEnv) & static_cast<uint16_t>(Process::eProcEnv::ProcEnv64Bits)) != 0)
    {
        return static_cast<unsigned int>(NEService::eInstanceBitness::Bitness64);
    }
    else
    {
        return static_cast<unsigned int>(NEService::eInstanceBitness::BitnessUnknown);
    }
}

String Process::getSafeEnvVariable( const char * var ) const
{
    return _osGetEnvVariable( var );
}

